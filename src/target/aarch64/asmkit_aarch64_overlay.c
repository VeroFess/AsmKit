#include "core/asmkit_internal.h"
#include "asmkit_gen_aarch64_defs.h"
#include "asmkit_gen_aarch64_mnemonics.h"

static asmkit_mnemonic_id_t a64_inst_mnemonic(const asmkit_inst_t* inst);
static int a64_is_address_materialization(const asmkit_inst_t* inst);
static int a64_is_literal_pc_relative(const asmkit_inst_t* inst);

static asmkit_status_t a64_decode(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const uint8_t* code, size_t code_size, uint64_t address, asmkit_inst_t* out_inst)
{
    (void)workspace;
    return asmkit_gen_aarch64_decode_one(engine, code, code_size, address, out_inst);
}

static const asmkit_operand_t* a64_find_pc_rel_operand(const asmkit_inst_t* inst, uint8_t* out_index)
{
    uint8_t i;
    if (inst == 0) {
        return 0;
    }
    for (i = 0u; i < inst->operand_count && i < ASMKIT_MAX_OPERANDS; ++i) {
        if (inst->operands[i].kind == ASMKIT_OP_PC_REL) {
            if (out_index != 0) {
                *out_index = i;
            }
            return &inst->operands[i];
        }
    }
    return 0;
}

static uint64_t a64_operand_target(const asmkit_inst_t* inst, const asmkit_operand_t* operand)
{
    if (operand == 0) {
        return 0u;
    }
    if (operand->abs_target != 0u || operand->imm == 0) {
        return operand->abs_target;
    }
    return inst->address + (uint64_t)operand->imm;
}

static asmkit_status_t a64_analyze(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, asmkit_inst_semantics_t* out_semantics)
{
    const asmkit_operand_t* pc_operand;
    uint64_t pc_target;

    (void)engine;
    (void)workspace;
    asmkit_zero(out_semantics, sizeof(*out_semantics));
    out_semantics->inst_class = inst->inst_class;
    out_semantics->reloc_kind = ASMKIT_RELOC_COPY_AS_IS;
    out_semantics->min_emit_size = inst->size;
    out_semantics->max_emit_size = inst->size;
    if (inst->inst_class == ASMKIT_INST_BTI) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_BTI | ASMKIT_HAZARD_LANDING_PAD;
    }
    if (inst->inst_class == ASMKIT_INST_PAC) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_PAC;
    }
    if (inst->inst_class == ASMKIT_INST_DIRECT_BRANCH ||
        inst->inst_class == ASMKIT_INST_COND_BRANCH ||
        inst->inst_class == ASMKIT_INST_DIRECT_CALL ||
        inst->inst_class == ASMKIT_INST_INDIRECT_BRANCH ||
        inst->inst_class == ASMKIT_INST_INDIRECT_CALL) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_BRANCH;
        out_semantics->branch.is_branch = inst->inst_class != ASMKIT_INST_DIRECT_CALL && inst->inst_class != ASMKIT_INST_INDIRECT_CALL;
        out_semantics->branch.is_call = inst->inst_class == ASMKIT_INST_DIRECT_CALL || inst->inst_class == ASMKIT_INST_INDIRECT_CALL;
        out_semantics->branch.is_conditional = inst->inst_class == ASMKIT_INST_COND_BRANCH;
        out_semantics->branch.is_direct = (inst->flags & ASMKIT_INST_FLAG_DIRECT) != 0u;
        out_semantics->branch.is_indirect = !out_semantics->branch.is_direct;
        if (out_semantics->branch.is_call) {
            out_semantics->hazard_flags |= ASMKIT_HAZARD_CALL;
        }
    }
    if (inst->inst_class == ASMKIT_INST_RETURN) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_RETURN;
        out_semantics->branch.is_return = true;
    }
    pc_operand = a64_find_pc_rel_operand(inst, 0);
    if ((inst->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u && pc_operand != 0) {
        pc_target = a64_operand_target(inst, pc_operand);
        out_semantics->hazard_flags |= ASMKIT_HAZARD_PC_RELATIVE;
        out_semantics->pc_rel.is_pc_relative = true;
        out_semantics->pc_rel.original_target = pc_target;
        out_semantics->pc_rel.is_literal_load = a64_is_literal_pc_relative(inst);
        out_semantics->pc_rel.is_address_materialization = a64_is_address_materialization(inst);
        out_semantics->pc_rel.is_control_flow =
            inst->inst_class == ASMKIT_INST_DIRECT_BRANCH ||
            inst->inst_class == ASMKIT_INST_COND_BRANCH ||
            inst->inst_class == ASMKIT_INST_DIRECT_CALL;
        out_semantics->branch.has_target = out_semantics->pc_rel.is_control_flow;
        out_semantics->branch.target = pc_target;
        out_semantics->branch.min_disp = -134217728;
        out_semantics->branch.max_disp = 134217724;
        out_semantics->reloc_kind = ASMKIT_RELOC_REENCODE_WITH_NEW_PC;
        if (inst->inst_class == ASMKIT_INST_COND_BRANCH) {
            out_semantics->max_emit_size = 20u;
        } else if (inst->inst_class == ASMKIT_INST_DIRECT_BRANCH || inst->inst_class == ASMKIT_INST_DIRECT_CALL) {
            out_semantics->max_emit_size = 16u;
        }
    }
    return ASMKIT_OK;
}

static asmkit_status_t a64_emit_abs(int is_call, uint64_t to_address, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    asmkit_zero(out_result, sizeof(*out_result));
    out_result->size = 16u;
    out_result->clobber_mask_lo = (uint64_t)1u << ASMKIT_REG_AARCH64_X16;
    if (out_capacity < 16u) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    asmkit_store32le(out_code, 0x58000050u);
    asmkit_store32le(out_code + 4, is_call ? 0xd63f0200u : 0xd61f0200u);
    asmkit_store64le(out_code + 8, to_address);
    return ASMKIT_OK;
}

static asmkit_status_t a64_emit_any(const asmkit_engine_t* engine, int is_call, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    asmkit_branch_mode_t mode;
    asmkit_status_t status;
    uint64_t x16_mask;

    mode = options != 0 ? options->mode : ASMKIT_BRANCH_AUTO;
    if (mode != ASMKIT_BRANCH_ABSOLUTE_ONLY) {
        status = asmkit_gen_aarch64_emit_rel(engine, is_call, from_address, to_address, out_code, out_capacity, out_result);
        if (status == ASMKIT_OK || status == ASMKIT_ERR_OUTPUT_TOO_SMALL || mode == ASMKIT_BRANCH_DIRECT_ONLY) {
            return status;
        }
    }
    if (mode == ASMKIT_BRANCH_PREFER_CLOBBERLESS) {
        return ASMKIT_ERR_FORBIDDEN_CLOBBER;
    }
    x16_mask = (uint64_t)1u << ASMKIT_REG_AARCH64_X16;
    if (options != 0 &&
        mode == ASMKIT_BRANCH_ALLOW_SCRATCH_CLOBBER &&
        (options->allowed_clobber_mask_lo & x16_mask) == 0u) {
        return ASMKIT_ERR_FORBIDDEN_CLOBBER;
    }
    return a64_emit_abs(is_call, to_address, out_code, out_capacity, out_result);
}

static asmkit_status_t a64_emit_jump(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    (void)workspace;
    return a64_emit_any(engine, 0, from_address, to_address, options, out_code, out_capacity, out_result);
}

static asmkit_status_t a64_emit_call(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    (void)workspace;
    return a64_emit_any(engine, 1, from_address, to_address, options, out_code, out_capacity, out_result);
}

static asmkit_mnemonic_id_t a64_inst_mnemonic(const asmkit_inst_t* inst)
{
    if (inst == 0) {
        return ASMKIT_MNEMONIC_INVALID;
    }
    if (inst->mnemonic_id != ASMKIT_MNEMONIC_INVALID) {
        return inst->mnemonic_id;
    }
    switch (inst->id) {
    case ASMKIT_A64_B:
        return ASMKIT_GEN_AARCH64_MNEMONIC_B_ID;
    case ASMKIT_A64_BL:
        return ASMKIT_GEN_AARCH64_MNEMONIC_BL_ID;
    case ASMKIT_A64_BR:
        return ASMKIT_GEN_AARCH64_MNEMONIC_BR_ID;
    case ASMKIT_A64_BLR:
        return ASMKIT_GEN_AARCH64_MNEMONIC_BLR_ID;
    case ASMKIT_A64_RET:
        return ASMKIT_GEN_AARCH64_MNEMONIC_RET_ID;
    default:
        return ASMKIT_MNEMONIC_INVALID;
    }
}

static uint32_t a64_inst_word(const asmkit_inst_t* inst)
{
    return inst != 0 && inst->size >= 4u ? asmkit_load32le(inst->bytes) : 0u;
}

static int a64_word_is_b_cond(uint32_t word)
{
    return (word & 0xff000010u) == 0x54000000u;
}

static int a64_word_is_cbz_cbnz(uint32_t word)
{
    return (word & 0x7e000000u) == 0x34000000u || (word & 0x7e000000u) == 0x35000000u;
}

static int a64_word_is_tbz_tbnz(uint32_t word)
{
    return (word & 0x7e000000u) == 0x36000000u || (word & 0x7e000000u) == 0x37000000u;
}

static int a64_word_is_ldr_literal(uint32_t word)
{
    return (word & 0x3b000000u) == 0x18000000u;
}

static int a64_word_is_prfm_literal(uint32_t word)
{
    return (word & 0xff000000u) == 0xd8000000u;
}

static int a64_word_is_adr(uint32_t word)
{
    return (word & 0x9f000000u) == 0x10000000u;
}

static int a64_word_is_adrp(uint32_t word)
{
    return (word & 0x9f000000u) == 0x90000000u;
}

static int a64_word_is_unconditional_b_bl(uint32_t word)
{
    return (word & 0xfc000000u) == 0x14000000u || (word & 0xfc000000u) == 0x94000000u;
}

static int a64_is_adrp(const asmkit_inst_t* inst)
{
    return a64_inst_mnemonic(inst) == ASMKIT_GEN_AARCH64_MNEMONIC_ADRP_ID || a64_word_is_adrp(a64_inst_word(inst));
}

static int a64_is_address_materialization(const asmkit_inst_t* inst)
{
    asmkit_mnemonic_id_t mnemonic;
    uint32_t word;
    mnemonic = a64_inst_mnemonic(inst);
    if (mnemonic == ASMKIT_GEN_AARCH64_MNEMONIC_ADR_ID || mnemonic == ASMKIT_GEN_AARCH64_MNEMONIC_ADRP_ID) {
        return 1;
    }
    word = a64_inst_word(inst);
    return a64_word_is_adr(word) || a64_word_is_adrp(word);
}

static int a64_is_literal_pc_relative(const asmkit_inst_t* inst)
{
    uint32_t word;
    if (inst != 0 && (inst->flags & ASMKIT_INST_FLAG_LITERAL) != 0u) {
        return 1;
    }
    word = a64_inst_word(inst);
    return a64_word_is_ldr_literal(word) || a64_word_is_prfm_literal(word);
}

static int a64_is_unconditional_pc_branch_or_call(const asmkit_inst_t* inst)
{
    asmkit_mnemonic_id_t mnemonic;
    if (inst == 0 || (inst->flags & ASMKIT_INST_FLAG_PC_RELATIVE) == 0u) {
        return 0;
    }
    mnemonic = a64_inst_mnemonic(inst);
    if ((mnemonic == ASMKIT_GEN_AARCH64_MNEMONIC_B_ID && inst->inst_class == ASMKIT_INST_DIRECT_BRANCH) ||
        (mnemonic == ASMKIT_GEN_AARCH64_MNEMONIC_BL_ID && inst->inst_class == ASMKIT_INST_DIRECT_CALL)) {
        return 1;
    }
    return a64_word_is_unconditional_b_bl(a64_inst_word(inst));
}

static int a64_is_call(const asmkit_inst_t* inst)
{
    return inst != 0 && (a64_inst_mnemonic(inst) == ASMKIT_GEN_AARCH64_MNEMONIC_BL_ID || inst->inst_class == ASMKIT_INST_DIRECT_CALL);
}

static uint64_t a64_branch_target(const asmkit_inst_t* inst)
{
    const asmkit_operand_t* operand;
    operand = a64_find_pc_rel_operand(inst, 0);
    return a64_operand_target(inst, operand);
}

static int a64_reg_encoding(const asmkit_operand_t* operand, uint16_t expected_width, uint32_t* out_encoding)
{
    const asmkit_register_info_t* info;
    if (operand == 0 || out_encoding == 0 || operand->kind != ASMKIT_OP_REG) {
        return 0;
    }
    info = asmkit_gen_aarch64_register_info((uint32_t)operand->reg);
    if (info != 0 && info->encoding <= 31u &&
        (expected_width == 0u || info->width == expected_width || (operand->width != 0u && operand->width == expected_width))) {
        *out_encoding = (uint32_t)info->encoding;
        return 1;
    }
    if (operand->reg <= 31u && (expected_width == 0u || operand->width == 0u || operand->width == expected_width)) {
        *out_encoding = (uint32_t)operand->reg;
        return 1;
    }
    return 0;
}

static asmkit_status_t a64_encode(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, const asmkit_encode_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_encode_result_t* out_result)
{
    (void)workspace;
    (void)options;
    if (engine == 0 || inst == 0 || out_code == 0 || out_result == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_result, sizeof(*out_result));
    if (inst->arch != ASMKIT_ARCH_AARCH64 || inst->mode != ASMKIT_MODE_AARCH64 || engine->config.mode != ASMKIT_MODE_AARCH64) {
        return ASMKIT_ERR_UNSUPPORTED_INSTRUCTION;
    }
    switch (a64_inst_mnemonic(inst)) {
    case ASMKIT_GEN_AARCH64_MNEMONIC_B_ID:
    case ASMKIT_GEN_AARCH64_MNEMONIC_BL_ID:
        if (inst->operand_count >= 1u && inst->operands[0].kind == ASMKIT_OP_PC_REL) {
            asmkit_emit_result_t emit_result;
            asmkit_status_t status;
            status = asmkit_gen_aarch64_emit_rel(
                engine,
                a64_inst_mnemonic(inst) == ASMKIT_GEN_AARCH64_MNEMONIC_BL_ID,
                inst->address,
                a64_branch_target(inst),
                out_code,
                out_capacity,
                &emit_result);
            out_result->size = emit_result.size;
            out_result->flags = emit_result.flags;
            return status;
        }
        break;
    case ASMKIT_GEN_AARCH64_MNEMONIC_BR_ID:
    case ASMKIT_GEN_AARCH64_MNEMONIC_BLR_ID:
        if (inst->operand_count >= 1u) {
            uint32_t reg_encoding;
            uint32_t word;
            if (!a64_reg_encoding(&inst->operands[0], 64u, &reg_encoding)) {
                break;
            }
            out_result->size = 4u;
            if (out_capacity < 4u) {
                return ASMKIT_ERR_OUTPUT_TOO_SMALL;
            }
            word = (a64_inst_mnemonic(inst) == ASMKIT_GEN_AARCH64_MNEMONIC_BLR_ID ? 0xd63f0000u : 0xd61f0000u) |
                ((reg_encoding & 31u) << 5);
            asmkit_store32le(out_code, word);
            return ASMKIT_OK;
        }
        break;
    case ASMKIT_GEN_AARCH64_MNEMONIC_RET_ID:
        out_result->size = 4u;
        if (out_capacity < 4u) {
            return ASMKIT_ERR_OUTPUT_TOO_SMALL;
        }
        asmkit_store32le(out_code, 0xd65f03c0u);
        return ASMKIT_OK;
    default:
        break;
    }
    return asmkit_gen_aarch64_encode_inst(engine, inst, out_code, out_capacity, out_result);
}

static uint32_t a64_min_patch(const asmkit_engine_t* engine, uint64_t from_address, uint64_t to_address, asmkit_branch_mode_t mode, uint64_t* clobber_lo, uint64_t* clobber_hi)
{
    int64_t disp;
    (void)engine;
    if (clobber_lo != 0) {
        *clobber_lo = 0u;
    }
    if (clobber_hi != 0) {
        *clobber_hi = 0u;
    }
    disp = (int64_t)to_address - (int64_t)from_address;
    if (mode != ASMKIT_BRANCH_ABSOLUTE_ONLY && asmkit_i64_fits_i26_words4(disp)) {
        return 4u;
    }
    if (mode == ASMKIT_BRANCH_DIRECT_ONLY || mode == ASMKIT_BRANCH_PREFER_CLOBBERLESS) {
        return 0u;
    }
    if (clobber_lo != 0) {
        *clobber_lo = (uint64_t)1u << ASMKIT_REG_AARCH64_X16;
    }
    return 16u;
}

static asmkit_status_t a64_plan_jump_back(const asmkit_engine_t* engine, asmkit_branch_mode_t mode, asmkit_branch_plan_bound_t* out_bound)
{
    (void)engine;
    if (out_bound == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_bound, sizeof(*out_bound));
    if (mode == ASMKIT_BRANCH_DIRECT_ONLY || mode == ASMKIT_BRANCH_PREFER_CLOBBERLESS) {
        out_bound->size = 4u;
        out_bound->requires_near_island = true;
        out_bound->island_min_distance = -134217728;
        out_bound->island_max_distance = 134217724;
        return ASMKIT_OK;
    }
    out_bound->size = 16u;
    out_bound->clobber_mask_lo = (uint64_t)1u << ASMKIT_REG_AARCH64_X16;
    return ASMKIT_OK;
}

static int a64_is_conditional_pc_branch(const asmkit_inst_t* inst)
{
    uint32_t word;
    if (inst == 0 || inst->inst_class != ASMKIT_INST_COND_BRANCH ||
        (inst->flags & ASMKIT_INST_FLAG_PC_RELATIVE) == 0u) {
        return 0;
    }
    word = a64_inst_word(inst);
    return a64_word_is_b_cond(word) || a64_word_is_cbz_cbnz(word) || a64_word_is_tbz_tbnz(word);
}

static uint32_t a64_invert_conditional_to_skip(const asmkit_inst_t* inst)
{
    uint32_t word;
    word = asmkit_load32le(inst->bytes);
    if (a64_word_is_b_cond(word)) {
        return (word & ~((0x7ffffu << 5) | 0x0fu)) |
            (5u << 5) |
            ((word ^ 1u) & 0x0fu);
    }
    if (a64_word_is_cbz_cbnz(word)) {
        return (word & ~(0x7ffffu << 5)) ^ 0x01000000u | (5u << 5);
    }
    return (word & ~(0x3fffu << 5)) ^ 0x01000000u | (5u << 5);
}

static asmkit_status_t a64_expand_conditional_abs(const asmkit_inst_t* inst, uint64_t target, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    asmkit_emit_result_t abs_result;
    asmkit_status_t status;

    asmkit_zero(out_result, sizeof(*out_result));
    out_result->size = 20u;
    out_result->clobber_mask_lo = (uint64_t)1u << ASMKIT_REG_AARCH64_X16;
    if (out_capacity < 20u) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    asmkit_store32le(out_code, a64_invert_conditional_to_skip(inst));
    status = a64_emit_abs(0, target, out_code + 4, out_capacity - 4u, &abs_result);
    if (status != ASMKIT_OK) {
        return status;
    }
    out_result->clobber_mask_lo |= abs_result.clobber_mask_lo;
    out_result->clobber_mask_hi |= abs_result.clobber_mask_hi;
    return ASMKIT_OK;
}

static int64_t a64_range_min(const asmkit_inst_t* inst)
{
    uint32_t word;
    word = a64_inst_word(inst);
    if (a64_word_is_b_cond(word) || a64_word_is_cbz_cbnz(word) || a64_word_is_ldr_literal(word) || a64_word_is_prfm_literal(word)) {
        return -1048576;
    }
    if (a64_word_is_tbz_tbnz(word)) {
        return -32768;
    }
    if (a64_is_address_materialization(inst)) {
        return -1048576;
    }
    return -134217728;
}

static int64_t a64_range_max(const asmkit_inst_t* inst)
{
    uint32_t word;
    word = a64_inst_word(inst);
    if (a64_word_is_b_cond(word) || a64_word_is_cbz_cbnz(word) || a64_word_is_ldr_literal(word) || a64_word_is_prfm_literal(word)) {
        return 1048572;
    }
    if (a64_word_is_tbz_tbnz(word)) {
        return 32764;
    }
    if (a64_is_address_materialization(inst)) {
        return 1048575;
    }
    return 134217724;
}

static asmkit_status_t a64_relocate(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, uint64_t relocated_address, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    const asmkit_operand_t* pc_operand;
    uint32_t word;
    int64_t disp;
    uint64_t target;

    (void)workspace;
    if (inst == 0 || out_code == 0 || out_result == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_result, sizeof(*out_result));
    pc_operand = a64_find_pc_rel_operand(inst, 0);
    if (a64_is_unconditional_pc_branch_or_call(inst)) {
        asmkit_status_t status;
        if (pc_operand == 0) {
            return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
        }
        target = a64_operand_target(inst, pc_operand);
        status = asmkit_gen_aarch64_emit_rel(engine, a64_is_call(inst), relocated_address, target, out_code, out_capacity, out_result);
        if (status == ASMKIT_OK || status == ASMKIT_ERR_OUTPUT_TOO_SMALL) {
            return status;
        }
        return a64_emit_abs(a64_is_call(inst), target, out_code, out_capacity, out_result);
    }
    if ((inst->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u && pc_operand != 0) {
        target = a64_operand_target(inst, pc_operand);
        disp = (int64_t)target - (int64_t)relocated_address;
        if (a64_is_adrp(inst)) {
            disp = (int64_t)(target & ~(uint64_t)0xfffu) - (int64_t)(relocated_address & ~(uint64_t)0xfffu);
            if ((disp & 0xfff) != 0) {
                return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
            }
            disp >>= 12;
        }
        if (disp < a64_range_min(inst) || disp > a64_range_max(inst)) {
            if (a64_is_conditional_pc_branch(inst)) {
                return a64_expand_conditional_abs(inst, target, out_code, out_capacity, out_result);
            }
            return ASMKIT_ERR_BRANCH_OUT_OF_RANGE;
        }
        out_result->size = 4u;
        if (out_capacity < 4u) {
            return ASMKIT_ERR_OUTPUT_TOO_SMALL;
        }
        word = asmkit_load32le(inst->bytes);
        if (a64_word_is_b_cond(word) || a64_word_is_cbz_cbnz(word) || a64_word_is_ldr_literal(word) || a64_word_is_prfm_literal(word)) {
            word = (word & ~(0x7ffffu << 5)) | ((((uint32_t)(disp >> 2)) & 0x7ffffu) << 5);
        } else if (a64_word_is_tbz_tbnz(word)) {
            word = (word & ~(0x3fffu << 5)) | ((((uint32_t)(disp >> 2)) & 0x3fffu) << 5);
        } else if (a64_word_is_adr(word) || a64_word_is_adrp(word)) {
            uint32_t imm;
            imm = (uint32_t)disp & 0x1fffffu;
            word = (word & ~((0x3u << 29) | (0x7ffffu << 5))) |
                ((imm & 0x3u) << 29) | (((imm >> 2) & 0x7ffffu) << 5);
        } else {
            return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
        }
        asmkit_store32le(out_code, word);
        return ASMKIT_OK;
    }
    out_result->size = inst->size;
    if (out_capacity < inst->size) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    asmkit_copy(out_code, inst->bytes, inst->size);
    return ASMKIT_OK;
}

const asmkit_target_ops_t asmkit_aarch64_ops = {
    ASMKIT_ARCH_AARCH64,
    ASMKIT_MODE_AARCH64,
    ASMKIT_MODE_AARCH64,
    a64_decode,
    a64_analyze,
    a64_encode,
    a64_emit_jump,
    a64_emit_call,
    a64_relocate,
    a64_min_patch,
    a64_plan_jump_back
};

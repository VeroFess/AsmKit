#include "core/asmkit_internal.h"
#include "asmkit_gen_x86_defs.h"
#include "asmkit_gen_x86_mnemonics.h"

static const asmkit_operand_t* x86_pc_rel_operand(const asmkit_inst_t* inst, uint8_t* out_index);

static asmkit_status_t x86_decode(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const uint8_t* code, size_t code_size, uint64_t address, asmkit_inst_t* out_inst)
{
    (void)workspace;
    return asmkit_gen_x86_decode_one(engine, code, code_size, address, out_inst);
}

static int x86_find_opcode(const asmkit_inst_t* inst, uint32_t* opcode_offset)
{
    uint32_t i;
    i = 0u;
    while (i < inst->size && i < ASMKIT_MAX_INST_BYTES) {
        uint8_t b;
        b = inst->bytes[i];
        if (b == 0xf0u || b == 0xf2u || b == 0xf3u ||
            b == 0x2eu || b == 0x36u || b == 0x3eu || b == 0x26u ||
            b == 0x64u || b == 0x65u || b == 0x66u || b == 0x67u ||
            (inst->mode == ASMKIT_MODE_X86_64 && b >= 0x40u && b <= 0x4fu)) {
            ++i;
        } else {
            *opcode_offset = i;
            return 1;
        }
    }
    return 0;
}

static int x86_find_rip_disp(const asmkit_inst_t* inst, uint32_t* disp_offset)
{
    uint32_t i;
    uint8_t op;
    uint8_t modrm;
    uint8_t mod;
    uint8_t rm;

    if (!x86_find_opcode(inst, &i) || i >= inst->size) {
        return 0;
    }
    op = inst->bytes[i++];
    if (op == 0x0fu) {
        if (i >= inst->size) {
            return 0;
        }
        ++i;
    }
    if (i >= inst->size) {
        return 0;
    }
    modrm = inst->bytes[i++];
    mod = (uint8_t)(modrm >> 6);
    rm = (uint8_t)(modrm & 7u);
    if (inst->mode == ASMKIT_MODE_X86_64 && mod == 0u && rm == 5u) {
        *disp_offset = i;
        return i + 4u <= inst->size;
    }
    return 0;
}

static void x86_set_branch_limits(asmkit_branch_info_t* branch, uint16_t width)
{
    if (width == 8u) {
        branch->min_disp = -128;
        branch->max_disp = 127;
    } else if (width == 16u) {
        branch->min_disp = -32768;
        branch->max_disp = 32767;
    } else {
        branch->min_disp = (int64_t)(-2147483647 - 1);
        branch->max_disp = 2147483647;
    }
}

static asmkit_status_t x86_analyze(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, asmkit_inst_semantics_t* out_semantics)
{
    (void)engine;
    (void)workspace;
    asmkit_zero(out_semantics, sizeof(*out_semantics));
    out_semantics->inst_class = inst->inst_class;
    out_semantics->reloc_kind = ASMKIT_RELOC_COPY_AS_IS;
    out_semantics->min_emit_size = inst->size;
    out_semantics->max_emit_size = inst->size;

    if (inst->inst_class == ASMKIT_INST_CET_ENDBR) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_CET | ASMKIT_HAZARD_LANDING_PAD;
    }
    if ((inst->flags & ASMKIT_INST_FLAG_UNSUPPORTED_RELOC) != 0u) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_UNSUPPORTED_RELOC;
        out_semantics->reloc_kind = ASMKIT_RELOC_REJECT;
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
    if ((inst->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u && inst->operand_count > 0u) {
        const asmkit_operand_t* pc_operand;
        uint8_t pc_operand_index;
        pc_operand = x86_pc_rel_operand(inst, &pc_operand_index);
        if (pc_operand == 0) {
            return ASMKIT_OK;
        }
        out_semantics->hazard_flags |= ASMKIT_HAZARD_PC_RELATIVE;
        out_semantics->pc_rel.is_pc_relative = true;
        out_semantics->pc_rel.original_target = pc_operand->abs_target;
        out_semantics->pc_rel.operand_index = pc_operand_index;
        out_semantics->pc_rel.is_control_flow =
            inst->inst_class == ASMKIT_INST_DIRECT_BRANCH ||
            inst->inst_class == ASMKIT_INST_COND_BRANCH ||
            inst->inst_class == ASMKIT_INST_DIRECT_CALL;
        out_semantics->branch.has_target = out_semantics->pc_rel.is_control_flow;
        out_semantics->branch.target = pc_operand->abs_target;
        x86_set_branch_limits(&out_semantics->branch, pc_operand->width);
        out_semantics->reloc_kind = ASMKIT_RELOC_REENCODE_WITH_NEW_PC;
        out_semantics->max_emit_size = inst->inst_class == ASMKIT_INST_COND_BRANCH ? 16u : 14u;
    }
    return ASMKIT_OK;
}

static asmkit_status_t x86_emit_abs(const asmkit_engine_t* engine, int is_call, uint64_t to_address, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    (void)engine;
    asmkit_zero(out_result, sizeof(*out_result));
    out_result->size = 14u;
    if (out_capacity < 14u) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    out_code[0] = 0xffu;
    out_code[1] = is_call ? 0x15u : 0x25u;
    asmkit_store32le(out_code + 2, 0u);
    asmkit_store64le(out_code + 6, to_address);
    return ASMKIT_OK;
}

static asmkit_status_t x86_emit_any(
    const asmkit_engine_t* engine,
    int is_call,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* options,
    uint8_t* out_code,
    size_t out_capacity,
    asmkit_emit_result_t* out_result)
{
    asmkit_branch_mode_t mode;
    asmkit_status_t status;

    mode = options != 0 ? options->mode : ASMKIT_BRANCH_AUTO;
    if (mode != ASMKIT_BRANCH_ABSOLUTE_ONLY) {
        status = asmkit_gen_x86_emit_rel(engine, is_call, from_address, to_address, out_code, out_capacity, out_result);
        if (status == ASMKIT_OK || status == ASMKIT_ERR_OUTPUT_TOO_SMALL || mode == ASMKIT_BRANCH_DIRECT_ONLY) {
            return status;
        }
    }
    if (engine->config.mode != ASMKIT_MODE_X86_64) {
        return ASMKIT_ERR_BRANCH_OUT_OF_RANGE;
    }
    return x86_emit_abs(engine, is_call, to_address, out_code, out_capacity, out_result);
}

static asmkit_status_t x86_emit_jump(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    (void)workspace;
    return x86_emit_any(engine, 0, from_address, to_address, options, out_code, out_capacity, out_result);
}

static asmkit_status_t x86_emit_call(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    (void)workspace;
    return x86_emit_any(engine, 1, from_address, to_address, options, out_code, out_capacity, out_result);
}

static void x86_encode_from_emit_result(const asmkit_emit_result_t* emit_result, asmkit_encode_result_t* out_result)
{
    asmkit_zero(out_result, sizeof(*out_result));
    out_result->size = emit_result->size;
    out_result->flags = emit_result->flags;
}

static asmkit_mnemonic_id_t x86_inst_mnemonic(const asmkit_inst_t* inst)
{
    if (inst->mnemonic_id != ASMKIT_MNEMONIC_INVALID) {
        return inst->mnemonic_id;
    }
    switch (inst->id) {
    case ASMKIT_X86_NOP:
        return ASMKIT_GEN_X86_MNEMONIC_NOP_ID;
    case ASMKIT_X86_RET:
        return ASMKIT_GEN_X86_MNEMONIC_RET_ID;
    case ASMKIT_X86_JMP_REL:
    case ASMKIT_X86_JMP_INDIRECT:
        return ASMKIT_GEN_X86_MNEMONIC_JMP_ID;
    case ASMKIT_X86_CALL_REL:
    case ASMKIT_X86_CALL_INDIRECT:
        return ASMKIT_GEN_X86_MNEMONIC_CALL_ID;
    default:
        return ASMKIT_MNEMONIC_INVALID;
    }
}

static uint64_t x86_branch_target(const asmkit_engine_t* engine, const asmkit_inst_t* inst, uint32_t direct_size)
{
    const asmkit_operand_t* operand;
    (void)engine;
    operand = &inst->operands[0];
    if (operand->abs_target != 0u || operand->imm == 0) {
        return operand->abs_target;
    }
    return inst->address + (uint64_t)direct_size + (uint64_t)operand->imm;
}

static const asmkit_operand_t* x86_pc_rel_operand(const asmkit_inst_t* inst, uint8_t* out_index)
{
    uint8_t i;
    for (i = 0u; i < inst->operand_count && i < ASMKIT_MAX_OPERANDS; ++i) {
        const asmkit_operand_t* operand;
        operand = &inst->operands[i];
        if (operand->kind == ASMKIT_OP_PC_REL ||
            operand->abs_target != 0u ||
            (operand->kind == ASMKIT_OP_MEM && operand->mem.base == ASMKIT_X86_REG_RIP)) {
            if (out_index != 0) {
                *out_index = i;
            }
            return operand;
        }
    }
    return 0;
}

static asmkit_status_t x86_encode(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, const asmkit_encode_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_encode_result_t* out_result)
{
    (void)workspace;
    (void)options;
    if (engine == 0 || inst == 0 || out_code == 0 || out_result == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_result, sizeof(*out_result));
    if (inst->arch != ASMKIT_ARCH_X86 || inst->mode != engine->config.mode) {
        return ASMKIT_ERR_UNSUPPORTED_INSTRUCTION;
    }
    switch (x86_inst_mnemonic(inst)) {
    case ASMKIT_GEN_X86_MNEMONIC_NOP_ID:
        out_result->size = 1u;
        if (out_capacity < 1u) {
            return ASMKIT_ERR_OUTPUT_TOO_SMALL;
        }
        out_code[0] = 0x90u;
        return ASMKIT_OK;
    case ASMKIT_GEN_X86_MNEMONIC_RET_ID:
        out_result->size = 1u;
        if (out_capacity < 1u) {
            return ASMKIT_ERR_OUTPUT_TOO_SMALL;
        }
        out_code[0] = 0xc3u;
        return ASMKIT_OK;
    case ASMKIT_GEN_X86_MNEMONIC_JMP_ID:
    case ASMKIT_GEN_X86_MNEMONIC_CALL_ID:
        if (inst->operand_count >= 1u && inst->operands[0].kind == ASMKIT_OP_PC_REL) {
            asmkit_emit_result_t emit_result;
            uint32_t direct_size;
            uint64_t target;
            asmkit_status_t status;
            direct_size = engine->config.mode == ASMKIT_MODE_X86_16 ? 3u : 5u;
            target = x86_branch_target(engine, inst, direct_size);
            status = asmkit_gen_x86_emit_rel(
                engine,
                x86_inst_mnemonic(inst) == ASMKIT_GEN_X86_MNEMONIC_CALL_ID,
                inst->address,
                target,
                out_code,
                out_capacity,
                &emit_result);
            x86_encode_from_emit_result(&emit_result, out_result);
            return status;
        }
        break;
    default:
        break;
    }
    return asmkit_gen_x86_encode_inst(engine, inst, out_code, out_capacity, out_result);
}

static uint32_t x86_min_patch(const asmkit_engine_t* engine, uint64_t from_address, uint64_t to_address, asmkit_branch_mode_t mode, uint64_t* clobber_lo, uint64_t* clobber_hi)
{
    uint32_t direct_size;
    int64_t disp;
    if (clobber_lo != 0) {
        *clobber_lo = 0u;
    }
    if (clobber_hi != 0) {
        *clobber_hi = 0u;
    }
    direct_size = engine->config.mode == ASMKIT_MODE_X86_16 ? 3u : 5u;
    disp = (int64_t)to_address - (int64_t)(from_address + direct_size);
    if (mode != ASMKIT_BRANCH_ABSOLUTE_ONLY &&
        ((engine->config.mode == ASMKIT_MODE_X86_16 && asmkit_i64_fits_i16(disp)) ||
         (engine->config.mode != ASMKIT_MODE_X86_16 && asmkit_i64_fits_i32(disp)))) {
        return direct_size;
    }
    if (mode == ASMKIT_BRANCH_DIRECT_ONLY) {
        return 0u;
    }
    return engine->config.mode == ASMKIT_MODE_X86_64 ? 14u : 0u;
}

static asmkit_status_t x86_plan_jump_back(const asmkit_engine_t* engine, asmkit_branch_mode_t mode, asmkit_branch_plan_bound_t* out_bound)
{
    uint32_t direct_size;

    if (engine == 0 || out_bound == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_bound, sizeof(*out_bound));
    direct_size = engine->config.mode == ASMKIT_MODE_X86_16 ? 3u : 5u;
    if (mode == ASMKIT_BRANCH_DIRECT_ONLY || engine->config.mode != ASMKIT_MODE_X86_64) {
        if (mode == ASMKIT_BRANCH_ABSOLUTE_ONLY) {
            return ASMKIT_ERR_BRANCH_OUT_OF_RANGE;
        }
        out_bound->size = direct_size;
        out_bound->requires_near_island = true;
        if (engine->config.mode == ASMKIT_MODE_X86_16) {
            out_bound->island_min_distance = -32768;
            out_bound->island_max_distance = 32767;
        } else {
            out_bound->island_min_distance = (int64_t)(-2147483647 - 1);
            out_bound->island_max_distance = 2147483647;
        }
        return ASMKIT_OK;
    }
    out_bound->size = 14u;
    return ASMKIT_OK;
}

static asmkit_status_t x86_emit_jcc(uint8_t cc, uint64_t from, uint64_t to, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    int64_t disp;
    asmkit_zero(out_result, sizeof(*out_result));
    disp = (int64_t)to - (int64_t)(from + 6u);
    out_result->size = 6u;
    if (!asmkit_i64_fits_i32(disp)) {
        return ASMKIT_ERR_BRANCH_OUT_OF_RANGE;
    }
    if (out_capacity < 6u) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    out_code[0] = 0x0fu;
    out_code[1] = (uint8_t)(0x80u | (cc & 0x0fu));
    asmkit_store32le(out_code + 2, (uint32_t)disp);
    return ASMKIT_OK;
}

static asmkit_status_t x86_relocate(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, uint64_t relocated_address, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    uint32_t op_off;
    uint32_t disp_off;
    uint8_t op;
    int64_t disp;
    asmkit_status_t status;

    (void)workspace;
    if (out_result == 0 || out_code == 0 || inst == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_result, sizeof(*out_result));

    if (inst->inst_class == ASMKIT_INST_DIRECT_CALL) {
        status = asmkit_gen_x86_emit_rel(engine, 1, relocated_address, inst->operands[0].abs_target, out_code, out_capacity, out_result);
        if (status == ASMKIT_OK || status == ASMKIT_ERR_OUTPUT_TOO_SMALL || engine->config.mode != ASMKIT_MODE_X86_64) {
            return status;
        }
        return x86_emit_abs(engine, 1, inst->operands[0].abs_target, out_code, out_capacity, out_result);
    }
    if (inst->inst_class == ASMKIT_INST_DIRECT_BRANCH) {
        status = asmkit_gen_x86_emit_rel(engine, 0, relocated_address, inst->operands[0].abs_target, out_code, out_capacity, out_result);
        if (status == ASMKIT_OK || status == ASMKIT_ERR_OUTPUT_TOO_SMALL || engine->config.mode != ASMKIT_MODE_X86_64) {
            return status;
        }
        return x86_emit_abs(engine, 0, inst->operands[0].abs_target, out_code, out_capacity, out_result);
    }
    if (inst->inst_class == ASMKIT_INST_COND_BRANCH) {
        if (!x86_find_opcode(inst, &op_off)) {
            return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
        }
        op = inst->bytes[op_off];
        if (op == 0x0fu && op_off + 1u < inst->size) {
            op = inst->bytes[op_off + 1u];
        }
        if ((op >= 0x70u && op <= 0x7fu) || (op >= 0x80u && op <= 0x8fu)) {
            status = x86_emit_jcc((uint8_t)(op & 0x0fu), relocated_address, inst->operands[0].abs_target, out_code, out_capacity, out_result);
            if (status == ASMKIT_OK || status == ASMKIT_ERR_OUTPUT_TOO_SMALL || engine->config.mode != ASMKIT_MODE_X86_64) {
                return status;
            }
            out_result->size = 16u;
            if (out_capacity < 16u) {
                return ASMKIT_ERR_OUTPUT_TOO_SMALL;
            }
            out_code[0] = (uint8_t)(0x70u | ((op ^ 1u) & 0x0fu));
            out_code[1] = 14u;
            status = x86_emit_abs(engine, 0, inst->operands[0].abs_target, out_code + 2, out_capacity - 2u, out_result);
            if (status != ASMKIT_OK) {
                return status;
            }
            out_result->size = 16u;
            return ASMKIT_OK;
        }
        return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
    }
    if ((inst->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u && inst->operand_count > 0u) {
        const asmkit_operand_t* pc_operand;
        if (!x86_find_rip_disp(inst, &disp_off)) {
            return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
        }
        pc_operand = x86_pc_rel_operand(inst, 0);
        if (pc_operand == 0) {
            return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
        }
        disp = (int64_t)pc_operand->abs_target - (int64_t)(relocated_address + inst->size);
        if (!asmkit_i64_fits_i32(disp)) {
            return ASMKIT_ERR_BRANCH_OUT_OF_RANGE;
        }
        out_result->size = inst->size;
        if (out_capacity < inst->size) {
            return ASMKIT_ERR_OUTPUT_TOO_SMALL;
        }
        asmkit_copy(out_code, inst->bytes, inst->size);
        asmkit_store32le(out_code + disp_off, (uint32_t)disp);
        return ASMKIT_OK;
    }
    out_result->size = inst->size;
    if (out_capacity < inst->size) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    asmkit_copy(out_code, inst->bytes, inst->size);
    return ASMKIT_OK;
}

const asmkit_target_ops_t asmkit_x86_ops = {
    ASMKIT_ARCH_X86,
    ASMKIT_MODE_X86_16,
    ASMKIT_MODE_X86_64,
    x86_decode,
    x86_analyze,
    x86_encode,
    x86_emit_jump,
    x86_emit_call,
    x86_relocate,
    x86_min_patch,
    x86_plan_jump_back
};

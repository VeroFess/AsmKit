#include "core/asmkit_internal.h"
#include "asmkit_gen_arm_defs.h"
#include "asmkit_gen_arm_mnemonics.h"

static asmkit_status_t arm_decode(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const uint8_t* code, size_t code_size, uint64_t address, asmkit_inst_t* out_inst)
{
    (void)workspace;
    return asmkit_gen_arm_decode_one(engine, code, code_size, address, out_inst);
}

static asmkit_status_t arm_analyze(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, asmkit_inst_semantics_t* out_semantics)
{
    (void)engine;
    (void)workspace;
    asmkit_zero(out_semantics, sizeof(*out_semantics));
    out_semantics->inst_class = inst->inst_class;
    out_semantics->reloc_kind = ASMKIT_RELOC_COPY_AS_IS;
    out_semantics->min_emit_size = inst->size;
    out_semantics->max_emit_size = inst->size;
    if (inst->inst_class == ASMKIT_INST_ARM_IT) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_ARM_IT;
        out_semantics->reloc_kind = ASMKIT_RELOC_REJECT;
    }
    if (inst->inst_class == ASMKIT_INST_DIRECT_BRANCH || inst->inst_class == ASMKIT_INST_COND_BRANCH ||
        inst->inst_class == ASMKIT_INST_DIRECT_CALL || inst->inst_class == ASMKIT_INST_INDIRECT_BRANCH ||
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
    if ((inst->flags & ASMKIT_INST_FLAG_STATE_SWITCH) != 0u) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_STATE_SWITCH;
    }
    if ((inst->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u && inst->operand_count > 0u) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_PC_RELATIVE;
        out_semantics->pc_rel.is_pc_relative = true;
        out_semantics->pc_rel.original_target = inst->operands[0].abs_target;
        out_semantics->pc_rel.is_literal_load = (inst->flags & ASMKIT_INST_FLAG_LITERAL) != 0u;
        out_semantics->pc_rel.is_control_flow =
            inst->inst_class == ASMKIT_INST_DIRECT_BRANCH || inst->inst_class == ASMKIT_INST_COND_BRANCH || inst->inst_class == ASMKIT_INST_DIRECT_CALL;
        out_semantics->branch.has_target = out_semantics->pc_rel.is_control_flow;
        out_semantics->branch.target = inst->operands[0].abs_target;
        out_semantics->reloc_kind = ASMKIT_RELOC_REENCODE_WITH_NEW_PC;
        out_semantics->max_emit_size = 12u;
    }
    return ASMKIT_OK;
}

static asmkit_status_t arm_emit_abs_a32(int is_call, uint64_t to_address, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    asmkit_zero(out_result, sizeof(*out_result));
    if (!is_call) {
        out_result->size = 8u;
        if (out_capacity < 8u) {
            return ASMKIT_ERR_OUTPUT_TOO_SMALL;
        }
        asmkit_store32le(out_code, 0xe51ff004u);
        asmkit_store32le(out_code + 4, (uint32_t)to_address);
        return ASMKIT_OK;
    }
    out_result->size = 12u;
    out_result->clobber_mask_lo = (uint64_t)1u << ASMKIT_REG_ARM_IP;
    if (out_capacity < 12u) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    asmkit_store32le(out_code, 0xe59fc004u);
    asmkit_store32le(out_code + 4, 0xe12fff3cu);
    asmkit_store32le(out_code + 8, (uint32_t)to_address);
    return ASMKIT_OK;
}

static asmkit_status_t arm_emit_any(const asmkit_engine_t* engine, int is_call, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    asmkit_branch_mode_t mode;
    asmkit_status_t status;

    mode = options != 0 ? options->mode : ASMKIT_BRANCH_AUTO;
    if (mode != ASMKIT_BRANCH_ABSOLUTE_ONLY) {
        status = asmkit_gen_arm_emit_rel(engine, is_call, from_address, to_address, out_code, out_capacity, out_result);
        if (status == ASMKIT_OK || status == ASMKIT_ERR_OUTPUT_TOO_SMALL || mode == ASMKIT_BRANCH_DIRECT_ONLY) {
            return status;
        }
    }
    if (engine->config.mode == ASMKIT_MODE_ARM_A32) {
        return arm_emit_abs_a32(is_call, to_address, out_code, out_capacity, out_result);
    }
    return ASMKIT_ERR_BRANCH_OUT_OF_RANGE;
}

static asmkit_status_t arm_emit_jump(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    (void)workspace;
    return arm_emit_any(engine, 0, from_address, to_address, options, out_code, out_capacity, out_result);
}

static asmkit_status_t arm_emit_call(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    (void)workspace;
    return arm_emit_any(engine, 1, from_address, to_address, options, out_code, out_capacity, out_result);
}

static asmkit_mnemonic_id_t arm_inst_mnemonic(const asmkit_inst_t* inst)
{
    if (inst->mnemonic_id != ASMKIT_MNEMONIC_INVALID) {
        return inst->mnemonic_id;
    }
    switch (inst->id) {
    case ASMKIT_ARM_B:
    case ASMKIT_THUMB_B:
    case ASMKIT_THUMB_B_COND:
        return ASMKIT_GEN_ARM_MNEMONIC_B_ID;
    case ASMKIT_ARM_BL:
    case ASMKIT_THUMB_BL:
        return ASMKIT_GEN_ARM_MNEMONIC_BL_ID;
    default:
        return ASMKIT_MNEMONIC_INVALID;
    }
}

static uint64_t arm_branch_target(const asmkit_inst_t* inst, uint32_t direct_size)
{
    const asmkit_operand_t* operand;
    operand = &inst->operands[0];
    if (operand->abs_target != 0u || operand->imm == 0) {
        return operand->abs_target;
    }
    return inst->address + (uint64_t)direct_size + (uint64_t)operand->imm;
}

static asmkit_status_t arm_encode(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, const asmkit_encode_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_encode_result_t* out_result)
{
    (void)workspace;
    (void)options;
    if (engine == 0 || inst == 0 || out_code == 0 || out_result == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_result, sizeof(*out_result));
    if (inst->arch != ASMKIT_ARCH_ARM || inst->mode != engine->config.mode) {
        return ASMKIT_ERR_UNSUPPORTED_INSTRUCTION;
    }
    switch (arm_inst_mnemonic(inst)) {
    case ASMKIT_GEN_ARM_MNEMONIC_B_ID:
    case ASMKIT_GEN_ARM_MNEMONIC_BL_ID:
        if (inst->operand_count >= 1u && inst->operands[0].kind == ASMKIT_OP_PC_REL) {
            asmkit_emit_result_t emit_result;
            asmkit_status_t status;
            uint32_t direct_size;
            direct_size = engine->config.mode == ASMKIT_MODE_ARM_A32 ? 4u : 2u;
            status = asmkit_gen_arm_emit_rel(
                engine,
                arm_inst_mnemonic(inst) == ASMKIT_GEN_ARM_MNEMONIC_BL_ID,
                inst->address,
                arm_branch_target(inst, direct_size),
                out_code,
                out_capacity,
                &emit_result);
            asmkit_zero(out_result, sizeof(*out_result));
            out_result->size = emit_result.size;
            out_result->flags = emit_result.flags;
            return status;
        }
        break;
    default:
        break;
    }
    return asmkit_gen_arm_encode_inst(engine, inst, out_code, out_capacity, out_result);
}

static uint32_t arm_min_patch(const asmkit_engine_t* engine, uint64_t from_address, uint64_t to_address, asmkit_branch_mode_t mode, uint64_t* clobber_lo, uint64_t* clobber_hi)
{
    int64_t disp;
    if (clobber_lo != 0) {
        *clobber_lo = 0u;
    }
    if (clobber_hi != 0) {
        *clobber_hi = 0u;
    }
    if (engine->config.mode == ASMKIT_MODE_ARM_A32) {
        disp = (int64_t)to_address - (int64_t)(from_address + 8u);
        if (mode != ASMKIT_BRANCH_ABSOLUTE_ONLY && asmkit_i64_fits_i24_words4(disp)) {
            return 4u;
        }
        return mode == ASMKIT_BRANCH_DIRECT_ONLY ? 0u : 8u;
    }
    disp = (int64_t)to_address - (int64_t)(from_address + 4u);
    if (mode != ASMKIT_BRANCH_ABSOLUTE_ONLY && (disp & 1) == 0 && disp >= -2048 && disp <= 2046) {
        return 2u;
    }
    return 0u;
}

static asmkit_status_t arm_plan_jump_back(const asmkit_engine_t* engine, asmkit_branch_mode_t mode, asmkit_branch_plan_bound_t* out_bound)
{
    if (engine == 0 || out_bound == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_bound, sizeof(*out_bound));
    if (engine->config.mode == ASMKIT_MODE_ARM_A32) {
        if (mode == ASMKIT_BRANCH_DIRECT_ONLY) {
            out_bound->size = 4u;
            out_bound->requires_near_island = true;
            out_bound->island_min_distance = -33554432;
            out_bound->island_max_distance = 33554428;
            return ASMKIT_OK;
        }
        out_bound->size = 8u;
        return ASMKIT_OK;
    }
    if (mode == ASMKIT_BRANCH_ABSOLUTE_ONLY) {
        return ASMKIT_ERR_BRANCH_OUT_OF_RANGE;
    }
    out_bound->size = 2u;
    out_bound->requires_near_island = true;
    out_bound->island_min_distance = -2048;
    out_bound->island_max_distance = 2046;
    return ASMKIT_OK;
}

static asmkit_status_t arm_relocate(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, uint64_t relocated_address, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
{
    (void)workspace;
    if (inst == 0 || out_code == 0 || out_result == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_result, sizeof(*out_result));
    if (inst->inst_class == ASMKIT_INST_ARM_IT) {
        return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
    }
    if (inst->inst_class == ASMKIT_INST_DIRECT_BRANCH || inst->inst_class == ASMKIT_INST_DIRECT_CALL) {
        asmkit_status_t status;
        int is_call;
        is_call = inst->inst_class == ASMKIT_INST_DIRECT_CALL;
        status = asmkit_gen_arm_emit_rel(engine, is_call, relocated_address, inst->operands[0].abs_target, out_code, out_capacity, out_result);
        if (status == ASMKIT_OK || status == ASMKIT_ERR_OUTPUT_TOO_SMALL || engine->config.mode != ASMKIT_MODE_ARM_A32) {
            return status;
        }
        return arm_emit_abs_a32(is_call, inst->operands[0].abs_target, out_code, out_capacity, out_result);
    }
    if ((inst->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u) {
        return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
    }
    out_result->size = inst->size;
    if (out_capacity < inst->size) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    asmkit_copy(out_code, inst->bytes, inst->size);
    return ASMKIT_OK;
}

const asmkit_target_ops_t asmkit_arm_ops = {
    ASMKIT_ARCH_ARM,
    ASMKIT_MODE_ARM_A32,
    ASMKIT_MODE_ARM_THUMB,
    arm_decode,
    arm_analyze,
    arm_encode,
    arm_emit_jump,
    arm_emit_call,
    arm_relocate,
    arm_min_patch,
    arm_plan_jump_back
};

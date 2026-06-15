#include "core/asmkit_internal.h"

static asmkit_status_t bpf_decode(const asmkit_engine_t* ASMKIT_RESTRICT engine, asmkit_workspace_t* ASMKIT_RESTRICT workspace, const uint8_t* ASMKIT_RESTRICT code, size_t code_size, uint64_t address, asmkit_inst_t* ASMKIT_RESTRICT out_inst)
{
    (void)workspace;
    return asmkit_gen_bpf_decode_one(engine, code, code_size, address, out_inst);
}

static asmkit_status_t bpf_analyze(const asmkit_engine_t* ASMKIT_RESTRICT engine, asmkit_workspace_t* ASMKIT_RESTRICT workspace, const asmkit_inst_t* ASMKIT_RESTRICT inst, asmkit_inst_semantics_t* ASMKIT_RESTRICT out_semantics)
{
    (void)engine;
    (void)workspace;
    asmkit_zero(out_semantics, sizeof(*out_semantics));
    out_semantics->inst_class = inst->inst_class;
    out_semantics->reloc_kind = ASMKIT_RELOC_COPY_AS_IS;
    out_semantics->min_emit_size = inst->size;
    out_semantics->max_emit_size = inst->size;
    if (inst->inst_class == ASMKIT_INST_BPF_PSEUDO) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_UNSUPPORTED_RELOC;
        out_semantics->reloc_kind = ASMKIT_RELOC_REJECT;
    }
    if (inst->inst_class == ASMKIT_INST_DIRECT_BRANCH || inst->inst_class == ASMKIT_INST_COND_BRANCH) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_BRANCH | ASMKIT_HAZARD_PC_RELATIVE;
        out_semantics->branch.is_branch = true;
        out_semantics->branch.is_conditional = inst->inst_class == ASMKIT_INST_COND_BRANCH;
        out_semantics->branch.is_direct = true;
        out_semantics->branch.has_target = true;
        out_semantics->branch.target = inst->operands[0].abs_target;
        out_semantics->pc_rel.is_pc_relative = true;
        out_semantics->pc_rel.is_control_flow = true;
        out_semantics->pc_rel.original_target = inst->operands[0].abs_target;
        out_semantics->reloc_kind = ASMKIT_RELOC_REENCODE_WITH_NEW_PC;
    }
    if (inst->inst_class == ASMKIT_INST_DIRECT_CALL) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_CALL;
        out_semantics->branch.is_call = true;
        out_semantics->branch.is_direct = true;
    }
    if (inst->inst_class == ASMKIT_INST_RETURN) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_RETURN;
        out_semantics->branch.is_return = true;
    }
    return ASMKIT_OK;
}

static asmkit_status_t bpf_unsupported_emit(const asmkit_engine_t* ASMKIT_RESTRICT engine, asmkit_workspace_t* ASMKIT_RESTRICT workspace, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* ASMKIT_RESTRICT options, uint8_t* ASMKIT_RESTRICT out_code, size_t out_capacity, asmkit_emit_result_t* ASMKIT_RESTRICT out_result)
{
    (void)engine;
    (void)workspace;
    (void)from_address;
    (void)to_address;
    (void)options;
    (void)out_code;
    (void)out_capacity;
    if (out_result != 0) {
        asmkit_zero(out_result, sizeof(*out_result));
    }
    return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
}

static asmkit_status_t bpf_encode(const asmkit_engine_t* ASMKIT_RESTRICT engine, asmkit_workspace_t* ASMKIT_RESTRICT workspace, const asmkit_inst_t* ASMKIT_RESTRICT inst, const asmkit_encode_options_t* ASMKIT_RESTRICT options, uint8_t* ASMKIT_RESTRICT out_code, size_t out_capacity, asmkit_encode_result_t* ASMKIT_RESTRICT out_result)
{
    (void)workspace;
    (void)options;
    return asmkit_gen_bpf_encode_inst(engine, inst, out_code, out_capacity, out_result);
}

static asmkit_status_t bpf_relocate(const asmkit_engine_t* ASMKIT_RESTRICT engine, asmkit_workspace_t* ASMKIT_RESTRICT workspace, const asmkit_inst_t* ASMKIT_RESTRICT inst, uint64_t relocated_address, uint8_t* ASMKIT_RESTRICT out_code, size_t out_capacity, asmkit_emit_result_t* ASMKIT_RESTRICT out_result)
{
    (void)engine;
    (void)workspace;
    (void)inst;
    (void)relocated_address;
    (void)out_code;
    (void)out_capacity;
    if (out_result != 0) {
        asmkit_zero(out_result, sizeof(*out_result));
    }
    return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
}

static uint32_t bpf_min_patch(const asmkit_engine_t* ASMKIT_RESTRICT engine, uint64_t from_address, uint64_t to_address, asmkit_branch_mode_t mode, uint64_t* ASMKIT_RESTRICT clobber_lo, uint64_t* ASMKIT_RESTRICT clobber_hi)
{
    (void)engine;
    (void)from_address;
    (void)to_address;
    (void)mode;
    if (clobber_lo != 0) {
        *clobber_lo = 0u;
    }
    if (clobber_hi != 0) {
        *clobber_hi = 0u;
    }
    return 0u;
}

static asmkit_status_t bpf_plan_jump_back(const asmkit_engine_t* ASMKIT_RESTRICT engine, asmkit_branch_mode_t mode, asmkit_branch_plan_bound_t* ASMKIT_RESTRICT out_bound)
{
    (void)engine;
    (void)mode;
    if (out_bound != 0) {
        asmkit_zero(out_bound, sizeof(*out_bound));
    }
    return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
}

const asmkit_target_ops_t asmkit_bpf_ops = {
    ASMKIT_ARCH_BPF,
    ASMKIT_MODE_BPF64,
    ASMKIT_MODE_BPF64,
    bpf_decode,
    bpf_analyze,
    bpf_encode,
    bpf_unsupported_emit,
    bpf_unsupported_emit,
    bpf_relocate,
    bpf_min_patch,
    bpf_plan_jump_back
};

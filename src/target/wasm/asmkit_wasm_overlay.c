#include "core/asmkit_internal.h"

static asmkit_status_t wasm_decode(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const uint8_t* code, size_t code_size, uint64_t address, asmkit_inst_t* out_inst)
{
    (void)workspace;
    return asmkit_gen_wasm_decode_one(engine, code, code_size, address, out_inst);
}

static asmkit_status_t wasm_analyze(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, asmkit_inst_semantics_t* out_semantics)
{
    (void)engine;
    (void)workspace;
    asmkit_zero(out_semantics, sizeof(*out_semantics));
    out_semantics->inst_class = inst->inst_class;
    out_semantics->reloc_kind = ASMKIT_RELOC_COPY_AS_IS;
    out_semantics->min_emit_size = inst->size;
    out_semantics->max_emit_size = inst->size;
    if (inst->inst_class == ASMKIT_INST_WASM_BLOCK || inst->inst_class == ASMKIT_INST_WASM_LOOP ||
        inst->inst_class == ASMKIT_INST_WASM_IF || inst->inst_class == ASMKIT_INST_WASM_ELSE ||
        inst->inst_class == ASMKIT_INST_WASM_END) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_STRUCTURED_CF;
    }
    if (inst->inst_class == ASMKIT_INST_DIRECT_BRANCH || inst->inst_class == ASMKIT_INST_COND_BRANCH ||
        inst->inst_class == ASMKIT_INST_INDIRECT_BRANCH) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_BRANCH | ASMKIT_HAZARD_STRUCTURED_CF;
        out_semantics->branch.is_branch = true;
        out_semantics->branch.is_conditional = inst->inst_class == ASMKIT_INST_COND_BRANCH;
        out_semantics->branch.is_direct = inst->inst_class != ASMKIT_INST_INDIRECT_BRANCH;
        out_semantics->branch.is_indirect = inst->inst_class == ASMKIT_INST_INDIRECT_BRANCH;
    }
    if (inst->inst_class == ASMKIT_INST_DIRECT_CALL || inst->inst_class == ASMKIT_INST_INDIRECT_CALL) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_CALL;
        out_semantics->branch.is_call = true;
        out_semantics->branch.is_direct = inst->inst_class == ASMKIT_INST_DIRECT_CALL;
        out_semantics->branch.is_indirect = inst->inst_class == ASMKIT_INST_INDIRECT_CALL;
    }
    if (inst->inst_class == ASMKIT_INST_RETURN) {
        out_semantics->hazard_flags |= ASMKIT_HAZARD_RETURN | ASMKIT_HAZARD_STRUCTURED_CF;
        out_semantics->branch.is_return = true;
    }
    return ASMKIT_OK;
}

static asmkit_status_t wasm_unsupported_emit(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, uint64_t from_address, uint64_t to_address, const asmkit_emit_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
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

static asmkit_status_t wasm_encode(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, const asmkit_encode_options_t* options, uint8_t* out_code, size_t out_capacity, asmkit_encode_result_t* out_result)
{
    (void)workspace;
    (void)options;
    return asmkit_gen_wasm_encode_inst(engine, inst, out_code, out_capacity, out_result);
}

static asmkit_status_t wasm_relocate(const asmkit_engine_t* engine, asmkit_workspace_t* workspace, const asmkit_inst_t* inst, uint64_t relocated_address, uint8_t* out_code, size_t out_capacity, asmkit_emit_result_t* out_result)
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

static uint32_t wasm_min_patch(const asmkit_engine_t* engine, uint64_t from_address, uint64_t to_address, asmkit_branch_mode_t mode, uint64_t* clobber_lo, uint64_t* clobber_hi)
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

static asmkit_status_t wasm_plan_jump_back(const asmkit_engine_t* engine, asmkit_branch_mode_t mode, asmkit_branch_plan_bound_t* out_bound)
{
    (void)engine;
    (void)mode;
    if (out_bound != 0) {
        asmkit_zero(out_bound, sizeof(*out_bound));
    }
    return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
}

const asmkit_target_ops_t asmkit_wasm_ops = {
    ASMKIT_ARCH_WASM,
    ASMKIT_MODE_WASM32,
    ASMKIT_MODE_WASM64,
    wasm_decode,
    wasm_analyze,
    wasm_encode,
    wasm_unsupported_emit,
    wasm_unsupported_emit,
    wasm_relocate,
    wasm_min_patch,
    wasm_plan_jump_back
};

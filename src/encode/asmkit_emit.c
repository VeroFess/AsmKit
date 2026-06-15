#include "core/asmkit_internal.h"

static int asmkit_emit_result_clobbers_allowed(
    const asmkit_emit_options_t* ASMKIT_RESTRICT options,
    const asmkit_emit_result_t* ASMKIT_RESTRICT result)
{
    if (result == 0 || (result->clobber_mask_lo == 0u && result->clobber_mask_hi == 0u)) {
        return 1;
    }
    if (options == 0) {
        return 1;
    }
    if (options->mode == ASMKIT_BRANCH_PREFER_CLOBBERLESS) {
        return 0;
    }
    return (result->clobber_mask_lo & ~options->allowed_clobber_mask_lo) == 0u &&
        (result->clobber_mask_hi & ~options->allowed_clobber_mask_hi) == 0u;
}

static asmkit_status_t asmkit_emit_finish(
    asmkit_status_t status,
    const asmkit_emit_options_t* ASMKIT_RESTRICT options,
    const asmkit_emit_result_t* ASMKIT_RESTRICT result)
{
    if (status != ASMKIT_OK) {
        return status;
    }
    return asmkit_emit_result_clobbers_allowed(options, result) ? ASMKIT_OK : ASMKIT_ERR_FORBIDDEN_CLOBBER;
}

asmkit_status_t asmkit_encode_inst(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    asmkit_workspace_t* ASMKIT_RESTRICT workspace,
    const asmkit_inst_t* ASMKIT_RESTRICT inst,
    const asmkit_encode_options_t* ASMKIT_RESTRICT options,
    uint8_t* ASMKIT_RESTRICT out_code,
    size_t out_capacity,
    asmkit_encode_result_t* ASMKIT_RESTRICT out_result)
{
    const asmkit_target_ops_t* ops;

    if (engine == 0 || inst == 0 || out_code == 0 || out_result == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    ops = asmkit_engine_ops(engine);
    if (ops == 0 || ops->encode_inst == 0) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    return ops->encode_inst(engine, workspace, inst, options, out_code, out_capacity, out_result);
}

asmkit_status_t asmkit_emit_jump(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    asmkit_workspace_t* ASMKIT_RESTRICT workspace,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* ASMKIT_RESTRICT options,
    uint8_t* ASMKIT_RESTRICT out_code,
    size_t out_capacity,
    asmkit_emit_result_t* ASMKIT_RESTRICT out_result)
{
    const asmkit_target_ops_t* ops;

    if (engine == 0 || out_code == 0 || out_result == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    ops = asmkit_engine_ops(engine);
    if (ops == 0 || ops->emit_jump == 0) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    return asmkit_emit_finish(
        ops->emit_jump(engine, workspace, from_address, to_address, options, out_code, out_capacity, out_result),
        options,
        out_result);
}

asmkit_status_t asmkit_emit_call(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    asmkit_workspace_t* ASMKIT_RESTRICT workspace,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* ASMKIT_RESTRICT options,
    uint8_t* ASMKIT_RESTRICT out_code,
    size_t out_capacity,
    asmkit_emit_result_t* ASMKIT_RESTRICT out_result)
{
    const asmkit_target_ops_t* ops;

    if (engine == 0 || out_code == 0 || out_result == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    ops = asmkit_engine_ops(engine);
    if (ops == 0 || ops->emit_call == 0) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    return asmkit_emit_finish(
        ops->emit_call(engine, workspace, from_address, to_address, options, out_code, out_capacity, out_result),
        options,
        out_result);
}

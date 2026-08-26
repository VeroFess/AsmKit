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

asmkit_status_t asmkit_plan_control_transfer(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    asmkit_workspace_t* ASMKIT_RESTRICT workspace,
    asmkit_control_transfer_kind_t kind,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* ASMKIT_RESTRICT options,
    asmkit_control_transfer_plan_t* ASMKIT_RESTRICT out_plan)
{
    const asmkit_target_ops_t* ops;
    asmkit_branch_plan_bound_t bound;
    asmkit_emit_result_t emit;
    uint8_t code[32];
    asmkit_status_t status;

    if (engine == 0 || out_plan == 0 ||
        (kind != ASMKIT_CONTROL_TRANSFER_JUMP && kind != ASMKIT_CONTROL_TRANSFER_CALL)) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_plan, sizeof(*out_plan));
    out_plan->kind = kind;
    status = kind == ASMKIT_CONTROL_TRANSFER_CALL ?
        asmkit_emit_call(engine, workspace, from_address, to_address, options, code, sizeof(code), &emit) :
        asmkit_emit_jump(engine, workspace, from_address, to_address, options, code, sizeof(code), &emit);
    if (status == ASMKIT_OK) {
        out_plan->size = emit.size;
        out_plan->required_alignment = emit.required_alignment;
        out_plan->clobber_mask_lo = emit.clobber_mask_lo;
        out_plan->clobber_mask_hi = emit.clobber_mask_hi;
        return ASMKIT_OK;
    }
    if (status != ASMKIT_ERR_BRANCH_OUT_OF_RANGE &&
        status != ASMKIT_ERR_REQUIRES_BRANCH_ISLAND &&
        status != ASMKIT_ERR_FORBIDDEN_CLOBBER) {
        return status;
    }

    ops = asmkit_engine_ops(engine);
    if (ops == 0 || ops->plan_jump_back == 0) {
        return status;
    }
    asmkit_zero(&bound, sizeof(bound));
    status = ops->plan_jump_back(engine, options != 0 ? options->mode : ASMKIT_BRANCH_AUTO, &bound);
    if (status != ASMKIT_OK) {
        return status;
    }
    out_plan->size = bound.size;
    out_plan->requires_island = true;
    out_plan->island_min_distance = bound.island_min_distance;
    out_plan->island_max_distance = bound.island_max_distance;
    out_plan->clobber_mask_lo = bound.clobber_mask_lo;
    out_plan->clobber_mask_hi = bound.clobber_mask_hi;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_emit_padding(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    uint32_t size,
    uint8_t* ASMKIT_RESTRICT out_code,
    size_t out_capacity,
    asmkit_encode_result_t* ASMKIT_RESTRICT out_result)
{
    uint32_t i;
    uint32_t unit;
    uint32_t word;

    if (engine == 0 || out_code == 0 || out_result == 0 || size == 0u) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_result, sizeof(*out_result));
    out_result->size = size;
    if (out_capacity < size) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }

    switch (engine->config.mode) {
    case ASMKIT_MODE_X86_16:
    case ASMKIT_MODE_X86_32:
    case ASMKIT_MODE_X86_64:
        for (i = 0u; i < size; ++i) {
            out_code[i] = 0x90u;
        }
        return ASMKIT_OK;
    case ASMKIT_MODE_ARM_THUMB:
        unit = 2u;
        word = 0xbf00u;
        break;
    case ASMKIT_MODE_ARM_A32:
        unit = 4u;
        word = 0xe320f000u;
        break;
    case ASMKIT_MODE_AARCH64:
        unit = 4u;
        word = 0xd503201fu;
        break;
    default:
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    if ((size % unit) != 0u) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    for (i = 0u; i < size; i += unit) {
        if (unit == 2u) {
            asmkit_store16le(out_code + i, (uint16_t)word);
        } else {
            asmkit_store32le(out_code + i, word);
        }
    }
    return ASMKIT_OK;
}

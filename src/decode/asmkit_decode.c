#include "core/asmkit_internal.h"

asmkit_status_t asmkit_decode_one(
    const asmkit_engine_t* engine,
    asmkit_workspace_t* workspace,
    const uint8_t* code,
    size_t code_size,
    uint64_t address,
    asmkit_inst_t* out_inst)
{
    const asmkit_target_ops_t* ops;

    if (engine == 0 || code == 0 || out_inst == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    ops = asmkit_engine_ops(engine);
    if (ops == 0 || ops->decode_one == 0) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    return ops->decode_one(engine, workspace, code, code_size, address, out_inst);
}

asmkit_status_t asmkit_decode_block_until(
    const asmkit_engine_t* engine,
    asmkit_workspace_t* workspace,
    const uint8_t* code,
    size_t code_size,
    uint64_t address,
    uint32_t min_size,
    asmkit_inst_t* out_insts,
    uint32_t out_inst_capacity,
    asmkit_decode_block_result_t* out_result)
{
    uint32_t count;
    uint32_t total;
    asmkit_status_t status;

    if (engine == 0 || code == 0 || out_insts == 0 || out_result == 0 || out_inst_capacity == 0u) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(out_result, sizeof(*out_result));

    count = 0u;
    total = 0u;
    while (total < min_size && count < out_inst_capacity && count < ASMKIT_MAX_BLOCK_INSTRUCTIONS) {
        status = asmkit_decode_one(engine, workspace, code + total, code_size - total, address + total, &out_insts[count]);
        if (status != ASMKIT_OK) {
            out_result->inst_count = count;
            out_result->total_size = total;
            return status;
        }
        if (out_insts[count].size == 0u || out_insts[count].size > code_size - total) {
            out_result->inst_count = count;
            out_result->total_size = total;
            return ASMKIT_ERR_DECODE_FAILED;
        }
        total += out_insts[count].size;
        ++count;
        if ((out_insts[count - 1u].flags & ASMKIT_INST_FLAG_TERMINATOR) != 0u && total < min_size) {
            break;
        }
    }

    out_result->inst_count = count;
    out_result->total_size = total;
    if (total < min_size) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    return ASMKIT_OK;
}

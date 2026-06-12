#include "core/asmkit_internal.h"

static asmkit_branch_mode_t asmkit_plan_branch_mode(const asmkit_detour_plan_options_t* options)
{
    if (options == 0) {
        return ASMKIT_BRANCH_AUTO;
    }
    return options->branch_mode;
}

asmkit_status_t asmkit_plan_detour(
    const asmkit_engine_t* engine,
    asmkit_workspace_t* workspace,
    const uint8_t* target_code,
    size_t target_code_size,
    uint64_t target_address,
    uint64_t replacement_address,
    const asmkit_detour_plan_options_t* options,
    asmkit_detour_plan_t* out_plan)
{
    const asmkit_target_ops_t* ops;
    asmkit_inst_t insts[ASMKIT_MAX_BLOCK_INSTRUCTIONS];
    asmkit_decode_block_result_t block;
    asmkit_inst_semantics_t sem;
    asmkit_branch_plan_bound_t jump_back;
    uint64_t clob_lo;
    uint64_t clob_hi;
    uint32_t patch_size;
    uint32_t min_overwrite;
    uint32_t reloc_max_size;
    uint32_t i;
    asmkit_status_t status;

    if (engine == 0 || target_code == 0 || out_plan == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    ops = asmkit_engine_ops(engine);
    if (ops == 0 || ops->min_patch_size == 0) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (engine->config.arch == ASMKIT_ARCH_BPF || engine->config.arch == ASMKIT_ARCH_WASM) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }

    asmkit_zero(out_plan, sizeof(*out_plan));
    clob_lo = 0u;
    clob_hi = 0u;
    patch_size = ops->min_patch_size(engine, target_address, replacement_address, asmkit_plan_branch_mode(options), &clob_lo, &clob_hi);
    if (patch_size == 0u) {
        return ASMKIT_ERR_BRANCH_OUT_OF_RANGE;
    }
    if (ops->plan_jump_back == 0) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    status = ops->plan_jump_back(engine, asmkit_plan_branch_mode(options), &jump_back);
    if (status != ASMKIT_OK) {
        return status;
    }
    min_overwrite = patch_size;
    if (options != 0 && options->min_patch_size_override > min_overwrite) {
        min_overwrite = options->min_patch_size_override;
    }

    status = asmkit_decode_block_until(engine, workspace, target_code, target_code_size, target_address, min_overwrite, insts, ASMKIT_ARRAY_COUNT(insts), &block);
    if (status != ASMKIT_OK) {
        return status;
    }

    out_plan->patch_size = patch_size;
    out_plan->min_overwrite_size = block.total_size;
    out_plan->entry_stub_size = patch_size;
    out_plan->island_stub_size = patch_size;
    out_plan->requires_near_island = jump_back.requires_near_island;
    out_plan->island_min_distance = jump_back.island_min_distance;
    out_plan->island_max_distance = jump_back.island_max_distance;
    out_plan->clobber_mask_lo = clob_lo | jump_back.clobber_mask_lo;
    out_plan->clobber_mask_hi = clob_hi | jump_back.clobber_mask_hi;

    reloc_max_size = 0u;
    for (i = 0u; i < block.inst_count; ++i) {
        status = asmkit_analyze_inst(engine, workspace, &insts[i], &sem);
        if (status != ASMKIT_OK) {
            return status;
        }
        reloc_max_size += sem.max_emit_size != 0u ? sem.max_emit_size : insts[i].size;
        out_plan->hazard_flags |= sem.hazard_flags;
        out_plan->clobber_mask_lo |= sem.clobber_mask_lo;
        out_plan->clobber_mask_hi |= sem.clobber_mask_hi;
        if (sem.reloc_kind == ASMKIT_RELOC_REJECT) {
            return ASMKIT_ERR_UNSUPPORTED_RELOCATION;
        }
    }
    out_plan->trampoline_min_size = reloc_max_size + jump_back.size;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_relocate_prologue(
    const asmkit_engine_t* engine,
    asmkit_workspace_t* workspace,
    const uint8_t* original_code,
    size_t original_code_size,
    uint64_t original_address,
    uint64_t relocated_address,
    const asmkit_relocate_options_t* options,
    uint8_t* out_code,
    size_t out_capacity,
    asmkit_relocate_result_t* out_result)
{
    const asmkit_target_ops_t* ops;
    asmkit_inst_t insts[ASMKIT_MAX_BLOCK_INSTRUCTIONS];
    asmkit_decode_block_result_t block;
    asmkit_inst_semantics_t sem;
    asmkit_emit_result_t emit;
    asmkit_emit_options_t jump_opts;
    uint32_t min_overwrite;
    uint32_t offset;
    uint32_t i;
    asmkit_status_t status;

    if (engine == 0 || original_code == 0 || options == 0 || out_code == 0 || out_result == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    if (engine->config.arch == ASMKIT_ARCH_BPF || engine->config.arch == ASMKIT_ARCH_WASM) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    ops = asmkit_engine_ops(engine);
    if (ops == 0 || ops->relocate_inst == 0) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }

    asmkit_zero(out_result, sizeof(*out_result));
    min_overwrite = options->min_overwrite_size;
    if (min_overwrite == 0u) {
        min_overwrite = 1u;
    }

    status = asmkit_decode_block_until(engine, workspace, original_code, original_code_size, original_address, min_overwrite, insts, ASMKIT_ARRAY_COUNT(insts), &block);
    if (status != ASMKIT_OK) {
        return status;
    }

    offset = 0u;
    for (i = 0u; i < block.inst_count; ++i) {
        status = asmkit_analyze_inst(engine, workspace, &insts[i], &sem);
        if (status != ASMKIT_OK) {
            return status;
        }
        out_result->hazard_flags |= sem.hazard_flags;
        status = ops->relocate_inst(engine, workspace, &insts[i], relocated_address + offset, out_code + offset, out_capacity - offset, &emit);
        if (status != ASMKIT_OK) {
            return status;
        }
        if ((emit.clobber_mask_lo & ~options->allowed_clobber_mask_lo) != 0u ||
            (emit.clobber_mask_hi & ~options->allowed_clobber_mask_hi) != 0u) {
            out_result->clobber_mask_lo |= emit.clobber_mask_lo;
            out_result->clobber_mask_hi |= emit.clobber_mask_hi;
            return ASMKIT_ERR_FORBIDDEN_CLOBBER;
        }
        offset += emit.size;
        out_result->clobber_mask_lo |= emit.clobber_mask_lo;
        out_result->clobber_mask_hi |= emit.clobber_mask_hi;
    }

    jump_opts.mode = options->jump_back_mode;
    jump_opts.allowed_clobber_mask_lo = options->allowed_clobber_mask_lo;
    jump_opts.allowed_clobber_mask_hi = options->allowed_clobber_mask_hi;
    jump_opts.flags = 0u;

    out_result->jump_back_offset = offset;
    status = asmkit_emit_jump(engine, workspace, relocated_address + offset, options->continuation_address, &jump_opts, out_code + offset, out_capacity - offset, &emit);
    if (status != ASMKIT_OK) {
        if (status == ASMKIT_ERR_FORBIDDEN_CLOBBER) {
            out_result->clobber_mask_lo |= emit.clobber_mask_lo;
            out_result->clobber_mask_hi |= emit.clobber_mask_hi;
        }
        return status;
    }

    out_result->overwritten_size = block.total_size;
    out_result->original_address = original_address;
    out_result->continuation_address = options->continuation_address;
    out_result->relocated_address = relocated_address;
    out_result->jump_back_size = emit.size;
    out_result->relocated_size = offset + emit.size;
    out_result->clobber_mask_lo |= emit.clobber_mask_lo;
    out_result->clobber_mask_hi |= emit.clobber_mask_hi;
    return ASMKIT_OK;
}

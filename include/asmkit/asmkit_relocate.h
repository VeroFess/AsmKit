#ifndef ASMKIT_RELOCATE_H
#define ASMKIT_RELOCATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "asmkit/asmkit_encode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asmkit_detour_plan_options {
    asmkit_branch_mode_t branch_mode;
    uint32_t min_patch_size_override;
    uint32_t flags;
} asmkit_detour_plan_options_t;

typedef struct asmkit_detour_plan {
    uint32_t patch_size;
    uint32_t min_overwrite_size;
    uint32_t entry_stub_size;
    uint32_t island_stub_size;
    uint32_t trampoline_min_size;
    bool requires_near_island;
    int64_t island_min_distance;
    int64_t island_max_distance;
    uint64_t clobber_mask_lo;
    uint64_t clobber_mask_hi;
    uint64_t hazard_flags;
} asmkit_detour_plan_t;

typedef struct asmkit_relocate_options {
    uint32_t min_overwrite_size;
    uint64_t continuation_address;
    asmkit_branch_mode_t jump_back_mode;
    uint64_t allowed_clobber_mask_lo;
    uint64_t allowed_clobber_mask_hi;
    uint32_t flags;
} asmkit_relocate_options_t;

typedef struct asmkit_relocate_result {
    uint32_t overwritten_size;
    uint64_t original_address;
    uint64_t continuation_address;
    uint64_t relocated_address;
    uint32_t relocated_size;
    uint32_t jump_back_offset;
    uint32_t jump_back_size;
    uint64_t clobber_mask_lo;
    uint64_t clobber_mask_hi;
    uint64_t hazard_flags;
} asmkit_relocate_result_t;

struct asmkit_engine;
struct asmkit_workspace;

asmkit_status_t asmkit_plan_detour(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const uint8_t* target_code,
    size_t target_code_size,
    uint64_t target_address,
    uint64_t replacement_address,
    const asmkit_detour_plan_options_t* options,
    asmkit_detour_plan_t* out_plan);

asmkit_status_t asmkit_relocate_prologue(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const uint8_t* original_code,
    size_t original_code_size,
    uint64_t original_address,
    uint64_t relocated_address,
    const asmkit_relocate_options_t* options,
    uint8_t* out_code,
    size_t out_capacity,
    asmkit_relocate_result_t* out_result);

#ifdef __cplusplus
}
#endif

#endif

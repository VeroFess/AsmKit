#ifndef ASMKIT_ENCODE_H
#define ASMKIT_ENCODE_H

#include <stddef.h>
#include <stdint.h>
#include "asmkit/asmkit_decode.h"
#include "asmkit/asmkit_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum asmkit_branch_mode {
    ASMKIT_BRANCH_AUTO = 0,
    ASMKIT_BRANCH_DIRECT_ONLY,
    ASMKIT_BRANCH_ABSOLUTE_ONLY,
    ASMKIT_BRANCH_PREFER_DIRECT,
    ASMKIT_BRANCH_PREFER_CLOBBERLESS,
    ASMKIT_BRANCH_ALLOW_SCRATCH_CLOBBER
} asmkit_branch_mode_t;

typedef struct asmkit_emit_options {
    asmkit_branch_mode_t mode;
    uint64_t allowed_clobber_mask_lo;
    uint64_t allowed_clobber_mask_hi;
    uint32_t flags;
} asmkit_emit_options_t;

typedef struct asmkit_emit_result {
    uint32_t size;
    uint32_t required_alignment;
    uint64_t clobber_mask_lo;
    uint64_t clobber_mask_hi;
    uint32_t flags;
} asmkit_emit_result_t;

typedef struct asmkit_encode_options {
    uint32_t flags;
} asmkit_encode_options_t;

typedef struct asmkit_encode_result {
    uint32_t size;
    uint32_t flags;
} asmkit_encode_result_t;

typedef enum asmkit_control_transfer_kind {
    ASMKIT_CONTROL_TRANSFER_JUMP = 0,
    ASMKIT_CONTROL_TRANSFER_CALL
} asmkit_control_transfer_kind_t;

typedef struct asmkit_control_transfer_plan {
    asmkit_control_transfer_kind_t kind;
    uint32_t size;
    uint32_t required_alignment;
    uint64_t clobber_mask_lo;
    uint64_t clobber_mask_hi;
    bool requires_island;
    int64_t island_min_distance;
    int64_t island_max_distance;
} asmkit_control_transfer_plan_t;

struct asmkit_engine;
struct asmkit_workspace;

asmkit_status_t asmkit_encode_inst(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const asmkit_inst_t* inst,
    const asmkit_encode_options_t* options,
    uint8_t* out_code,
    size_t out_capacity,
    asmkit_encode_result_t* out_result);

asmkit_status_t asmkit_emit_jump(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* options,
    uint8_t* out_code,
    size_t out_capacity,
    asmkit_emit_result_t* out_result);

asmkit_status_t asmkit_emit_call(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* options,
    uint8_t* out_code,
    size_t out_capacity,
    asmkit_emit_result_t* out_result);

asmkit_status_t asmkit_plan_control_transfer(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    asmkit_control_transfer_kind_t kind,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* options,
    asmkit_control_transfer_plan_t* out_plan);

asmkit_status_t asmkit_emit_padding(
    const struct asmkit_engine* engine,
    uint32_t size,
    uint8_t* out_code,
    size_t out_capacity,
    asmkit_encode_result_t* out_result);

#ifdef __cplusplus
}
#endif

#endif

#ifndef ASMKIT_TARGET_H
#define ASMKIT_TARGET_H

#include <stdint.h>
#include "asmkit/asmkit_arch.h"
#include "asmkit/asmkit_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum asmkit_target_capability_flags {
    ASMKIT_TARGET_CAP_NATIVE_LINEAR = 1u << 0,
    ASMKIT_TARGET_CAP_STRUCTURED_CONTROL_FLOW = 1u << 1,
    ASMKIT_TARGET_CAP_ENCODE = 1u << 2,
    ASMKIT_TARGET_CAP_RELOCATE_INSTRUCTION = 1u << 3,
    ASMKIT_TARGET_CAP_EMIT_BRANCH = 1u << 4,
    ASMKIT_TARGET_CAP_EMIT_CALL = 1u << 5,
    ASMKIT_TARGET_CAP_BRANCH_ISLAND = 1u << 6,
    ASMKIT_TARGET_CAP_EMIT_PADDING = 1u << 7
} asmkit_target_capability_flags_t;

typedef struct asmkit_target_capabilities {
    asmkit_arch_t arch;
    asmkit_mode_t mode;
    uint32_t flags;
    uint32_t minimum_instruction_unit;
    uint32_t instruction_alignment;
    uint32_t maximum_instruction_size;
} asmkit_target_capabilities_t;

struct asmkit_engine;

asmkit_status_t asmkit_get_target_capabilities(
    const struct asmkit_engine* engine,
    asmkit_target_capabilities_t* out_capabilities);

#ifdef __cplusplus
}
#endif

#endif

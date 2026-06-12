#ifndef ASMKIT_H
#define ASMKIT_H

#include <stddef.h>
#include <stdint.h>
#include "asmkit/asmkit_arch.h"
#include "asmkit/asmkit_error.h"
#include "asmkit/asmkit_decode.h"
#include "asmkit/asmkit_metadata.h"
#include "asmkit/asmkit_bpf.h"
#include "asmkit/asmkit_analysis.h"
#include "asmkit/asmkit_encode.h"
#include "asmkit/asmkit_relocate.h"
#include "asmkit/asmkit_text.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    ASMKIT_FEATURE_WORD_COUNT = 8u,
    ASMKIT_FEATURE_BIT_CAPACITY = ASMKIT_FEATURE_WORD_COUNT * 64u
};

typedef struct asmkit_feature_set {
    uint64_t words[ASMKIT_FEATURE_WORD_COUNT];
} asmkit_feature_set_t;

typedef struct asmkit_engine_config {
    asmkit_arch_t arch;
    asmkit_mode_t mode;
    uint64_t feature_bits_lo;
    uint64_t feature_bits_hi;
    uint64_t feature_bits_2;
    uint64_t feature_bits_3;
    uint64_t feature_bits_4;
    uint64_t feature_bits_5;
    uint64_t feature_bits_6;
    uint64_t feature_bits_7;
    uint32_t flags;
} asmkit_engine_config_t;

typedef enum asmkit_engine_flags {
    ASMKIT_ENGINE_FLAG_EXPLICIT_FEATURES = 1u << 0,
    ASMKIT_ENGINE_FLAG_ALL_TARGET_FEATURES = 1u << 1
} asmkit_engine_flags_t;

typedef struct asmkit_engine {
    asmkit_engine_config_t config;
    const void* target_tables;
} asmkit_engine_t;

typedef struct asmkit_workspace {
    uint8_t* memory;
    size_t size;
    size_t used;
    uint32_t flags;
    asmkit_error_info_t last_error;
} asmkit_workspace_t;

asmkit_status_t asmkit_feature_set_clear(
    asmkit_feature_set_t* feature_set);

asmkit_status_t asmkit_feature_set_fill(
    asmkit_feature_set_t* feature_set);

asmkit_status_t asmkit_feature_set_enable(
    asmkit_feature_set_t* feature_set,
    uint32_t feature_id);

asmkit_status_t asmkit_feature_set_disable(
    asmkit_feature_set_t* feature_set,
    uint32_t feature_id);

asmkit_status_t asmkit_feature_set_contains(
    const asmkit_feature_set_t* feature_set,
    uint32_t feature_id,
    int* out_enabled);

asmkit_status_t asmkit_feature_set_from_cpu(
    asmkit_arch_t arch,
    uint32_t cpu_id,
    asmkit_feature_set_t* out_feature_set);

asmkit_status_t asmkit_engine_config_init(
    asmkit_engine_config_t* config,
    asmkit_arch_t arch,
    asmkit_mode_t mode);

asmkit_status_t asmkit_engine_config_set_features(
    asmkit_engine_config_t* config,
    const asmkit_feature_set_t* feature_set);

asmkit_status_t asmkit_engine_config_get_features(
    const asmkit_engine_config_t* config,
    asmkit_feature_set_t* out_feature_set);

asmkit_status_t asmkit_engine_config_enable_feature(
    asmkit_engine_config_t* config,
    uint32_t feature_id);

asmkit_status_t asmkit_engine_config_enable_cpu_features(
    asmkit_engine_config_t* config,
    asmkit_arch_t arch,
    uint32_t cpu_id);

asmkit_status_t asmkit_engine_config_enable_all_features(
    asmkit_engine_config_t* config);

asmkit_status_t asmkit_engine_init(
    asmkit_engine_t* engine,
    const asmkit_engine_config_t* config);

size_t asmkit_workspace_required_size(
    const asmkit_engine_t* engine);

asmkit_status_t asmkit_workspace_init(
    asmkit_workspace_t* workspace,
    void* memory,
    size_t memory_size);

void asmkit_workspace_reset(asmkit_workspace_t* workspace);

#ifdef __cplusplus
}
#endif

#include "asmkit/asmkit_easy.h"

#endif

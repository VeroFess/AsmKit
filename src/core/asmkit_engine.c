#include "core/asmkit_internal.h"

static asmkit_mode_t asmkit_default_mode_for_arch(asmkit_arch_t arch)
{
    switch (arch) {
    case ASMKIT_ARCH_X86: return ASMKIT_MODE_X86_64;
    case ASMKIT_ARCH_ARM: return ASMKIT_MODE_ARM_A32;
    case ASMKIT_ARCH_AARCH64: return ASMKIT_MODE_AARCH64;
    case ASMKIT_ARCH_BPF: return ASMKIT_MODE_BPF64;
    case ASMKIT_ARCH_WASM: return ASMKIT_MODE_WASM32;
    default: return ASMKIT_MODE_DEFAULT;
    }
}

static uint64_t* asmkit_engine_config_feature_word_ptr(asmkit_engine_config_t* ASMKIT_RESTRICT config, uint32_t word_index)
{
    switch (word_index) {
    case 0u: return &config->feature_bits_lo;
    case 1u: return &config->feature_bits_hi;
    case 2u: return &config->feature_bits_2;
    case 3u: return &config->feature_bits_3;
    case 4u: return &config->feature_bits_4;
    case 5u: return &config->feature_bits_5;
    case 6u: return &config->feature_bits_6;
    case 7u: return &config->feature_bits_7;
    default: return 0;
    }
}

const asmkit_target_ops_t* asmkit_lookup_target_ops(asmkit_arch_t arch, asmkit_mode_t mode)
{
    const asmkit_target_ops_t* ops[5];
    size_t i;

    ops[0] = &asmkit_x86_ops;
    ops[1] = &asmkit_arm_ops;
    ops[2] = &asmkit_aarch64_ops;
    ops[3] = &asmkit_bpf_ops;
    ops[4] = &asmkit_wasm_ops;

    if (mode == ASMKIT_MODE_DEFAULT) {
        mode = asmkit_default_mode_for_arch(arch);
    }

    for (i = 0u; i < ASMKIT_ARRAY_COUNT(ops); ++i) {
        if (ops[i]->arch == arch && mode >= ops[i]->first_mode && mode <= ops[i]->last_mode) {
            return ops[i];
        }
    }
    return 0;
}

const asmkit_target_ops_t* asmkit_engine_ops(const asmkit_engine_t* ASMKIT_RESTRICT engine)
{
    if (engine == 0 || engine->target_tables == 0) {
        return 0;
    }
    return (const asmkit_target_ops_t*)engine->target_tables;
}

int asmkit_target_feature_enabled(const asmkit_engine_t* ASMKIT_RESTRICT engine, uint32_t feature_bit)
{
    return (asmkit_engine_feature_word(engine, feature_bit / 64u) & ((uint64_t)1u << (feature_bit % 64u))) != 0u;
}

uint64_t asmkit_engine_feature_word(const asmkit_engine_t* ASMKIT_RESTRICT engine, uint32_t word_index)
{
    if (engine == 0) {
        return 0u;
    }
    if ((engine->config.flags & ASMKIT_ENGINE_FLAG_ALL_TARGET_FEATURES) != 0u) {
        return ~(uint64_t)0;
    }
    switch (word_index) {
    case 0u: return engine->config.feature_bits_lo;
    case 1u: return engine->config.feature_bits_hi;
    case 2u: return engine->config.feature_bits_2;
    case 3u: return engine->config.feature_bits_3;
    case 4u: return engine->config.feature_bits_4;
    case 5u: return engine->config.feature_bits_5;
    case 6u: return engine->config.feature_bits_6;
    case 7u: return engine->config.feature_bits_7;
    default: return 0u;
    }
}

asmkit_status_t asmkit_feature_set_clear(asmkit_feature_set_t* feature_set)
{
    uint32_t i;

    if (feature_set == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    for (i = 0u; i < ASMKIT_FEATURE_WORD_COUNT; ++i) {
        feature_set->words[i] = 0u;
    }
    return ASMKIT_OK;
}

asmkit_status_t asmkit_feature_set_fill(asmkit_feature_set_t* feature_set)
{
    uint32_t i;

    if (feature_set == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    for (i = 0u; i < ASMKIT_FEATURE_WORD_COUNT; ++i) {
        feature_set->words[i] = ~(uint64_t)0;
    }
    return ASMKIT_OK;
}

asmkit_status_t asmkit_feature_set_enable(asmkit_feature_set_t* feature_set, uint32_t feature_id)
{
    if (feature_set == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    if (feature_id >= ASMKIT_FEATURE_BIT_CAPACITY) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    feature_set->words[feature_id / 64u] |= UINT64_C(1) << (feature_id % 64u);
    return ASMKIT_OK;
}

asmkit_status_t asmkit_feature_set_disable(asmkit_feature_set_t* feature_set, uint32_t feature_id)
{
    if (feature_set == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    if (feature_id >= ASMKIT_FEATURE_BIT_CAPACITY) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    feature_set->words[feature_id / 64u] &= ~(UINT64_C(1) << (feature_id % 64u));
    return ASMKIT_OK;
}

asmkit_status_t asmkit_feature_set_contains(const asmkit_feature_set_t* feature_set, uint32_t feature_id, int* out_enabled)
{
    if (feature_set == 0 || out_enabled == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_enabled = 0;
    if (feature_id >= ASMKIT_FEATURE_BIT_CAPACITY) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    *out_enabled = (feature_set->words[feature_id / 64u] & (UINT64_C(1) << (feature_id % 64u))) != 0u;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_feature_set_from_cpu(asmkit_arch_t arch, uint32_t cpu_id, asmkit_feature_set_t* out_feature_set)
{
    const asmkit_cpu_info_t* cpu;
    const asmkit_feature_info_t* feature;
    asmkit_status_t status;
    uint32_t i;

    if (out_feature_set == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    status = asmkit_get_cpu_info(arch, cpu_id, &cpu);
    if (status != ASMKIT_OK) {
        return status;
    }
    status = asmkit_feature_set_clear(out_feature_set);
    if (status != ASMKIT_OK) {
        return status;
    }
    for (i = 0u; i < cpu->feature_count; ++i) {
        status = asmkit_get_cpu_feature_info(arch, cpu_id, i, &feature);
        if (status != ASMKIT_OK) {
            asmkit_feature_set_clear(out_feature_set);
            return status;
        }
        status = asmkit_feature_set_enable(out_feature_set, feature->id);
        if (status != ASMKIT_OK) {
            asmkit_feature_set_clear(out_feature_set);
            return status;
        }
    }
    return ASMKIT_OK;
}

asmkit_status_t asmkit_engine_config_init(asmkit_engine_config_t* config, asmkit_arch_t arch, asmkit_mode_t mode)
{
    if (config == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(config, sizeof(*config));
    config->arch = arch;
    config->mode = mode;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_engine_config_set_features(asmkit_engine_config_t* config, const asmkit_feature_set_t* feature_set)
{
    if (config == 0 || feature_set == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    config->feature_bits_lo = feature_set->words[0];
    config->feature_bits_hi = feature_set->words[1];
    config->feature_bits_2 = feature_set->words[2];
    config->feature_bits_3 = feature_set->words[3];
    config->feature_bits_4 = feature_set->words[4];
    config->feature_bits_5 = feature_set->words[5];
    config->feature_bits_6 = feature_set->words[6];
    config->feature_bits_7 = feature_set->words[7];
    config->flags |= ASMKIT_ENGINE_FLAG_EXPLICIT_FEATURES;
    config->flags &= ~ASMKIT_ENGINE_FLAG_ALL_TARGET_FEATURES;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_engine_config_get_features(const asmkit_engine_config_t* config, asmkit_feature_set_t* out_feature_set)
{
    if (config == 0 || out_feature_set == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    if ((config->flags & ASMKIT_ENGINE_FLAG_ALL_TARGET_FEATURES) != 0u) {
        return asmkit_feature_set_fill(out_feature_set);
    }
    out_feature_set->words[0] = config->feature_bits_lo;
    out_feature_set->words[1] = config->feature_bits_hi;
    out_feature_set->words[2] = config->feature_bits_2;
    out_feature_set->words[3] = config->feature_bits_3;
    out_feature_set->words[4] = config->feature_bits_4;
    out_feature_set->words[5] = config->feature_bits_5;
    out_feature_set->words[6] = config->feature_bits_6;
    out_feature_set->words[7] = config->feature_bits_7;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_engine_config_enable_feature(asmkit_engine_config_t* config, uint32_t feature_id)
{
    uint64_t* word;

    if (config == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    if (feature_id >= ASMKIT_FEATURE_BIT_CAPACITY) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    word = asmkit_engine_config_feature_word_ptr(config, feature_id / 64u);
    if (word == 0) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    *word |= UINT64_C(1) << (feature_id % 64u);
    config->flags |= ASMKIT_ENGINE_FLAG_EXPLICIT_FEATURES;
    config->flags &= ~ASMKIT_ENGINE_FLAG_ALL_TARGET_FEATURES;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_engine_config_enable_cpu_features(asmkit_engine_config_t* config, asmkit_arch_t arch, uint32_t cpu_id)
{
    asmkit_feature_set_t feature_set;
    asmkit_status_t status;

    if (config == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    status = asmkit_feature_set_from_cpu(arch, cpu_id, &feature_set);
    if (status != ASMKIT_OK) {
        return status;
    }
    return asmkit_engine_config_set_features(config, &feature_set);
}

asmkit_status_t asmkit_engine_config_enable_all_features(asmkit_engine_config_t* config)
{
    if (config == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    config->feature_bits_lo = ~(uint64_t)0;
    config->feature_bits_hi = ~(uint64_t)0;
    config->feature_bits_2 = ~(uint64_t)0;
    config->feature_bits_3 = ~(uint64_t)0;
    config->feature_bits_4 = ~(uint64_t)0;
    config->feature_bits_5 = ~(uint64_t)0;
    config->feature_bits_6 = ~(uint64_t)0;
    config->feature_bits_7 = ~(uint64_t)0;
    config->flags |= ASMKIT_ENGINE_FLAG_ALL_TARGET_FEATURES;
    config->flags &= ~ASMKIT_ENGINE_FLAG_EXPLICIT_FEATURES;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_engine_init(asmkit_engine_t* engine, const asmkit_engine_config_t* config)
{
    asmkit_engine_config_t local;
    const asmkit_target_ops_t* ops;

    if (engine == 0 || config == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }

    local = *config;
    if (local.mode == ASMKIT_MODE_DEFAULT) {
        local.mode = asmkit_default_mode_for_arch(local.arch);
    }
    if ((local.flags & ASMKIT_ENGINE_FLAG_ALL_TARGET_FEATURES) != 0u ||
        (local.flags & ASMKIT_ENGINE_FLAG_EXPLICIT_FEATURES) == 0u) {
        local.feature_bits_lo = ~(uint64_t)0;
        local.feature_bits_hi = ~(uint64_t)0;
        local.feature_bits_2 = ~(uint64_t)0;
        local.feature_bits_3 = ~(uint64_t)0;
        local.feature_bits_4 = ~(uint64_t)0;
        local.feature_bits_5 = ~(uint64_t)0;
        local.feature_bits_6 = ~(uint64_t)0;
        local.feature_bits_7 = ~(uint64_t)0;
        local.flags |= ASMKIT_ENGINE_FLAG_ALL_TARGET_FEATURES;
    }

    ops = asmkit_lookup_target_ops(local.arch, local.mode);
    if (ops == 0) {
        return ASMKIT_ERR_UNSUPPORTED_MODE;
    }

    engine->config = local;
    engine->target_tables = ops;
    return ASMKIT_OK;
}

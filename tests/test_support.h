#ifndef ASMKIT_TEST_SUPPORT_H
#define ASMKIT_TEST_SUPPORT_H

#include <stdio.h>
#include "asmkit/asmkit.h"

#define ASMKIT_CHECK(expr) do { \
    if (!(expr)) { \
        printf("check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); \
        return 1; \
    } \
} while (0)

static uint32_t asmkit_test_load32le(const uint8_t* p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint64_t asmkit_test_load64le(const uint8_t* p)
{
    return ((uint64_t)asmkit_test_load32le(p)) |
           ((uint64_t)asmkit_test_load32le(p + 4) << 32);
}

static int asmkit_test_init_engine(asmkit_engine_t* engine, asmkit_arch_t arch, asmkit_mode_t mode)
{
    asmkit_engine_config_t config;
    if (asmkit_engine_config_init(&config, arch, mode) != ASMKIT_OK) {
        return 1;
    }
    return asmkit_engine_init(engine, &config) == ASMKIT_OK ? 0 : 1;
}

#define ASMKIT_INIT_ENGINE(engine_ptr, arch_value, mode_value) \
    ASMKIT_CHECK(asmkit_test_init_engine((engine_ptr), (arch_value), (mode_value)) == 0)

#endif

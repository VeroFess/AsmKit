#include "core/asmkit_internal.h"

void asmkit_zero(void* ptr, size_t size)
{
    uint8_t* p;
    size_t i;
    if (ptr == 0) {
        return;
    }
    p = (uint8_t*)ptr;
    for (i = 0; i < size; ++i) {
        p[i] = 0u;
    }
}

void asmkit_copy(void* dst, const void* src, size_t size)
{
    uint8_t* d;
    const uint8_t* s;
    size_t i;
    if (dst == 0 || src == 0) {
        return;
    }
    d = (uint8_t*)dst;
    s = (const uint8_t*)src;
    for (i = 0; i < size; ++i) {
        d[i] = s[i];
    }
}

int asmkit_i64_fits_i8(int64_t value)
{
    return value >= -128 && value <= 127;
}

int asmkit_i64_fits_i16(int64_t value)
{
    return value >= -32768 && value <= 32767;
}

int asmkit_i64_fits_i24_words4(int64_t value)
{
    return (value % 4) == 0 && value >= -33554432 && value <= 33554428;
}

int asmkit_i64_fits_i26_words4(int64_t value)
{
    return (value % 4) == 0 && value >= -134217728 && value <= 134217724;
}

int asmkit_i64_fits_i32(int64_t value)
{
    return value >= (int64_t)(-2147483647 - 1) && value <= (int64_t)2147483647;
}

uint16_t asmkit_load16le(const uint8_t* p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

uint32_t asmkit_load32le(const uint8_t* p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

uint64_t asmkit_load64le(const uint8_t* p)
{
    return ((uint64_t)asmkit_load32le(p)) | ((uint64_t)asmkit_load32le(p + 4) << 32);
}

void asmkit_store16le(uint8_t* p, uint16_t v)
{
    p[0] = (uint8_t)(v & 0xffu);
    p[1] = (uint8_t)((v >> 8) & 0xffu);
}

void asmkit_store32le(uint8_t* p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xffu);
    p[1] = (uint8_t)((v >> 8) & 0xffu);
    p[2] = (uint8_t)((v >> 16) & 0xffu);
    p[3] = (uint8_t)((v >> 24) & 0xffu);
}

void asmkit_store64le(uint8_t* p, uint64_t v)
{
    asmkit_store32le(p, (uint32_t)(v & 0xffffffffu));
    asmkit_store32le(p + 4, (uint32_t)(v >> 32));
}

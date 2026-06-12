#ifndef ASMKIT_TEXT_H
#define ASMKIT_TEXT_H

#include <stddef.h>
#include <stdint.h>
#include "asmkit/asmkit_decode.h"

#ifndef ASMKIT_ENABLE_TEXT
#define ASMKIT_ENABLE_TEXT 0
#endif

#ifndef ASMKIT_TEXT_ENABLE_OPERANDS
#define ASMKIT_TEXT_ENABLE_OPERANDS 1
#endif

#ifndef ASMKIT_TEXT_ENABLE_ALIAS_MNEMONICS
#define ASMKIT_TEXT_ENABLE_ALIAS_MNEMONICS 0
#endif

#ifndef ASMKIT_TEXT_NO_LIBC
#define ASMKIT_TEXT_NO_LIBC 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asmkit_text_options {
    uint32_t flags;
} asmkit_text_options_t;

typedef struct asmkit_text_result {
    uint32_t written;
    uint32_t required;
    uint32_t flags;
} asmkit_text_result_t;

asmkit_status_t asmkit_format_inst(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const asmkit_inst_t* inst,
    const asmkit_text_options_t* options,
    char* out_text,
    size_t out_capacity,
    asmkit_text_result_t* out_result);

#ifdef __cplusplus
}
#endif

#endif

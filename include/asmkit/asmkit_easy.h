#ifndef ASMKIT_EASY_H
#define ASMKIT_EASY_H

#include "asmkit/asmkit.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asmkit_decoder {
    asmkit_engine_t engine;
} asmkit_decoder_t;

typedef struct asmkit_encoder {
    asmkit_engine_t engine;
} asmkit_encoder_t;

typedef struct asmkit_formatter {
    uint32_t flags;
} asmkit_formatter_t;

asmkit_status_t asmkit_decoder_init(
    asmkit_decoder_t* decoder,
    asmkit_arch_t arch,
    asmkit_mode_t mode);

asmkit_status_t asmkit_decoder_init_ex(
    asmkit_decoder_t* decoder,
    const asmkit_engine_config_t* config);

const asmkit_engine_t* asmkit_decoder_engine(
    const asmkit_decoder_t* decoder);

asmkit_status_t asmkit_decoder_decode_full(
    const asmkit_decoder_t* decoder,
    const void* code,
    size_t code_size,
    uint64_t address,
    asmkit_inst_t* out_inst);

asmkit_status_t asmkit_decoder_decode_block_until(
    const asmkit_decoder_t* decoder,
    const void* code,
    size_t code_size,
    uint64_t address,
    uint32_t min_size,
    asmkit_inst_t* out_insts,
    uint32_t out_inst_capacity,
    asmkit_decode_block_result_t* out_result);

asmkit_status_t asmkit_decode_full(
    asmkit_arch_t arch,
    asmkit_mode_t mode,
    const void* code,
    size_t code_size,
    uint64_t address,
    asmkit_inst_t* out_inst);

asmkit_status_t asmkit_encoder_init(
    asmkit_encoder_t* encoder,
    asmkit_arch_t arch,
    asmkit_mode_t mode);

asmkit_status_t asmkit_encoder_init_ex(
    asmkit_encoder_t* encoder,
    const asmkit_engine_config_t* config);

const asmkit_engine_t* asmkit_encoder_engine(
    const asmkit_encoder_t* encoder);

asmkit_status_t asmkit_encoder_encode_inst(
    const asmkit_encoder_t* encoder,
    const asmkit_inst_t* inst,
    void* out_code,
    size_t out_capacity,
    asmkit_encode_result_t* out_result);

asmkit_status_t asmkit_encoder_emit_jump(
    const asmkit_encoder_t* encoder,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* options,
    void* out_code,
    size_t out_capacity,
    asmkit_emit_result_t* out_result);

asmkit_status_t asmkit_encoder_emit_call(
    const asmkit_encoder_t* encoder,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* options,
    void* out_code,
    size_t out_capacity,
    asmkit_emit_result_t* out_result);

asmkit_status_t asmkit_encode_full(
    asmkit_arch_t arch,
    asmkit_mode_t mode,
    const asmkit_inst_t* inst,
    void* out_code,
    size_t out_capacity,
    asmkit_encode_result_t* out_result);

asmkit_status_t asmkit_formatter_init(
    asmkit_formatter_t* formatter,
    uint32_t flags);

asmkit_status_t asmkit_formatter_format_inst(
    const asmkit_formatter_t* formatter,
    const asmkit_inst_t* inst,
    char* out_text,
    size_t out_capacity,
    asmkit_text_result_t* out_result);

#ifdef __cplusplus
}
#endif

#endif

#include "core/asmkit_internal.h"

asmkit_status_t asmkit_decoder_init(
    asmkit_decoder_t* decoder,
    asmkit_arch_t arch,
    asmkit_mode_t mode)
{
    asmkit_engine_config_t config;

    if (decoder == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(&config, sizeof(config));
    config.arch = arch;
    config.mode = mode;
    return asmkit_decoder_init_ex(decoder, &config);
}

asmkit_status_t asmkit_decoder_init_ex(
    asmkit_decoder_t* decoder,
    const asmkit_engine_config_t* config)
{
    if (decoder == 0 || config == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(decoder, sizeof(*decoder));
    return asmkit_engine_init(&decoder->engine, config);
}

const asmkit_engine_t* asmkit_decoder_engine(
    const asmkit_decoder_t* decoder)
{
    return decoder != 0 ? &decoder->engine : 0;
}

asmkit_status_t asmkit_decoder_decode_full(
    const asmkit_decoder_t* decoder,
    const void* code,
    size_t code_size,
    uint64_t address,
    asmkit_inst_t* out_inst)
{
    if (decoder == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    return asmkit_decode_one(&decoder->engine, 0, (const uint8_t*)code, code_size, address, out_inst);
}

asmkit_status_t asmkit_decoder_decode_block_until(
    const asmkit_decoder_t* decoder,
    const void* code,
    size_t code_size,
    uint64_t address,
    uint32_t min_size,
    asmkit_inst_t* out_insts,
    uint32_t out_inst_capacity,
    asmkit_decode_block_result_t* out_result)
{
    if (decoder == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    return asmkit_decode_block_until(
        &decoder->engine,
        0,
        (const uint8_t*)code,
        code_size,
        address,
        min_size,
        out_insts,
        out_inst_capacity,
        out_result);
}

asmkit_status_t asmkit_decode_full(
    asmkit_arch_t arch,
    asmkit_mode_t mode,
    const void* code,
    size_t code_size,
    uint64_t address,
    asmkit_inst_t* out_inst)
{
    asmkit_decoder_t decoder;
    asmkit_status_t status;

    status = asmkit_decoder_init(&decoder, arch, mode);
    if (status != ASMKIT_OK) {
        return status;
    }
    return asmkit_decoder_decode_full(&decoder, code, code_size, address, out_inst);
}

asmkit_status_t asmkit_encoder_init(
    asmkit_encoder_t* encoder,
    asmkit_arch_t arch,
    asmkit_mode_t mode)
{
    asmkit_engine_config_t config;

    if (encoder == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(&config, sizeof(config));
    config.arch = arch;
    config.mode = mode;
    return asmkit_encoder_init_ex(encoder, &config);
}

asmkit_status_t asmkit_encoder_init_ex(
    asmkit_encoder_t* encoder,
    const asmkit_engine_config_t* config)
{
    if (encoder == 0 || config == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(encoder, sizeof(*encoder));
    return asmkit_engine_init(&encoder->engine, config);
}

const asmkit_engine_t* asmkit_encoder_engine(
    const asmkit_encoder_t* encoder)
{
    return encoder != 0 ? &encoder->engine : 0;
}

asmkit_status_t asmkit_encoder_encode_inst(
    const asmkit_encoder_t* encoder,
    const asmkit_inst_t* inst,
    void* out_code,
    size_t out_capacity,
    asmkit_encode_result_t* out_result)
{
    if (encoder == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    return asmkit_encode_inst(&encoder->engine, 0, inst, 0, (uint8_t*)out_code, out_capacity, out_result);
}

asmkit_status_t asmkit_encoder_emit_jump(
    const asmkit_encoder_t* encoder,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* options,
    void* out_code,
    size_t out_capacity,
    asmkit_emit_result_t* out_result)
{
    if (encoder == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    return asmkit_emit_jump(&encoder->engine, 0, from_address, to_address, options, (uint8_t*)out_code, out_capacity, out_result);
}

asmkit_status_t asmkit_encoder_emit_call(
    const asmkit_encoder_t* encoder,
    uint64_t from_address,
    uint64_t to_address,
    const asmkit_emit_options_t* options,
    void* out_code,
    size_t out_capacity,
    asmkit_emit_result_t* out_result)
{
    if (encoder == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    return asmkit_emit_call(&encoder->engine, 0, from_address, to_address, options, (uint8_t*)out_code, out_capacity, out_result);
}

asmkit_status_t asmkit_encode_full(
    asmkit_arch_t arch,
    asmkit_mode_t mode,
    const asmkit_inst_t* inst,
    void* out_code,
    size_t out_capacity,
    asmkit_encode_result_t* out_result)
{
    asmkit_encoder_t encoder;
    asmkit_status_t status;

    status = asmkit_encoder_init(&encoder, arch, mode);
    if (status != ASMKIT_OK) {
        return status;
    }
    return asmkit_encoder_encode_inst(&encoder, inst, out_code, out_capacity, out_result);
}

asmkit_status_t asmkit_formatter_init(
    asmkit_formatter_t* formatter,
    uint32_t flags)
{
    if (formatter == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(formatter, sizeof(*formatter));
    formatter->flags = flags;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_formatter_format_inst(
    const asmkit_formatter_t* formatter,
    const asmkit_inst_t* inst,
    char* out_text,
    size_t out_capacity,
    asmkit_text_result_t* out_result)
{
    asmkit_engine_config_t config;
    asmkit_engine_t engine;
    asmkit_text_options_t options;
    asmkit_status_t status;

    if (formatter == 0 || inst == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    asmkit_zero(&config, sizeof(config));
    config.arch = inst->arch;
    config.mode = inst->mode;
    status = asmkit_engine_init(&engine, &config);
    if (status != ASMKIT_OK) {
        return status;
    }
    asmkit_zero(&options, sizeof(options));
    options.flags = formatter->flags;
    return asmkit_format_inst(&engine, 0, inst, &options, out_text, out_capacity, out_result);
}

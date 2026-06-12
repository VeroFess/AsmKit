#include "core/asmkit_internal.h"

#if ASMKIT_ENABLE_TEXT
typedef struct asmkit_text_builder {
    char* out_text;
    size_t out_capacity;
    uint32_t required;
    uint32_t written;
} asmkit_text_builder_t;

static void asmkit_text_builder_append_char(asmkit_text_builder_t* builder, char ch)
{
    if (builder->out_text != 0 && builder->out_capacity > 0u && (size_t)builder->written + 1u < builder->out_capacity) {
        builder->out_text[builder->written] = ch;
        ++builder->written;
    }
    ++builder->required;
}

static void asmkit_text_builder_append_cstr(asmkit_text_builder_t* builder, const char* text)
{
    uint32_t i;

    if (text == 0) {
        return;
    }
    i = 0u;
    while (text[i] != '\0') {
        asmkit_text_builder_append_char(builder, text[i]);
        ++i;
    }
}

static void asmkit_text_builder_append_unsigned_dec(asmkit_text_builder_t* builder, uint64_t value)
{
    char digits[20];
    uint32_t count;

    count = 0u;
    do {
        digits[count] = (char)('0' + (char)(value % 10u));
        value /= 10u;
        ++count;
    } while (value != 0u && count < (uint32_t)sizeof(digits));
    while (count > 0u) {
        --count;
        asmkit_text_builder_append_char(builder, digits[count]);
    }
}

static void asmkit_text_builder_append_signed_dec(asmkit_text_builder_t* builder, int64_t value)
{
    uint64_t magnitude;

    if (value < 0) {
        asmkit_text_builder_append_char(builder, '-');
        magnitude = 0u - (uint64_t)value;
    } else {
        magnitude = (uint64_t)value;
    }
    asmkit_text_builder_append_unsigned_dec(builder, magnitude);
}

static void asmkit_text_builder_append_hex(asmkit_text_builder_t* builder, uint64_t value)
{
    char digits[16];
    uint32_t count;
    static const char hex_chars[] = "0123456789abcdef";

    asmkit_text_builder_append_cstr(builder, "0x");
    count = 0u;
    do {
        digits[count] = hex_chars[value & 0xfu];
        value >>= 4u;
        ++count;
    } while (value != 0u && count < (uint32_t)sizeof(digits));
    while (count > 0u) {
        --count;
        asmkit_text_builder_append_char(builder, digits[count]);
    }
}

static asmkit_status_t asmkit_text_builder_finish(asmkit_text_builder_t* builder, asmkit_text_result_t* out_result)
{
    if (builder->out_text != 0 && builder->out_capacity > 0u) {
        builder->out_text[builder->written] = '\0';
    }
    if (out_result != 0) {
        out_result->required = builder->required;
        out_result->written = builder->written;
        out_result->flags = 0u;
    }
    if (builder->out_text == 0 || builder->out_capacity == 0u) {
        return ASMKIT_ERR_OUTPUT_TOO_SMALL;
    }
    return builder->written < builder->required ? ASMKIT_ERR_OUTPUT_TOO_SMALL : ASMKIT_OK;
}

static asmkit_status_t asmkit_text_copy(const char* text, char* out_text, size_t out_capacity, asmkit_text_result_t* out_result)
{
    asmkit_text_builder_t builder;

    builder.out_text = out_text;
    builder.out_capacity = out_capacity;
    builder.required = 0u;
    builder.written = 0u;
    asmkit_text_builder_append_cstr(&builder, text);
    return asmkit_text_builder_finish(&builder, out_result);
}

static const char* asmkit_text_register_name(asmkit_arch_t arch, uint64_t encoding, uint16_t width)
{
    const asmkit_register_info_t* width_agnostic_match;
    uint32_t count;
    uint32_t i;

    width_agnostic_match = 0;
    count = asmkit_get_register_count(arch);
    for (i = 0u; i < count; ++i) {
        const asmkit_register_info_t* info;
        if (asmkit_get_register_info_by_index(arch, i, &info) != ASMKIT_OK || info->encoding != (uint16_t)encoding) {
            continue;
        }
        if (info->width == width) {
            return info->name;
        }
        if (width_agnostic_match == 0) {
            width_agnostic_match = info;
        }
    }
    return width_agnostic_match != 0 ? width_agnostic_match->name : 0;
}

static void asmkit_text_append_register(asmkit_text_builder_t* builder, asmkit_arch_t arch, uint64_t encoding, uint16_t width)
{
    const char* name;

    name = asmkit_text_register_name(arch, encoding, width);
    if (name != 0) {
        asmkit_text_builder_append_cstr(builder, name);
        return;
    }
    asmkit_text_builder_append_char(builder, 'r');
    asmkit_text_builder_append_unsigned_dec(builder, encoding);
}

static const asmkit_operand_info_t* asmkit_text_operand_info(const asmkit_inst_t* inst, const asmkit_operand_t* operand)
{
    const asmkit_operand_info_t* info;

    info = 0;
    if (asmkit_get_instruction_operand_info(inst->arch, inst->id, operand->operand_index, &info) == ASMKIT_OK) {
        return info;
    }
    return 0;
}

static void asmkit_text_append_operand(asmkit_text_builder_t* builder, const asmkit_inst_t* inst, const asmkit_operand_t* operand)
{
    const asmkit_operand_info_t* operand_info;
    uint16_t width;

    operand_info = asmkit_text_operand_info(inst, operand);
    width = operand_info != 0 && operand_info->width != 0u ? operand_info->width : operand->width;
    switch (operand->kind) {
    case ASMKIT_OP_REG:
        asmkit_text_append_register(builder, inst->arch, operand->reg, width);
        break;
    case ASMKIT_OP_IMM:
    case ASMKIT_OP_WASM_BLOCKTYPE:
    case ASMKIT_OP_WASM_INDEX:
        asmkit_text_builder_append_signed_dec(builder, operand->imm);
        break;
    case ASMKIT_OP_MEM:
        if (operand->mem.segment != 0u) {
            asmkit_text_append_register(builder, inst->arch, operand->mem.segment, 16u);
            asmkit_text_builder_append_char(builder, ':');
        }
        asmkit_text_builder_append_char(builder, '[');
        if (operand->mem.base != 0u) {
            asmkit_text_append_register(builder, inst->arch, operand->mem.base, operand->mem.address_width != 0u ? operand->mem.address_width : 64u);
        }
        if (operand->mem.index != 0u) {
            if (operand->mem.base != 0u) {
                asmkit_text_builder_append_char(builder, '+');
            }
            asmkit_text_append_register(builder, inst->arch, operand->mem.index, operand->mem.address_width != 0u ? operand->mem.address_width : 64u);
            if (operand->mem.scale > 1u) {
                asmkit_text_builder_append_char(builder, '*');
                asmkit_text_builder_append_unsigned_dec(builder, operand->mem.scale);
            }
        }
        if (operand->mem.displacement != 0 &&
            (operand->mem.base != 0u || operand->mem.index != 0u)) {
            if (operand->mem.displacement > 0) {
                asmkit_text_builder_append_char(builder, '+');
            }
            asmkit_text_builder_append_signed_dec(builder, operand->mem.displacement);
        } else if (operand->mem.base == 0u && operand->mem.index == 0u) {
            asmkit_text_builder_append_signed_dec(builder, operand->mem.displacement);
        }
        asmkit_text_builder_append_char(builder, ']');
        break;
    case ASMKIT_OP_PC_REL:
        asmkit_text_builder_append_hex(builder, operand->abs_target);
        break;
    default:
        asmkit_text_builder_append_char(builder, '?');
        break;
    }
}

static asmkit_status_t asmkit_text_format_with_operands(
    const char* mnemonic,
    const asmkit_inst_t* inst,
    char* out_text,
    size_t out_capacity,
    asmkit_text_result_t* out_result)
{
    asmkit_text_builder_t builder;

    builder.out_text = out_text;
    builder.out_capacity = out_capacity;
    builder.required = 0u;
    builder.written = 0u;
    if (inst->arch == ASMKIT_ARCH_X86 && (inst->flags & ASMKIT_INST_FLAG_X86_LOCK) != 0u) {
        asmkit_text_builder_append_cstr(&builder, "lock ");
    }
    asmkit_text_builder_append_cstr(&builder, mnemonic);
#if ASMKIT_TEXT_ENABLE_OPERANDS
    if (inst->arch == ASMKIT_ARCH_BPF && inst->operand_count > 0u) {
        uint32_t i;
        asmkit_text_builder_append_char(&builder, ' ');
        for (i = 0u; i < inst->operand_count && i < ASMKIT_MAX_OPERANDS; ++i) {
            if (i != 0u) {
                asmkit_text_builder_append_cstr(&builder, ", ");
            }
            asmkit_text_append_operand(&builder, inst, &inst->operands[i]);
        }
    }
#else
    (void)inst;
#endif
    return asmkit_text_builder_finish(&builder, out_result);
}
#endif

asmkit_status_t asmkit_format_inst(
    const asmkit_engine_t* engine,
    asmkit_workspace_t* workspace,
    const asmkit_inst_t* inst,
    const asmkit_text_options_t* options,
    char* out_text,
    size_t out_capacity,
    asmkit_text_result_t* out_result)
{
    (void)workspace;
    (void)options;
#if ASMKIT_ENABLE_TEXT
    const char* mnemonic;
    uint32_t text_id;
    if (engine == 0 || inst == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    text_id = inst->mnemonic_id != ASMKIT_MNEMONIC_INVALID ? inst->mnemonic_id : inst->id;
    switch (engine->config.arch) {
    case ASMKIT_ARCH_X86: mnemonic = asmkit_gen_x86_mnemonic(text_id); break;
    case ASMKIT_ARCH_ARM: mnemonic = asmkit_gen_arm_mnemonic(text_id); break;
    case ASMKIT_ARCH_AARCH64: mnemonic = asmkit_gen_aarch64_mnemonic(text_id); break;
    case ASMKIT_ARCH_BPF: mnemonic = asmkit_gen_bpf_mnemonic(text_id); break;
    case ASMKIT_ARCH_WASM: mnemonic = asmkit_gen_wasm_mnemonic(text_id); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (inst->operand_count == 0u && (inst->flags & ASMKIT_INST_FLAG_X86_LOCK) == 0u) {
        return asmkit_text_copy(mnemonic, out_text, out_capacity, out_result);
    }
    return asmkit_text_format_with_operands(mnemonic, inst, out_text, out_capacity, out_result);
#else
    (void)engine;
    (void)inst;
    (void)out_text;
    (void)out_capacity;
    if (out_result != 0) {
        out_result->written = 0u;
        out_result->required = 0u;
        out_result->flags = 0u;
    }
    return ASMKIT_ERR_FEATURE_DISABLED;
#endif
}

#include "core/asmkit_internal.h"
#include "asmkit/target/arm.h"

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

static int asmkit_text_cstr_eq(const char* left, const char* right)
{
    uint32_t i;

    if (left == 0 || right == 0) {
        return 0;
    }
    i = 0u;
    while (left[i] != '\0' && right[i] != '\0') {
        if (left[i] != right[i]) {
            return 0;
        }
        ++i;
    }
    return left[i] == right[i];
}

static int asmkit_text_cstr_starts_with(const char* text, const char* prefix)
{
    uint32_t i;

    if (text == 0 || prefix == 0) {
        return 0;
    }
    i = 0u;
    while (prefix[i] != '\0') {
        if (text[i] != prefix[i]) {
            return 0;
        }
        ++i;
    }
    return 1;
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

static const char* asmkit_text_register_name(asmkit_arch_t arch, uint64_t register_id, uint16_t width)
{
    const asmkit_register_info_t* info;

    (void)width;
    if (register_id <= UINT32_MAX && asmkit_get_register_info(arch, (uint32_t)register_id, &info) == ASMKIT_OK) {
        return info->name;
    }
    return 0;
}

static void asmkit_text_append_register(asmkit_text_builder_t* builder, asmkit_arch_t arch, uint64_t register_id, uint16_t width)
{
    const char* name;

    name = asmkit_text_register_name(arch, register_id, width);
    if (name != 0) {
        asmkit_text_builder_append_cstr(builder, name);
        return;
    }
    asmkit_text_builder_append_char(builder, 'r');
    asmkit_text_builder_append_unsigned_dec(builder, register_id);
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

static const char* asmkit_text_aarch64_condition_name(uint32_t condition)
{
    static const char* names[16] = {
        "eq", "ne", "hs", "lo", "mi", "pl", "vs", "vc",
        "hi", "ls", "ge", "lt", "gt", "le", "al", "nv"
    };
    return condition < 16u ? names[condition] : 0;
}

static int asmkit_text_append_arm_conditional_branch_mnemonic(
    asmkit_text_builder_t* builder,
    const char* mnemonic,
    const asmkit_inst_t* inst)
{
    const char* condition_name;
    uint32_t condition;
    uint32_t word;
    uint16_t h1;
    uint16_t h2;

    if (inst->arch != ASMKIT_ARCH_ARM ||
        (!asmkit_text_cstr_eq(mnemonic, "b") && !asmkit_text_cstr_eq(mnemonic, "bl")) ||
        (inst->flags & ASMKIT_INST_FLAG_CONDITIONAL) == 0u) {
        return 0;
    }
    condition = 0xffu;
    if (inst->mode == ASMKIT_MODE_ARM_A32 && inst->size >= 4u) {
        word = asmkit_load32le(inst->bytes);
        condition = word >> 28u;
    } else if (inst->mode == ASMKIT_MODE_ARM_THUMB && inst->size >= 2u) {
        h1 = asmkit_load16le(inst->bytes);
        if ((h1 & 0xf000u) == 0xd000u) {
            condition = (uint32_t)((h1 >> 8u) & 0x0fu);
        } else if (inst->size >= 4u) {
            h2 = asmkit_load16le(inst->bytes + 2u);
            if ((h1 & 0xf800u) == 0xf000u && (h2 & 0xd000u) == 0x8000u) {
                condition = (uint32_t)((h1 >> 6u) & 0x0fu);
            }
        }
    }
    if (condition >= 0x0eu) {
        return 0;
    }
    condition_name = asmkit_text_aarch64_condition_name(condition);
    if (condition_name == 0) {
        return 0;
    }
    asmkit_text_builder_append_cstr(builder, mnemonic);
    asmkit_text_builder_append_cstr(builder, condition_name);
    return 1;
}

static int asmkit_text_append_arm_cps_mnemonic(
    asmkit_text_builder_t* builder,
    const char* mnemonic,
    const asmkit_inst_t* inst)
{
    if (inst->arch != ASMKIT_ARCH_ARM ||
        !asmkit_text_cstr_eq(mnemonic, "cps") ||
        inst->operand_count == 0u ||
        inst->operands[0].kind != ASMKIT_OP_IMM) {
        return 0;
    }
    if (inst->operands[0].imm == 1) {
        asmkit_text_builder_append_cstr(builder, "cpsid");
        return 1;
    }
    if (inst->operands[0].imm == 0) {
        asmkit_text_builder_append_cstr(builder, "cpsie");
        return 1;
    }
    return 0;
}

static int asmkit_text_append_arm_rrx_mnemonic(
    asmkit_text_builder_t* builder,
    const char* mnemonic,
    const asmkit_inst_t* inst)
{
    uint32_t set_flags;

    if (inst->arch == ASMKIT_ARCH_ARM &&
        inst->mode == ASMKIT_MODE_ARM_THUMB &&
        asmkit_text_cstr_eq(mnemonic, "rrx") &&
        inst->size >= 4u) {
        asmkit_text_builder_append_cstr(builder, (inst->bytes[0] & 0x10u) != 0u ? "rrxs" : "rrx");
        return 1;
    }
    if (inst->arch != ASMKIT_ARCH_ARM ||
        !asmkit_text_cstr_eq(mnemonic, "mov") ||
        inst->operand_count < 2u ||
        inst->operands[1].kind != ASMKIT_OP_REG ||
        inst->operands[1].shift_kind != ASMKIT_ARM_SHIFT_RRX) {
        return 0;
    }
    set_flags = 0u;
    if (inst->mode == ASMKIT_MODE_ARM_A32 && inst->size >= 4u) {
        set_flags = (asmkit_load32le(inst->bytes) & UINT32_C(0x00100000)) != 0u;
    }
    asmkit_text_builder_append_cstr(builder, set_flags ? "rrxs" : "rrx");
    return 1;
}

static int asmkit_text_append_arm_it_mnemonic(
    asmkit_text_builder_t* builder,
    const char* mnemonic,
    const asmkit_inst_t* inst)
{
    uint32_t condition;
    uint32_t mask;
    uint32_t trailing_zeroes;
    int32_t bit;
    const char* condition_name;

    if (inst->arch != ASMKIT_ARCH_ARM ||
        inst->mode != ASMKIT_MODE_ARM_THUMB ||
        !asmkit_text_cstr_eq(mnemonic, "it") ||
        inst->size < 2u) {
        return 0;
    }
    if (inst->operand_count >= 2u &&
        inst->operands[0].kind == ASMKIT_OP_IMM &&
        inst->operands[1].kind == ASMKIT_OP_IMM) {
        condition = (uint32_t)inst->operands[0].imm;
        mask = (uint32_t)inst->operands[1].imm;
    } else {
        uint16_t encoded = asmkit_load16le(inst->bytes);
        condition = (uint32_t)((encoded >> 4u) & 0x0fu);
        mask = (uint32_t)(encoded & 0x000fu);
    }
    if (mask == 0u) {
        return 0;
    }
    if (condition == 0x0fu) {
        condition = 0x0eu;
    }
    condition_name = asmkit_text_aarch64_condition_name(condition);
    if (condition_name == 0) {
        return 0;
    }
    trailing_zeroes = 0u;
    while (trailing_zeroes < 4u && (mask & (1u << trailing_zeroes)) == 0u) {
        ++trailing_zeroes;
    }
    asmkit_text_builder_append_cstr(builder, "it");
    for (bit = 3; bit > (int32_t)trailing_zeroes; --bit) {
        uint32_t mask_bit = (mask >> (uint32_t)bit) & 1u;
        asmkit_text_builder_append_char(builder, mask_bit == (condition & 1u) ? 't' : 'e');
    }
    asmkit_text_builder_append_char(builder, ' ');
    asmkit_text_builder_append_cstr(builder, condition_name);
    return 1;
}

static int asmkit_text_append_aarch64_conditional_mnemonic(
    asmkit_text_builder_t* builder,
    const char* mnemonic,
    const asmkit_inst_t* inst)
{
    const char* condition;

    if (inst->arch != ASMKIT_ARCH_AARCH64 ||
        !asmkit_text_cstr_eq(mnemonic, "b") ||
        (inst->flags & ASMKIT_INST_FLAG_CONDITIONAL) == 0u ||
        inst->operand_count == 0u ||
        inst->operands[0].kind != ASMKIT_OP_IMM) {
        return 0;
    }
    condition = asmkit_text_aarch64_condition_name((uint32_t)inst->operands[0].imm);
    if (condition == 0) {
        return 0;
    }
    asmkit_text_builder_append_cstr(builder, "b.");
    asmkit_text_builder_append_cstr(builder, condition);
    return 1;
}

static int asmkit_text_append_aarch64_bti_mnemonic(
    asmkit_text_builder_t* builder,
    const char* mnemonic,
    const asmkit_inst_t* inst)
{
    int64_t hint;

    if (inst->arch != ASMKIT_ARCH_AARCH64 || !asmkit_text_cstr_eq(mnemonic, "bti")) {
        return 0;
    }
    hint = 0;
    if (inst->operand_count > 0u && inst->operands[0].kind == ASMKIT_OP_IMM) {
        hint = inst->operands[0].imm;
    } else if (inst->size >= 4u) {
        hint = (int64_t)((asmkit_load32le(inst->bytes) >> 5u) & 0x7fu);
    }
    asmkit_text_builder_append_cstr(builder, "bti");
    if (hint == 34) {
        asmkit_text_builder_append_cstr(builder, " c");
    } else if (hint == 36) {
        asmkit_text_builder_append_cstr(builder, " j");
    } else if (hint == 38) {
        asmkit_text_builder_append_cstr(builder, " jc");
    }
    return 1;
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
    if (inst->arch == ASMKIT_ARCH_X86 && (inst->flags & ASMKIT_INST_FLAG_X86_REPNE) != 0u) {
        asmkit_text_builder_append_cstr(&builder, "repne ");
    } else if (inst->arch == ASMKIT_ARCH_X86 && (inst->flags & ASMKIT_INST_FLAG_X86_REP) != 0u) {
        if (asmkit_text_cstr_starts_with(mnemonic, "cmps") || asmkit_text_cstr_starts_with(mnemonic, "scas")) {
            asmkit_text_builder_append_cstr(&builder, "repe ");
        } else {
            asmkit_text_builder_append_cstr(&builder, "rep ");
        }
    }
    if (!asmkit_text_append_arm_cps_mnemonic(&builder, mnemonic, inst) &&
        !asmkit_text_append_arm_rrx_mnemonic(&builder, mnemonic, inst) &&
        !asmkit_text_append_arm_conditional_branch_mnemonic(&builder, mnemonic, inst) &&
        !asmkit_text_append_arm_it_mnemonic(&builder, mnemonic, inst) &&
        !asmkit_text_append_aarch64_conditional_mnemonic(&builder, mnemonic, inst) &&
        !asmkit_text_append_aarch64_bti_mnemonic(&builder, mnemonic, inst)) {
        asmkit_text_builder_append_cstr(&builder, mnemonic);
    }
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
    if (inst->operand_count == 0u && (inst->flags & (ASMKIT_INST_FLAG_X86_LOCK | ASMKIT_INST_FLAG_X86_REP | ASMKIT_INST_FLAG_X86_REPNE)) == 0u) {
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

/*
 * Generated neutral instruction corpus tests.
 * Do not edit by hand.
 */
#include <string.h>
#include "test_support.h"
#include "asmkit/asmkit_metadata.h"
#include "asmkit/target/x86.h"

#define ASMKIT_INSTRUCTION_CORPUS_TARGET_CASE_COUNT 28u
#define ASMKIT_INSTRUCTION_CORPUS_TARGET_DETAIL_CASE_COUNT 23u
#define ASMKIT_INSTRUCTION_CORPUS_TARGET_REGISTER_EFFECT_CHECK_COUNT 0u
#define ASMKIT_INSTRUCTION_CORPUS_TARGET_ARCH ASMKIT_ARCH_X86

static inline int asmkit_instruction_corpus_init_engine(asmkit_engine_t* engine, asmkit_arch_t arch, asmkit_mode_t mode)
{
    asmkit_engine_config_t config;
    ASMKIT_CHECK(asmkit_engine_config_init(&config, arch, mode) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_enable_all_features(&config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(engine, &config) == ASMKIT_OK);
    return 0;
}

static inline int asmkit_instruction_corpus_mnemonic_matches(asmkit_mnemonic_id_t actual, asmkit_mnemonic_id_t expected)
{
    if (actual == expected) {
        return 1;
    }
    switch (expected) {
    case ASMKIT_X86_VCVTPD2DQ:
        return
            actual == ASMKIT_X86_VCVTPD2DQX ||
            actual == ASMKIT_X86_VCVTPD2DQY;
    default:
        break;
    }
    return 0;
}

#if ASMKIT_INSTRUCTION_CORPUS_TARGET_DETAIL_CASE_COUNT != 0u
#define ASMKIT_CORPUS_CHECK_KIND 1u
#define ASMKIT_CORPUS_CHECK_REG 2u
#define ASMKIT_CORPUS_CHECK_IMM 4u
#define ASMKIT_CORPUS_CHECK_MEM_BASE 8u
#define ASMKIT_CORPUS_CHECK_MEM_INDEX 16u
#define ASMKIT_CORPUS_CHECK_MEM_DISP 32u
#define ASMKIT_CORPUS_CHECK_MEM_SCALE 64u
#define ASMKIT_CORPUS_CHECK_WIDTH 128u
#define ASMKIT_CORPUS_CHECK_FLAGS 256u
#define ASMKIT_CORPUS_CHECK_MEM_SEGMENT 512u

static inline uint32_t asmkit_instruction_corpus_arm_reg_bit(uint64_t reg)
{
#ifdef ASMKIT_TARGET_ARM_H
    if (reg >= ASMKIT_ARM_REG_R0 && reg <= ASMKIT_ARM_REG_PC) {
        return (uint32_t)(reg - ASMKIT_ARM_REG_R0);
    }
#else
    (void)reg;
#endif
    return UINT32_MAX;
}

static inline uint32_t asmkit_instruction_corpus_arm_s_reg_number(uint64_t reg)
{
#ifdef ASMKIT_TARGET_ARM_H
    switch (reg) {
    case ASMKIT_ARM_REG_S0: return 0u;
    case ASMKIT_ARM_REG_S1: return 1u;
    case ASMKIT_ARM_REG_S2: return 2u;
    case ASMKIT_ARM_REG_S3: return 3u;
    case ASMKIT_ARM_REG_S4: return 4u;
    case ASMKIT_ARM_REG_S5: return 5u;
    case ASMKIT_ARM_REG_S6: return 6u;
    case ASMKIT_ARM_REG_S7: return 7u;
    case ASMKIT_ARM_REG_S8: return 8u;
    case ASMKIT_ARM_REG_S9: return 9u;
    case ASMKIT_ARM_REG_S10: return 10u;
    case ASMKIT_ARM_REG_S11: return 11u;
    case ASMKIT_ARM_REG_S12: return 12u;
    case ASMKIT_ARM_REG_S13: return 13u;
    case ASMKIT_ARM_REG_S14: return 14u;
    case ASMKIT_ARM_REG_S15: return 15u;
    case ASMKIT_ARM_REG_S16: return 16u;
    case ASMKIT_ARM_REG_S17: return 17u;
    case ASMKIT_ARM_REG_S18: return 18u;
    case ASMKIT_ARM_REG_S19: return 19u;
    case ASMKIT_ARM_REG_S20: return 20u;
    case ASMKIT_ARM_REG_S21: return 21u;
    case ASMKIT_ARM_REG_S22: return 22u;
    case ASMKIT_ARM_REG_S23: return 23u;
    case ASMKIT_ARM_REG_S24: return 24u;
    case ASMKIT_ARM_REG_S25: return 25u;
    case ASMKIT_ARM_REG_S26: return 26u;
    case ASMKIT_ARM_REG_S27: return 27u;
    case ASMKIT_ARM_REG_S28: return 28u;
    case ASMKIT_ARM_REG_S29: return 29u;
    case ASMKIT_ARM_REG_S30: return 30u;
    case ASMKIT_ARM_REG_S31: return 31u;
    default: break;
    }
#else
    (void)reg;
#endif
    return UINT32_MAX;
}

static inline uint32_t asmkit_instruction_corpus_arm_d_reg_number(uint64_t reg)
{
#ifdef ASMKIT_TARGET_ARM_H
    if (reg >= ASMKIT_ARM_REG_D0 && reg <= ASMKIT_ARM_REG_D31) {
        return (uint32_t)(reg - ASMKIT_ARM_REG_D0);
    }
#else
    (void)reg;
#endif
    return UINT32_MAX;
}

static inline int asmkit_instruction_corpus_is_arm_pc(uint64_t reg)
{
#ifdef ASMKIT_TARGET_ARM_H
    return reg == ASMKIT_ARM_REG_PC;
#else
    (void)reg;
    return 0;
#endif
}

static inline int asmkit_instruction_corpus_reg_list_contains(const asmkit_operand_t* operand, uint64_t expected_reg)
{
    uint32_t bit = asmkit_instruction_corpus_arm_reg_bit(expected_reg);
    uint32_t number;
    uint32_t start;
    uint32_t count;
    if ((operand->flags & ASMKIT_OPERAND_FLAG_REGISTER_LIST) == 0u) {
        return 0;
    }
    if (bit < 32u && ((operand->reg >> bit) & 1u) != 0u) {
        return 1;
    }
    start = (uint32_t)((operand->reg >> 8u) & 0xffu);
    number = asmkit_instruction_corpus_arm_s_reg_number(expected_reg);
    if (number != UINT32_MAX) {
        count = (uint32_t)(operand->reg & 0xffu);
        return count != 0u && number >= start && number < start + count;
    }
    number = asmkit_instruction_corpus_arm_d_reg_number(expected_reg);
    if (number != UINT32_MAX) {
        count = (uint32_t)((operand->reg >> 1u) & 0x7fu);
        return count != 0u && number >= start && number < start + count;
    }
#ifdef ASMKIT_TARGET_ARM_H
    if (expected_reg == ASMKIT_ARM_REG_VPR) {
        return (operand->reg & 0xffu) != 0u;
    }
#endif
    return 0;
}

static inline uint8_t asmkit_instruction_corpus_vec_count(const asmkit_operand_t* operand)
{
    return (uint8_t)((uint32_t)operand->imm & 0xffu);
}

static inline uint8_t asmkit_instruction_corpus_vec_stride(const asmkit_operand_t* operand)
{
    uint8_t stride = (uint8_t)(((uint32_t)operand->imm >> 8u) & 0xffu);
    return stride == 0u ? 1u : stride;
}

static inline int asmkit_instruction_corpus_vec_list_contains(const asmkit_operand_t* operand, uint64_t expected_reg)
{
    const asmkit_register_info_t* base_info;
    const asmkit_register_info_t* expected_info;
    uint64_t reg;
    uint8_t count;
    uint8_t stride;
    uint8_t index;
    if ((operand->flags & ASMKIT_OPERAND_FLAG_VECTOR_LIST) == 0u) {
        return 0;
    }
    if (asmkit_get_register_info(ASMKIT_INSTRUCTION_CORPUS_TARGET_ARCH, (uint32_t)operand->reg, &base_info) == ASMKIT_OK &&
        asmkit_get_register_info(ASMKIT_INSTRUCTION_CORPUS_TARGET_ARCH, (uint32_t)expected_reg, &expected_info) == ASMKIT_OK &&
        base_info != 0 && expected_info != 0 && base_info->width == expected_info->width) {
        count = asmkit_instruction_corpus_vec_count(operand);
        stride = asmkit_instruction_corpus_vec_stride(operand);
        for (index = 0u; index < count; ++index) {
            uint32_t encoding = (uint32_t)base_info->encoding + (uint32_t)index * (uint32_t)stride;
            if (expected_info->encoding == encoding) {
                return 1;
            }
        }
    }
    if (expected_reg < operand->reg) {
        return 0;
    }
    count = asmkit_instruction_corpus_vec_count(operand);
    stride = asmkit_instruction_corpus_vec_stride(operand);
    for (index = 0u; index < count; ++index) {
        reg = operand->reg + (uint64_t)index * (uint64_t)stride;
        if (reg == expected_reg) {
            return 1;
        }
    }
    return 0;
}

static inline int asmkit_instruction_corpus_reg_operand_matches(const asmkit_operand_t* operand, uint64_t expected_reg)
{
    if (operand->kind == ASMKIT_OP_PC_REL && asmkit_instruction_corpus_is_arm_pc(expected_reg)) {
        return 1;
    }
    if (operand->kind == ASMKIT_OP_MEM) {
        return operand->mem.base == expected_reg || operand->mem.index == expected_reg || operand->mem.segment == expected_reg;
    }
    if (operand->kind != ASMKIT_OP_REG) {
        return 0;
    }
    return operand->reg == expected_reg ||
        asmkit_instruction_corpus_reg_list_contains(operand, expected_reg) ||
        asmkit_instruction_corpus_vec_list_contains(operand, expected_reg);
}

static inline int asmkit_instruction_corpus_find_reg(const asmkit_inst_t* inst, uint32_t start, uint64_t expected_reg, uint32_t* out_index)
{
    uint32_t index;
    for (index = start; index < inst->operand_count && index < ASMKIT_MAX_OPERANDS; ++index) {
        const asmkit_operand_t* operand = &inst->operands[index];
        if (asmkit_instruction_corpus_reg_operand_matches(operand, expected_reg)) {
            *out_index = index;
            return 1;
        }
    }
    return 0;
}

static inline int asmkit_instruction_corpus_find_imm(const asmkit_inst_t* inst, uint32_t start, int64_t expected_imm, uint32_t* out_index)
{
    uint32_t index;
    for (index = start; index < inst->operand_count && index < ASMKIT_MAX_OPERANDS; ++index) {
        const asmkit_operand_t* operand = &inst->operands[index];
        if (operand->kind == ASMKIT_OP_IMM && operand->imm == expected_imm) {
            *out_index = index;
            return 1;
        }
    }
    return 0;
}

static inline int asmkit_instruction_corpus_find_mem_index(const asmkit_inst_t* inst, uint32_t start, uint64_t expected_reg, uint32_t* out_index)
{
    uint32_t index;
    for (index = start; index < inst->operand_count && index < ASMKIT_MAX_OPERANDS; ++index) {
        const asmkit_operand_t* operand = &inst->operands[index];
        if (operand->kind == ASMKIT_OP_MEM && operand->mem.index == expected_reg) {
            *out_index = index;
            return 1;
        }
    }
    return 0;
}

static inline int asmkit_instruction_corpus_find_mem_disp(const asmkit_inst_t* inst, uint32_t start, int64_t expected_disp, uint32_t* out_index)
{
    uint32_t index;
    for (index = start; index < inst->operand_count && index < ASMKIT_MAX_OPERANDS; ++index) {
        const asmkit_operand_t* operand = &inst->operands[index];
        if (operand->kind == ASMKIT_OP_MEM && operand->mem.displacement == expected_disp) {
            *out_index = index;
            return 1;
        }
    }
    return 0;
}

static inline void asmkit_instruction_corpus_advance_cursor(uint32_t* cursor, uint32_t consumed_index)
{
    uint32_t next = consumed_index + 1u;
    if (*cursor < next) {
        *cursor = next;
    }
}

static inline int asmkit_instruction_corpus_operand_scale_matches(const asmkit_operand_t* operand, uint32_t mem_scale)
{
    if (mem_scale == 0u || mem_scale == 1u) {
        return 1;
    }
    if (operand->kind == ASMKIT_OP_MEM && operand->mem.scale == mem_scale) {
        return 1;
    }
    if (operand->kind == ASMKIT_OP_REG && operand->shift_amount < 31u &&
        (UINT32_C(1) << operand->shift_amount) == mem_scale) {
        return 1;
    }
    return 0;
}

static inline int asmkit_instruction_corpus_operand_matches(
    const asmkit_inst_t* inst, uint32_t actual_index, uint32_t checks, asmkit_operand_kind_t kind,
    uint64_t reg, int64_t imm, uint64_t mem_base, uint64_t mem_index, uint64_t mem_segment, int64_t mem_disp,
    uint32_t mem_scale, uint32_t width, uint32_t flags,
    uint32_t* next_cursor)
{
    const asmkit_operand_t* operand;
    uint32_t found_index = 0u;
    int split_mem_expected;
    int split_mem_used_current;
    if (actual_index >= inst->operand_count || actual_index >= ASMKIT_MAX_OPERANDS) {
        return 0;
    }
    operand = &inst->operands[actual_index];
    split_mem_expected = kind == ASMKIT_OP_MEM &&
        (checks & (ASMKIT_CORPUS_CHECK_MEM_BASE | ASMKIT_CORPUS_CHECK_MEM_INDEX | ASMKIT_CORPUS_CHECK_MEM_DISP | ASMKIT_CORPUS_CHECK_MEM_SCALE)) != 0u &&
        operand->kind != ASMKIT_OP_MEM;
    if ((checks & ASMKIT_CORPUS_CHECK_KIND) != 0u) {
        if (operand->kind != kind) {
            if (!split_mem_expected &&
                !(kind == ASMKIT_OP_IMM && operand->kind == ASMKIT_OP_PC_REL) &&
                !(kind == ASMKIT_OP_REG && asmkit_instruction_corpus_is_arm_pc(reg) && operand->kind == ASMKIT_OP_PC_REL) &&
                !(kind == ASMKIT_OP_REG && operand->kind == ASMKIT_OP_MEM && asmkit_instruction_corpus_reg_operand_matches(operand, reg)) &&
                !(kind == ASMKIT_OP_IMM && operand->kind == ASMKIT_OP_MEM && operand->mem.displacement == imm)) {
                return 0;
            }
        }
    }
    if ((checks & ASMKIT_CORPUS_CHECK_REG) != 0u) {
        if (operand->kind == ASMKIT_OP_PC_REL && asmkit_instruction_corpus_is_arm_pc(reg)) {
            return 1;
        }
        if (!asmkit_instruction_corpus_reg_operand_matches(operand, reg)) {
            return 0;
        }
    }
    if ((checks & ASMKIT_CORPUS_CHECK_IMM) != 0u) {
        if (operand->kind == ASMKIT_OP_PC_REL) {
            return 1;
        }
        if (operand->kind == ASMKIT_OP_WASM_INDEX) {
            if (operand->imm != imm) {
                return 0;
            }
        } else
        if (operand->kind == ASMKIT_OP_MEM) {
            if (operand->mem.displacement != imm) {
                return 0;
            }
        } else
        if (operand->kind != ASMKIT_OP_IMM || operand->imm != imm) {
            return 0;
        }
    }
    if ((checks & ASMKIT_CORPUS_CHECK_FLAGS) != 0u && (operand->flags & flags) != flags) {
        return 0;
    }
    if ((checks & (ASMKIT_CORPUS_CHECK_MEM_BASE | ASMKIT_CORPUS_CHECK_MEM_INDEX | ASMKIT_CORPUS_CHECK_MEM_DISP | ASMKIT_CORPUS_CHECK_MEM_SCALE | ASMKIT_CORPUS_CHECK_MEM_SEGMENT)) != 0u) {
        if (operand->kind == ASMKIT_OP_MEM) {
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_BASE) != 0u && operand->mem.base != mem_base) {
                return 0;
            }
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_SEGMENT) != 0u && operand->mem.segment != mem_segment) {
                return 0;
            }
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_INDEX) != 0u && operand->mem.index != mem_index &&
                !asmkit_instruction_corpus_find_reg(inst, actual_index + 1u, mem_index, &found_index) &&
                !asmkit_instruction_corpus_find_mem_index(inst, actual_index + 1u, mem_index, &found_index)) {
                return 0;
            }
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_INDEX) != 0u && operand->mem.index != mem_index) {
                asmkit_instruction_corpus_advance_cursor(next_cursor, found_index);
            }
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_DISP) != 0u && operand->mem.displacement != mem_disp &&
                !(operand->mem.displacement == 0 &&
                  (asmkit_instruction_corpus_find_imm(inst, actual_index + 1u, mem_disp, &found_index) ||
                   asmkit_instruction_corpus_find_mem_disp(inst, actual_index + 1u, mem_disp, &found_index)))) {
                return 0;
            }
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_DISP) != 0u && operand->mem.displacement != mem_disp) {
                asmkit_instruction_corpus_advance_cursor(next_cursor, found_index);
            }
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_SCALE) != 0u && operand->mem.scale != mem_scale) {
                return 0;
            }
        } else {
            split_mem_used_current = 0;
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_BASE) != 0u) {
                if (!asmkit_instruction_corpus_reg_operand_matches(operand, mem_base)) {
                    return 0;
                }
                split_mem_used_current = 1;
            }
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_INDEX) != 0u) {
                if (!split_mem_used_current && asmkit_instruction_corpus_reg_operand_matches(operand, mem_index)) {
                    split_mem_used_current = 1;
                } else
                if (!asmkit_instruction_corpus_find_reg(inst, actual_index + 1u, mem_index, &found_index) &&
                    !asmkit_instruction_corpus_find_mem_index(inst, actual_index + 1u, mem_index, &found_index)) {
                    return 0;
                } else {
                    asmkit_instruction_corpus_advance_cursor(next_cursor, found_index);
                }
            }
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_DISP) != 0u) {
                if (mem_disp != 0) {
                    if (!split_mem_used_current && operand->kind == ASMKIT_OP_IMM && operand->imm == mem_disp) {
                        split_mem_used_current = 1;
                    } else
                    if (!asmkit_instruction_corpus_find_imm(inst, actual_index + 1u, mem_disp, &found_index) &&
                        !asmkit_instruction_corpus_find_mem_disp(inst, actual_index + 1u, mem_disp, &found_index)) {
                        return 0;
                    } else {
                        asmkit_instruction_corpus_advance_cursor(next_cursor, found_index);
                    }
                }
            }
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_SCALE) != 0u && !asmkit_instruction_corpus_operand_scale_matches(operand, mem_scale)) {
                if (!asmkit_instruction_corpus_find_reg(inst, actual_index + 1u, mem_index, &found_index) ||
                    !asmkit_instruction_corpus_operand_scale_matches(&inst->operands[found_index], mem_scale)) {
                    return 0;
                }
                asmkit_instruction_corpus_advance_cursor(next_cursor, found_index);
            }
            if (!split_mem_used_current && mem_disp == 0) {
                return 0;
            }
        }
    }
    if ((checks & ASMKIT_CORPUS_CHECK_WIDTH) != 0u &&
        !(kind == ASMKIT_OP_MEM && operand->kind != ASMKIT_OP_MEM) &&
        operand->width != width) {
        return 0;
    }
    return 1;
}

#if ASMKIT_INSTRUCTION_CORPUS_TARGET_REGISTER_EFFECT_CHECK_COUNT != 0u
static inline uint32_t asmkit_instruction_corpus_operand_register_effect_flags(
    const asmkit_operand_t* operand, uint64_t expected_reg)
{
    uint32_t effect_flags;
    if (!asmkit_instruction_corpus_reg_operand_matches(operand, expected_reg)) {
        return 0;
    }
    if (operand->kind == ASMKIT_OP_MEM &&
        (operand->mem.base == expected_reg || operand->mem.index == expected_reg || operand->mem.segment == expected_reg)) {
        effect_flags = ASMKIT_REGISTER_EFFECT_READ;
        if ((operand->flags & ASMKIT_OPERAND_FLAG_WRITE) != 0u &&
            (operand->mem.base == expected_reg || operand->mem.index == expected_reg)) {
            effect_flags |= ASMKIT_REGISTER_EFFECT_WRITE;
        }
        return effect_flags;
    }
    if (operand->kind == ASMKIT_OP_PC_REL && asmkit_instruction_corpus_is_arm_pc(expected_reg)) {
        return ASMKIT_REGISTER_EFFECT_READ;
    }
    effect_flags = 0u;
    if ((operand->flags & ASMKIT_OPERAND_FLAG_READ) != 0u) {
        effect_flags |= ASMKIT_REGISTER_EFFECT_READ;
    }
    if ((operand->flags & ASMKIT_OPERAND_FLAG_WRITE) != 0u) {
        effect_flags |= ASMKIT_REGISTER_EFFECT_WRITE;
    }
    return effect_flags;
}

static inline int asmkit_instruction_corpus_x86_flags_register(uint64_t reg)
{
#ifdef ASMKIT_TARGET_X86_H
    return reg == ASMKIT_X86_REG_EFLAGS || reg == ASMKIT_X86_REG_RFLAGS;
#else
    (void)reg;
    return 0;
#endif
}

static inline int asmkit_instruction_corpus_register_effect_reg_matches(asmkit_arch_t arch, uint64_t actual_reg, uint64_t expected_reg)
{
    const asmkit_register_info_t* actual_info;
    const asmkit_register_info_t* expected_info;
    if (actual_reg == expected_reg) {
        return 1;
    }
    if (arch == ASMKIT_ARCH_X86 && asmkit_instruction_corpus_x86_flags_register(actual_reg) && asmkit_instruction_corpus_x86_flags_register(expected_reg)) {
        return 1;
    }
    if (asmkit_get_register_info(arch, (uint32_t)actual_reg, &actual_info) != ASMKIT_OK ||
        asmkit_get_register_info(arch, (uint32_t)expected_reg, &expected_info) != ASMKIT_OK ||
        actual_info == 0 || expected_info == 0) {
        return 0;
    }
    if (actual_info->arch != expected_info->arch || actual_info->encoding != expected_info->encoding) {
        return 0;
    }
    if (actual_info->width == 0u || expected_info->width == 0u) {
        return 0;
    }
    if (actual_info->width <= expected_info->width) {
        return 1;
    }
    return arch == ASMKIT_ARCH_X86 && expected_info->width == 32u && actual_info->width == 64u;
}

static inline int asmkit_instruction_corpus_effect_flags_satisfied(uint32_t seen_flags, uint32_t expected_flags)
{
    return (seen_flags & expected_flags) == expected_flags;
}

static inline int asmkit_instruction_corpus_has_register_effect(
    const asmkit_inst_t* inst, uint64_t expected_reg, uint32_t flags)
{
    const asmkit_instruction_register_effect_t* effect;
    uint32_t seen_flags;
    uint32_t index;
    seen_flags = 0u;
    for (index = 0u; index < inst->operand_count && index < ASMKIT_MAX_OPERANDS; ++index) {
        seen_flags |= asmkit_instruction_corpus_operand_register_effect_flags(&inst->operands[index], expected_reg);
        if (asmkit_instruction_corpus_effect_flags_satisfied(seen_flags, flags)) {
            return 1;
        }
    }
    for (index = 0u; asmkit_get_instruction_register_effect(inst->arch, inst->id, index, &effect) == ASMKIT_OK; ++index) {
        if (effect != 0 && asmkit_instruction_corpus_register_effect_reg_matches(inst->arch, effect->register_id, expected_reg)) {
            seen_flags |= effect->flags;
        }
        if (asmkit_instruction_corpus_effect_flags_satisfied(seen_flags, flags)) {
            return 1;
        }
    }
    return 0;
}

#endif

static inline int asmkit_instruction_corpus_composite_operand(const asmkit_operand_t* operand)
{
    return operand->kind == ASMKIT_OP_PC_REL ||
        operand->kind == ASMKIT_OP_MEM ||
        (operand->kind == ASMKIT_OP_REG &&
         (operand->flags & (ASMKIT_OPERAND_FLAG_REGISTER_LIST | ASMKIT_OPERAND_FLAG_VECTOR_LIST)) != 0u);
}

static inline int asmkit_instruction_corpus_expect_operand(
    const asmkit_inst_t* inst, uint32_t* cursor, uint32_t checks, asmkit_operand_kind_t kind,
    uint64_t reg, int64_t imm, uint64_t mem_base, uint64_t mem_index, uint64_t mem_segment, int64_t mem_disp,
    uint32_t mem_scale, uint32_t width, uint32_t flags)
{
    uint32_t index;
    uint32_t next_cursor;
    for (index = *cursor; index < inst->operand_count && index < ASMKIT_MAX_OPERANDS; ++index) {
        next_cursor = asmkit_instruction_corpus_composite_operand(&inst->operands[index]) ? index : index + 1u;
        if (asmkit_instruction_corpus_operand_matches(inst, index, checks, kind, reg, imm, mem_base, mem_index, mem_segment, mem_disp, mem_scale, width, flags, &next_cursor)) {
            *cursor = next_cursor;
            return 1;
        }
    }
    if ((checks & ASMKIT_CORPUS_CHECK_REG) != 0u && kind == ASMKIT_OP_REG &&
        *cursor < inst->operand_count && inst->operands[*cursor].kind == ASMKIT_OP_MEM) {
        for (index = 0u; index < *cursor && index < ASMKIT_MAX_OPERANDS; ++index) {
            next_cursor = *cursor;
            if (inst->operands[index].kind != ASMKIT_OP_MEM &&
                asmkit_instruction_corpus_operand_matches(inst, index, checks, kind, reg, imm, mem_base, mem_index, mem_segment, mem_disp, mem_scale, width, flags, &next_cursor)) {
                return 1;
            }
        }
    }
    return 0;
}

#endif

static int asmkit_instruction_corpus_x86_16_chunk_000(
    asmkit_engine_t* engine,
    uint32_t* decoded_count,
    uint32_t* roundtrip_count,
    uint32_t* detail_count)
{
    asmkit_inst_t inst;
    asmkit_status_t status;
    uint8_t out[ASMKIT_MAX_INST_BYTES];
    asmkit_encode_result_t encode_result;
    asmkit_status_t encode_status;
    uint32_t operand_cursor;

    {
        uint8_t asmkit_instruction_corpus_x86_16_00000[] = {0x91u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00000, sizeof(asmkit_instruction_corpus_x86_16_00000), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00000));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00000, inst.size) == 0);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00001[] = {0x33u, 0xc0u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00001, sizeof(asmkit_instruction_corpus_x86_16_00001), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00001));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00001, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_XOR) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00002[] = {0x87u, 0xc0u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00002, sizeof(asmkit_instruction_corpus_x86_16_00002), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00002));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00002, inst.size) == 0);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00003[] = {0xffu, 0x1bu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00003, sizeof(asmkit_instruction_corpus_x86_16_00003), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00003));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00003, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_CALL) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00004[] = {0xa0u, 0x03u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00004, sizeof(asmkit_instruction_corpus_x86_16_00004), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00004));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00004, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_MOV) != 0);
        ASMKIT_CHECK(inst.operand_count >= 2u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 131u, ASMKIT_OP_REG,
            ASMKIT_X86_REG_AL, INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 8u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 225u, ASMKIT_OP_MEM,
            UINT64_C(0), INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(3), 1u, 8u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00005[] = {0xbau, 0x5au, 0xffu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00005, sizeof(asmkit_instruction_corpus_x86_16_00005), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00005));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00005, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_MOV) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00006[] = {0xe8u, 0x35u, 0x64u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00006, sizeof(asmkit_instruction_corpus_x86_16_00006), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00006));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00006, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_CALL) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00007[] = {0xe9u, 0x35u, 0x64u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00007, sizeof(asmkit_instruction_corpus_x86_16_00007), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00007));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00007, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_JMP) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00008[] = {0xffu, 0x50u, 0xffu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00008, sizeof(asmkit_instruction_corpus_x86_16_00008), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00008));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00008, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_CALL) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00009[] = {0x00u, 0x9eu, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00009, sizeof(asmkit_instruction_corpus_x86_16_00009), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00009));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00009, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_ADD) != 0);
        ASMKIT_CHECK(inst.operand_count >= 2u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 233u, ASMKIT_OP_MEM,
            UINT64_C(0), INT64_C(0),
            ASMKIT_X86_REG_BP, UINT64_C(0), UINT64_C(0),
            INT64_C(0), 1u, 8u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 131u, ASMKIT_OP_REG,
            ASMKIT_X86_REG_BL, INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 8u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00010[] = {0x26u, 0x26u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00010, sizeof(asmkit_instruction_corpus_x86_16_00010), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00010));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00010, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_ADD) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00011[] = {0x3eu, 0xe8u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00011, sizeof(asmkit_instruction_corpus_x86_16_00011), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00011));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00011, inst.size) == 0);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00012[] = {0x64u, 0x3eu, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00012, sizeof(asmkit_instruction_corpus_x86_16_00012), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00012));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00012, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_ADD) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00013[] = {0xc5u, 0xc5u, 0xd9u, 0xd9u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00013, sizeof(asmkit_instruction_corpus_x86_16_00013), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00013));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00013, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_VPSUBUSW) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00014[] = {0xd3u, 0x26u, 0x00u, 0xd3u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00014, sizeof(asmkit_instruction_corpus_x86_16_00014), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00014));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00014, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_SHL) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00015[] = {0x2eu, 0xc5u, 0xffu, 0xe6u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00015, sizeof(asmkit_instruction_corpus_x86_16_00015), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00015));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00015, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_VCVTPD2DQ) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00016[] = {0xeau, 0xaau, 0xffu, 0x00u, 0xf0u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00016, sizeof(asmkit_instruction_corpus_x86_16_00016), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00016));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00016, inst.size) == 0);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00017[] = {0xf2u, 0x0fu, 0x38u, 0xf1u, 0x0du};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00017, sizeof(asmkit_instruction_corpus_x86_16_00017), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00017));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00017, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_CRC32) != 0);
        ASMKIT_CHECK(inst.operand_count >= 2u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 131u, ASMKIT_OP_REG,
            ASMKIT_X86_REG_ECX, INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 32u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 233u, ASMKIT_OP_MEM,
            UINT64_C(0), INT64_C(0),
            ASMKIT_X86_REG_DI, UINT64_C(0), UINT64_C(0),
            INT64_C(0), 1u, 16u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00018[] = {0x66u, 0x66u, 0x66u, 0xc2u, 0xb6u, 0xb6u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00018, sizeof(asmkit_instruction_corpus_x86_16_00018), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00018));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00018, inst.size) == 0);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00019[] = {0x66u, 0xe8u, 0x35u, 0x64u, 0x93u, 0x53u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00019, sizeof(asmkit_instruction_corpus_x86_16_00019), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00019));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00019, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_CALL) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00020[] = {0x66u, 0xe9u, 0x35u, 0x64u, 0x93u, 0x53u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00020, sizeof(asmkit_instruction_corpus_x86_16_00020), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00020));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00020, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_JMP) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00021[] = {0x67u, 0x67u, 0x67u, 0x67u, 0x67u, 0xaau};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00021, sizeof(asmkit_instruction_corpus_x86_16_00021), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00021));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00021, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_STOSB) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00022[] = {0xc4u, 0xc3u, 0xf9u, 0x61u, 0xe1u, 0xc4u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00022, sizeof(asmkit_instruction_corpus_x86_16_00022), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00022));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00022, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_VPCMPESTRI) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00023[] = {0x67u, 0x00u, 0x05u, 0x00u, 0x00u, 0x66u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00023, sizeof(asmkit_instruction_corpus_x86_16_00023), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00023));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00023, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_ADD) != 0);
        ASMKIT_CHECK(inst.operand_count >= 2u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 225u, ASMKIT_OP_MEM,
            UINT64_C(0), INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(6684672), 1u, 8u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 131u, ASMKIT_OP_REG,
            ASMKIT_X86_REG_AL, INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 8u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00024[] = {0x67u, 0x00u, 0x8bu, 0x00u, 0x00u, 0x10u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00024, sizeof(asmkit_instruction_corpus_x86_16_00024), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00024));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00024, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_ADD) != 0);
        ASMKIT_CHECK(inst.operand_count >= 2u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 233u, ASMKIT_OP_MEM,
            UINT64_C(0), INT64_C(0),
            ASMKIT_X86_REG_EBX, UINT64_C(0), UINT64_C(0),
            INT64_C(1048576), 1u, 8u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 131u, ASMKIT_OP_REG,
            ASMKIT_X86_REG_CL, INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 8u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00025[] = {0x67u, 0x8du, 0x99u, 0x9au, 0x2du, 0x9bu, 0x34u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00025, sizeof(asmkit_instruction_corpus_x86_16_00025), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00025));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00025, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_LEA) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00026[] = {0x26u, 0x65u, 0x64u, 0x3eu, 0x65u, 0x2eu, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00026, sizeof(asmkit_instruction_corpus_x86_16_00026), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00026));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00026, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_ADD) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    {
        uint8_t asmkit_instruction_corpus_x86_16_00027[] = {0x3eu, 0x3eu, 0x26u, 0x36u, 0x64u, 0x36u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_x86_16_00027, sizeof(asmkit_instruction_corpus_x86_16_00027), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_16);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_x86_16_00027));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_x86_16_00027, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_X86_ADD) != 0);
        ++(*detail_count);
        memset(out, 0, sizeof(out));
        memset(&encode_result, 0, sizeof(encode_result));
        encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
        if (encode_status == ASMKIT_OK &&
            encode_result.size == inst.size &&
            memcmp(out, inst.bytes, inst.size) == 0) {
            ++(*roundtrip_count);
        }
        ++(*decoded_count);
    }

    return 0;
}

int asmkit_test_instruction_corpus_x86_16(void)
{
    asmkit_engine_t engine;
    uint32_t decoded_count;
    uint32_t roundtrip_count;
    uint32_t detail_count;
    int failures;

    ASMKIT_CHECK(asmkit_instruction_corpus_init_engine(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_16) == 0);
    decoded_count = 0u;
    roundtrip_count = 0u;
    detail_count = 0u;
    failures = 0;
    failures += asmkit_instruction_corpus_x86_16_chunk_000(
        &engine,
        &decoded_count,
        &roundtrip_count,
        &detail_count);
    ASMKIT_CHECK(failures == 0);
    ASMKIT_CHECK(decoded_count == ASMKIT_INSTRUCTION_CORPUS_TARGET_CASE_COUNT);
    ASMKIT_CHECK(detail_count == ASMKIT_INSTRUCTION_CORPUS_TARGET_DETAIL_CASE_COUNT);
    ASMKIT_CHECK(roundtrip_count <= decoded_count);
    return 0;
}

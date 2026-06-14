/*
 * Generated neutral instruction corpus tests.
 * Do not edit by hand.
 */
#include <string.h>
#include "test_support.h"
#include "asmkit/asmkit_metadata.h"
#include "asmkit/target/bpf.h"

#define ASMKIT_INSTRUCTION_CORPUS_TARGET_CASE_COUNT 92u
#define ASMKIT_INSTRUCTION_CORPUS_TARGET_DETAIL_CASE_COUNT 35u
#define ASMKIT_INSTRUCTION_CORPUS_TARGET_REGISTER_EFFECT_CHECK_COUNT 7u
#define ASMKIT_INSTRUCTION_CORPUS_TARGET_ARCH ASMKIT_ARCH_BPF

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

static int asmkit_instruction_corpus_bpf64_chunk_000(
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
        uint8_t asmkit_instruction_corpus_bpf64_00000[] = {0x05u, 0xc7u, 0x71u, 0xb0u, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00000, sizeof(asmkit_instruction_corpus_bpf64_00000), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00000));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00000, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JA) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00001[] = {0x06u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00001, sizeof(asmkit_instruction_corpus_bpf64_00001), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00001));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00001, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00002[] = {0x15u, 0x6au, 0x9fu, 0x38u, 0x1au, 0x9du, 0xb7u, 0x4du};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00002, sizeof(asmkit_instruction_corpus_bpf64_00002), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00002));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00002, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JEQ) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00003[] = {0x16u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00003, sizeof(asmkit_instruction_corpus_bpf64_00003), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00003));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00003, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00004[] = {0x1du, 0x21u, 0x20u, 0x4du, 0xe3u, 0x47u, 0xafu, 0x1bu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00004, sizeof(asmkit_instruction_corpus_bpf64_00004), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00004));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00004, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JEQ) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00005[] = {0x20u, 0xc7u, 0x0cu, 0x70u, 0xdau, 0x41u, 0x1au, 0xcau};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00005, sizeof(asmkit_instruction_corpus_bpf64_00005), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00005));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00005, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00006[] = {0x25u, 0x89u, 0xdau, 0x53u, 0x19u, 0x73u, 0x8au, 0xc0u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00006, sizeof(asmkit_instruction_corpus_bpf64_00006), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00006));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00006, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JGT) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00007[] = {0x26u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00007, sizeof(asmkit_instruction_corpus_bpf64_00007), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00007));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00007, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00008[] = {0x28u, 0x4eu, 0xb0u, 0x62u, 0xe8u, 0x48u, 0x0bu, 0x0du};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00008, sizeof(asmkit_instruction_corpus_bpf64_00008), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00008));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00008, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00009[] = {0x2du, 0x18u, 0x07u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00009, sizeof(asmkit_instruction_corpus_bpf64_00009), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00009));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00009, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JGT) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00010[] = {0x2du, 0x18u, 0x5bu, 0xfdu, 0x8fu, 0x53u, 0x3bu, 0xf0u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00010, sizeof(asmkit_instruction_corpus_bpf64_00010), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00010));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00010, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JGT) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00011[] = {0x30u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00011, sizeof(asmkit_instruction_corpus_bpf64_00011), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00011));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00011, inst.size) == 0);
        ASMKIT_CHECK(inst.operand_count >= 1u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 33u, ASMKIT_OP_MEM,
            UINT64_C(0), INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 0u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ASMKIT_CHECK(asmkit_instruction_corpus_has_register_effect(&inst, ASMKIT_BPF_REG_R0, ASMKIT_REGISTER_EFFECT_WRITE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00012[] = {0x30u, 0x5fu, 0xfeu, 0xfcu, 0x85u, 0x66u, 0x7cu, 0x4bu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00012, sizeof(asmkit_instruction_corpus_bpf64_00012), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00012));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00012, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00013[] = {0x34u, 0x46u, 0x00u, 0x00u, 0xe1u, 0x72u, 0xd4u, 0xcbu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00013, sizeof(asmkit_instruction_corpus_bpf64_00013), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00013));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00013, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_DIV) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00014[] = {0x35u, 0xa5u, 0x42u, 0xb9u, 0x5bu, 0x37u, 0xa1u, 0x3du};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00014, sizeof(asmkit_instruction_corpus_bpf64_00014), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00014));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00014, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JGE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00015[] = {0x36u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00015, sizeof(asmkit_instruction_corpus_bpf64_00015), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00015));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00015, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00016[] = {0x37u, 0x84u, 0x00u, 0x00u, 0x3bu, 0x84u, 0x55u, 0x1fu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00016, sizeof(asmkit_instruction_corpus_bpf64_00016), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00016));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00016, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00017[] = {0x3fu, 0x36u, 0x00u, 0x00u, 0x7eu, 0x07u, 0x59u, 0x7au};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00017, sizeof(asmkit_instruction_corpus_bpf64_00017), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00017));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00017, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00018[] = {0x40u, 0x95u, 0xc2u, 0x39u, 0x6bu, 0xe7u, 0xd7u, 0xc4u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00018, sizeof(asmkit_instruction_corpus_bpf64_00018), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00018));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00018, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00019[] = {0x45u, 0x12u, 0xa2u, 0xf2u, 0x14u, 0xe7u, 0x2du, 0x1eu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00019, sizeof(asmkit_instruction_corpus_bpf64_00019), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00019));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00019, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSET) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00020[] = {0x46u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00020, sizeof(asmkit_instruction_corpus_bpf64_00020), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00020));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00020, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00021[] = {0x48u, 0x7eu, 0xfbu, 0x77u, 0xebu, 0x0eu, 0x5au, 0x0du};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00021, sizeof(asmkit_instruction_corpus_bpf64_00021), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00021));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00021, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00022[] = {0x4du, 0x10u, 0x67u, 0x44u, 0x4du, 0x3fu, 0x4du, 0x8bu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00022, sizeof(asmkit_instruction_corpus_bpf64_00022), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00022));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00022, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSET) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00023[] = {0x50u, 0x38u, 0x80u, 0xf8u, 0x04u, 0x70u, 0xd1u, 0x6cu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00023, sizeof(asmkit_instruction_corpus_bpf64_00023), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00023));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00023, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00024[] = {0x55u, 0xb9u, 0xa3u, 0x80u, 0x90u, 0xbcu, 0xc8u, 0x96u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00024, sizeof(asmkit_instruction_corpus_bpf64_00024), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00024));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00024, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JNE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00025[] = {0x56u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00025, sizeof(asmkit_instruction_corpus_bpf64_00025), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00025));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00025, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00026[] = {0x5du, 0x56u, 0xa3u, 0x4cu, 0x2au, 0xc8u, 0x4au, 0xc5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00026, sizeof(asmkit_instruction_corpus_bpf64_00026), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00026));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00026, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JNE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00027[] = {0x61u, 0x28u, 0xb2u, 0xedu, 0xb8u, 0xcfu, 0xb5u, 0xe4u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00027, sizeof(asmkit_instruction_corpus_bpf64_00027), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00027));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00027, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00028[] = {0x62u, 0xa5u, 0xdfu, 0xe0u, 0x14u, 0x7du, 0x95u, 0x78u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00028, sizeof(asmkit_instruction_corpus_bpf64_00028), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00028));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00028, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00029[] = {0x63u, 0x77u, 0x2fu, 0xcfu, 0x76u, 0xb7u, 0xd3u, 0xfau};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00029, sizeof(asmkit_instruction_corpus_bpf64_00029), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00029));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00029, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00030[] = {0x65u, 0xe8u, 0x97u, 0xe1u, 0x87u, 0xbeu, 0x8fu, 0xf8u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00030, sizeof(asmkit_instruction_corpus_bpf64_00030), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00030));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00030, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSGT) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00031[] = {0x66u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00031, sizeof(asmkit_instruction_corpus_bpf64_00031), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00031));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00031, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00032[] = {0x69u, 0x14u, 0xc7u, 0x8eu, 0x0bu, 0xc1u, 0xadu, 0x69u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00032, sizeof(asmkit_instruction_corpus_bpf64_00032), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00032));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00032, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00033[] = {0x6au, 0xb5u, 0xbcu, 0x8cu, 0x4fu, 0x5cu, 0x94u, 0x01u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00033, sizeof(asmkit_instruction_corpus_bpf64_00033), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00033));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00033, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00034[] = {0x6bu, 0x34u, 0x58u, 0xf5u, 0xc8u, 0x27u, 0x9eu, 0x14u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00034, sizeof(asmkit_instruction_corpus_bpf64_00034), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00034));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00034, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00035[] = {0x6du, 0x33u, 0x17u, 0x02u, 0x00u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00035, sizeof(asmkit_instruction_corpus_bpf64_00035), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00035));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00035, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSGT) != 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_has_register_effect(&inst, ASMKIT_BPF_REG_R3, ASMKIT_REGISTER_EFFECT_READ) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00036[] = {0x6du, 0x38u, 0x69u, 0xe3u, 0xc9u, 0xacu, 0x3cu, 0xdbu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00036, sizeof(asmkit_instruction_corpus_bpf64_00036), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00036));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00036, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSGT) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00037[] = {0x71u, 0xa0u, 0xebu, 0xfbu, 0x3du, 0x6bu, 0x58u, 0x45u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00037, sizeof(asmkit_instruction_corpus_bpf64_00037), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00037));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00037, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00038[] = {0x72u, 0xe2u, 0xc1u, 0x1bu, 0x25u, 0x2fu, 0x4au, 0xdcu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00038, sizeof(asmkit_instruction_corpus_bpf64_00038), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00038));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00038, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00039[] = {0x73u, 0x44u, 0x09u, 0x0fu, 0xc1u, 0x07u, 0xa8u, 0xf4u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00039, sizeof(asmkit_instruction_corpus_bpf64_00039), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00039));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00039, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00040[] = {0x75u, 0x04u, 0x8eu, 0x18u, 0x6au, 0xccu, 0x3cu, 0x09u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00040, sizeof(asmkit_instruction_corpus_bpf64_00040), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00040));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00040, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSGE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00041[] = {0x76u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00041, sizeof(asmkit_instruction_corpus_bpf64_00041), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00041));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00041, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00042[] = {0x79u, 0xa9u, 0x5cu, 0x7bu, 0x16u, 0x1fu, 0xfbu, 0x01u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00042, sizeof(asmkit_instruction_corpus_bpf64_00042), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00042));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00042, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00043[] = {0x7au, 0xd8u, 0x6bu, 0x04u, 0x76u, 0xf0u, 0x51u, 0x75u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00043, sizeof(asmkit_instruction_corpus_bpf64_00043), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00043));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00043, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00044[] = {0x7bu, 0x72u, 0x0fu, 0x30u, 0x51u, 0x78u, 0xd2u, 0x9au};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00044, sizeof(asmkit_instruction_corpus_bpf64_00044), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00044));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00044, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00045[] = {0x7du, 0x58u, 0x52u, 0x01u, 0x90u, 0xf9u, 0x30u, 0x9au};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00045, sizeof(asmkit_instruction_corpus_bpf64_00045), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00045));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00045, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSGE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00046[] = {0x84u, 0x02u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00046, sizeof(asmkit_instruction_corpus_bpf64_00046), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00046));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00046, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_NEG) != 0);
        ASMKIT_CHECK(inst.operand_count >= 1u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 131u, ASMKIT_OP_REG,
            ASMKIT_BPF_REG_W2, INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 32u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ASMKIT_CHECK(asmkit_instruction_corpus_has_register_effect(&inst, ASMKIT_BPF_REG_W2, ASMKIT_REGISTER_EFFECT_READ | ASMKIT_REGISTER_EFFECT_WRITE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00047[] = {0x85u, 0x00u, 0x00u, 0xe2u, 0x83u, 0x3du, 0xbdu, 0xfdu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00047, sizeof(asmkit_instruction_corpus_bpf64_00047), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00047));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00047, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_CALL) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00048[] = {0x85u, 0xd3u, 0xa5u, 0xe2u, 0x83u, 0x3du, 0xbdu, 0x5du};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00048, sizeof(asmkit_instruction_corpus_bpf64_00048), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00048));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00048, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_CALL) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00049[] = {0x8du, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00049, sizeof(asmkit_instruction_corpus_bpf64_00049), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00049));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00049, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00050[] = {0x94u, 0x39u, 0x00u, 0x00u, 0x0bu, 0xd2u, 0xd1u, 0xc9u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00050, sizeof(asmkit_instruction_corpus_bpf64_00050), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00050));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00050, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_MOD) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00051[] = {0x95u, 0xf2u, 0xd1u, 0x83u, 0x53u, 0xa9u, 0x09u, 0x9fu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00051, sizeof(asmkit_instruction_corpus_bpf64_00051), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00051));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00051, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00052[] = {0x97u, 0x09u, 0x00u, 0x00u, 0x37u, 0x13u, 0x03u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00052, sizeof(asmkit_instruction_corpus_bpf64_00052), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00052));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00052, inst.size) == 0);
        ASMKIT_CHECK(inst.operand_count >= 2u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 131u, ASMKIT_OP_REG,
            ASMKIT_BPF_REG_R9, INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 64u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 5u, ASMKIT_OP_IMM,
            UINT64_C(0), INT64_C(201527),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 0u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ASMKIT_CHECK(asmkit_instruction_corpus_has_register_effect(&inst, ASMKIT_BPF_REG_R9, ASMKIT_REGISTER_EFFECT_READ | ASMKIT_REGISTER_EFFECT_WRITE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00053[] = {0x97u, 0xc8u, 0x00u, 0x00u, 0xd2u, 0x09u, 0x98u, 0x09u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00053, sizeof(asmkit_instruction_corpus_bpf64_00053), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00053));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00053, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00054[] = {0x9cu, 0x96u, 0x00u, 0x00u, 0x0fu, 0x69u, 0x13u, 0x90u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00054, sizeof(asmkit_instruction_corpus_bpf64_00054), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00054));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00054, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_MOD) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00055[] = {0x9fu, 0x35u, 0x00u, 0x00u, 0xd6u, 0x70u, 0xd9u, 0x5eu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00055, sizeof(asmkit_instruction_corpus_bpf64_00055), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00055));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00055, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00056[] = {0xa5u, 0xd4u, 0xefu, 0x79u, 0xd3u, 0xbbu, 0xdeu, 0xfdu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00056, sizeof(asmkit_instruction_corpus_bpf64_00056), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00056));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00056, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JLT) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00057[] = {0xa6u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00057, sizeof(asmkit_instruction_corpus_bpf64_00057), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00057));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00057, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00058[] = {0xb4u, 0xa1u, 0x00u, 0x00u, 0xf9u, 0x3fu, 0x77u, 0x1fu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00058, sizeof(asmkit_instruction_corpus_bpf64_00058), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00058));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00058, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_MOV) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00059[] = {0xb5u, 0x92u, 0x5du, 0x5au, 0x49u, 0x33u, 0xfcu, 0x33u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00059, sizeof(asmkit_instruction_corpus_bpf64_00059), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00059));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00059, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JLE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00060[] = {0xb6u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00060, sizeof(asmkit_instruction_corpus_bpf64_00060), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00060));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00060, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00061[] = {0xb7u, 0x70u, 0x00u, 0x00u, 0x5bu, 0x52u, 0x2au, 0x99u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00061, sizeof(asmkit_instruction_corpus_bpf64_00061), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00061));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00061, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00062[] = {0xbcu, 0x72u, 0x00u, 0x00u, 0xc9u, 0x8au, 0x56u, 0xd6u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00062, sizeof(asmkit_instruction_corpus_bpf64_00062), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00062));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00062, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_MOV) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00063[] = {0xbdu, 0x19u, 0x80u, 0xe8u, 0x29u, 0x85u, 0xcfu, 0x51u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00063, sizeof(asmkit_instruction_corpus_bpf64_00063), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00063));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00063, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JLE) != 0);
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

static int asmkit_instruction_corpus_bpf64_chunk_001(
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
        uint8_t asmkit_instruction_corpus_bpf64_00064[] = {0xbfu, 0x86u, 0x00u, 0x00u, 0xb2u, 0x6du, 0x14u, 0x03u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00064, sizeof(asmkit_instruction_corpus_bpf64_00064), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00064));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00064, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00065[] = {0xc3u, 0x1au, 0xfcu, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00065, sizeof(asmkit_instruction_corpus_bpf64_00065), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00065));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00065, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00066[] = {0xc3u, 0x1au, 0xfcu, 0xffu, 0x01u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00066, sizeof(asmkit_instruction_corpus_bpf64_00066), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00066));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00066, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00067[] = {0xc3u, 0x1au, 0xfcu, 0xffu, 0x40u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00067, sizeof(asmkit_instruction_corpus_bpf64_00067), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00067));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00067, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00068[] = {0xc3u, 0x1au, 0xfcu, 0xffu, 0x41u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00068, sizeof(asmkit_instruction_corpus_bpf64_00068), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00068));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00068, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00069[] = {0xc3u, 0x1au, 0xfcu, 0xffu, 0x50u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00069, sizeof(asmkit_instruction_corpus_bpf64_00069), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00069));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00069, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00070[] = {0xc3u, 0x1au, 0xfcu, 0xffu, 0x51u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00070, sizeof(asmkit_instruction_corpus_bpf64_00070), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00070));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00070, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00071[] = {0xc3u, 0x1au, 0xfcu, 0xffu, 0xa0u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00071, sizeof(asmkit_instruction_corpus_bpf64_00071), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00071));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00071, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00072[] = {0xc3u, 0x1au, 0xfcu, 0xffu, 0xa1u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00072, sizeof(asmkit_instruction_corpus_bpf64_00072), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00072));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00072, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00073[] = {0xc5u, 0xf2u, 0xebu, 0xe4u, 0xbau, 0xc0u, 0xceu, 0x4fu};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00073, sizeof(asmkit_instruction_corpus_bpf64_00073), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00073));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00073, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSLT) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00074[] = {0xc6u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00074, sizeof(asmkit_instruction_corpus_bpf64_00074), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00074));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00074, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00075[] = {0xcdu, 0x90u, 0x67u, 0x88u, 0x6bu, 0xd0u, 0x27u, 0xf4u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00075, sizeof(asmkit_instruction_corpus_bpf64_00075), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00075));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00075, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSLT) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00076[] = {0xd5u, 0xe9u, 0xf6u, 0xb2u, 0x50u, 0xfdu, 0xb0u, 0xe5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00076, sizeof(asmkit_instruction_corpus_bpf64_00076), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00076));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00076, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSLE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00077[] = {0xd6u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00077, sizeof(asmkit_instruction_corpus_bpf64_00077), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00077));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00077, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00078[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00078, sizeof(asmkit_instruction_corpus_bpf64_00078), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00078));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00078, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00079[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0x01u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00079, sizeof(asmkit_instruction_corpus_bpf64_00079), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00079));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00079, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00080[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0x40u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00080, sizeof(asmkit_instruction_corpus_bpf64_00080), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00080));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00080, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00081[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0x41u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00081, sizeof(asmkit_instruction_corpus_bpf64_00081), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00081));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00081, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00082[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0x50u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00082, sizeof(asmkit_instruction_corpus_bpf64_00082), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00082));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00082, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00083[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0x51u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00083, sizeof(asmkit_instruction_corpus_bpf64_00083), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00083));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00083, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00084[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0xa0u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00084, sizeof(asmkit_instruction_corpus_bpf64_00084), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00084));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00084, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00085[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0xa1u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00085, sizeof(asmkit_instruction_corpus_bpf64_00085), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00085));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00085, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00086[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0xe1u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00086, sizeof(asmkit_instruction_corpus_bpf64_00086), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00086));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00086, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00087[] = {0xdbu, 0x1au, 0xfcu, 0xffu, 0xf1u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00087, sizeof(asmkit_instruction_corpus_bpf64_00087), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00087));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00087, inst.size) == 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00088[] = {0xdbu, 0x3au, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00088, sizeof(asmkit_instruction_corpus_bpf64_00088), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00088));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00088, inst.size) == 0);
        ASMKIT_CHECK(inst.operand_count >= 2u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 41u, ASMKIT_OP_MEM,
            UINT64_C(0), INT64_C(0),
            ASMKIT_BPF_REG_R10, UINT64_C(0), UINT64_C(0),
            INT64_C(256), 0u, 0u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 131u, ASMKIT_OP_REG,
            ASMKIT_BPF_REG_R3, INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 64u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ASMKIT_CHECK(asmkit_instruction_corpus_has_register_effect(&inst, ASMKIT_BPF_REG_R3, ASMKIT_REGISTER_EFFECT_READ) != 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_has_register_effect(&inst, ASMKIT_BPF_REG_R10, ASMKIT_REGISTER_EFFECT_READ) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00089[] = {0xdcu, 0x02u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00089, sizeof(asmkit_instruction_corpus_bpf64_00089), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00089));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00089, inst.size) == 0);
        ASMKIT_CHECK(inst.operand_count >= 1u);
        operand_cursor = 0u;
        if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 131u, ASMKIT_OP_REG,
            ASMKIT_BPF_REG_W2, INT64_C(0),
            UINT64_C(0), UINT64_C(0), UINT64_C(0),
            INT64_C(0), 0u, 32u,
            0u)) {
            ASMKIT_CHECK(0);
        }
        ASMKIT_CHECK(asmkit_instruction_corpus_has_register_effect(&inst, ASMKIT_BPF_REG_W2, ASMKIT_REGISTER_EFFECT_READ | ASMKIT_REGISTER_EFFECT_WRITE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00090[] = {0xddu, 0x95u, 0xbfu, 0xb1u, 0xf2u, 0x5fu, 0x7bu, 0xc4u};
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00090, sizeof(asmkit_instruction_corpus_bpf64_00090), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00090));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00090, inst.size) == 0);
        ASMKIT_CHECK(asmkit_instruction_corpus_mnemonic_matches(inst.mnemonic_id, ASMKIT_BPF_JSLE) != 0);
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
        uint8_t asmkit_instruction_corpus_bpf64_00091[] = {
            0x18u, 0xa3u, 0x5cu, 0x14u, 0xdeu, 0xf0u, 0xa5u, 0xffu, 0x9au, 0x7eu, 0x10u, 0xeeu, 0xd8u, 0xa4u, 0x2bu, 0x2fu,
        };
        status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00091, sizeof(asmkit_instruction_corpus_bpf64_00091), UINT64_C(0x10000000), &inst);
        ASMKIT_CHECK(status == ASMKIT_OK);
        ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
        ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00091));
        ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
        ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
        ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00091, inst.size) == 0);
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

int asmkit_test_instruction_corpus_bpf64(void)
{
    asmkit_engine_t engine;
    uint32_t decoded_count;
    uint32_t roundtrip_count;
    uint32_t detail_count;
    int failures;

    ASMKIT_CHECK(asmkit_instruction_corpus_init_engine(&engine, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64) == 0);
    decoded_count = 0u;
    roundtrip_count = 0u;
    detail_count = 0u;
    failures = 0;
    failures += asmkit_instruction_corpus_bpf64_chunk_000(
        &engine,
        &decoded_count,
        &roundtrip_count,
        &detail_count);
    failures += asmkit_instruction_corpus_bpf64_chunk_001(
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

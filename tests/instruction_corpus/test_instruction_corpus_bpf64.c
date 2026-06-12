/*
 * Generated neutral instruction corpus tests.
 * Do not edit by hand.
 */
#include <string.h>
#include "test_support.h"
#include "asmkit/target/bpf.h"

#define ASMKIT_INSTRUCTION_CORPUS_TARGET_CASE_COUNT 92u
#define ASMKIT_INSTRUCTION_CORPUS_TARGET_DETAIL_CASE_COUNT 5u

static int asmkit_instruction_corpus_init_engine(asmkit_engine_t* engine, asmkit_arch_t arch, asmkit_mode_t mode)
{
    asmkit_engine_config_t config;
    ASMKIT_CHECK(asmkit_engine_config_init(&config, arch, mode) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_enable_all_features(&config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(engine, &config) == ASMKIT_OK);
    return 0;
}

static void asmkit_instruction_corpus_print_bytes(const uint8_t* bytes, uint32_t size)
{
    uint32_t index;
    for (index = 0u; index < size; ++index) {
        printf("%02x", (unsigned)bytes[index]);
    }
}

static void asmkit_instruction_corpus_print_operands(const asmkit_inst_t* inst)
{
    uint32_t index;
    for (index = 0u; index < inst->operand_count && index < ASMKIT_MAX_OPERANDS; ++index) {
        const asmkit_operand_t* operand = &inst->operands[index];
        printf(
            " operand[%u]={kind=%u flags=%u reg=%llu shift_reg=%llu imm=%lld abs=%llu width=%u mem.base=%llu mem.index=%llu mem.disp=%lld mem.scale=%u mem.addr=%u shift=%u/%u extend=%u/%u}",
            (unsigned)index,
            (unsigned)operand->kind,
            (unsigned)operand->flags,
            (unsigned long long)operand->reg,
            (unsigned long long)operand->shift_reg,
            (long long)operand->imm,
            (unsigned long long)operand->abs_target,
            (unsigned)operand->width,
            (unsigned long long)operand->mem.base,
            (unsigned long long)operand->mem.index,
            (long long)operand->mem.displacement,
            (unsigned)operand->mem.scale,
            (unsigned)operand->mem.address_width,
            (unsigned)operand->shift_kind,
            (unsigned)operand->shift_amount,
            (unsigned)operand->extend_kind,
            (unsigned)operand->extend_amount);
    }
}

#define ASMKIT_CORPUS_CHECK_KIND 1u
#define ASMKIT_CORPUS_CHECK_REG 2u
#define ASMKIT_CORPUS_CHECK_IMM 4u
#define ASMKIT_CORPUS_CHECK_MEM_BASE 8u
#define ASMKIT_CORPUS_CHECK_MEM_INDEX 16u
#define ASMKIT_CORPUS_CHECK_MEM_DISP 32u
#define ASMKIT_CORPUS_CHECK_MEM_SCALE 64u
#define ASMKIT_CORPUS_CHECK_WIDTH 128u

static uint32_t asmkit_instruction_corpus_arm_reg_bit(uint64_t reg)
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

static uint32_t asmkit_instruction_corpus_arm_s_reg_number(uint64_t reg)
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

static uint32_t asmkit_instruction_corpus_arm_d_reg_number(uint64_t reg)
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

static int asmkit_instruction_corpus_is_arm_pc(uint64_t reg)
{
#ifdef ASMKIT_TARGET_ARM_H
    return reg == ASMKIT_ARM_REG_PC;
#else
    (void)reg;
    return 0;
#endif
}

static int asmkit_instruction_corpus_reg_list_contains(const asmkit_operand_t* operand, uint64_t expected_reg)
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

static uint8_t asmkit_instruction_corpus_vec_count(const asmkit_operand_t* operand)
{
    return (uint8_t)((uint32_t)operand->imm & 0xffu);
}

static uint8_t asmkit_instruction_corpus_vec_stride(const asmkit_operand_t* operand)
{
    uint8_t stride = (uint8_t)(((uint32_t)operand->imm >> 8u) & 0xffu);
    return stride == 0u ? 1u : stride;
}

static int asmkit_instruction_corpus_vec_list_contains(const asmkit_operand_t* operand, uint64_t expected_reg)
{
    uint64_t reg;
    uint8_t count;
    uint8_t stride;
    uint8_t index;
    if ((operand->flags & ASMKIT_OPERAND_FLAG_VECTOR_LIST) == 0u || expected_reg < operand->reg) {
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

static int asmkit_instruction_corpus_reg_operand_matches(const asmkit_operand_t* operand, uint64_t expected_reg)
{
    if (operand->kind == ASMKIT_OP_PC_REL && asmkit_instruction_corpus_is_arm_pc(expected_reg)) {
        return 1;
    }
    if (operand->kind != ASMKIT_OP_REG) {
        return 0;
    }
    return operand->reg == expected_reg ||
        asmkit_instruction_corpus_reg_list_contains(operand, expected_reg) ||
        asmkit_instruction_corpus_vec_list_contains(operand, expected_reg);
}

static int asmkit_instruction_corpus_find_reg(const asmkit_inst_t* inst, uint32_t start, uint64_t expected_reg, uint32_t* out_index)
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

static int asmkit_instruction_corpus_find_imm(const asmkit_inst_t* inst, uint32_t start, int64_t expected_imm, uint32_t* out_index)
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

static int asmkit_instruction_corpus_find_mem_base(const asmkit_inst_t* inst, uint32_t start, uint64_t expected_reg, uint32_t* out_index)
{
    uint32_t index;
    for (index = start; index < inst->operand_count && index < ASMKIT_MAX_OPERANDS; ++index) {
        const asmkit_operand_t* operand = &inst->operands[index];
        if (operand->kind == ASMKIT_OP_MEM && operand->mem.base == expected_reg) {
            *out_index = index;
            return 1;
        }
    }
    return 0;
}

static int asmkit_instruction_corpus_find_mem_index(const asmkit_inst_t* inst, uint32_t start, uint64_t expected_reg, uint32_t* out_index)
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

static int asmkit_instruction_corpus_find_mem_disp(const asmkit_inst_t* inst, uint32_t start, int64_t expected_disp, uint32_t* out_index)
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

static void asmkit_instruction_corpus_advance_cursor(uint32_t* cursor, uint32_t consumed_index)
{
    uint32_t next = consumed_index + 1u;
    if (*cursor < next) {
        *cursor = next;
    }
}

static int asmkit_instruction_corpus_operand_scale_matches(const asmkit_operand_t* operand, uint32_t mem_scale)
{
    if (mem_scale == 0u || mem_scale == 1u) {
        return 1;
    }
    if (operand->kind == ASMKIT_OP_REG && operand->shift_amount < 31u &&
        (UINT32_C(1) << operand->shift_amount) == mem_scale) {
        return 1;
    }
    return 0;
}

static int asmkit_instruction_corpus_operand_matches(
    const asmkit_inst_t* inst, uint32_t actual_index, uint32_t checks, asmkit_operand_kind_t kind,
    uint64_t reg, int64_t imm, uint64_t mem_base, uint64_t mem_index, int64_t mem_disp, uint32_t mem_scale, uint32_t width,
    uint32_t* next_cursor)
{
    const asmkit_operand_t* operand;
    uint32_t found_index;
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
                !(kind == ASMKIT_OP_REG && asmkit_instruction_corpus_is_arm_pc(reg) && operand->kind == ASMKIT_OP_PC_REL)) {
                return 0;
            }
        }
    }
    if ((checks & ASMKIT_CORPUS_CHECK_REG) != 0u) {
        if (operand->kind == ASMKIT_OP_PC_REL && asmkit_instruction_corpus_is_arm_pc(reg)) {
            return 1;
        }
        if (operand->kind != ASMKIT_OP_REG) {
            return 0;
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
        if (operand->kind != ASMKIT_OP_IMM || operand->imm != imm) {
            return 0;
        }
    }
    if ((checks & (ASMKIT_CORPUS_CHECK_MEM_BASE | ASMKIT_CORPUS_CHECK_MEM_INDEX | ASMKIT_CORPUS_CHECK_MEM_DISP | ASMKIT_CORPUS_CHECK_MEM_SCALE)) != 0u) {
        if (operand->kind == ASMKIT_OP_MEM) {
            if ((checks & ASMKIT_CORPUS_CHECK_MEM_BASE) != 0u && operand->mem.base != mem_base) {
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

static int asmkit_instruction_corpus_composite_operand(const asmkit_operand_t* operand)
{
    return operand->kind == ASMKIT_OP_PC_REL ||
        (operand->kind == ASMKIT_OP_REG &&
         (operand->flags & (ASMKIT_OPERAND_FLAG_REGISTER_LIST | ASMKIT_OPERAND_FLAG_VECTOR_LIST)) != 0u);
}

static int asmkit_instruction_corpus_expect_operand(
    const asmkit_inst_t* inst, uint32_t* cursor, uint32_t checks, asmkit_operand_kind_t kind,
    uint64_t reg, int64_t imm, uint64_t mem_base, uint64_t mem_index, int64_t mem_disp, uint32_t mem_scale, uint32_t width)
{
    uint32_t index;
    uint32_t next_cursor;
    for (index = *cursor; index < inst->operand_count && index < ASMKIT_MAX_OPERANDS; ++index) {
        next_cursor = asmkit_instruction_corpus_composite_operand(&inst->operands[index]) ? index : index + 1u;
        if (asmkit_instruction_corpus_operand_matches(inst, index, checks, kind, reg, imm, mem_base, mem_index, mem_disp, mem_scale, width, &next_cursor)) {
            *cursor = next_cursor;
            return 1;
        }
    }
    return 0;
}

static const uint8_t asmkit_instruction_corpus_bpf64_00000[] = {
    0x05u, 0xc7u, 0x71u, 0xb0u, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00001[] = {
    0x06u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00002[] = {
    0x15u, 0x6au, 0x9fu, 0x38u, 0x1au, 0x9du, 0xb7u, 0x4du,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00003[] = {
    0x16u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00004[] = {
    0x1du, 0x21u, 0x20u, 0x4du, 0xe3u, 0x47u, 0xafu, 0x1bu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00005[] = {
    0x20u, 0xc7u, 0x0cu, 0x70u, 0xdau, 0x41u, 0x1au, 0xcau,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00006[] = {
    0x25u, 0x89u, 0xdau, 0x53u, 0x19u, 0x73u, 0x8au, 0xc0u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00007[] = {
    0x26u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00008[] = {
    0x28u, 0x4eu, 0xb0u, 0x62u, 0xe8u, 0x48u, 0x0bu, 0x0du,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00009[] = {
    0x2du, 0x18u, 0x07u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00010[] = {
    0x2du, 0x18u, 0x5bu, 0xfdu, 0x8fu, 0x53u, 0x3bu, 0xf0u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00011[] = {
    0x30u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00012[] = {
    0x30u, 0x5fu, 0xfeu, 0xfcu, 0x85u, 0x66u, 0x7cu, 0x4bu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00013[] = {
    0x34u, 0x46u, 0x00u, 0x00u, 0xe1u, 0x72u, 0xd4u, 0xcbu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00014[] = {
    0x35u, 0xa5u, 0x42u, 0xb9u, 0x5bu, 0x37u, 0xa1u, 0x3du,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00015[] = {
    0x36u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00016[] = {
    0x37u, 0x84u, 0x00u, 0x00u, 0x3bu, 0x84u, 0x55u, 0x1fu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00017[] = {
    0x3fu, 0x36u, 0x00u, 0x00u, 0x7eu, 0x07u, 0x59u, 0x7au,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00018[] = {
    0x40u, 0x95u, 0xc2u, 0x39u, 0x6bu, 0xe7u, 0xd7u, 0xc4u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00019[] = {
    0x45u, 0x12u, 0xa2u, 0xf2u, 0x14u, 0xe7u, 0x2du, 0x1eu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00020[] = {
    0x46u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00021[] = {
    0x48u, 0x7eu, 0xfbu, 0x77u, 0xebu, 0x0eu, 0x5au, 0x0du,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00022[] = {
    0x4du, 0x10u, 0x67u, 0x44u, 0x4du, 0x3fu, 0x4du, 0x8bu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00023[] = {
    0x50u, 0x38u, 0x80u, 0xf8u, 0x04u, 0x70u, 0xd1u, 0x6cu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00024[] = {
    0x55u, 0xb9u, 0xa3u, 0x80u, 0x90u, 0xbcu, 0xc8u, 0x96u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00025[] = {
    0x56u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00026[] = {
    0x5du, 0x56u, 0xa3u, 0x4cu, 0x2au, 0xc8u, 0x4au, 0xc5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00027[] = {
    0x61u, 0x28u, 0xb2u, 0xedu, 0xb8u, 0xcfu, 0xb5u, 0xe4u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00028[] = {
    0x62u, 0xa5u, 0xdfu, 0xe0u, 0x14u, 0x7du, 0x95u, 0x78u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00029[] = {
    0x63u, 0x77u, 0x2fu, 0xcfu, 0x76u, 0xb7u, 0xd3u, 0xfau,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00030[] = {
    0x65u, 0xe8u, 0x97u, 0xe1u, 0x87u, 0xbeu, 0x8fu, 0xf8u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00031[] = {
    0x66u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00032[] = {
    0x69u, 0x14u, 0xc7u, 0x8eu, 0x0bu, 0xc1u, 0xadu, 0x69u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00033[] = {
    0x6au, 0xb5u, 0xbcu, 0x8cu, 0x4fu, 0x5cu, 0x94u, 0x01u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00034[] = {
    0x6bu, 0x34u, 0x58u, 0xf5u, 0xc8u, 0x27u, 0x9eu, 0x14u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00035[] = {
    0x6du, 0x33u, 0x17u, 0x02u, 0x00u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00036[] = {
    0x6du, 0x38u, 0x69u, 0xe3u, 0xc9u, 0xacu, 0x3cu, 0xdbu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00037[] = {
    0x71u, 0xa0u, 0xebu, 0xfbu, 0x3du, 0x6bu, 0x58u, 0x45u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00038[] = {
    0x72u, 0xe2u, 0xc1u, 0x1bu, 0x25u, 0x2fu, 0x4au, 0xdcu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00039[] = {
    0x73u, 0x44u, 0x09u, 0x0fu, 0xc1u, 0x07u, 0xa8u, 0xf4u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00040[] = {
    0x75u, 0x04u, 0x8eu, 0x18u, 0x6au, 0xccu, 0x3cu, 0x09u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00041[] = {
    0x76u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00042[] = {
    0x79u, 0xa9u, 0x5cu, 0x7bu, 0x16u, 0x1fu, 0xfbu, 0x01u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00043[] = {
    0x7au, 0xd8u, 0x6bu, 0x04u, 0x76u, 0xf0u, 0x51u, 0x75u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00044[] = {
    0x7bu, 0x72u, 0x0fu, 0x30u, 0x51u, 0x78u, 0xd2u, 0x9au,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00045[] = {
    0x7du, 0x58u, 0x52u, 0x01u, 0x90u, 0xf9u, 0x30u, 0x9au,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00046[] = {
    0x84u, 0x02u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00047[] = {
    0x85u, 0x00u, 0x00u, 0xe2u, 0x83u, 0x3du, 0xbdu, 0xfdu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00048[] = {
    0x85u, 0xd3u, 0xa5u, 0xe2u, 0x83u, 0x3du, 0xbdu, 0x5du,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00049[] = {
    0x8du, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00050[] = {
    0x94u, 0x39u, 0x00u, 0x00u, 0x0bu, 0xd2u, 0xd1u, 0xc9u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00051[] = {
    0x95u, 0xf2u, 0xd1u, 0x83u, 0x53u, 0xa9u, 0x09u, 0x9fu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00052[] = {
    0x97u, 0x09u, 0x00u, 0x00u, 0x37u, 0x13u, 0x03u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00053[] = {
    0x97u, 0xc8u, 0x00u, 0x00u, 0xd2u, 0x09u, 0x98u, 0x09u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00054[] = {
    0x9cu, 0x96u, 0x00u, 0x00u, 0x0fu, 0x69u, 0x13u, 0x90u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00055[] = {
    0x9fu, 0x35u, 0x00u, 0x00u, 0xd6u, 0x70u, 0xd9u, 0x5eu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00056[] = {
    0xa5u, 0xd4u, 0xefu, 0x79u, 0xd3u, 0xbbu, 0xdeu, 0xfdu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00057[] = {
    0xa6u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00058[] = {
    0xb4u, 0xa1u, 0x00u, 0x00u, 0xf9u, 0x3fu, 0x77u, 0x1fu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00059[] = {
    0xb5u, 0x92u, 0x5du, 0x5au, 0x49u, 0x33u, 0xfcu, 0x33u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00060[] = {
    0xb6u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00061[] = {
    0xb7u, 0x70u, 0x00u, 0x00u, 0x5bu, 0x52u, 0x2au, 0x99u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00062[] = {
    0xbcu, 0x72u, 0x00u, 0x00u, 0xc9u, 0x8au, 0x56u, 0xd6u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00063[] = {
    0xbdu, 0x19u, 0x80u, 0xe8u, 0x29u, 0x85u, 0xcfu, 0x51u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00064[] = {
    0xbfu, 0x86u, 0x00u, 0x00u, 0xb2u, 0x6du, 0x14u, 0x03u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00065[] = {
    0xc3u, 0x1au, 0xfcu, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00066[] = {
    0xc3u, 0x1au, 0xfcu, 0xffu, 0x01u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00067[] = {
    0xc3u, 0x1au, 0xfcu, 0xffu, 0x40u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00068[] = {
    0xc3u, 0x1au, 0xfcu, 0xffu, 0x41u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00069[] = {
    0xc3u, 0x1au, 0xfcu, 0xffu, 0x50u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00070[] = {
    0xc3u, 0x1au, 0xfcu, 0xffu, 0x51u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00071[] = {
    0xc3u, 0x1au, 0xfcu, 0xffu, 0xa0u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00072[] = {
    0xc3u, 0x1au, 0xfcu, 0xffu, 0xa1u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00073[] = {
    0xc5u, 0xf2u, 0xebu, 0xe4u, 0xbau, 0xc0u, 0xceu, 0x4fu,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00074[] = {
    0xc6u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00075[] = {
    0xcdu, 0x90u, 0x67u, 0x88u, 0x6bu, 0xd0u, 0x27u, 0xf4u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00076[] = {
    0xd5u, 0xe9u, 0xf6u, 0xb2u, 0x50u, 0xfdu, 0xb0u, 0xe5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00077[] = {
    0xd6u, 0xc7u, 0x0fu, 0xccu, 0x43u, 0x1fu, 0xb9u, 0xf5u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00078[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00079[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0x01u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00080[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0x40u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00081[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0x41u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00082[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0x50u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00083[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0x51u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00084[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0xa0u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00085[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0xa1u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00086[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0xe1u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00087[] = {
    0xdbu, 0x1au, 0xfcu, 0xffu, 0xf1u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00088[] = {
    0xdbu, 0x3au, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00089[] = {
    0xdcu, 0x02u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00090[] = {
    0xddu, 0x95u, 0xbfu, 0xb1u, 0xf2u, 0x5fu, 0x7bu, 0xc4u,
};

static const uint8_t asmkit_instruction_corpus_bpf64_00091[] = {
    0x18u, 0xa3u, 0x5cu, 0x14u, 0xdeu, 0xf0u, 0xa5u, 0xffu, 0x9au, 0x7eu, 0x10u, 0xeeu, 0xd8u, 0xa4u, 0x2bu, 0x2fu,
};

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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00000, sizeof(asmkit_instruction_corpus_bpf64_00000), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31198u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00000),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00000, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00000));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00000)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31198u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00000));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00000, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00000));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00000));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00000, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00001, sizeof(asmkit_instruction_corpus_bpf64_00001), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31199u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00001),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00001, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00001));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00001)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31199u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00001));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00001, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00001));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00001));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00002, sizeof(asmkit_instruction_corpus_bpf64_00002), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31200u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00002),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00002, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00002));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00002)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31200u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00002));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00002, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00002));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00002));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00002, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00003, sizeof(asmkit_instruction_corpus_bpf64_00003), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31201u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00003),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00003, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00003));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00003)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31201u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00003));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00003, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00003));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00003));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00004, sizeof(asmkit_instruction_corpus_bpf64_00004), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31202u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00004),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00004, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00004));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00004)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31202u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00004));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00004, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00004));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00004));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00004, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00005, sizeof(asmkit_instruction_corpus_bpf64_00005), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31203u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00005),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00005, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00005));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00005)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31203u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00005));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00005, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00005));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00005));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00006, sizeof(asmkit_instruction_corpus_bpf64_00006), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31204u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00006),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00006, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00006));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00006)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31204u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00006));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00006, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00006));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00006));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00006, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00007, sizeof(asmkit_instruction_corpus_bpf64_00007), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31205u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00007),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00007, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00007));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00007)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31205u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00007));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00007, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00007));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00007));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00008, sizeof(asmkit_instruction_corpus_bpf64_00008), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31206u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00008),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00008, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00008));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00008)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31206u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00008));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00008, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00008));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00008));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00009, sizeof(asmkit_instruction_corpus_bpf64_00009), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31207u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00009),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00009, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00009));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00009)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31207u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00009));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00009, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00009));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00009));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00009, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00010, sizeof(asmkit_instruction_corpus_bpf64_00010), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31208u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00010),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00010, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00010));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00010)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31208u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00010));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00010, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00010));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00010));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00010, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00011, sizeof(asmkit_instruction_corpus_bpf64_00011), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31209u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00011),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00011, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00011));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00011)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31209u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00011));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00011, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00011));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00011));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00011, inst.size) == 0);
    operand_cursor = 0u;
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 33u, ASMKIT_OP_MEM,
        UINT64_C(0), INT64_C(0),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31209u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            0u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00011, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00011));
        printf("\n");
        asmkit_instruction_corpus_print_operands(&inst);
        printf("\n");
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00012, sizeof(asmkit_instruction_corpus_bpf64_00012), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31210u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00012),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00012, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00012));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00012)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31210u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00012));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00012, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00012));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00012));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00013, sizeof(asmkit_instruction_corpus_bpf64_00013), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31211u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00013),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00013, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00013));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00013)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31211u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00013));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00013, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00013));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00013));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00013, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00014, sizeof(asmkit_instruction_corpus_bpf64_00014), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31212u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00014),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00014, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00014));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00014)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31212u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00014));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00014, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00014));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00014));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00014, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00015, sizeof(asmkit_instruction_corpus_bpf64_00015), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31213u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00015),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00015, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00015));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00015)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31213u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00015));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00015, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00015));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00015));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00016, sizeof(asmkit_instruction_corpus_bpf64_00016), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31214u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00016),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00016, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00016));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00016)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31214u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00016));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00016, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00016));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00016));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00017, sizeof(asmkit_instruction_corpus_bpf64_00017), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31215u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00017),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00017, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00017));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00017)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31215u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00017));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00017, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00017));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00017));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00018, sizeof(asmkit_instruction_corpus_bpf64_00018), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31216u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00018),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00018, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00018));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00018)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31216u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00018));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00018, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00018));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00018));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00019, sizeof(asmkit_instruction_corpus_bpf64_00019), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31217u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00019),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00019, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00019));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00019)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31217u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00019));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00019, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00019));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00019));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00019, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00020, sizeof(asmkit_instruction_corpus_bpf64_00020), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31218u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00020),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00020, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00020));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00020)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31218u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00020));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00020, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00020));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00020));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00021, sizeof(asmkit_instruction_corpus_bpf64_00021), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31219u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00021),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00021, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00021));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00021)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31219u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00021));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00021, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00021));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00021));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00022, sizeof(asmkit_instruction_corpus_bpf64_00022), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31220u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00022),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00022, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00022));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00022)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31220u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00022));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00022, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00022));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00022));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00022, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00023, sizeof(asmkit_instruction_corpus_bpf64_00023), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31221u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00023),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00023, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00023));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00023)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31221u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00023));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00023, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00023));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00023));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00024, sizeof(asmkit_instruction_corpus_bpf64_00024), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31222u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00024),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00024, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00024));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00024)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31222u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00024));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00024, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00024));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00024));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00024, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00025, sizeof(asmkit_instruction_corpus_bpf64_00025), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31223u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00025),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00025, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00025));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00025)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31223u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00025));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00025, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00025));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00025));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00026, sizeof(asmkit_instruction_corpus_bpf64_00026), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31224u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00026),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00026, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00026));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00026)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31224u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00026));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00026, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00026));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00026));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00026, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00027, sizeof(asmkit_instruction_corpus_bpf64_00027), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31225u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00027),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00027, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00027));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00027)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31225u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00027));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00027, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00027));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00027));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00028, sizeof(asmkit_instruction_corpus_bpf64_00028), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31226u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00028),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00028, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00028));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00028)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31226u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00028));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00028, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00028));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00028));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00029, sizeof(asmkit_instruction_corpus_bpf64_00029), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31227u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00029),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00029, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00029));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00029)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31227u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00029));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00029, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00029));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00029));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00030, sizeof(asmkit_instruction_corpus_bpf64_00030), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31228u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00030),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00030, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00030));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00030)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31228u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00030));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00030, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00030));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00030));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00030, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00031, sizeof(asmkit_instruction_corpus_bpf64_00031), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31229u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00031),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00031, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00031));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00031)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31229u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00031));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00031, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00031));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00031));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00032, sizeof(asmkit_instruction_corpus_bpf64_00032), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31230u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00032),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00032, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00032));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00032)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31230u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00032));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00032, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00032));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00032));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00033, sizeof(asmkit_instruction_corpus_bpf64_00033), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31231u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00033),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00033, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00033));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00033)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31231u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00033));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00033, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00033));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00033));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00034, sizeof(asmkit_instruction_corpus_bpf64_00034), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31232u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00034),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00034, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00034));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00034)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31232u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00034));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00034, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00034));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00034));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00035, sizeof(asmkit_instruction_corpus_bpf64_00035), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31233u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00035),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00035, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00035));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00035)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31233u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00035));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00035, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00035));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00035));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00035, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00036, sizeof(asmkit_instruction_corpus_bpf64_00036), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31234u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00036),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00036, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00036));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00036)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31234u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00036));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00036, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00036));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00036));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00036, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00037, sizeof(asmkit_instruction_corpus_bpf64_00037), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31235u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00037),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00037, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00037));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00037)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31235u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00037));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00037, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00037));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00037));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00038, sizeof(asmkit_instruction_corpus_bpf64_00038), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31236u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00038),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00038, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00038));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00038)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31236u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00038));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00038, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00038));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00038));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00039, sizeof(asmkit_instruction_corpus_bpf64_00039), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31237u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00039),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00039, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00039));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00039)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31237u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00039));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00039, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00039));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00039));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00040, sizeof(asmkit_instruction_corpus_bpf64_00040), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31238u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00040),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00040, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00040));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00040)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31238u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00040));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00040, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00040));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00040));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00040, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00041, sizeof(asmkit_instruction_corpus_bpf64_00041), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31239u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00041),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00041, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00041));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00041)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31239u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00041));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00041, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00041));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00041));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00042, sizeof(asmkit_instruction_corpus_bpf64_00042), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31240u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00042),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00042, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00042));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00042)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31240u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00042));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00042, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00042));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00042));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00043, sizeof(asmkit_instruction_corpus_bpf64_00043), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31241u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00043),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00043, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00043));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00043)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31241u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00043));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00043, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00043));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00043));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00044, sizeof(asmkit_instruction_corpus_bpf64_00044), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31242u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00044),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00044, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00044));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00044)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31242u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00044));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00044, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00044));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00044));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00045, sizeof(asmkit_instruction_corpus_bpf64_00045), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31243u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00045),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00045, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00045));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00045)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31243u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00045));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00045, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00045));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00045));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00045, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00046, sizeof(asmkit_instruction_corpus_bpf64_00046), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31244u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00046),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00046, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00046));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00046)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31244u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00046));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00046, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00046));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00046));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00046, inst.size) == 0);
    operand_cursor = 0u;
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 3u, ASMKIT_OP_REG,
        2u, INT64_C(0),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31244u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            0u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00046, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00046));
        printf("\n");
        asmkit_instruction_corpus_print_operands(&inst);
        printf("\n");
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00047, sizeof(asmkit_instruction_corpus_bpf64_00047), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31245u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00047),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00047, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00047));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00047)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31245u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00047));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00047, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00047));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00047));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00047, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00048, sizeof(asmkit_instruction_corpus_bpf64_00048), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31246u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00048),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00048, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00048));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00048)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31246u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00048));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00048, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00048));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00048));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00048, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00049, sizeof(asmkit_instruction_corpus_bpf64_00049), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31247u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00049),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00049, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00049));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00049)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31247u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00049));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00049, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00049));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00049));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00050, sizeof(asmkit_instruction_corpus_bpf64_00050), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31248u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00050),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00050, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00050));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00050)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31248u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00050));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00050, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00050));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00050));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00050, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00051, sizeof(asmkit_instruction_corpus_bpf64_00051), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31249u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00051),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00051, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00051));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00051)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31249u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00051));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00051, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00051));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00051));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00052, sizeof(asmkit_instruction_corpus_bpf64_00052), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31250u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00052),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00052, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00052));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00052)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31250u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00052));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00052, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00052));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00052));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00052, inst.size) == 0);
    operand_cursor = 0u;
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 3u, ASMKIT_OP_REG,
        9u, INT64_C(0),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31250u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            0u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00052, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00052));
        printf("\n");
        asmkit_instruction_corpus_print_operands(&inst);
        printf("\n");
        ASMKIT_CHECK(0);
    }
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 5u, ASMKIT_OP_IMM,
        UINT64_C(0), INT64_C(201527),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31250u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            1u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00052, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00052));
        printf("\n");
        asmkit_instruction_corpus_print_operands(&inst);
        printf("\n");
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00053, sizeof(asmkit_instruction_corpus_bpf64_00053), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31251u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00053),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00053, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00053));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00053)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31251u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00053));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00053, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00053));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00053));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00054, sizeof(asmkit_instruction_corpus_bpf64_00054), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31252u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00054),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00054, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00054));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00054)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31252u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00054));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00054, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00054));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00054));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00054, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00055, sizeof(asmkit_instruction_corpus_bpf64_00055), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31253u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00055),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00055, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00055));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00055)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31253u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00055));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00055, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00055));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00055));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00056, sizeof(asmkit_instruction_corpus_bpf64_00056), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31254u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00056),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00056, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00056));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00056)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31254u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00056));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00056, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00056));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00056));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00056, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00057, sizeof(asmkit_instruction_corpus_bpf64_00057), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31255u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00057),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00057, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00057));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00057)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31255u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00057));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00057, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00057));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00057));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00058, sizeof(asmkit_instruction_corpus_bpf64_00058), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31256u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00058),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00058, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00058));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00058)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31256u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00058));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00058, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00058));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00058));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00058, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00059, sizeof(asmkit_instruction_corpus_bpf64_00059), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31257u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00059),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00059, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00059));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00059)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31257u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00059));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00059, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00059));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00059));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00059, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00060, sizeof(asmkit_instruction_corpus_bpf64_00060), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31258u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00060),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00060, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00060));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00060)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31258u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00060));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00060, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00060));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00060));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00061, sizeof(asmkit_instruction_corpus_bpf64_00061), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31259u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00061),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00061, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00061));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00061)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31259u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00061));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00061, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00061));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00061));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00062, sizeof(asmkit_instruction_corpus_bpf64_00062), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31260u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00062),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00062, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00062));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00062)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31260u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00062));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00062, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00062));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00062));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00062, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00063, sizeof(asmkit_instruction_corpus_bpf64_00063), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31261u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00063),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00063, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00063));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00063)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31261u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00063));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00063, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00063));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00063));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00063, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00064, sizeof(asmkit_instruction_corpus_bpf64_00064), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31262u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00064),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00064, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00064));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00064)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31262u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00064));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00064, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00064));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00064));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00065, sizeof(asmkit_instruction_corpus_bpf64_00065), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31263u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00065),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00065, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00065));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00065)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31263u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00065));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00065, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00065));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00065));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00066, sizeof(asmkit_instruction_corpus_bpf64_00066), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31264u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00066),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00066, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00066));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00066)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31264u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00066));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00066, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00066));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00066));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00067, sizeof(asmkit_instruction_corpus_bpf64_00067), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31265u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00067),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00067, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00067));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00067)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31265u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00067));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00067, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00067));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00067));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00068, sizeof(asmkit_instruction_corpus_bpf64_00068), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31266u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00068),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00068, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00068));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00068)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31266u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00068));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00068, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00068));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00068));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00069, sizeof(asmkit_instruction_corpus_bpf64_00069), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31267u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00069),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00069, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00069));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00069)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31267u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00069));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00069, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00069));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00069));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00070, sizeof(asmkit_instruction_corpus_bpf64_00070), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31268u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00070),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00070, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00070));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00070)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31268u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00070));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00070, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00070));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00070));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00071, sizeof(asmkit_instruction_corpus_bpf64_00071), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31269u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00071),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00071, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00071));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00071)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31269u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00071));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00071, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00071));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00071));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00072, sizeof(asmkit_instruction_corpus_bpf64_00072), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31270u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00072),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00072, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00072));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00072)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31270u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00072));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00072, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00072));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00072));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00073, sizeof(asmkit_instruction_corpus_bpf64_00073), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31271u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00073),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00073, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00073));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00073)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31271u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00073));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00073, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00073));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00073));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00073, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00074, sizeof(asmkit_instruction_corpus_bpf64_00074), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31272u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00074),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00074, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00074));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00074)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31272u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00074));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00074, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00074));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00074));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00075, sizeof(asmkit_instruction_corpus_bpf64_00075), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31273u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00075),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00075, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00075));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00075)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31273u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00075));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00075, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00075));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00075));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00075, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00076, sizeof(asmkit_instruction_corpus_bpf64_00076), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31274u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00076),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00076, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00076));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00076)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31274u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00076));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00076, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00076));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00076));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00076, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00077, sizeof(asmkit_instruction_corpus_bpf64_00077), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31275u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00077),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00077, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00077));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00077)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31275u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00077));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00077, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00077));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00077));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00078, sizeof(asmkit_instruction_corpus_bpf64_00078), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31276u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00078),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00078, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00078));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00078)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31276u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00078));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00078, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00078));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00078));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00079, sizeof(asmkit_instruction_corpus_bpf64_00079), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31277u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00079),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00079, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00079));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00079)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31277u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00079));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00079, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00079));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00079));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00080, sizeof(asmkit_instruction_corpus_bpf64_00080), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31278u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00080),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00080, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00080));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00080)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31278u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00080));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00080, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00080));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00080));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00081, sizeof(asmkit_instruction_corpus_bpf64_00081), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31279u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00081),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00081, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00081));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00081)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31279u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00081));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00081, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00081));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00081));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00082, sizeof(asmkit_instruction_corpus_bpf64_00082), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31280u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00082),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00082, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00082));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00082)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31280u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00082));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00082, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00082));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00082));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00083, sizeof(asmkit_instruction_corpus_bpf64_00083), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31281u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00083),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00083, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00083));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00083)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31281u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00083));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00083, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00083));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00083));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00084, sizeof(asmkit_instruction_corpus_bpf64_00084), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31282u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00084),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00084, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00084));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00084)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31282u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00084));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00084, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00084));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00084));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00085, sizeof(asmkit_instruction_corpus_bpf64_00085), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31283u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00085),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00085, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00085));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00085)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31283u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00085));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00085, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00085));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00085));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00086, sizeof(asmkit_instruction_corpus_bpf64_00086), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31284u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00086),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00086, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00086));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00086)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31284u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00086));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00086, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00086));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00086));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00087, sizeof(asmkit_instruction_corpus_bpf64_00087), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31285u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00087),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00087, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00087));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00087)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31285u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00087));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00087, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00087));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00087));
    }
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00088, sizeof(asmkit_instruction_corpus_bpf64_00088), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31286u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00088),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00088, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00088));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00088)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31286u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00088));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00088, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00088));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00088));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00088, inst.size) == 0);
    operand_cursor = 0u;
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 41u, ASMKIT_OP_MEM,
        UINT64_C(0), INT64_C(0),
        10u, UINT64_C(0),
        INT64_C(256), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31286u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            0u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00088, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00088));
        printf("\n");
        asmkit_instruction_corpus_print_operands(&inst);
        printf("\n");
        ASMKIT_CHECK(0);
    }
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 3u, ASMKIT_OP_REG,
        3u, INT64_C(0),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31286u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            1u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00088, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00088));
        printf("\n");
        asmkit_instruction_corpus_print_operands(&inst);
        printf("\n");
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00089, sizeof(asmkit_instruction_corpus_bpf64_00089), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31287u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00089),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00089, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00089));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00089)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31287u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00089));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00089, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00089));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00089));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00089, inst.size) == 0);
    operand_cursor = 0u;
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 3u, ASMKIT_OP_REG,
        2u, INT64_C(0),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31287u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            0u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00089, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00089));
        printf("\n");
        asmkit_instruction_corpus_print_operands(&inst);
        printf("\n");
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00090, sizeof(asmkit_instruction_corpus_bpf64_00090), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31288u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00090),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00090, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00090));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00090)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31288u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00090));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00090, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00090));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00090));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_bpf64_00090, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_bpf64_00091, sizeof(asmkit_instruction_corpus_bpf64_00091), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31289u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00091),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00091, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00091));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_BPF);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_BPF64);
    if (inst.size != sizeof(asmkit_instruction_corpus_bpf64_00091)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31289u,
            (unsigned)ASMKIT_ARCH_BPF,
            (unsigned)ASMKIT_MODE_BPF64,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_bpf64_00091));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_bpf64_00091, (uint32_t)sizeof(asmkit_instruction_corpus_bpf64_00091));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_bpf64_00091));
    }
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
    ASMKIT_CHECK(failures == 0);
    ASMKIT_CHECK(decoded_count == ASMKIT_INSTRUCTION_CORPUS_TARGET_CASE_COUNT);
    ASMKIT_CHECK(detail_count == ASMKIT_INSTRUCTION_CORPUS_TARGET_DETAIL_CASE_COUNT);
    ASMKIT_CHECK(roundtrip_count <= decoded_count);
    return 0;
}

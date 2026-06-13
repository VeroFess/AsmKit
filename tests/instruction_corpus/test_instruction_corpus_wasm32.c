/*
 * Generated neutral instruction corpus tests.
 * Do not edit by hand.
 */
#include <string.h>
#include "test_support.h"
#include "asmkit/asmkit_metadata.h"
#include "asmkit/target/wasm.h"

#define ASMKIT_INSTRUCTION_CORPUS_TARGET_CASE_COUNT 6u
#define ASMKIT_INSTRUCTION_CORPUS_TARGET_DETAIL_CASE_COUNT 4u
#define ASMKIT_INSTRUCTION_CORPUS_TARGET_ARCH ASMKIT_ARCH_WASM

static inline int asmkit_instruction_corpus_init_engine(asmkit_engine_t* engine, asmkit_arch_t arch, asmkit_mode_t mode)
{
    asmkit_engine_config_t config;
    ASMKIT_CHECK(asmkit_engine_config_init(&config, arch, mode) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_enable_all_features(&config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(engine, &config) == ASMKIT_OK);
    return 0;
}

static inline void asmkit_instruction_corpus_print_bytes(const uint8_t* bytes, uint32_t size)
{
    uint32_t index;
    for (index = 0u; index < size; ++index) {
        printf("%02x", (unsigned)bytes[index]);
    }
}

#if ASMKIT_INSTRUCTION_CORPUS_TARGET_DETAIL_CASE_COUNT != 0u
static inline void asmkit_instruction_corpus_print_operands(const asmkit_inst_t* inst)
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
        return operand->mem.base == expected_reg || operand->mem.index == expected_reg;
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
    uint64_t reg, int64_t imm, uint64_t mem_base, uint64_t mem_index, int64_t mem_disp, uint32_t mem_scale, uint32_t width,
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

static inline int asmkit_instruction_corpus_composite_operand(const asmkit_operand_t* operand)
{
    return operand->kind == ASMKIT_OP_PC_REL ||
        operand->kind == ASMKIT_OP_MEM ||
        (operand->kind == ASMKIT_OP_REG &&
         (operand->flags & (ASMKIT_OPERAND_FLAG_REGISTER_LIST | ASMKIT_OPERAND_FLAG_VECTOR_LIST)) != 0u);
}

static inline int asmkit_instruction_corpus_expect_operand(
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
    if ((checks & ASMKIT_CORPUS_CHECK_REG) != 0u && kind == ASMKIT_OP_REG &&
        *cursor < inst->operand_count && inst->operands[*cursor].kind == ASMKIT_OP_MEM) {
        for (index = 0u; index < *cursor && index < ASMKIT_MAX_OPERANDS; ++index) {
            next_cursor = *cursor;
            if (inst->operands[index].kind != ASMKIT_OP_MEM &&
                asmkit_instruction_corpus_operand_matches(inst, index, checks, kind, reg, imm, mem_base, mem_index, mem_disp, mem_scale, width, &next_cursor)) {
                return 1;
            }
        }
    }
    return 0;
}

#endif

static const uint8_t asmkit_instruction_corpus_wasm32_00000[] = {
    0x0bu,
};

static const uint8_t asmkit_instruction_corpus_wasm32_00001[] = {
    0x45u,
};

static const uint8_t asmkit_instruction_corpus_wasm32_00002[] = {
    0x20u, 0x00u,
};

static const uint8_t asmkit_instruction_corpus_wasm32_00003[] = {
    0x20u, 0x01u,
};

static const uint8_t asmkit_instruction_corpus_wasm32_00004[] = {
    0x41u, 0x20u,
};

static const uint8_t asmkit_instruction_corpus_wasm32_00005[] = {
    0x10u, 0xc9u, 0x01u,
};

static int asmkit_instruction_corpus_wasm32_chunk_000(
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_wasm32_00000, sizeof(asmkit_instruction_corpus_wasm32_00000), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31290u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00000),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00000, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00000));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_WASM);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_WASM32);
    if (inst.size != sizeof(asmkit_instruction_corpus_wasm32_00000)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31290u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00000));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00000, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00000));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_wasm32_00000));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_wasm32_00000, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_wasm32_00001, sizeof(asmkit_instruction_corpus_wasm32_00001), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31291u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00001),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00001, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00001));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_WASM);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_WASM32);
    if (inst.size != sizeof(asmkit_instruction_corpus_wasm32_00001)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31291u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00001));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00001, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00001));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_wasm32_00001));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_wasm32_00001, inst.size) == 0);
    memset(out, 0, sizeof(out));
    memset(&encode_result, 0, sizeof(encode_result));
    encode_status = asmkit_encode_inst(engine, 0, &inst, 0, out, sizeof(out), &encode_result);
    if (encode_status == ASMKIT_OK &&
        encode_result.size == inst.size &&
        memcmp(out, inst.bytes, inst.size) == 0) {
        ++(*roundtrip_count);
    }
    ++(*decoded_count);

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_wasm32_00002, sizeof(asmkit_instruction_corpus_wasm32_00002), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31292u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00002),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00002, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00002));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_WASM);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_WASM32);
    if (inst.size != sizeof(asmkit_instruction_corpus_wasm32_00002)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31292u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00002));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00002, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00002));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_wasm32_00002));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_wasm32_00002, inst.size) == 0);
    operand_cursor = 0u;
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 5u, ASMKIT_OP_WASM_INDEX,
        UINT64_C(0), INT64_C(0),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31292u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            0u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00002, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00002));
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_wasm32_00003, sizeof(asmkit_instruction_corpus_wasm32_00003), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31293u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00003),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00003, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00003));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_WASM);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_WASM32);
    if (inst.size != sizeof(asmkit_instruction_corpus_wasm32_00003)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31293u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00003));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00003, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00003));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_wasm32_00003));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_wasm32_00003, inst.size) == 0);
    operand_cursor = 0u;
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 5u, ASMKIT_OP_WASM_INDEX,
        UINT64_C(0), INT64_C(1),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31293u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            0u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00003, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00003));
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_wasm32_00004, sizeof(asmkit_instruction_corpus_wasm32_00004), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31294u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00004),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00004, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00004));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_WASM);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_WASM32);
    if (inst.size != sizeof(asmkit_instruction_corpus_wasm32_00004)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31294u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00004));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00004, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00004));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_wasm32_00004));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_wasm32_00004, inst.size) == 0);
    operand_cursor = 0u;
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 5u, ASMKIT_OP_IMM,
        UINT64_C(0), INT64_C(32),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31294u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            0u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00004, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00004));
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

    status = asmkit_decode_one(engine, 0, asmkit_instruction_corpus_wasm32_00005, sizeof(asmkit_instruction_corpus_wasm32_00005), UINT64_C(0x10000000), &inst);
    if (status != ASMKIT_OK) {
        printf(
            "instruction corpus decode failed: case=%u arch=%u mode=%u size=%u status=%d bytes=",
            31295u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00005),
            (int)status);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00005, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00005));
        printf("\n");
        ASMKIT_CHECK(status == ASMKIT_OK);
    }
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_WASM);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_WASM32);
    if (inst.size != sizeof(asmkit_instruction_corpus_wasm32_00005)) {
        printf(
            "instruction corpus size mismatch: case=%u arch=%u mode=%u got=%u expected=%u bytes=",
            31295u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.size,
            (unsigned)sizeof(asmkit_instruction_corpus_wasm32_00005));
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00005, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00005));
        printf("\n");
        ASMKIT_CHECK(inst.size == sizeof(asmkit_instruction_corpus_wasm32_00005));
    }
    ASMKIT_CHECK(inst.size <= ASMKIT_MAX_INST_BYTES);
    ASMKIT_CHECK(inst.mnemonic_id != ASMKIT_MNEMONIC_INVALID);
    ASMKIT_CHECK(memcmp(inst.bytes, asmkit_instruction_corpus_wasm32_00005, inst.size) == 0);
    operand_cursor = 0u;
    if (!asmkit_instruction_corpus_expect_operand(&inst, &operand_cursor, 5u, ASMKIT_OP_WASM_INDEX,
        UINT64_C(0), INT64_C(201),
        UINT64_C(0), UINT64_C(0),
        INT64_C(0), 0u, 0u)) {
        printf(
            "instruction corpus operand detail mismatch: case=%u arch=%u mode=%u id=%u mnemonic=%u class=%u flags=%u expected_operand=%u operand_count=%u bytes=",
            31295u,
            (unsigned)ASMKIT_ARCH_WASM,
            (unsigned)ASMKIT_MODE_WASM32,
            (unsigned)inst.id,
            (unsigned)inst.mnemonic_id,
            (unsigned)inst.inst_class,
            (unsigned)inst.flags,
            0u,
            (unsigned)inst.operand_count);
        asmkit_instruction_corpus_print_bytes(asmkit_instruction_corpus_wasm32_00005, (uint32_t)sizeof(asmkit_instruction_corpus_wasm32_00005));
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

    return 0;
}

int asmkit_test_instruction_corpus_wasm32(void)
{
    asmkit_engine_t engine;
    uint32_t decoded_count;
    uint32_t roundtrip_count;
    uint32_t detail_count;
    int failures;

    ASMKIT_CHECK(asmkit_instruction_corpus_init_engine(&engine, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32) == 0);
    decoded_count = 0u;
    roundtrip_count = 0u;
    detail_count = 0u;
    failures = 0;
    failures += asmkit_instruction_corpus_wasm32_chunk_000(
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

#ifndef ASMKIT_DECODE_H
#define ASMKIT_DECODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "asmkit/asmkit_arch.h"
#include "asmkit/asmkit_error.h"
#include "asmkit/asmkit_mnemonic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ASMKIT_MAX_INST_BYTES 16u
#define ASMKIT_MAX_OPERANDS 8u
#define ASMKIT_MAX_PREFIX_BYTES 16u
#define ASMKIT_MAX_BLOCK_INSTRUCTIONS 32u

typedef enum asmkit_inst_class {
    ASMKIT_INST_OTHER = 0,
    ASMKIT_INST_NOP,
    ASMKIT_INST_HINT,
    ASMKIT_INST_LANDING_PAD,
    ASMKIT_INST_DIRECT_BRANCH,
    ASMKIT_INST_INDIRECT_BRANCH,
    ASMKIT_INST_COND_BRANCH,
    ASMKIT_INST_DIRECT_CALL,
    ASMKIT_INST_INDIRECT_CALL,
    ASMKIT_INST_RETURN,
    ASMKIT_INST_LOAD,
    ASMKIT_INST_STORE,
    ASMKIT_INST_ALU,
    ASMKIT_INST_SYSTEM,
    ASMKIT_INST_BARRIER,
    ASMKIT_INST_PAC,
    ASMKIT_INST_BTI,
    ASMKIT_INST_CET_ENDBR,
    ASMKIT_INST_ARM_IT,
    ASMKIT_INST_WASM_BLOCK,
    ASMKIT_INST_WASM_LOOP,
    ASMKIT_INST_WASM_IF,
    ASMKIT_INST_WASM_ELSE,
    ASMKIT_INST_WASM_END,
    ASMKIT_INST_BPF_PSEUDO
} asmkit_inst_class_t;

typedef enum asmkit_operand_kind {
    ASMKIT_OP_NONE = 0,
    ASMKIT_OP_REG,
    ASMKIT_OP_IMM,
    ASMKIT_OP_MEM,
    ASMKIT_OP_PC_REL,
    ASMKIT_OP_WASM_BLOCKTYPE,
    ASMKIT_OP_WASM_INDEX,
    ASMKIT_OP_OPAQUE,
    ASMKIT_OP_X86_ROUNDING
} asmkit_operand_kind_t;

typedef enum asmkit_instruction_attributes {
    ASMKIT_INSTRUCTION_ATTR_BRANCH = 1u << 0,
    ASMKIT_INSTRUCTION_ATTR_CONDITIONAL = 1u << 1,
    ASMKIT_INSTRUCTION_ATTR_INDIRECT = 1u << 2,
    ASMKIT_INSTRUCTION_ATTR_CALL = 1u << 3,
    ASMKIT_INSTRUCTION_ATTR_RETURN = 1u << 4,
    ASMKIT_INSTRUCTION_ATTR_TERMINATOR = 1u << 5,
    ASMKIT_INSTRUCTION_ATTR_LOAD = 1u << 6,
    ASMKIT_INSTRUCTION_ATTR_STORE = 1u << 7,
    ASMKIT_INSTRUCTION_ATTR_ALU = 1u << 8,
    ASMKIT_INSTRUCTION_ATTR_PC_RELATIVE = 1u << 9,
    ASMKIT_INSTRUCTION_ATTR_PSEUDO = 1u << 10,
    ASMKIT_INSTRUCTION_ATTR_ATOMIC = 1u << 11,
    ASMKIT_INSTRUCTION_ATTR_ACQUIRE_RELEASE = 1u << 12,
    ASMKIT_INSTRUCTION_ATTR_PAIR = 1u << 13,
    ASMKIT_INSTRUCTION_ATTR_X86_NF = 1u << 14,
    ASMKIT_INSTRUCTION_ATTR_X86_SAE = 1u << 15
} asmkit_instruction_attributes_t;

typedef enum asmkit_operand_flags {
    ASMKIT_OPERAND_FLAG_NONE = 0u,
    ASMKIT_OPERAND_FLAG_EXPLICIT = 1u << 0,
    ASMKIT_OPERAND_FLAG_READ = 1u << 1,
    ASMKIT_OPERAND_FLAG_WRITE = 1u << 2,
    ASMKIT_OPERAND_FLAG_RELATIVE = 1u << 3,
    ASMKIT_OPERAND_FLAG_MEMORY = 1u << 4,
    ASMKIT_OPERAND_FLAG_SIGNED = 1u << 5,
    ASMKIT_OPERAND_FLAG_X86_ZEROING = 1u << 6,
    ASMKIT_OPERAND_FLAG_X86_BROADCAST = 1u << 7,
    ASMKIT_OPERAND_FLAG_AARCH64_EXTENDED_REG = 1u << 8,
    ASMKIT_OPERAND_FLAG_REGISTER_LIST = 1u << 9,
    ASMKIT_OPERAND_FLAG_VECTOR_LIST = 1u << 10,
    ASMKIT_OPERAND_FLAG_ARM_NEGATIVE_OFFSET = 1u << 11,
    ASMKIT_OPERAND_FLAG_REGISTER_PAIR = 1u << 12
} asmkit_operand_flags_t;

typedef enum asmkit_inst_flags {
    ASMKIT_INST_FLAG_PC_RELATIVE = 1u << 0,
    ASMKIT_INST_FLAG_DIRECT = 1u << 1,
    ASMKIT_INST_FLAG_CONDITIONAL = 1u << 2,
    ASMKIT_INST_FLAG_CALL = 1u << 3,
    ASMKIT_INST_FLAG_RETURN = 1u << 4,
    ASMKIT_INST_FLAG_TERMINATOR = 1u << 5,
    ASMKIT_INST_FLAG_LITERAL = 1u << 6,
    ASMKIT_INST_FLAG_STATE_SWITCH = 1u << 7,
    ASMKIT_INST_FLAG_PAIR_PREFIX = 1u << 8,
    ASMKIT_INST_FLAG_UNSUPPORTED_RELOC = 1u << 9,
    ASMKIT_INST_FLAG_X86_LOCK = 1u << 10
} asmkit_inst_flags_t;

typedef struct asmkit_memory_operand {
    uint64_t base;
    uint64_t index;
    uint64_t segment;
    int64_t displacement;
    uint16_t address_width;
    uint8_t scale;
} asmkit_memory_operand_t;

typedef struct asmkit_operand {
    asmkit_operand_kind_t kind;
    uint32_t flags;
    uint64_t reg;
    uint64_t shift_reg;
    int64_t imm;
    uint64_t abs_target;
    asmkit_memory_operand_t mem;
    uint16_t width;
    uint8_t shift_kind;
    uint8_t shift_amount;
    uint8_t extend_kind;
    uint8_t extend_amount;
    uint8_t operand_index;
} asmkit_operand_t;

typedef struct asmkit_inst {
    asmkit_arch_t arch;
    asmkit_mode_t mode;
    uint32_t id;
    asmkit_mnemonic_id_t mnemonic_id;
    asmkit_inst_class_t inst_class;
    uint64_t address;
    uint32_t size;
    uint8_t bytes[ASMKIT_MAX_INST_BYTES];
    uint32_t flags;
    uint32_t attributes;
    uint8_t operand_count;
    asmkit_operand_t operands[ASMKIT_MAX_OPERANDS];
} asmkit_inst_t;

static inline asmkit_operand_t asmkit_operand_none(void)
{
    asmkit_operand_t operand;
    operand.kind = ASMKIT_OP_NONE;
    operand.flags = ASMKIT_OPERAND_FLAG_NONE;
    operand.reg = 0u;
    operand.shift_reg = 0u;
    operand.imm = 0;
    operand.abs_target = 0u;
    operand.mem.base = 0u;
    operand.mem.index = 0u;
    operand.mem.segment = 0u;
    operand.mem.displacement = 0;
    operand.mem.address_width = 0u;
    operand.mem.scale = 0u;
    operand.width = 0u;
    operand.shift_kind = 0u;
    operand.shift_amount = 0u;
    operand.extend_kind = 0u;
    operand.extend_amount = 0u;
    operand.operand_index = 0u;
    return operand;
}

static inline asmkit_operand_t asmkit_operand_reg(uint64_t reg, uint16_t width)
{
    asmkit_operand_t operand = asmkit_operand_none();
    operand.kind = ASMKIT_OP_REG;
    operand.reg = reg;
    operand.width = width;
    return operand;
}

static inline asmkit_operand_t asmkit_operand_imm(int64_t imm, uint16_t width)
{
    asmkit_operand_t operand = asmkit_operand_none();
    operand.kind = ASMKIT_OP_IMM;
    operand.imm = imm;
    operand.width = width;
    return operand;
}

static inline asmkit_operand_t asmkit_operand_opaque(uint64_t value, uint16_t width)
{
    asmkit_operand_t operand = asmkit_operand_none();
    operand.kind = ASMKIT_OP_OPAQUE;
    operand.imm = (int64_t)value;
    operand.width = width;
    return operand;
}

static inline asmkit_operand_t asmkit_operand_mem(uint64_t base_reg, int64_t disp, uint16_t width)
{
    asmkit_operand_t operand = asmkit_operand_none();
    operand.kind = ASMKIT_OP_MEM;
    operand.mem.base = base_reg;
    operand.mem.displacement = disp;
    operand.mem.scale = 1u;
    operand.width = width;
    return operand;
}

static inline asmkit_operand_t asmkit_operand_mem_full(
    uint64_t base_reg,
    uint64_t index_reg,
    uint8_t scale,
    int64_t disp,
    uint16_t width,
    uint16_t address_width)
{
    asmkit_operand_t operand = asmkit_operand_mem(base_reg, disp, width);
    operand.mem.index = index_reg;
    operand.mem.scale = scale == 0u ? 1u : scale;
    operand.mem.address_width = address_width;
    return operand;
}

static inline asmkit_operand_t asmkit_operand_with_flags(asmkit_operand_t operand, uint32_t flags)
{
    operand.flags |= flags;
    return operand;
}

static inline asmkit_operand_t asmkit_operand_x86_zeroing_mask(uint64_t mask_reg, uint16_t width)
{
    return asmkit_operand_with_flags(asmkit_operand_reg(mask_reg, width), ASMKIT_OPERAND_FLAG_X86_ZEROING);
}

static inline asmkit_operand_t asmkit_operand_x86_broadcast_mem(
    uint64_t base_reg,
    uint64_t index_reg,
    uint8_t scale,
    int64_t disp,
    uint16_t element_width,
    uint16_t address_width)
{
    return asmkit_operand_with_flags(
        asmkit_operand_mem_full(base_reg, index_reg, scale, disp, element_width, address_width),
        ASMKIT_OPERAND_FLAG_X86_BROADCAST);
}

static inline asmkit_operand_t asmkit_operand_mem_segment(
    uint64_t segment_reg,
    uint64_t base_reg,
    uint64_t index_reg,
    uint8_t scale,
    int64_t disp,
    uint16_t width,
    uint16_t address_width)
{
    asmkit_operand_t operand = asmkit_operand_mem_full(base_reg, index_reg, scale, disp, width, address_width);
    operand.mem.segment = segment_reg;
    return operand;
}

static inline asmkit_operand_t asmkit_operand_pc_rel(int64_t disp, uint16_t width)
{
    asmkit_operand_t operand = asmkit_operand_none();
    operand.kind = ASMKIT_OP_PC_REL;
    operand.imm = disp;
    operand.width = width;
    return operand;
}

static inline asmkit_operand_t asmkit_operand_branch_target(uint64_t abs_target)
{
    asmkit_operand_t operand = asmkit_operand_none();
    operand.kind = ASMKIT_OP_PC_REL;
    operand.abs_target = abs_target;
    return operand;
}

static inline void asmkit_inst_init(asmkit_inst_t* inst, asmkit_arch_t arch, asmkit_mode_t mode, asmkit_mnemonic_id_t mnemonic_id)
{
    uint32_t i;
    inst->arch = arch;
    inst->mode = mode;
    inst->id = 0u;
    inst->mnemonic_id = mnemonic_id;
    inst->inst_class = ASMKIT_INST_OTHER;
    inst->address = 0u;
    inst->size = 0u;
    for (i = 0u; i < ASMKIT_MAX_INST_BYTES; ++i) {
        inst->bytes[i] = 0u;
    }
    inst->flags = 0u;
    inst->attributes = 0u;
    inst->operand_count = 0u;
    for (i = 0u; i < ASMKIT_MAX_OPERANDS; ++i) {
        inst->operands[i] = asmkit_operand_none();
        inst->operands[i].operand_index = (uint8_t)i;
    }
}

static inline asmkit_status_t asmkit_inst_set_operand(asmkit_inst_t* inst, uint8_t index, asmkit_operand_t operand)
{
    if (inst == 0 || index >= ASMKIT_MAX_OPERANDS) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    operand.operand_index = index;
    inst->operands[index] = operand;
    if (inst->operand_count <= index) {
        inst->operand_count = (uint8_t)(index + 1u);
    }
    return ASMKIT_OK;
}

struct asmkit_engine;
struct asmkit_workspace;

asmkit_status_t asmkit_decode_one(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const uint8_t* code,
    size_t code_size,
    uint64_t address,
    asmkit_inst_t* out_inst);

typedef struct asmkit_decode_block_result {
    uint32_t inst_count;
    uint32_t total_size;
    uint32_t flags;
} asmkit_decode_block_result_t;

asmkit_status_t asmkit_decode_block_until(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const uint8_t* code,
    size_t code_size,
    uint64_t address,
    uint32_t min_size,
    asmkit_inst_t* out_insts,
    uint32_t out_inst_capacity,
    asmkit_decode_block_result_t* out_result);

#ifdef __cplusplus
}
#endif

#endif

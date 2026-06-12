#ifndef ASMKIT_METADATA_H
#define ASMKIT_METADATA_H

#include <stdint.h>
#include "asmkit/asmkit_arch.h"
#include "asmkit/asmkit_decode.h"
#include "asmkit/asmkit_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum asmkit_register_flags {
    ASMKIT_REGISTER_FLAG_SUBREGISTER = 1u << 0,
    ASMKIT_REGISTER_FLAG_STACK_POINTER = 1u << 1,
    ASMKIT_REGISTER_FLAG_FRAME_POINTER = 1u << 2,
    ASMKIT_REGISTER_FLAG_RETURN_VALUE = 1u << 3
} asmkit_register_flags_t;

typedef enum asmkit_encoding_form {
    ASMKIT_ENCODING_FORM_UNKNOWN = 0,
    ASMKIT_ENCODING_FORM_BPF_ALU_RR,
    ASMKIT_ENCODING_FORM_BPF_ALU_RI,
    ASMKIT_ENCODING_FORM_BPF_ALU_DST,
    ASMKIT_ENCODING_FORM_BPF_LOAD_MEM,
    ASMKIT_ENCODING_FORM_BPF_STORE_MEM_REG,
    ASMKIT_ENCODING_FORM_BPF_STORE_MEM_IMM,
    ASMKIT_ENCODING_FORM_BPF_BRANCH,
    ASMKIT_ENCODING_FORM_BPF_BRANCH_RR,
    ASMKIT_ENCODING_FORM_BPF_BRANCH_RI,
    ASMKIT_ENCODING_FORM_BPF_CALL_IMM,
    ASMKIT_ENCODING_FORM_BPF_CALL_REG,
    ASMKIT_ENCODING_FORM_BPF_LD_IMM64,
    ASMKIT_ENCODING_FORM_BPF_LD_PSEUDO,
    ASMKIT_ENCODING_FORM_BPF_LOAD_ABS,
    ASMKIT_ENCODING_FORM_BPF_LOAD_IND,
    ASMKIT_ENCODING_FORM_X86_RAW,
    ASMKIT_ENCODING_FORM_X86_ADD_REG,
    ASMKIT_ENCODING_FORM_X86_MODRM,
    ASMKIT_ENCODING_FORM_X86_MODRM_REG,
    ASMKIT_ENCODING_FORM_X86_MODRM_MEM,
    ASMKIT_ENCODING_FORM_X86_FIXED_MODRM,
    ASMKIT_ENCODING_FORM_FIXED,
    ASMKIT_ENCODING_FORM_ARM_BITFIELD,
    ASMKIT_ENCODING_FORM_AARCH64_BITFIELD,
    ASMKIT_ENCODING_FORM_WASM_OPCODE,
    ASMKIT_ENCODING_FORM_WASM_PREFIXED_OPCODE
} asmkit_encoding_form_t;

typedef enum asmkit_encoding_flags {
    ASMKIT_ENCODING_FLAG_FIXED_OFF = 1u << 0,
    ASMKIT_ENCODING_FLAG_FIXED_IMM = 1u << 1,
    ASMKIT_ENCODING_FLAG_FIXED_SRC = 1u << 2,
    ASMKIT_ENCODING_FLAG_ATOMIC = 1u << 3,
    ASMKIT_ENCODING_FLAG_ACQUIRE_RELEASE = 1u << 4,
    ASMKIT_ENCODING_FLAG_PSEUDO_PAIR = 1u << 5,
    ASMKIT_ENCODING_FLAG_X86_MODRM = 1u << 6,
    ASMKIT_ENCODING_FLAG_X86_MODRM_REG_ONLY = 1u << 7,
    ASMKIT_ENCODING_FLAG_X86_MODRM_MEM_ONLY = 1u << 8,
    ASMKIT_ENCODING_FLAG_X86_FIXED_REG = 1u << 9,
    ASMKIT_ENCODING_FLAG_X86_FIXED_MODRM = 1u << 10,
    ASMKIT_ENCODING_FLAG_X86_REX_W = 1u << 11,
    ASMKIT_ENCODING_FLAG_X86_MANDATORY_PREFIX = 1u << 12,
    ASMKIT_ENCODING_FLAG_X86_PC_RELATIVE = 1u << 13,
    ASMKIT_ENCODING_FLAG_X86_OPCODE_PLUS_REG = 1u << 14,
    ASMKIT_ENCODING_FLAG_X86_VEX = 1u << 15,
    ASMKIT_ENCODING_FLAG_X86_XOP = 1u << 16,
    ASMKIT_ENCODING_FLAG_X86_EVEX = 1u << 17,
    ASMKIT_ENCODING_FLAG_X86_VEX_L = 1u << 18,
    ASMKIT_ENCODING_FLAG_X86_EVEX_L2 = 1u << 19,
    ASMKIT_ENCODING_FLAG_X86_VEX_4V = 1u << 20,
    ASMKIT_ENCODING_FLAG_X86_EVEX_K = 1u << 21,
    ASMKIT_ENCODING_FLAG_X86_EVEX_Z = 1u << 22,
    ASMKIT_ENCODING_FLAG_X86_EVEX_B = 1u << 23,
    ASMKIT_ENCODING_FLAG_X86_IGNORES_W = 1u << 24,
    ASMKIT_ENCODING_FLAG_X86_IGNORES_VEX_L = 1u << 25,
    ASMKIT_ENCODING_FLAG_X86_EVEX_NF = 1u << 26,
    ASMKIT_ENCODING_FLAG_X86_EVEX_U = 1u << 27
} asmkit_encoding_flags_t;

typedef enum asmkit_instruction_metadata_flags {
    ASMKIT_INSTRUCTION_META_FLAG_DECODE = 1u << 0,
    ASMKIT_INSTRUCTION_META_FLAG_ENCODE = 1u << 1,
    ASMKIT_INSTRUCTION_META_FLAG_PREDICATE_GATED = 1u << 2
} asmkit_instruction_metadata_flags_t;

typedef enum asmkit_operand_role {
    ASMKIT_OPERAND_ROLE_NONE = 0,
    ASMKIT_OPERAND_ROLE_DESTINATION,
    ASMKIT_OPERAND_ROLE_SOURCE,
    ASMKIT_OPERAND_ROLE_MEMORY,
    ASMKIT_OPERAND_ROLE_BASE,
    ASMKIT_OPERAND_ROLE_IMMEDIATE,
    ASMKIT_OPERAND_ROLE_BRANCH_TARGET,
    ASMKIT_OPERAND_ROLE_CALL_TARGET,
    ASMKIT_OPERAND_ROLE_PSEUDO_KIND,
    ASMKIT_OPERAND_ROLE_ROUNDING_CONTROL
} asmkit_operand_role_t;

typedef enum asmkit_operand_encoding {
    ASMKIT_OPERAND_ENCODING_NONE = 0,
    ASMKIT_OPERAND_ENCODING_REGISTER,
    ASMKIT_OPERAND_ENCODING_IMMEDIATE,
    ASMKIT_OPERAND_ENCODING_MEMORY_BASE_DISP,
    ASMKIT_OPERAND_ENCODING_PC_RELATIVE,
    ASMKIT_OPERAND_ENCODING_WIDE_IMMEDIATE,
    ASMKIT_OPERAND_ENCODING_X86_ROUNDING,
    ASMKIT_OPERAND_ENCODING_ARM_SHIFTED_REGISTER,
    ASMKIT_OPERAND_ENCODING_ARM_MEMORY_SHIFTED_REGISTER,
    ASMKIT_OPERAND_ENCODING_ARM_MEMORY_OFFSET
} asmkit_operand_encoding_t;

typedef enum asmkit_register_effect_flags {
    ASMKIT_REGISTER_EFFECT_READ = 1u << 0,
    ASMKIT_REGISTER_EFFECT_WRITE = 1u << 1,
    ASMKIT_REGISTER_EFFECT_IMPLICIT = 1u << 2
} asmkit_register_effect_flags_t;

typedef enum asmkit_predicate_flags {
    ASMKIT_PREDICATE_FLAG_TARGET_LOCAL = 1u << 0,
    ASMKIT_PREDICATE_FLAG_COMMON = 1u << 1,
    ASMKIT_PREDICATE_FLAG_TARGET_FEATURE = 1u << 2,
    ASMKIT_PREDICATE_FLAG_SUBTARGET_CAPABILITY = 1u << 3
} asmkit_predicate_flags_t;

typedef enum asmkit_predicate_role {
    ASMKIT_PREDICATE_ROLE_UNKNOWN = 0,
    ASMKIT_PREDICATE_ROLE_TARGET_FEATURE = 1,
    ASMKIT_PREDICATE_ROLE_SUBTARGET_CAPABILITY = 2
} asmkit_predicate_role_t;

#define ASMKIT_FEATURE_ID_INVALID UINT32_C(0xffffffff)

typedef struct asmkit_register_info {
    uint32_t id;
    asmkit_arch_t arch;
    const char* name;
    uint16_t encoding;
    uint16_t parent_id;
    uint16_t width;
    uint32_t flags;
} asmkit_register_info_t;

typedef struct asmkit_operand_type_info {
    uint32_t id;
    asmkit_arch_t arch;
    const char* name;
    asmkit_operand_kind_t kind;
    asmkit_operand_encoding_t encoding;
    uint16_t width;
} asmkit_operand_type_info_t;

typedef struct asmkit_instruction_info {
    uint32_t id;
    asmkit_mnemonic_id_t mnemonic_id;
    asmkit_arch_t arch;
    const char* name;
    const char* llvm_name;
    asmkit_inst_class_t inst_class;
    uint32_t flags;
    uint8_t size;
    uint8_t operand_count;
    uint8_t opcode;
    asmkit_encoding_form_t encoding_form;
    uint32_t encoding_flags;
    uint32_t attributes;
    uint32_t register_effect_index;
    uint32_t register_effect_count;
    uint32_t operand_info_index;
    uint32_t operand_info_count;
    uint32_t metadata_flags;
} asmkit_instruction_info_t;

typedef struct asmkit_operand_info {
    uint32_t instruction_id;
    asmkit_arch_t arch;
    uint8_t index;
    uint32_t target_operand_type_id;
    const char* name;
    asmkit_operand_kind_t kind;
    asmkit_operand_role_t role;
    asmkit_operand_encoding_t encoding;
    uint16_t width;
    uint32_t flags;
} asmkit_operand_info_t;

typedef struct asmkit_instruction_register_effect {
    uint32_t instruction_id;
    asmkit_arch_t arch;
    uint8_t index;
    uint32_t register_id;
    uint32_t flags;
} asmkit_instruction_register_effect_t;

typedef struct asmkit_predicate_info {
    uint32_t id;
    asmkit_arch_t arch;
    const char* name;
    uint32_t flags;
    asmkit_predicate_role_t role;
    uint32_t feature_id;
} asmkit_predicate_info_t;

typedef struct asmkit_feature_info {
    uint32_t id;
    asmkit_arch_t arch;
    const char* name;
    const char* llvm_name;
} asmkit_feature_info_t;

typedef struct asmkit_cpu_info {
    uint32_t id;
    asmkit_arch_t arch;
    const char* name;
    uint64_t feature_bits_lo;
    uint64_t feature_bits_hi;
    uint64_t feature_bits_2;
    uint64_t feature_bits_3;
    uint64_t feature_bits_4;
    uint64_t feature_bits_5;
    uint64_t feature_bits_6;
    uint64_t feature_bits_7;
    uint32_t feature_index;
    uint32_t feature_count;
} asmkit_cpu_info_t;

uint32_t asmkit_get_register_count(asmkit_arch_t arch);
uint32_t asmkit_get_instruction_count(asmkit_arch_t arch);
uint32_t asmkit_get_predicate_count(asmkit_arch_t arch);
uint32_t asmkit_get_feature_count(asmkit_arch_t arch);
uint32_t asmkit_get_cpu_count(asmkit_arch_t arch);
uint32_t asmkit_get_operand_type_count(asmkit_arch_t arch);

asmkit_status_t asmkit_get_register_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_register_info_t** out_info);
asmkit_status_t asmkit_get_register_info(asmkit_arch_t arch, uint32_t register_id, const asmkit_register_info_t** out_info);
asmkit_status_t asmkit_find_register_by_name(asmkit_arch_t arch, const char* name, const asmkit_register_info_t** out_info);

asmkit_status_t asmkit_get_operand_type_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_operand_type_info_t** out_info);
asmkit_status_t asmkit_get_operand_type_info(asmkit_arch_t arch, uint32_t operand_type_id, const asmkit_operand_type_info_t** out_info);
asmkit_status_t asmkit_find_operand_type_by_name(asmkit_arch_t arch, const char* name, const asmkit_operand_type_info_t** out_info);

asmkit_status_t asmkit_get_instruction_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_instruction_info_t** out_info);
asmkit_status_t asmkit_get_instruction_info(asmkit_arch_t arch, uint32_t instruction_id, const asmkit_instruction_info_t** out_info);
asmkit_status_t asmkit_find_instruction_by_name(asmkit_arch_t arch, const char* name, const asmkit_instruction_info_t** out_info);
asmkit_status_t asmkit_get_instruction_operand_info(asmkit_arch_t arch, uint32_t instruction_id, uint32_t operand_index, const asmkit_operand_info_t** out_info);
asmkit_status_t asmkit_get_instruction_register_effect(
    asmkit_arch_t arch,
    uint32_t instruction_id,
    uint32_t effect_index,
    const asmkit_instruction_register_effect_t** out_info);

asmkit_status_t asmkit_get_predicate_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_predicate_info_t** out_info);
asmkit_status_t asmkit_get_predicate_info(asmkit_arch_t arch, uint32_t predicate_id, const asmkit_predicate_info_t** out_info);
asmkit_status_t asmkit_find_predicate_by_name(asmkit_arch_t arch, const char* name, const asmkit_predicate_info_t** out_info);

asmkit_status_t asmkit_get_feature_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_feature_info_t** out_info);
asmkit_status_t asmkit_get_feature_info(asmkit_arch_t arch, uint32_t feature_id, const asmkit_feature_info_t** out_info);
asmkit_status_t asmkit_find_feature_by_name(asmkit_arch_t arch, const char* name, const asmkit_feature_info_t** out_info);

asmkit_status_t asmkit_get_cpu_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_cpu_info_t** out_info);
asmkit_status_t asmkit_get_cpu_info(asmkit_arch_t arch, uint32_t cpu_id, const asmkit_cpu_info_t** out_info);
asmkit_status_t asmkit_find_cpu_by_name(asmkit_arch_t arch, const char* name, const asmkit_cpu_info_t** out_info);
asmkit_status_t asmkit_get_cpu_feature_info(
    asmkit_arch_t arch,
    uint32_t cpu_id,
    uint32_t cpu_feature_index,
    const asmkit_feature_info_t** out_info);
asmkit_status_t asmkit_cpu_has_feature(asmkit_arch_t arch, uint32_t cpu_id, uint32_t feature_id, int* out_enabled);

#ifdef __cplusplus
}
#endif

#endif

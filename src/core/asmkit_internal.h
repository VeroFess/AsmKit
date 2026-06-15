#ifndef ASMKIT_INTERNAL_H
#define ASMKIT_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include "asmkit/asmkit.h"

#ifndef ASMKIT_RESTRICT
#if defined(__cplusplus)
#if defined(_MSC_VER)
#define ASMKIT_RESTRICT __restrict
#elif defined(__GNUC__) || defined(__clang__)
#define ASMKIT_RESTRICT __restrict__
#else
#define ASMKIT_RESTRICT
#endif
#elif defined(_MSC_VER)
#define ASMKIT_RESTRICT __restrict
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define ASMKIT_RESTRICT restrict
#elif defined(__GNUC__) || defined(__clang__)
#define ASMKIT_RESTRICT __restrict__
#else
#define ASMKIT_RESTRICT
#endif
#endif

#ifndef ASMKIT_RESTRICT_X86
#define ASMKIT_RESTRICT_X86
#endif
#ifndef ASMKIT_RESTRICT_ARM
#define ASMKIT_RESTRICT_ARM
#endif
#ifndef ASMKIT_RESTRICT_AARCH64
#define ASMKIT_RESTRICT_AARCH64
#endif
#ifndef ASMKIT_RESTRICT_BPF
#define ASMKIT_RESTRICT_BPF ASMKIT_RESTRICT
#endif
#ifndef ASMKIT_RESTRICT_WASM
#define ASMKIT_RESTRICT_WASM ASMKIT_RESTRICT
#endif

#ifndef ASMKIT_RESTRICT_X86_CONST
#define ASMKIT_RESTRICT_X86_CONST ASMKIT_RESTRICT_X86
#endif
#ifndef ASMKIT_RESTRICT_X86_CODE
#define ASMKIT_RESTRICT_X86_CODE ASMKIT_RESTRICT
#endif
#ifndef ASMKIT_RESTRICT_X86_OUT
#define ASMKIT_RESTRICT_X86_OUT ASMKIT_RESTRICT
#endif
#ifndef ASMKIT_RESTRICT_X86_SCRATCH
#define ASMKIT_RESTRICT_X86_SCRATCH ASMKIT_RESTRICT_X86
#endif

#ifndef ASMKIT_RESTRICT_ARM_CONST
#define ASMKIT_RESTRICT_ARM_CONST ASMKIT_RESTRICT
#endif
#ifndef ASMKIT_RESTRICT_ARM_CODE
#define ASMKIT_RESTRICT_ARM_CODE ASMKIT_RESTRICT
#endif
#ifndef ASMKIT_RESTRICT_ARM_OUT
#define ASMKIT_RESTRICT_ARM_OUT ASMKIT_RESTRICT
#endif
#ifndef ASMKIT_RESTRICT_ARM_SCRATCH
#define ASMKIT_RESTRICT_ARM_SCRATCH ASMKIT_RESTRICT
#endif

#ifndef ASMKIT_RESTRICT_AARCH64_CONST
#define ASMKIT_RESTRICT_AARCH64_CONST ASMKIT_RESTRICT_AARCH64
#endif
#ifndef ASMKIT_RESTRICT_AARCH64_CODE
#define ASMKIT_RESTRICT_AARCH64_CODE ASMKIT_RESTRICT
#endif
#ifndef ASMKIT_RESTRICT_AARCH64_OUT
#define ASMKIT_RESTRICT_AARCH64_OUT ASMKIT_RESTRICT
#endif
#ifndef ASMKIT_RESTRICT_AARCH64_SCRATCH
#define ASMKIT_RESTRICT_AARCH64_SCRATCH ASMKIT_RESTRICT
#endif

#ifndef ASMKIT_RESTRICT_BPF_CONST
#define ASMKIT_RESTRICT_BPF_CONST ASMKIT_RESTRICT_BPF
#endif
#ifndef ASMKIT_RESTRICT_BPF_CODE
#define ASMKIT_RESTRICT_BPF_CODE ASMKIT_RESTRICT_BPF
#endif
#ifndef ASMKIT_RESTRICT_BPF_OUT
#define ASMKIT_RESTRICT_BPF_OUT ASMKIT_RESTRICT_BPF
#endif
#ifndef ASMKIT_RESTRICT_BPF_SCRATCH
#define ASMKIT_RESTRICT_BPF_SCRATCH ASMKIT_RESTRICT_BPF
#endif

#ifndef ASMKIT_RESTRICT_WASM_CONST
#define ASMKIT_RESTRICT_WASM_CONST ASMKIT_RESTRICT_WASM
#endif
#ifndef ASMKIT_RESTRICT_WASM_CODE
#define ASMKIT_RESTRICT_WASM_CODE ASMKIT_RESTRICT_WASM
#endif
#ifndef ASMKIT_RESTRICT_WASM_OUT
#define ASMKIT_RESTRICT_WASM_OUT ASMKIT_RESTRICT_WASM
#endif
#ifndef ASMKIT_RESTRICT_WASM_SCRATCH
#define ASMKIT_RESTRICT_WASM_SCRATCH ASMKIT_RESTRICT_WASM
#endif

#define ASMKIT_ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define ASMKIT_REG_X86_RAX 0u
#define ASMKIT_REG_X86_RCX 1u
#define ASMKIT_REG_X86_RDX 2u
#define ASMKIT_REG_X86_RBX 3u
#define ASMKIT_REG_X86_RSP 4u
#define ASMKIT_REG_X86_RBP 5u
#define ASMKIT_REG_X86_RSI 6u
#define ASMKIT_REG_X86_RDI 7u
#define ASMKIT_REG_AARCH64_X16 16u
#define ASMKIT_REG_ARM_IP 12u

typedef struct asmkit_branch_plan_bound {
    uint32_t size;
    bool requires_near_island;
    int64_t island_min_distance;
    int64_t island_max_distance;
    uint64_t clobber_mask_lo;
    uint64_t clobber_mask_hi;
} asmkit_branch_plan_bound_t;

typedef struct asmkit_target_ops {
    asmkit_arch_t arch;
    asmkit_mode_t first_mode;
    asmkit_mode_t last_mode;
    asmkit_status_t (*decode_one)(const asmkit_engine_t* ASMKIT_RESTRICT, asmkit_workspace_t* ASMKIT_RESTRICT, const uint8_t* ASMKIT_RESTRICT, size_t, uint64_t, asmkit_inst_t* ASMKIT_RESTRICT);
    asmkit_status_t (*analyze_inst)(const asmkit_engine_t* ASMKIT_RESTRICT, asmkit_workspace_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, asmkit_inst_semantics_t* ASMKIT_RESTRICT);
    asmkit_status_t (*encode_inst)(const asmkit_engine_t* ASMKIT_RESTRICT, asmkit_workspace_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, const asmkit_encode_options_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_encode_result_t* ASMKIT_RESTRICT);
    asmkit_status_t (*emit_jump)(const asmkit_engine_t* ASMKIT_RESTRICT, asmkit_workspace_t* ASMKIT_RESTRICT, uint64_t, uint64_t, const asmkit_emit_options_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_emit_result_t* ASMKIT_RESTRICT);
    asmkit_status_t (*emit_call)(const asmkit_engine_t* ASMKIT_RESTRICT, asmkit_workspace_t* ASMKIT_RESTRICT, uint64_t, uint64_t, const asmkit_emit_options_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_emit_result_t* ASMKIT_RESTRICT);
    asmkit_status_t (*relocate_inst)(const asmkit_engine_t* ASMKIT_RESTRICT, asmkit_workspace_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, uint64_t, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_emit_result_t* ASMKIT_RESTRICT);
    uint32_t (*min_patch_size)(const asmkit_engine_t* ASMKIT_RESTRICT, uint64_t, uint64_t, asmkit_branch_mode_t, uint64_t* ASMKIT_RESTRICT, uint64_t* ASMKIT_RESTRICT);
    asmkit_status_t (*plan_jump_back)(const asmkit_engine_t* ASMKIT_RESTRICT, asmkit_branch_mode_t, asmkit_branch_plan_bound_t* ASMKIT_RESTRICT);
} asmkit_target_ops_t;

const asmkit_target_ops_t* asmkit_lookup_target_ops(asmkit_arch_t arch, asmkit_mode_t mode);
const asmkit_target_ops_t* asmkit_engine_ops(const asmkit_engine_t* ASMKIT_RESTRICT engine);
int asmkit_target_feature_enabled(const asmkit_engine_t* ASMKIT_RESTRICT engine, uint32_t feature_bit);
uint64_t asmkit_engine_feature_word(const asmkit_engine_t* ASMKIT_RESTRICT engine, uint32_t word_index);

void asmkit_zero(void* ptr, size_t size);
void asmkit_copy(void* dst, const void* src, size_t size);
int asmkit_i64_fits_i8(int64_t value);
int asmkit_i64_fits_i16(int64_t value);
int asmkit_i64_fits_i24_words4(int64_t value);
int asmkit_i64_fits_i26_words4(int64_t value);
int asmkit_i64_fits_i32(int64_t value);

uint16_t asmkit_load16le(const uint8_t* p);
uint32_t asmkit_load32le(const uint8_t* p);
uint64_t asmkit_load64le(const uint8_t* p);
void asmkit_store16le(uint8_t* p, uint16_t v);
void asmkit_store32le(uint8_t* p, uint32_t v);
void asmkit_store64le(uint8_t* p, uint64_t v);

asmkit_status_t asmkit_gen_x86_decode_one(const asmkit_engine_t* ASMKIT_RESTRICT, const uint8_t* ASMKIT_RESTRICT, size_t, uint64_t, asmkit_inst_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_arm_decode_one(const asmkit_engine_t* ASMKIT_RESTRICT, const uint8_t* ASMKIT_RESTRICT, size_t, uint64_t, asmkit_inst_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_aarch64_decode_one(const asmkit_engine_t* ASMKIT_RESTRICT, const uint8_t* ASMKIT_RESTRICT, size_t, uint64_t, asmkit_inst_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_bpf_decode_one(const asmkit_engine_t* ASMKIT_RESTRICT, const uint8_t* ASMKIT_RESTRICT, size_t, uint64_t, asmkit_inst_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_wasm_decode_one(const asmkit_engine_t* ASMKIT_RESTRICT, const uint8_t* ASMKIT_RESTRICT, size_t, uint64_t, asmkit_inst_t* ASMKIT_RESTRICT);

asmkit_status_t asmkit_gen_x86_emit_rel(const asmkit_engine_t* ASMKIT_RESTRICT, int is_call, uint64_t, uint64_t, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_emit_result_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_aarch64_emit_rel(const asmkit_engine_t* ASMKIT_RESTRICT, int is_call, uint64_t, uint64_t, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_emit_result_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_arm_emit_rel(const asmkit_engine_t* ASMKIT_RESTRICT, int is_call, uint64_t, uint64_t, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_emit_result_t* ASMKIT_RESTRICT);

asmkit_status_t asmkit_gen_x86_encode_inst(const asmkit_engine_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_encode_result_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_aarch64_encode_inst(const asmkit_engine_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_encode_result_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_aarch64_encode_fixed(const asmkit_engine_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_encode_result_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_arm_encode_fixed(const asmkit_engine_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_encode_result_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_arm_encode_inst(const asmkit_engine_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_encode_result_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_bpf_encode_inst(const asmkit_engine_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_encode_result_t* ASMKIT_RESTRICT);
asmkit_status_t asmkit_gen_wasm_encode_inst(const asmkit_engine_t* ASMKIT_RESTRICT, const asmkit_inst_t* ASMKIT_RESTRICT, uint8_t* ASMKIT_RESTRICT, size_t, asmkit_encode_result_t* ASMKIT_RESTRICT);

const char* asmkit_gen_x86_mnemonic(uint32_t id);
const char* asmkit_gen_arm_mnemonic(uint32_t id);
const char* asmkit_gen_aarch64_mnemonic(uint32_t id);
const char* asmkit_gen_bpf_mnemonic(uint32_t id);
const char* asmkit_gen_wasm_mnemonic(uint32_t id);

const asmkit_feature_info_t* asmkit_gen_x86_feature_info_by_index(uint32_t index);
const asmkit_feature_info_t* asmkit_gen_x86_feature_info(uint32_t id);
uint32_t asmkit_gen_x86_feature_count(void);
const asmkit_register_info_t* asmkit_gen_x86_register_info_by_index(uint32_t index);
const asmkit_register_info_t* asmkit_gen_x86_register_info(uint32_t id);
uint32_t asmkit_gen_x86_register_count(void);
const asmkit_operand_type_info_t* asmkit_gen_x86_operand_type_info_by_index(uint32_t index);
const asmkit_operand_type_info_t* asmkit_gen_x86_operand_type_info(uint32_t id);
uint32_t asmkit_gen_x86_operand_type_count(void);
const asmkit_predicate_info_t* asmkit_gen_x86_predicate_info_by_index(uint32_t index);
const asmkit_predicate_info_t* asmkit_gen_x86_predicate_info(uint32_t id);
uint32_t asmkit_gen_x86_predicate_count(void);
const asmkit_cpu_info_t* asmkit_gen_x86_cpu_info_by_index(uint32_t index);
const asmkit_cpu_info_t* asmkit_gen_x86_cpu_info(uint32_t id);
const asmkit_feature_info_t* asmkit_gen_x86_cpu_feature_info(uint32_t id, uint32_t index);
uint32_t asmkit_gen_x86_cpu_count(void);
const asmkit_instruction_info_t* asmkit_gen_x86_instruction_info_by_index(uint32_t index);
const asmkit_instruction_info_t* asmkit_gen_x86_instruction_info(uint32_t id);
uint32_t asmkit_gen_x86_instruction_count(void);
const asmkit_operand_info_t* asmkit_gen_x86_instruction_operand_infos(uint32_t id, uint32_t* ASMKIT_RESTRICT out_count);
const asmkit_operand_info_t* asmkit_gen_x86_instruction_operand_info(uint32_t id, uint32_t operand_index);
const asmkit_instruction_register_effect_t* asmkit_gen_x86_instruction_register_effect(uint32_t id, uint32_t effect_index);

const asmkit_feature_info_t* asmkit_gen_arm_feature_info_by_index(uint32_t index);
const asmkit_feature_info_t* asmkit_gen_arm_feature_info(uint32_t id);
uint32_t asmkit_gen_arm_feature_count(void);
const asmkit_register_info_t* asmkit_gen_arm_register_info_by_index(uint32_t index);
const asmkit_register_info_t* asmkit_gen_arm_register_info(uint32_t id);
uint32_t asmkit_gen_arm_register_count(void);
const asmkit_operand_type_info_t* asmkit_gen_arm_operand_type_info_by_index(uint32_t index);
const asmkit_operand_type_info_t* asmkit_gen_arm_operand_type_info(uint32_t id);
uint32_t asmkit_gen_arm_operand_type_count(void);
const asmkit_predicate_info_t* asmkit_gen_arm_predicate_info_by_index(uint32_t index);
const asmkit_predicate_info_t* asmkit_gen_arm_predicate_info(uint32_t id);
uint32_t asmkit_gen_arm_predicate_count(void);
const asmkit_cpu_info_t* asmkit_gen_arm_cpu_info_by_index(uint32_t index);
const asmkit_cpu_info_t* asmkit_gen_arm_cpu_info(uint32_t id);
const asmkit_feature_info_t* asmkit_gen_arm_cpu_feature_info(uint32_t id, uint32_t index);
uint32_t asmkit_gen_arm_cpu_count(void);
const asmkit_instruction_info_t* asmkit_gen_arm_instruction_info_by_index(uint32_t index);
const asmkit_instruction_info_t* asmkit_gen_arm_instruction_info(uint32_t id);
uint32_t asmkit_gen_arm_instruction_count(void);
const asmkit_operand_info_t* asmkit_gen_arm_instruction_operand_infos(uint32_t id, uint32_t* ASMKIT_RESTRICT out_count);
const asmkit_operand_info_t* asmkit_gen_arm_instruction_operand_info(uint32_t id, uint32_t operand_index);
const asmkit_instruction_register_effect_t* asmkit_gen_arm_instruction_register_effect(uint32_t id, uint32_t effect_index);

const asmkit_feature_info_t* asmkit_gen_aarch64_feature_info_by_index(uint32_t index);
const asmkit_feature_info_t* asmkit_gen_aarch64_feature_info(uint32_t id);
uint32_t asmkit_gen_aarch64_feature_count(void);
const asmkit_register_info_t* asmkit_gen_aarch64_register_info_by_index(uint32_t index);
const asmkit_register_info_t* asmkit_gen_aarch64_register_info(uint32_t id);
uint32_t asmkit_gen_aarch64_register_count(void);
const asmkit_operand_type_info_t* asmkit_gen_aarch64_operand_type_info_by_index(uint32_t index);
const asmkit_operand_type_info_t* asmkit_gen_aarch64_operand_type_info(uint32_t id);
uint32_t asmkit_gen_aarch64_operand_type_count(void);
const asmkit_predicate_info_t* asmkit_gen_aarch64_predicate_info_by_index(uint32_t index);
const asmkit_predicate_info_t* asmkit_gen_aarch64_predicate_info(uint32_t id);
uint32_t asmkit_gen_aarch64_predicate_count(void);
const asmkit_cpu_info_t* asmkit_gen_aarch64_cpu_info_by_index(uint32_t index);
const asmkit_cpu_info_t* asmkit_gen_aarch64_cpu_info(uint32_t id);
const asmkit_feature_info_t* asmkit_gen_aarch64_cpu_feature_info(uint32_t id, uint32_t index);
uint32_t asmkit_gen_aarch64_cpu_count(void);
const asmkit_instruction_info_t* asmkit_gen_aarch64_instruction_info_by_index(uint32_t index);
const asmkit_instruction_info_t* asmkit_gen_aarch64_instruction_info(uint32_t id);
uint32_t asmkit_gen_aarch64_instruction_count(void);
const asmkit_operand_info_t* asmkit_gen_aarch64_instruction_operand_infos(uint32_t id, uint32_t* ASMKIT_RESTRICT out_count);
const asmkit_operand_info_t* asmkit_gen_aarch64_instruction_operand_info(uint32_t id, uint32_t operand_index);
const asmkit_instruction_register_effect_t* asmkit_gen_aarch64_instruction_register_effect(uint32_t id, uint32_t effect_index);

const asmkit_feature_info_t* asmkit_gen_bpf_feature_info_by_index(uint32_t index);
const asmkit_feature_info_t* asmkit_gen_bpf_feature_info(uint32_t id);
uint32_t asmkit_gen_bpf_feature_count(void);
const asmkit_predicate_info_t* asmkit_gen_bpf_predicate_info_by_index(uint32_t index);
const asmkit_predicate_info_t* asmkit_gen_bpf_predicate_info(uint32_t id);
uint32_t asmkit_gen_bpf_predicate_count(void);
const asmkit_cpu_info_t* asmkit_gen_bpf_cpu_info_by_index(uint32_t index);
const asmkit_cpu_info_t* asmkit_gen_bpf_cpu_info(uint32_t id);
const asmkit_feature_info_t* asmkit_gen_bpf_cpu_feature_info(uint32_t id, uint32_t index);
uint32_t asmkit_gen_bpf_cpu_count(void);
const asmkit_register_info_t* asmkit_gen_bpf_register_info_by_index(uint32_t index);
const asmkit_register_info_t* asmkit_gen_bpf_register_info(uint32_t id);
uint32_t asmkit_gen_bpf_register_count(void);
const asmkit_operand_type_info_t* asmkit_gen_bpf_operand_type_info_by_index(uint32_t index);
const asmkit_operand_type_info_t* asmkit_gen_bpf_operand_type_info(uint32_t id);
uint32_t asmkit_gen_bpf_operand_type_count(void);
const asmkit_instruction_info_t* asmkit_gen_bpf_instruction_info_by_index(uint32_t index);
const asmkit_instruction_info_t* asmkit_gen_bpf_instruction_info(uint32_t id);
uint32_t asmkit_gen_bpf_instruction_count(void);
const asmkit_operand_info_t* asmkit_gen_bpf_instruction_operand_infos(uint32_t id, uint32_t* ASMKIT_RESTRICT out_count);
const asmkit_operand_info_t* asmkit_gen_bpf_instruction_operand_info(uint32_t id, uint32_t operand_index);
const asmkit_instruction_register_effect_t* asmkit_gen_bpf_instruction_register_effect(uint32_t id, uint32_t effect_index);
const asmkit_bpf_opcode_field_info_t* asmkit_gen_bpf_opcode_field_info_by_index(uint32_t kind, uint32_t index);
uint32_t asmkit_gen_bpf_opcode_field_count(uint32_t kind);

const asmkit_feature_info_t* asmkit_gen_wasm_feature_info_by_index(uint32_t index);
const asmkit_feature_info_t* asmkit_gen_wasm_feature_info(uint32_t id);
uint32_t asmkit_gen_wasm_feature_count(void);
const asmkit_register_info_t* asmkit_gen_wasm_register_info_by_index(uint32_t index);
const asmkit_register_info_t* asmkit_gen_wasm_register_info(uint32_t id);
uint32_t asmkit_gen_wasm_register_count(void);
const asmkit_operand_type_info_t* asmkit_gen_wasm_operand_type_info_by_index(uint32_t index);
const asmkit_operand_type_info_t* asmkit_gen_wasm_operand_type_info(uint32_t id);
uint32_t asmkit_gen_wasm_operand_type_count(void);
const asmkit_predicate_info_t* asmkit_gen_wasm_predicate_info_by_index(uint32_t index);
const asmkit_predicate_info_t* asmkit_gen_wasm_predicate_info(uint32_t id);
uint32_t asmkit_gen_wasm_predicate_count(void);
const asmkit_cpu_info_t* asmkit_gen_wasm_cpu_info_by_index(uint32_t index);
const asmkit_cpu_info_t* asmkit_gen_wasm_cpu_info(uint32_t id);
const asmkit_feature_info_t* asmkit_gen_wasm_cpu_feature_info(uint32_t id, uint32_t index);
uint32_t asmkit_gen_wasm_cpu_count(void);
const asmkit_instruction_info_t* asmkit_gen_wasm_instruction_info_by_index(uint32_t index);
const asmkit_instruction_info_t* asmkit_gen_wasm_instruction_info(uint32_t id);
uint32_t asmkit_gen_wasm_instruction_count(void);
const asmkit_operand_info_t* asmkit_gen_wasm_instruction_operand_infos(uint32_t id, uint32_t* ASMKIT_RESTRICT out_count);
const asmkit_operand_info_t* asmkit_gen_wasm_instruction_operand_info(uint32_t id, uint32_t operand_index);
const asmkit_instruction_register_effect_t* asmkit_gen_wasm_instruction_register_effect(uint32_t id, uint32_t effect_index);

extern const asmkit_target_ops_t asmkit_x86_ops;
extern const asmkit_target_ops_t asmkit_arm_ops;
extern const asmkit_target_ops_t asmkit_aarch64_ops;
extern const asmkit_target_ops_t asmkit_bpf_ops;
extern const asmkit_target_ops_t asmkit_wasm_ops;

#endif

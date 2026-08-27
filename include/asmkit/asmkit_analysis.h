#ifndef ASMKIT_ANALYSIS_H
#define ASMKIT_ANALYSIS_H

#include <stdbool.h>
#include <stdint.h>
#include "asmkit/asmkit_decode.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum asmkit_hazard_flags {
    ASMKIT_HAZARD_NONE = 0u,
    ASMKIT_HAZARD_PC_RELATIVE = 1u << 0,
    ASMKIT_HAZARD_BRANCH = 1u << 1,
    ASMKIT_HAZARD_CALL = 1u << 2,
    ASMKIT_HAZARD_RETURN = 1u << 3,
    ASMKIT_HAZARD_LANDING_PAD = 1u << 4,
    ASMKIT_HAZARD_PAC = 1u << 5,
    ASMKIT_HAZARD_BTI = 1u << 6,
    ASMKIT_HAZARD_CET = 1u << 7,
    ASMKIT_HAZARD_ARM_IT = 1u << 8,
    ASMKIT_HAZARD_STATE_SWITCH = 1u << 9,
    ASMKIT_HAZARD_STRUCTURED_CF = 1u << 10,
    ASMKIT_HAZARD_UNSUPPORTED_RELOC = 1u << 11
} asmkit_hazard_flags_t;

typedef struct asmkit_branch_info {
    bool is_branch;
    bool is_call;
    bool is_return;
    bool is_conditional;
    bool is_direct;
    bool is_indirect;
    bool has_target;
    uint64_t target;
    int64_t min_disp;
    int64_t max_disp;
} asmkit_branch_info_t;

typedef struct asmkit_pc_relative_info {
    bool is_pc_relative;
    bool is_literal_load;
    bool is_address_materialization;
    bool is_control_flow;
    uint8_t operand_index;
    uint64_t original_target;
    uint8_t access_width;
} asmkit_pc_relative_info_t;

typedef enum asmkit_reloc_kind {
    ASMKIT_RELOC_NONE = 0,
    ASMKIT_RELOC_COPY_AS_IS,
    ASMKIT_RELOC_REENCODE_WITH_NEW_PC,
    ASMKIT_RELOC_EXPAND_TO_SEQUENCE,
    ASMKIT_RELOC_REJECT
} asmkit_reloc_kind_t;

typedef struct asmkit_inst_semantics {
    asmkit_inst_class_t inst_class;
    uint64_t hazard_flags;
    asmkit_branch_info_t branch;
    asmkit_pc_relative_info_t pc_rel;
    asmkit_reloc_kind_t reloc_kind;
    uint32_t min_emit_size;
    uint32_t max_emit_size;
    uint64_t clobber_mask_lo;
    uint64_t clobber_mask_hi;
} asmkit_inst_semantics_t;

typedef enum asmkit_isa_state_kind {
    ASMKIT_ISA_STATE_NONE = 0,
    ASMKIT_ISA_STATE_ARM_IT
} asmkit_isa_state_kind_t;

typedef struct asmkit_isa_state {
    asmkit_isa_state_kind_t kind;
    uint8_t arm_itstate;
} asmkit_isa_state_t;

typedef struct asmkit_decode_state {
    asmkit_mode_t mode;
    asmkit_isa_state_t isa;
} asmkit_decode_state_t;

typedef enum asmkit_execution_predicate_kind {
    ASMKIT_EXECUTION_PREDICATE_ALWAYS = 0,
    ASMKIT_EXECUTION_PREDICATE_ARM_CONDITION
} asmkit_execution_predicate_kind_t;

typedef struct asmkit_execution_predicate {
    asmkit_execution_predicate_kind_t kind;
    uint32_t predicate_id;
    bool inverted;
} asmkit_execution_predicate_t;

typedef enum asmkit_flow_kind {
    ASMKIT_FLOW_FALLTHROUGH = 0,
    ASMKIT_FLOW_BRANCH,
    ASMKIT_FLOW_CALL,
    ASMKIT_FLOW_RETURN,
    ASMKIT_FLOW_TRAP
} asmkit_flow_kind_t;

typedef struct asmkit_successor_state_transition {
    asmkit_flow_kind_t flow;
    asmkit_decode_state_t state;
} asmkit_successor_state_transition_t;

enum {
    ASMKIT_MAX_SUCCESSOR_STATES = 2u
};

typedef struct asmkit_control_analysis {
    asmkit_inst_semantics_t semantics;
    asmkit_execution_predicate_t predicate;
    uint32_t successor_count;
    asmkit_successor_state_transition_t successors[ASMKIT_MAX_SUCCESSOR_STATES];
} asmkit_control_analysis_t;

typedef enum asmkit_value_ref_kind {
    ASMKIT_VALUE_REF_NONE = 0,
    ASMKIT_VALUE_REF_OPERAND,
    ASMKIT_VALUE_REF_REGISTER,
    ASMKIT_VALUE_REF_IMMEDIATE,
    ASMKIT_VALUE_REF_PC,
    ASMKIT_VALUE_REF_TEMPORARY,
    ASMKIT_VALUE_REF_CONTROL_TARGET
} asmkit_value_ref_kind_t;

typedef struct asmkit_value_ref {
    asmkit_value_ref_kind_t kind;
    uint8_t operand_index;
    uint16_t width;
    uint64_t register_id;
    int64_t immediate;
} asmkit_value_ref_t;

typedef enum asmkit_value_effect_kind {
    ASMKIT_VALUE_EFFECT_UNKNOWN_CLOBBER = 0,
    ASMKIT_VALUE_EFFECT_COPY,
    ASMKIT_VALUE_EFFECT_ADDRESS,
    ASMKIT_VALUE_EFFECT_ADD,
    ASMKIT_VALUE_EFFECT_SUB,
    ASMKIT_VALUE_EFFECT_MUL,
    ASMKIT_VALUE_EFFECT_SHIFT_LEFT,
    ASMKIT_VALUE_EFFECT_AND,
    ASMKIT_VALUE_EFFECT_OR,
    ASMKIT_VALUE_EFFECT_XOR,
    ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_LOGICAL,
    ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_ARITHMETIC,
    ASMKIT_VALUE_EFFECT_ROTATE_LEFT,
    ASMKIT_VALUE_EFFECT_ROTATE_RIGHT,
    ASMKIT_VALUE_EFFECT_SIGN_EXTEND,
    ASMKIT_VALUE_EFFECT_ZERO_EXTEND,
    ASMKIT_VALUE_EFFECT_LOAD,
    ASMKIT_VALUE_EFFECT_STORE,
    ASMKIT_VALUE_EFFECT_COMPARE,
    ASMKIT_VALUE_EFFECT_CONDITION_USE
} asmkit_value_effect_kind_t;

enum {
    ASMKIT_MAX_VALUE_EFFECT_SOURCES = 2u,
    ASMKIT_MAX_VALUE_EFFECTS = 8u
};

typedef struct asmkit_value_effect {
    asmkit_value_effect_kind_t kind;
    asmkit_value_ref_t destination;
    uint32_t source_count;
    asmkit_value_ref_t sources[ASMKIT_MAX_VALUE_EFFECT_SOURCES];
} asmkit_value_effect_t;

typedef enum asmkit_compare_relation {
    ASMKIT_COMPARE_NONE = 0,
    ASMKIT_COMPARE_EQ,
    ASMKIT_COMPARE_NE,
    ASMKIT_COMPARE_ULT,
    ASMKIT_COMPARE_ULE,
    ASMKIT_COMPARE_UGT,
    ASMKIT_COMPARE_UGE,
    ASMKIT_COMPARE_SLT,
    ASMKIT_COMPARE_SLE,
    ASMKIT_COMPARE_SGT,
    ASMKIT_COMPARE_SGE,
    ASMKIT_COMPARE_BIT_CLEAR,
    ASMKIT_COMPARE_BIT_SET
} asmkit_compare_relation_t;

typedef enum asmkit_condition_state {
    ASMKIT_CONDITION_STATE_NONE = 0,
    ASMKIT_CONDITION_STATE_X86_FLAGS,
    ASMKIT_CONDITION_STATE_ARM_FLAGS,
    ASMKIT_CONDITION_STATE_AARCH64_FLAGS,
    ASMKIT_CONDITION_STATE_BPF_INLINE
} asmkit_condition_state_t;

typedef struct asmkit_condition_info {
    bool defines_condition;
    bool consumes_condition;
    asmkit_condition_state_t state;
    asmkit_compare_relation_t taken_relation;
    asmkit_value_ref_t left;
    asmkit_value_ref_t right;
} asmkit_condition_info_t;

typedef struct asmkit_value_analysis {
    uint32_t effect_count;
    asmkit_value_effect_t effects[ASMKIT_MAX_VALUE_EFFECTS];
    asmkit_condition_info_t condition;
} asmkit_value_analysis_t;

struct asmkit_engine;
struct asmkit_workspace;

asmkit_status_t asmkit_analyze_inst(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const asmkit_inst_t* inst,
    asmkit_inst_semantics_t* out_semantics);

asmkit_status_t asmkit_analyze_inst_with_state(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const asmkit_inst_t* inst,
    const asmkit_decode_state_t* state,
    asmkit_control_analysis_t* out_analysis);

asmkit_status_t asmkit_analyze_inst_values(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const asmkit_inst_t* inst,
    const asmkit_control_analysis_t* control,
    asmkit_value_analysis_t* out_analysis);

#ifdef __cplusplus
}
#endif

#endif

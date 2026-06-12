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

struct asmkit_engine;
struct asmkit_workspace;

asmkit_status_t asmkit_analyze_inst(
    const struct asmkit_engine* engine,
    struct asmkit_workspace* workspace,
    const asmkit_inst_t* inst,
    asmkit_inst_semantics_t* out_semantics);

#ifdef __cplusplus
}
#endif

#endif

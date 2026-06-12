#ifndef ASMKIT_BPF_H
#define ASMKIT_BPF_H

#include <stdint.h>
#include "asmkit/asmkit_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum asmkit_bpf_opcode_field_kind {
    ASMKIT_BPF_OPCODE_FIELD_ALL = 0,
    ASMKIT_BPF_OPCODE_FIELD_OP_CLASS,
    ASMKIT_BPF_OPCODE_FIELD_SRC_TYPE,
    ASMKIT_BPF_OPCODE_FIELD_ARITH_OP,
    ASMKIT_BPF_OPCODE_FIELD_ATOMIC_OP,
    ASMKIT_BPF_OPCODE_FIELD_ATOMIC_FLAG,
    ASMKIT_BPF_OPCODE_FIELD_END_DIR,
    ASMKIT_BPF_OPCODE_FIELD_JUMP_OP,
    ASMKIT_BPF_OPCODE_FIELD_WIDTH_MODIFIER,
    ASMKIT_BPF_OPCODE_FIELD_MODE_MODIFIER
} asmkit_bpf_opcode_field_kind_t;

typedef struct asmkit_bpf_opcode_field_info {
    uint32_t id;
    asmkit_bpf_opcode_field_kind_t kind;
    const char* name;
    const char* llvm_class;
    uint8_t value;
} asmkit_bpf_opcode_field_info_t;

uint32_t asmkit_bpf_get_opcode_field_count(asmkit_bpf_opcode_field_kind_t kind);
asmkit_status_t asmkit_bpf_get_opcode_field_info_by_index(
    asmkit_bpf_opcode_field_kind_t kind,
    uint32_t index,
    const asmkit_bpf_opcode_field_info_t** out_info);
asmkit_status_t asmkit_bpf_find_opcode_field_by_name(
    const char* name,
    const asmkit_bpf_opcode_field_info_t** out_info);
asmkit_status_t asmkit_bpf_find_opcode_field_by_value(
    asmkit_bpf_opcode_field_kind_t kind,
    uint8_t value,
    const asmkit_bpf_opcode_field_info_t** out_info);

#ifdef __cplusplus
}
#endif

#endif

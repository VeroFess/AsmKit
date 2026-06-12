#include "core/asmkit_internal.h"

static int asmkit_arch_known(asmkit_arch_t arch)
{
    switch (arch) {
    case ASMKIT_ARCH_X86:
    case ASMKIT_ARCH_ARM:
    case ASMKIT_ARCH_AARCH64:
    case ASMKIT_ARCH_BPF:
    case ASMKIT_ARCH_WASM:
        return 1;
    default:
        return 0;
    }
}

static int asmkit_cstr_equal(const char* left, const char* right)
{
    size_t i;

    if (left == 0 || right == 0) {
        return 0;
    }
    i = 0u;
    while (left[i] != '\0' && right[i] != '\0') {
        if (left[i] != right[i]) {
            return 0;
        }
        ++i;
    }
    return left[i] == right[i];
}

uint32_t asmkit_get_register_count(asmkit_arch_t arch)
{
    switch (arch) {
    case ASMKIT_ARCH_X86: return asmkit_gen_x86_register_count();
    case ASMKIT_ARCH_ARM: return asmkit_gen_arm_register_count();
    case ASMKIT_ARCH_AARCH64: return asmkit_gen_aarch64_register_count();
    case ASMKIT_ARCH_BPF: return asmkit_gen_bpf_register_count();
    case ASMKIT_ARCH_WASM: return asmkit_gen_wasm_register_count();
    default: return 0u;
    }
}

uint32_t asmkit_get_instruction_count(asmkit_arch_t arch)
{
    switch (arch) {
    case ASMKIT_ARCH_X86: return asmkit_gen_x86_instruction_count();
    case ASMKIT_ARCH_ARM: return asmkit_gen_arm_instruction_count();
    case ASMKIT_ARCH_AARCH64: return asmkit_gen_aarch64_instruction_count();
    case ASMKIT_ARCH_BPF: return asmkit_gen_bpf_instruction_count();
    case ASMKIT_ARCH_WASM: return asmkit_gen_wasm_instruction_count();
    default: return 0u;
    }
}

uint32_t asmkit_get_predicate_count(asmkit_arch_t arch)
{
    switch (arch) {
    case ASMKIT_ARCH_X86: return asmkit_gen_x86_predicate_count();
    case ASMKIT_ARCH_ARM: return asmkit_gen_arm_predicate_count();
    case ASMKIT_ARCH_AARCH64: return asmkit_gen_aarch64_predicate_count();
    case ASMKIT_ARCH_BPF: return asmkit_gen_bpf_predicate_count();
    case ASMKIT_ARCH_WASM: return asmkit_gen_wasm_predicate_count();
    default: return 0u;
    }
}

uint32_t asmkit_get_feature_count(asmkit_arch_t arch)
{
    switch (arch) {
    case ASMKIT_ARCH_X86: return asmkit_gen_x86_feature_count();
    case ASMKIT_ARCH_ARM: return asmkit_gen_arm_feature_count();
    case ASMKIT_ARCH_AARCH64: return asmkit_gen_aarch64_feature_count();
    case ASMKIT_ARCH_BPF: return asmkit_gen_bpf_feature_count();
    case ASMKIT_ARCH_WASM: return asmkit_gen_wasm_feature_count();
    default: return 0u;
    }
}

uint32_t asmkit_get_cpu_count(asmkit_arch_t arch)
{
    switch (arch) {
    case ASMKIT_ARCH_X86: return asmkit_gen_x86_cpu_count();
    case ASMKIT_ARCH_ARM: return asmkit_gen_arm_cpu_count();
    case ASMKIT_ARCH_AARCH64: return asmkit_gen_aarch64_cpu_count();
    case ASMKIT_ARCH_BPF: return asmkit_gen_bpf_cpu_count();
    case ASMKIT_ARCH_WASM: return asmkit_gen_wasm_cpu_count();
    default: return 0u;
    }
}

uint32_t asmkit_get_operand_type_count(asmkit_arch_t arch)
{
    switch (arch) {
    case ASMKIT_ARCH_X86: return asmkit_gen_x86_operand_type_count();
    case ASMKIT_ARCH_ARM: return asmkit_gen_arm_operand_type_count();
    case ASMKIT_ARCH_AARCH64: return asmkit_gen_aarch64_operand_type_count();
    case ASMKIT_ARCH_BPF: return asmkit_gen_bpf_operand_type_count();
    case ASMKIT_ARCH_WASM: return asmkit_gen_wasm_operand_type_count();
    default: return 0u;
    }
}

asmkit_status_t asmkit_get_register_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_register_info_t** out_info)
{
    const asmkit_register_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_register_info_by_index(index); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_register_info_by_index(index); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_register_info_by_index(index); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_register_info_by_index(index); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_register_info_by_index(index); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_get_register_info(asmkit_arch_t arch, uint32_t register_id, const asmkit_register_info_t** out_info)
{
    const asmkit_register_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_register_info(register_id); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_register_info(register_id); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_register_info(register_id); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_register_info(register_id); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_register_info(register_id); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_find_register_by_name(asmkit_arch_t arch, const char* name, const asmkit_register_info_t** out_info)
{
    uint32_t i;
    uint32_t count;

    if (name == 0 || out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    count = asmkit_get_register_count(arch);
    for (i = 0u; i < count; ++i) {
        const asmkit_register_info_t* info;
        if (asmkit_get_register_info_by_index(arch, i, &info) == ASMKIT_OK && asmkit_cstr_equal(info->name, name)) {
            *out_info = info;
            return ASMKIT_OK;
        }
    }
    return asmkit_arch_known(arch) ? ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION : ASMKIT_ERR_UNSUPPORTED_ARCH;
}

asmkit_status_t asmkit_get_operand_type_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_operand_type_info_t** out_info)
{
    const asmkit_operand_type_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_operand_type_info_by_index(index); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_operand_type_info_by_index(index); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_operand_type_info_by_index(index); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_operand_type_info_by_index(index); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_operand_type_info_by_index(index); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_get_operand_type_info(asmkit_arch_t arch, uint32_t operand_type_id, const asmkit_operand_type_info_t** out_info)
{
    const asmkit_operand_type_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_operand_type_info(operand_type_id); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_operand_type_info(operand_type_id); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_operand_type_info(operand_type_id); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_operand_type_info(operand_type_id); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_operand_type_info(operand_type_id); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_find_operand_type_by_name(asmkit_arch_t arch, const char* name, const asmkit_operand_type_info_t** out_info)
{
    uint32_t i;
    uint32_t count;

    if (name == 0 || out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    count = asmkit_get_operand_type_count(arch);
    for (i = 0u; i < count; ++i) {
        const asmkit_operand_type_info_t* info;
        if (asmkit_get_operand_type_info_by_index(arch, i, &info) == ASMKIT_OK && asmkit_cstr_equal(info->name, name)) {
            *out_info = info;
            return ASMKIT_OK;
        }
    }
    return asmkit_arch_known(arch) ? ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION : ASMKIT_ERR_UNSUPPORTED_ARCH;
}

asmkit_status_t asmkit_get_instruction_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_instruction_info_t** out_info)
{
    const asmkit_instruction_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    if (!asmkit_arch_known(arch)) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_instruction_info_by_index(index); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_instruction_info_by_index(index); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_instruction_info_by_index(index); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_instruction_info_by_index(index); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_instruction_info_by_index(index); break;
    default: info = 0; break;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_INSTRUCTION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_get_instruction_info(asmkit_arch_t arch, uint32_t instruction_id, const asmkit_instruction_info_t** out_info)
{
    const asmkit_instruction_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    if (!asmkit_arch_known(arch)) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_instruction_info(instruction_id); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_instruction_info(instruction_id); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_instruction_info(instruction_id); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_instruction_info(instruction_id); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_instruction_info(instruction_id); break;
    default: info = 0; break;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_INSTRUCTION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_find_instruction_by_name(asmkit_arch_t arch, const char* name, const asmkit_instruction_info_t** out_info)
{
    uint32_t i;
    uint32_t count;

    if (name == 0 || out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    count = asmkit_get_instruction_count(arch);
    for (i = 0u; i < count; ++i) {
        const asmkit_instruction_info_t* info;
        if (asmkit_get_instruction_info_by_index(arch, i, &info) == ASMKIT_OK && asmkit_cstr_equal(info->name, name)) {
            *out_info = info;
            return ASMKIT_OK;
        }
    }
    return asmkit_arch_known(arch) ? ASMKIT_ERR_UNSUPPORTED_INSTRUCTION : ASMKIT_ERR_UNSUPPORTED_ARCH;
}

asmkit_status_t asmkit_get_instruction_operand_info(
    asmkit_arch_t arch,
    uint32_t instruction_id,
    uint32_t operand_index,
    const asmkit_operand_info_t** out_info)
{
    const asmkit_operand_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    if (!asmkit_arch_known(arch)) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_instruction_operand_info(instruction_id, operand_index); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_instruction_operand_info(instruction_id, operand_index); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_instruction_operand_info(instruction_id, operand_index); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_instruction_operand_info(instruction_id, operand_index); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_instruction_operand_info(instruction_id, operand_index); break;
    default: info = 0; break;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_INSTRUCTION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_get_instruction_register_effect(
    asmkit_arch_t arch,
    uint32_t instruction_id,
    uint32_t effect_index,
    const asmkit_instruction_register_effect_t** out_info)
{
    const asmkit_instruction_register_effect_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    if (!asmkit_arch_known(arch)) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_instruction_register_effect(instruction_id, effect_index); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_instruction_register_effect(instruction_id, effect_index); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_instruction_register_effect(instruction_id, effect_index); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_instruction_register_effect(instruction_id, effect_index); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_instruction_register_effect(instruction_id, effect_index); break;
    default: info = 0; break;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_INSTRUCTION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_get_predicate_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_predicate_info_t** out_info)
{
    const asmkit_predicate_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_predicate_info_by_index(index); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_predicate_info_by_index(index); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_predicate_info_by_index(index); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_predicate_info_by_index(index); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_predicate_info_by_index(index); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_get_predicate_info(asmkit_arch_t arch, uint32_t predicate_id, const asmkit_predicate_info_t** out_info)
{
    const asmkit_predicate_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_predicate_info(predicate_id); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_predicate_info(predicate_id); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_predicate_info(predicate_id); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_predicate_info(predicate_id); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_predicate_info(predicate_id); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_find_predicate_by_name(asmkit_arch_t arch, const char* name, const asmkit_predicate_info_t** out_info)
{
    uint32_t i;
    uint32_t count;

    if (name == 0 || out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    count = asmkit_get_predicate_count(arch);
    for (i = 0u; i < count; ++i) {
        const asmkit_predicate_info_t* info;
        if (asmkit_get_predicate_info_by_index(arch, i, &info) == ASMKIT_OK && asmkit_cstr_equal(info->name, name)) {
            *out_info = info;
            return ASMKIT_OK;
        }
    }
    return asmkit_arch_known(arch) ? ASMKIT_ERR_UNSUPPORTED_FEATURE : ASMKIT_ERR_UNSUPPORTED_ARCH;
}

asmkit_status_t asmkit_get_feature_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_feature_info_t** out_info)
{
    const asmkit_feature_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_feature_info_by_index(index); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_feature_info_by_index(index); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_feature_info_by_index(index); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_feature_info_by_index(index); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_feature_info_by_index(index); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_get_feature_info(asmkit_arch_t arch, uint32_t feature_id, const asmkit_feature_info_t** out_info)
{
    const asmkit_feature_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_feature_info(feature_id); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_feature_info(feature_id); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_feature_info(feature_id); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_feature_info(feature_id); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_feature_info(feature_id); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_find_feature_by_name(asmkit_arch_t arch, const char* name, const asmkit_feature_info_t** out_info)
{
    uint32_t i;
    uint32_t count;

    if (name == 0 || out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    count = asmkit_get_feature_count(arch);
    for (i = 0u; i < count; ++i) {
        const asmkit_feature_info_t* info;
        if (asmkit_get_feature_info_by_index(arch, i, &info) == ASMKIT_OK &&
            (asmkit_cstr_equal(info->name, name) || asmkit_cstr_equal(info->llvm_name, name))) {
            *out_info = info;
            return ASMKIT_OK;
        }
    }
    return asmkit_arch_known(arch) ? ASMKIT_ERR_UNSUPPORTED_FEATURE : ASMKIT_ERR_UNSUPPORTED_ARCH;
}

asmkit_status_t asmkit_get_cpu_info_by_index(asmkit_arch_t arch, uint32_t index, const asmkit_cpu_info_t** out_info)
{
    const asmkit_cpu_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_cpu_info_by_index(index); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_cpu_info_by_index(index); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_cpu_info_by_index(index); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_cpu_info_by_index(index); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_cpu_info_by_index(index); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_get_cpu_info(asmkit_arch_t arch, uint32_t cpu_id, const asmkit_cpu_info_t** out_info)
{
    const asmkit_cpu_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_cpu_info(cpu_id); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_cpu_info(cpu_id); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_cpu_info(cpu_id); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_cpu_info(cpu_id); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_cpu_info(cpu_id); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_find_cpu_by_name(asmkit_arch_t arch, const char* name, const asmkit_cpu_info_t** out_info)
{
    uint32_t i;
    uint32_t count;

    if (name == 0 || out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    count = asmkit_get_cpu_count(arch);
    for (i = 0u; i < count; ++i) {
        const asmkit_cpu_info_t* info;
        if (asmkit_get_cpu_info_by_index(arch, i, &info) == ASMKIT_OK && asmkit_cstr_equal(info->name, name)) {
            *out_info = info;
            return ASMKIT_OK;
        }
    }
    return asmkit_arch_known(arch) ? ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION : ASMKIT_ERR_UNSUPPORTED_ARCH;
}

asmkit_status_t asmkit_get_cpu_feature_info(
    asmkit_arch_t arch,
    uint32_t cpu_id,
    uint32_t cpu_feature_index,
    const asmkit_feature_info_t** out_info)
{
    const asmkit_feature_info_t* info;

    if (out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    switch (arch) {
    case ASMKIT_ARCH_X86: info = asmkit_gen_x86_cpu_feature_info(cpu_id, cpu_feature_index); break;
    case ASMKIT_ARCH_ARM: info = asmkit_gen_arm_cpu_feature_info(cpu_id, cpu_feature_index); break;
    case ASMKIT_ARCH_AARCH64: info = asmkit_gen_aarch64_cpu_feature_info(cpu_id, cpu_feature_index); break;
    case ASMKIT_ARCH_BPF: info = asmkit_gen_bpf_cpu_feature_info(cpu_id, cpu_feature_index); break;
    case ASMKIT_ARCH_WASM: info = asmkit_gen_wasm_cpu_feature_info(cpu_id, cpu_feature_index); break;
    default: return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_FEATURE;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_cpu_has_feature(asmkit_arch_t arch, uint32_t cpu_id, uint32_t feature_id, int* out_enabled)
{
    const asmkit_cpu_info_t* cpu_info;
    const asmkit_feature_info_t* feature_info;
    asmkit_status_t status;
    uint32_t i;

    if (out_enabled == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_enabled = 0;
    status = asmkit_get_cpu_info(arch, cpu_id, &cpu_info);
    if (status != ASMKIT_OK) {
        return status;
    }
    status = asmkit_get_feature_info(arch, feature_id, &feature_info);
    if (status != ASMKIT_OK) {
        return status;
    }
    for (i = 0u; i < cpu_info->feature_count; ++i) {
        status = asmkit_get_cpu_feature_info(arch, cpu_id, i, &feature_info);
        if (status != ASMKIT_OK) {
            return status;
        }
        if (feature_info->id == feature_id) {
            *out_enabled = 1;
            return ASMKIT_OK;
        }
    }
    return ASMKIT_OK;
}

static int asmkit_bpf_opcode_field_kind_known(asmkit_bpf_opcode_field_kind_t kind)
{
    switch (kind) {
    case ASMKIT_BPF_OPCODE_FIELD_ALL:
    case ASMKIT_BPF_OPCODE_FIELD_OP_CLASS:
    case ASMKIT_BPF_OPCODE_FIELD_SRC_TYPE:
    case ASMKIT_BPF_OPCODE_FIELD_ARITH_OP:
    case ASMKIT_BPF_OPCODE_FIELD_ATOMIC_OP:
    case ASMKIT_BPF_OPCODE_FIELD_ATOMIC_FLAG:
    case ASMKIT_BPF_OPCODE_FIELD_END_DIR:
    case ASMKIT_BPF_OPCODE_FIELD_JUMP_OP:
    case ASMKIT_BPF_OPCODE_FIELD_WIDTH_MODIFIER:
    case ASMKIT_BPF_OPCODE_FIELD_MODE_MODIFIER:
        return 1;
    default:
        return 0;
    }
}

uint32_t asmkit_bpf_get_opcode_field_count(asmkit_bpf_opcode_field_kind_t kind)
{
    if (!asmkit_bpf_opcode_field_kind_known(kind)) {
        return 0u;
    }
    return asmkit_gen_bpf_opcode_field_count((uint32_t)kind);
}

asmkit_status_t asmkit_bpf_get_opcode_field_info_by_index(
    asmkit_bpf_opcode_field_kind_t kind,
    uint32_t index,
    const asmkit_bpf_opcode_field_info_t** out_info)
{
    const asmkit_bpf_opcode_field_info_t* info;

    if (out_info == 0 || !asmkit_bpf_opcode_field_kind_known(kind)) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    info = asmkit_gen_bpf_opcode_field_info_by_index((uint32_t)kind, index);
    if (info == 0) {
        return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
    }
    *out_info = info;
    return ASMKIT_OK;
}

asmkit_status_t asmkit_bpf_find_opcode_field_by_name(
    const char* name,
    const asmkit_bpf_opcode_field_info_t** out_info)
{
    const asmkit_bpf_opcode_field_info_t* info;
    uint32_t count;
    uint32_t i;

    if (name == 0 || out_info == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    count = asmkit_bpf_get_opcode_field_count(ASMKIT_BPF_OPCODE_FIELD_ALL);
    for (i = 0u; i < count; ++i) {
        if (asmkit_bpf_get_opcode_field_info_by_index(ASMKIT_BPF_OPCODE_FIELD_ALL, i, &info) != ASMKIT_OK) {
            return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
        }
        if (asmkit_cstr_equal(info->name, name)) {
            *out_info = info;
            return ASMKIT_OK;
        }
    }
    return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
}

asmkit_status_t asmkit_bpf_find_opcode_field_by_value(
    asmkit_bpf_opcode_field_kind_t kind,
    uint8_t value,
    const asmkit_bpf_opcode_field_info_t** out_info)
{
    const asmkit_bpf_opcode_field_info_t* info;
    uint32_t count;
    uint32_t i;

    if (out_info == 0 || kind == ASMKIT_BPF_OPCODE_FIELD_ALL || !asmkit_bpf_opcode_field_kind_known(kind)) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    *out_info = 0;
    count = asmkit_bpf_get_opcode_field_count(kind);
    for (i = 0u; i < count; ++i) {
        if (asmkit_bpf_get_opcode_field_info_by_index(kind, i, &info) != ASMKIT_OK) {
            return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
        }
        if (info->value == value) {
            *out_info = info;
            return ASMKIT_OK;
        }
    }
    return ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION;
}

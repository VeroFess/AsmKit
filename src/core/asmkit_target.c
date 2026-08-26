#include "core/asmkit_internal.h"

asmkit_status_t asmkit_get_target_capabilities(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    asmkit_target_capabilities_t* ASMKIT_RESTRICT out_capabilities)
{
    if (engine == 0 || out_capabilities == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }

    asmkit_zero(out_capabilities, sizeof(*out_capabilities));
    out_capabilities->arch = engine->config.arch;
    out_capabilities->mode = engine->config.mode;
    out_capabilities->maximum_instruction_size = ASMKIT_MAX_INST_BYTES;
    out_capabilities->flags = ASMKIT_TARGET_CAP_ENCODE;

    switch (engine->config.mode) {
    case ASMKIT_MODE_X86_16:
    case ASMKIT_MODE_X86_32:
    case ASMKIT_MODE_X86_64:
        out_capabilities->minimum_instruction_unit = 1u;
        out_capabilities->instruction_alignment = 1u;
        out_capabilities->flags |= ASMKIT_TARGET_CAP_NATIVE_LINEAR |
            ASMKIT_TARGET_CAP_RELOCATE_INSTRUCTION | ASMKIT_TARGET_CAP_EMIT_BRANCH |
            ASMKIT_TARGET_CAP_EMIT_CALL | ASMKIT_TARGET_CAP_BRANCH_ISLAND |
            ASMKIT_TARGET_CAP_EMIT_PADDING;
        return ASMKIT_OK;
    case ASMKIT_MODE_ARM_A32:
        out_capabilities->minimum_instruction_unit = 4u;
        out_capabilities->instruction_alignment = 4u;
        out_capabilities->flags |= ASMKIT_TARGET_CAP_NATIVE_LINEAR |
            ASMKIT_TARGET_CAP_RELOCATE_INSTRUCTION | ASMKIT_TARGET_CAP_EMIT_BRANCH |
            ASMKIT_TARGET_CAP_EMIT_CALL | ASMKIT_TARGET_CAP_BRANCH_ISLAND |
            ASMKIT_TARGET_CAP_EMIT_PADDING;
        return ASMKIT_OK;
    case ASMKIT_MODE_ARM_THUMB:
        out_capabilities->minimum_instruction_unit = 2u;
        out_capabilities->instruction_alignment = 2u;
        out_capabilities->flags |= ASMKIT_TARGET_CAP_NATIVE_LINEAR |
            ASMKIT_TARGET_CAP_RELOCATE_INSTRUCTION | ASMKIT_TARGET_CAP_EMIT_BRANCH |
            ASMKIT_TARGET_CAP_EMIT_CALL | ASMKIT_TARGET_CAP_BRANCH_ISLAND |
            ASMKIT_TARGET_CAP_EMIT_PADDING;
        return ASMKIT_OK;
    case ASMKIT_MODE_AARCH64:
        out_capabilities->minimum_instruction_unit = 4u;
        out_capabilities->instruction_alignment = 4u;
        out_capabilities->flags |= ASMKIT_TARGET_CAP_NATIVE_LINEAR |
            ASMKIT_TARGET_CAP_RELOCATE_INSTRUCTION | ASMKIT_TARGET_CAP_EMIT_BRANCH |
            ASMKIT_TARGET_CAP_EMIT_CALL | ASMKIT_TARGET_CAP_BRANCH_ISLAND |
            ASMKIT_TARGET_CAP_EMIT_PADDING;
        return ASMKIT_OK;
    case ASMKIT_MODE_BPF64:
        out_capabilities->minimum_instruction_unit = 8u;
        out_capabilities->instruction_alignment = 8u;
        out_capabilities->flags |= ASMKIT_TARGET_CAP_NATIVE_LINEAR |
            ASMKIT_TARGET_CAP_RELOCATE_INSTRUCTION;
        return ASMKIT_OK;
    case ASMKIT_MODE_WASM32:
    case ASMKIT_MODE_WASM64:
        out_capabilities->minimum_instruction_unit = 1u;
        out_capabilities->instruction_alignment = 1u;
        out_capabilities->flags |= ASMKIT_TARGET_CAP_STRUCTURED_CONTROL_FLOW;
        return ASMKIT_OK;
    default:
        return ASMKIT_ERR_UNSUPPORTED_MODE;
    }
}

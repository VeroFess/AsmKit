#include "test_support.h"

int asmkit_test_analysis(void)
{
    asmkit_engine_t engine;
    asmkit_inst_t inst;
    asmkit_inst_semantics_t sem;
    asmkit_control_analysis_t control;
    asmkit_decode_state_t state;
    asmkit_target_capabilities_t capabilities;
    asmkit_decode_failure_kind_t failure;
    uint8_t call_rel[] = {0xe8u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t thumb_it[] = {0x08u, 0xbfu};
    uint8_t thumb_nop[] = {0x00u, 0xbfu};
    uint8_t short_a64[] = {0x1fu};
    uint8_t wasm_block[] = {0x02u, 0x40u};

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, call_rel, sizeof(call_rel), 0x2000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst(&engine, 0, &inst, &sem) == ASMKIT_OK);
    ASMKIT_CHECK(sem.branch.is_call);
    ASMKIT_CHECK(sem.branch.has_target);
    ASMKIT_CHECK(sem.branch.target == 0x2005u);
    ASMKIT_CHECK((sem.hazard_flags & ASMKIT_HAZARD_PC_RELATIVE) != 0u);
    ASMKIT_CHECK(asmkit_get_target_capabilities(&engine, &capabilities) == ASMKIT_OK);
    ASMKIT_CHECK((capabilities.flags & ASMKIT_TARGET_CAP_NATIVE_LINEAR) != 0u);
    ASMKIT_CHECK(capabilities.minimum_instruction_unit == 1u);
    state.mode = ASMKIT_MODE_X86_64;
    state.isa.kind = ASMKIT_ISA_STATE_NONE;
    state.isa.arm_itstate = 0u;
    ASMKIT_CHECK(asmkit_analyze_inst_with_state(&engine, 0, &inst, &state, &control) == ASMKIT_OK);
    ASMKIT_CHECK(control.successor_count == 2u);
    ASMKIT_CHECK(control.successors[0].flow == ASMKIT_FLOW_CALL);
    ASMKIT_CHECK(control.successors[1].flow == ASMKIT_FLOW_FALLTHROUGH);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB);
    state.mode = ASMKIT_MODE_ARM_THUMB;
    state.isa.kind = ASMKIT_ISA_STATE_NONE;
    state.isa.arm_itstate = 0u;
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_it, sizeof(thumb_it), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst_with_state(&engine, 0, &inst, &state, &control) == ASMKIT_OK);
    ASMKIT_CHECK(control.successor_count == 1u);
    ASMKIT_CHECK(control.successors[0].state.isa.kind == ASMKIT_ISA_STATE_ARM_IT);
    ASMKIT_CHECK(control.successors[0].state.isa.arm_itstate == 0x08u);
    state = control.successors[0].state;
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_nop, sizeof(thumb_nop), 0x1002u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst_with_state(&engine, 0, &inst, &state, &control) == ASMKIT_OK);
    ASMKIT_CHECK(control.predicate.kind == ASMKIT_EXECUTION_PREDICATE_ARM_CONDITION);
    ASMKIT_CHECK(control.predicate.predicate_id == 0u);
    ASMKIT_CHECK(control.successors[0].state.isa.kind == ASMKIT_ISA_STATE_NONE);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64);
    ASMKIT_CHECK(asmkit_get_target_capabilities(&engine, &capabilities) == ASMKIT_OK);
    ASMKIT_CHECK(capabilities.minimum_instruction_unit == 4u);
    ASMKIT_CHECK(asmkit_decode_one_ex(&engine, 0, short_a64, sizeof(short_a64), 0u, &inst, &failure) == ASMKIT_ERR_DECODE_FAILED);
    ASMKIT_CHECK(failure == ASMKIT_DECODE_FAILURE_INSUFFICIENT_BYTES);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, wasm_block, sizeof(wasm_block), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst(&engine, 0, &inst, &sem) == ASMKIT_OK);
    ASMKIT_CHECK((sem.hazard_flags & ASMKIT_HAZARD_STRUCTURED_CF) != 0u);
    ASMKIT_CHECK(asmkit_get_target_capabilities(&engine, &capabilities) == ASMKIT_OK);
    ASMKIT_CHECK((capabilities.flags & ASMKIT_TARGET_CAP_NATIVE_LINEAR) == 0u);
    ASMKIT_CHECK((capabilities.flags & ASMKIT_TARGET_CAP_STRUCTURED_CONTROL_FLOW) != 0u);

    return 0;
}

#include "test_support.h"
#include <string.h>
#include "asmkit/target/aarch64.h"
#include "asmkit/target/arm.h"
#include "asmkit/target/bpf.h"
#include "asmkit/target/x86.h"

int asmkit_test_analysis(void)
{
    asmkit_engine_t engine;
    asmkit_inst_t inst;
    asmkit_inst_semantics_t sem;
    asmkit_control_analysis_t control;
    asmkit_decode_state_t state;
    asmkit_target_capabilities_t capabilities;
    asmkit_decode_failure_kind_t failure;
    asmkit_value_analysis_t values;
    uint8_t call_rel[] = {0xe8u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t thumb_it[] = {0x08u, 0xbfu};
    uint8_t thumb_nop[] = {0x00u, 0xbfu};
    uint8_t thumb_tbb[] = {0xdfu, 0xe8u, 0x00u, 0xf0u};
    uint8_t a32_blx[] = {0x00u, 0x00u, 0x00u, 0xfbu};
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

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOVSXD);
    inst.inst_class = ASMKIT_INST_LOAD;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_with_flags(
        asmkit_operand_reg(ASMKIT_X86_REG_RCX, 64u), ASMKIT_OPERAND_FLAG_WRITE)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_with_flags(
        asmkit_operand_mem_full(ASMKIT_X86_REG_RAX, ASMKIT_X86_REG_RCX, 4u, 0, 32u, 64u),
        ASMKIT_OPERAND_FLAG_READ)) == ASMKIT_OK);
    memset(&control, 0, sizeof(control));
    ASMKIT_CHECK(asmkit_analyze_inst_values(&engine, 0, &inst, &control, &values) == ASMKIT_OK);
    ASMKIT_CHECK(values.effect_count == 2u);
    ASMKIT_CHECK(values.effects[0].kind == ASMKIT_VALUE_EFFECT_LOAD);
    ASMKIT_CHECK(values.effects[1].kind == ASMKIT_VALUE_EFFECT_SIGN_EXTEND);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_CMP);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_with_flags(
        asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u), ASMKIT_OPERAND_FLAG_READ)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst_values(&engine, 0, &inst, &control, &values) == ASMKIT_OK);
    ASMKIT_CHECK(values.condition.defines_condition);
    ASMKIT_CHECK(values.condition.state == ASMKIT_CONDITION_STATE_X86_FLAGS);
    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_SUB);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_with_flags(
        asmkit_operand_reg(ASMKIT_X86_REG_AL, 8u), ASMKIT_OPERAND_FLAG_READ | ASMKIT_OPERAND_FLAG_WRITE)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(7, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst_values(&engine, 0, &inst, &control, &values) == ASMKIT_OK);
    ASMKIT_CHECK(values.condition.defines_condition);
    ASMKIT_CHECK(values.condition.left.kind == ASMKIT_VALUE_REF_OPERAND);
    ASMKIT_CHECK(values.condition.right.kind == ASMKIT_VALUE_REF_OPERAND);
    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_JAE);
    ASMKIT_CHECK(asmkit_analyze_inst_values(&engine, 0, &inst, &control, &values) == ASMKIT_OK);
    ASMKIT_CHECK(values.condition.consumes_condition);
    ASMKIT_CHECK(values.condition.taken_relation == ASMKIT_COMPARE_UGE);

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
    state.mode = ASMKIT_MODE_ARM_THUMB;
    state.isa.kind = ASMKIT_ISA_STATE_NONE;
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_tbb, sizeof(thumb_tbb), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst_with_state(&engine, 0, &inst, &state, &control) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst_values(&engine, 0, &inst, &control, &values) == ASMKIT_OK);
    ASMKIT_CHECK(values.effect_count == 4u);
    ASMKIT_CHECK(values.effects[0].kind == ASMKIT_VALUE_EFFECT_LOAD);
    ASMKIT_CHECK(values.effects[0].destination.kind == ASMKIT_VALUE_REF_TEMPORARY);
    ASMKIT_CHECK(values.effects[0].destination.width == 8u);
    ASMKIT_CHECK(values.effects[3].kind == ASMKIT_VALUE_EFFECT_ADD);
    ASMKIT_CHECK(values.effects[3].destination.kind == ASMKIT_VALUE_REF_CONTROL_TARGET);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32);
    state.mode = ASMKIT_MODE_ARM_A32;
    state.isa.kind = ASMKIT_ISA_STATE_NONE;
    state.isa.arm_itstate = 0u;
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a32_blx, sizeof(a32_blx), 0x2000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_STATE_SWITCH) != 0u);
    ASMKIT_CHECK((inst.operands[0].abs_target & 1u) != 0u);
    ASMKIT_CHECK(asmkit_analyze_inst_with_state(&engine, 0, &inst, &state, &control) == ASMKIT_OK);
    ASMKIT_CHECK(control.successor_count == 2u);
    ASMKIT_CHECK(control.successors[0].flow == ASMKIT_FLOW_CALL);
    ASMKIT_CHECK(control.successors[0].state.mode == ASMKIT_MODE_ARM_THUMB);
    ASMKIT_CHECK(control.successors[1].flow == ASMKIT_FLOW_FALLTHROUGH);
    ASMKIT_CHECK(control.successors[1].state.mode == ASMKIT_MODE_ARM_A32);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_ADD);
    inst.inst_class = ASMKIT_INST_ALU;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_with_flags(
        asmkit_operand_reg(0u, 32u), ASMKIT_OPERAND_FLAG_WRITE)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_with_flags(
        asmkit_operand_reg(1u, 32u), ASMKIT_OPERAND_FLAG_READ)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(4, 32u)) == ASMKIT_OK);
    memset(&control, 0, sizeof(control));
    ASMKIT_CHECK(asmkit_analyze_inst_values(&engine, 0, &inst, &control, &values) == ASMKIT_OK);
    ASMKIT_CHECK(values.effect_count == 1u && values.effects[0].kind == ASMKIT_VALUE_EFFECT_ADD);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64);
    ASMKIT_CHECK(asmkit_get_target_capabilities(&engine, &capabilities) == ASMKIT_OK);
    ASMKIT_CHECK(capabilities.minimum_instruction_unit == 4u);
    ASMKIT_CHECK(asmkit_decode_one_ex(&engine, 0, short_a64, sizeof(short_a64), 0u, &inst, &failure) == ASMKIT_ERR_DECODE_FAILED);
    ASMKIT_CHECK(failure == ASMKIT_DECODE_FAILURE_INSUFFICIENT_BYTES);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_LDRSW);
    inst.inst_class = ASMKIT_INST_LOAD;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_with_flags(
        asmkit_operand_reg(0u, 64u), ASMKIT_OPERAND_FLAG_WRITE)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_with_flags(
        asmkit_operand_mem_full(1u, 2u, 4u, 0, 32u, 64u), ASMKIT_OPERAND_FLAG_READ)) == ASMKIT_OK);
    memset(&control, 0, sizeof(control));
    ASMKIT_CHECK(asmkit_analyze_inst_values(&engine, 0, &inst, &control, &values) == ASMKIT_OK);
    ASMKIT_CHECK(values.effect_count == 2u);
    ASMKIT_CHECK(values.effects[0].kind == ASMKIT_VALUE_EFFECT_LOAD);
    ASMKIT_CHECK(values.effects[1].kind == ASMKIT_VALUE_EFFECT_SIGN_EXTEND);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64);
    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_ADD);
    inst.inst_class = ASMKIT_INST_ALU;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_with_flags(
        asmkit_operand_reg(ASMKIT_BPF_REG_R0, 64u), ASMKIT_OPERAND_FLAG_READ | ASMKIT_OPERAND_FLAG_WRITE)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(1, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst_values(&engine, 0, &inst, &control, &values) == ASMKIT_OK);
    ASMKIT_CHECK(values.effect_count == 1u && values.effects[0].kind == ASMKIT_VALUE_EFFECT_ADD);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, wasm_block, sizeof(wasm_block), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst(&engine, 0, &inst, &sem) == ASMKIT_OK);
    ASMKIT_CHECK((sem.hazard_flags & ASMKIT_HAZARD_STRUCTURED_CF) != 0u);
    ASMKIT_CHECK(asmkit_get_target_capabilities(&engine, &capabilities) == ASMKIT_OK);
    ASMKIT_CHECK((capabilities.flags & ASMKIT_TARGET_CAP_NATIVE_LINEAR) == 0u);
    ASMKIT_CHECK((capabilities.flags & ASMKIT_TARGET_CAP_STRUCTURED_CONTROL_FLOW) != 0u);

    return 0;
}

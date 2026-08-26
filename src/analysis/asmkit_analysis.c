#include "core/asmkit_internal.h"

asmkit_status_t asmkit_analyze_inst(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    asmkit_workspace_t* ASMKIT_RESTRICT workspace,
    const asmkit_inst_t* ASMKIT_RESTRICT inst,
    asmkit_inst_semantics_t* ASMKIT_RESTRICT out_semantics)
{
    const asmkit_target_ops_t* ops;

    if (engine == 0 || inst == 0 || out_semantics == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    ops = asmkit_engine_ops(engine);
    if (ops == 0 || ops->analyze_inst == 0) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    return ops->analyze_inst(engine, workspace, inst, out_semantics);
}

static asmkit_decode_state_t asmkit_default_decode_state(const asmkit_inst_t* inst)
{
    asmkit_decode_state_t state;
    state.mode = inst->mode;
    state.isa.kind = ASMKIT_ISA_STATE_NONE;
    state.isa.arm_itstate = 0u;
    return state;
}

static uint8_t asmkit_arm_advance_itstate(uint8_t value)
{
    if ((value & 0x07u) == 0u) {
        return 0u;
    }
    return (uint8_t)((value & 0xe0u) | ((value << 1u) & 0x1fu));
}

static void asmkit_add_successor(
    asmkit_control_analysis_t* analysis,
    asmkit_flow_kind_t flow,
    asmkit_decode_state_t state)
{
    if (analysis->successor_count < ASMKIT_MAX_SUCCESSOR_STATES) {
        analysis->successors[analysis->successor_count].flow = flow;
        analysis->successors[analysis->successor_count].state = state;
        ++analysis->successor_count;
    }
}

static void asmkit_arm_apply_predicate(
    const asmkit_inst_t* inst,
    const asmkit_decode_state_t* state,
    asmkit_control_analysis_t* analysis)
{
    uint32_t condition;

    if (inst->mode == ASMKIT_MODE_ARM_THUMB &&
        state->isa.kind == ASMKIT_ISA_STATE_ARM_IT && state->isa.arm_itstate != 0u &&
        inst->inst_class != ASMKIT_INST_ARM_IT) {
        analysis->predicate.kind = ASMKIT_EXECUTION_PREDICATE_ARM_CONDITION;
        analysis->predicate.predicate_id = (uint32_t)(state->isa.arm_itstate >> 4u);
        return;
    }
    if (inst->mode == ASMKIT_MODE_ARM_THUMB && inst->inst_class == ASMKIT_INST_COND_BRANCH && inst->size >= 2u) {
        condition = (uint32_t)((asmkit_load16le(inst->bytes) >> 8u) & 0x0fu);
        if (condition < 0x0eu) {
            analysis->predicate.kind = ASMKIT_EXECUTION_PREDICATE_ARM_CONDITION;
            analysis->predicate.predicate_id = condition;
        }
        return;
    }
    if (inst->mode == ASMKIT_MODE_ARM_A32 && inst->size >= 4u) {
        condition = asmkit_load32le(inst->bytes) >> 28u;
        if (condition < 0x0eu) {
            analysis->predicate.kind = ASMKIT_EXECUTION_PREDICATE_ARM_CONDITION;
            analysis->predicate.predicate_id = condition;
        }
    }
}

asmkit_status_t asmkit_analyze_inst_with_state(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    asmkit_workspace_t* ASMKIT_RESTRICT workspace,
    const asmkit_inst_t* ASMKIT_RESTRICT inst,
    const asmkit_decode_state_t* ASMKIT_RESTRICT state,
    asmkit_control_analysis_t* ASMKIT_RESTRICT out_analysis)
{
    asmkit_decode_state_t current;
    asmkit_decode_state_t next;
    asmkit_decode_state_t target;
    asmkit_status_t status;

    if (engine == 0 || inst == 0 || out_analysis == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    current = state != 0 ? *state : asmkit_default_decode_state(inst);
    if (current.mode == ASMKIT_MODE_DEFAULT) {
        current.mode = inst->mode;
    }
    if (current.mode != inst->mode) {
        return ASMKIT_ERR_UNSUPPORTED_MODE;
    }
    if (current.isa.kind != ASMKIT_ISA_STATE_NONE &&
        !(inst->arch == ASMKIT_ARCH_ARM && inst->mode == ASMKIT_MODE_ARM_THUMB &&
          current.isa.kind == ASMKIT_ISA_STATE_ARM_IT)) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }

    asmkit_zero(out_analysis, sizeof(*out_analysis));
    out_analysis->predicate.kind = ASMKIT_EXECUTION_PREDICATE_ALWAYS;
    status = asmkit_analyze_inst(engine, workspace, inst, &out_analysis->semantics);
    if (status != ASMKIT_OK) {
        return status;
    }

    if (inst->arch == ASMKIT_ARCH_ARM) {
        asmkit_arm_apply_predicate(inst, &current, out_analysis);
    }

    next = current;
    if (inst->arch == ASMKIT_ARCH_ARM && inst->mode == ASMKIT_MODE_ARM_THUMB) {
        if (inst->inst_class == ASMKIT_INST_ARM_IT && inst->operand_count >= 2u) {
            next.isa.kind = ASMKIT_ISA_STATE_ARM_IT;
            next.isa.arm_itstate = (uint8_t)(((uint8_t)inst->operands[0].imm << 4u) |
                                             ((uint8_t)inst->operands[1].imm & 0x0fu));
        } else if (current.isa.kind == ASMKIT_ISA_STATE_ARM_IT) {
            next.isa.arm_itstate = asmkit_arm_advance_itstate(current.isa.arm_itstate);
            if (next.isa.arm_itstate == 0u) {
                next.isa.kind = ASMKIT_ISA_STATE_NONE;
            }
        }
    }

    target = next;
    if (inst->arch == ASMKIT_ARCH_ARM && (inst->flags & ASMKIT_INST_FLAG_STATE_SWITCH) != 0u) {
        if (out_analysis->semantics.branch.has_target) {
            target.mode = (out_analysis->semantics.branch.target & 1u) != 0u ?
                ASMKIT_MODE_ARM_THUMB : ASMKIT_MODE_ARM_A32;
            target.isa.kind = ASMKIT_ISA_STATE_NONE;
            target.isa.arm_itstate = 0u;
        } else {
            target.mode = ASMKIT_MODE_DEFAULT;
        }
    }

    if (out_analysis->semantics.branch.is_call) {
        asmkit_add_successor(out_analysis, ASMKIT_FLOW_CALL, target);
        if ((inst->flags & ASMKIT_INST_FLAG_TERMINATOR) == 0u) {
            asmkit_add_successor(out_analysis, ASMKIT_FLOW_FALLTHROUGH, next);
        }
    } else if (out_analysis->semantics.branch.is_branch) {
        asmkit_add_successor(out_analysis, ASMKIT_FLOW_BRANCH, target);
        if (out_analysis->semantics.branch.is_conditional) {
            asmkit_add_successor(out_analysis, ASMKIT_FLOW_FALLTHROUGH, next);
        }
    } else if (out_analysis->semantics.branch.is_return) {
        asmkit_add_successor(out_analysis, ASMKIT_FLOW_RETURN, next);
    } else if ((inst->flags & ASMKIT_INST_FLAG_TERMINATOR) != 0u) {
        asmkit_add_successor(out_analysis, ASMKIT_FLOW_TRAP, next);
    } else {
        asmkit_add_successor(out_analysis, ASMKIT_FLOW_FALLTHROUGH, next);
    }
    return ASMKIT_OK;
}

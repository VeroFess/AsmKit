#include "core/asmkit_internal.h"
#include "asmkit/target/aarch64.h"
#include "asmkit/target/arm.h"
#include "asmkit/target/bpf.h"
#include "asmkit/target/x86.h"

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

static asmkit_value_ref_t asmkit_value_none(void)
{
    asmkit_value_ref_t ref;
    asmkit_zero(&ref, sizeof(ref));
    ref.kind = ASMKIT_VALUE_REF_NONE;
    return ref;
}

static asmkit_value_ref_t asmkit_value_operand(const asmkit_inst_t* inst, uint32_t index)
{
    asmkit_value_ref_t ref = asmkit_value_none();
    if (index >= inst->operand_count) {
        return ref;
    }
    ref.kind = ASMKIT_VALUE_REF_OPERAND;
    ref.operand_index = (uint8_t)index;
    ref.width = inst->operands[index].width;
    if (inst->operands[index].kind == ASMKIT_OP_REG) {
        ref.register_id = inst->operands[index].reg;
    } else if (inst->operands[index].kind == ASMKIT_OP_IMM || inst->operands[index].kind == ASMKIT_OP_PC_REL) {
        ref.immediate = inst->operands[index].imm;
    }
    return ref;
}

static asmkit_value_ref_t asmkit_value_immediate(uint64_t value, uint16_t width)
{
    asmkit_value_ref_t ref = asmkit_value_none();
    ref.kind = ASMKIT_VALUE_REF_IMMEDIATE;
    ref.width = width;
    ref.immediate = (int64_t)value;
    return ref;
}

static asmkit_value_ref_t asmkit_value_temporary(uint8_t index, uint16_t width)
{
    asmkit_value_ref_t ref = asmkit_value_none();
    ref.kind = ASMKIT_VALUE_REF_TEMPORARY;
    ref.operand_index = index;
    ref.width = width;
    return ref;
}

static asmkit_value_ref_t asmkit_value_control_target(uint16_t width)
{
    asmkit_value_ref_t ref = asmkit_value_none();
    ref.kind = ASMKIT_VALUE_REF_CONTROL_TARGET;
    ref.width = width;
    return ref;
}

static asmkit_value_effect_t* asmkit_value_add_effect(
    asmkit_value_analysis_t* analysis, asmkit_value_effect_kind_t kind)
{
    asmkit_value_effect_t* effect;
    if (analysis->effect_count >= ASMKIT_MAX_VALUE_EFFECTS) {
        return 0;
    }
    effect = &analysis->effects[analysis->effect_count++];
    asmkit_zero(effect, sizeof(*effect));
    effect->kind = kind;
    effect->destination = asmkit_value_none();
    effect->sources[0] = asmkit_value_none();
    effect->sources[1] = asmkit_value_none();
    return effect;
}

static int asmkit_find_memory_operand(const asmkit_inst_t* inst)
{
    uint32_t i;
    for (i = 0u; i < inst->operand_count; ++i) {
        if (inst->operands[i].kind == ASMKIT_OP_MEM) {
            return (int)i;
        }
    }
    return -1;
}

static int asmkit_find_destination_operand(const asmkit_inst_t* inst)
{
    uint32_t i;
    for (i = 0u; i < inst->operand_count; ++i) {
        if ((inst->operands[i].flags & ASMKIT_OPERAND_FLAG_WRITE) != 0u) {
            return (int)i;
        }
    }
    return inst->operand_count != 0u ? 0 : -1;
}

static int asmkit_find_source_operand(const asmkit_inst_t* inst, int destination, int memory)
{
    uint32_t i;
    for (i = 0u; i < inst->operand_count; ++i) {
        if ((int)i == destination || (int)i == memory) {
            continue;
        }
        if ((inst->operands[i].flags & ASMKIT_OPERAND_FLAG_READ) != 0u || i != 0u) {
            return (int)i;
        }
    }
    return -1;
}

static void asmkit_emit_memory_effect(const asmkit_inst_t* inst, asmkit_value_analysis_t* analysis)
{
    int destination = asmkit_find_destination_operand(inst);
    int memory = asmkit_find_memory_operand(inst);
    int source;
    asmkit_value_effect_t* effect;
    if (memory < 0) {
        return;
    }
    if (inst->inst_class == ASMKIT_INST_STORE || destination == memory) {
        source = asmkit_find_source_operand(inst, memory, -1);
        effect = asmkit_value_add_effect(analysis, ASMKIT_VALUE_EFFECT_STORE);
        if (effect == 0) return;
        effect->destination = asmkit_value_operand(inst, (uint32_t)memory);
        if (source >= 0) {
            effect->source_count = 1u;
            effect->sources[0] = asmkit_value_operand(inst, (uint32_t)source);
        }
    } else if (destination >= 0) {
        effect = asmkit_value_add_effect(analysis, ASMKIT_VALUE_EFFECT_LOAD);
        if (effect == 0) return;
        effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
        effect->source_count = 1u;
        effect->sources[0] = asmkit_value_operand(inst, (uint32_t)memory);
    }
}

static void asmkit_emit_binary_effect(
    const asmkit_inst_t* inst, asmkit_value_analysis_t* analysis, asmkit_value_effect_kind_t kind)
{
    int destination = asmkit_find_destination_operand(inst);
    asmkit_value_effect_t* effect;
    if (destination < 0 || inst->operand_count < 2u) return;
    effect = asmkit_value_add_effect(analysis, kind);
    if (effect == 0) return;
    effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
    effect->source_count = 2u;
    if (inst->operand_count >= 3u) {
        effect->sources[0] = asmkit_value_operand(inst, 1u);
        effect->sources[1] = asmkit_value_operand(inst, 2u);
    } else {
        effect->sources[0] = asmkit_value_operand(inst, (uint32_t)destination);
        effect->sources[1] = asmkit_value_operand(inst, destination == 0 ? 1u : 0u);
    }
}

static asmkit_compare_relation_t asmkit_x86_taken_relation(asmkit_mnemonic_id_t mnemonic)
{
    switch (mnemonic) {
    case ASMKIT_X86_JA: return ASMKIT_COMPARE_UGT;
    case ASMKIT_X86_JAE: return ASMKIT_COMPARE_UGE;
    case ASMKIT_X86_JB: return ASMKIT_COMPARE_ULT;
    case ASMKIT_X86_JBE: return ASMKIT_COMPARE_ULE;
    case ASMKIT_X86_JE: return ASMKIT_COMPARE_EQ;
    case ASMKIT_X86_JNE: return ASMKIT_COMPARE_NE;
    case ASMKIT_X86_JG: return ASMKIT_COMPARE_SGT;
    case ASMKIT_X86_JGE: return ASMKIT_COMPARE_SGE;
    case ASMKIT_X86_JL: return ASMKIT_COMPARE_SLT;
    case ASMKIT_X86_JLE: return ASMKIT_COMPARE_SLE;
    default: return ASMKIT_COMPARE_NONE;
    }
}

static asmkit_compare_relation_t asmkit_arm_taken_relation(uint32_t condition)
{
    switch (condition) {
    case 0u: return ASMKIT_COMPARE_EQ;
    case 1u: return ASMKIT_COMPARE_NE;
    case 2u: return ASMKIT_COMPARE_UGE;
    case 3u: return ASMKIT_COMPARE_ULT;
    case 8u: return ASMKIT_COMPARE_UGT;
    case 9u: return ASMKIT_COMPARE_ULE;
    case 10u: return ASMKIT_COMPARE_SGE;
    case 11u: return ASMKIT_COMPARE_SLT;
    case 12u: return ASMKIT_COMPARE_SGT;
    case 13u: return ASMKIT_COMPARE_SLE;
    default: return ASMKIT_COMPARE_NONE;
    }
}

static void asmkit_emit_compare(
    const asmkit_inst_t* inst, asmkit_value_analysis_t* analysis, asmkit_condition_state_t state)
{
    asmkit_value_effect_t* effect;
    if (inst->operand_count < 2u) return;
    effect = asmkit_value_add_effect(analysis, ASMKIT_VALUE_EFFECT_COMPARE);
    if (effect != 0) {
        effect->source_count = 2u;
        effect->sources[0] = asmkit_value_operand(inst, 0u);
        effect->sources[1] = asmkit_value_operand(inst, 1u);
    }
    analysis->condition.defines_condition = true;
    analysis->condition.state = state;
    analysis->condition.left = asmkit_value_operand(inst, 0u);
    analysis->condition.right = asmkit_value_operand(inst, 1u);
}

asmkit_status_t asmkit_analyze_inst_values(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    asmkit_workspace_t* ASMKIT_RESTRICT workspace,
    const asmkit_inst_t* ASMKIT_RESTRICT inst,
    const asmkit_control_analysis_t* ASMKIT_RESTRICT control,
    asmkit_value_analysis_t* ASMKIT_RESTRICT out_analysis)
{
    asmkit_value_effect_t* effect;
    int destination;
    int source;
    int memory;
    asmkit_mnemonic_id_t mnemonic;
    (void)workspace;
    if (engine == 0 || inst == 0 || control == 0 || out_analysis == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    if (engine->config.arch != inst->arch || engine->config.mode != inst->mode) {
        return ASMKIT_ERR_UNSUPPORTED_MODE;
    }
    asmkit_zero(out_analysis, sizeof(*out_analysis));
    out_analysis->condition.left = asmkit_value_none();
    out_analysis->condition.right = asmkit_value_none();
    mnemonic = inst->mnemonic_id;
    destination = asmkit_find_destination_operand(inst);
    memory = asmkit_find_memory_operand(inst);

    if (inst->inst_class == ASMKIT_INST_LOAD || inst->inst_class == ASMKIT_INST_STORE) {
        asmkit_emit_memory_effect(inst, out_analysis);
    }

    switch (inst->arch) {
    case ASMKIT_ARCH_X86:
        if ((mnemonic == ASMKIT_X86_MOV || mnemonic == ASMKIT_X86_MOVSX ||
             mnemonic == ASMKIT_X86_MOVSXD || mnemonic == ASMKIT_X86_MOVZX) &&
            memory >= 0 && out_analysis->effect_count == 0u) {
            asmkit_emit_memory_effect(inst, out_analysis);
        }
        if (mnemonic == ASMKIT_X86_LEA && destination >= 0) {
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_ADDRESS);
            if (effect != 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = control->semantics.pc_rel.is_pc_relative ?
                    asmkit_value_immediate(control->semantics.pc_rel.original_target, 64u) :
                    asmkit_value_operand(inst, (uint32_t)memory);
            }
        } else if (mnemonic == ASMKIT_X86_MOV && memory < 0 && destination >= 0) {
            source = asmkit_find_source_operand(inst, destination, -1);
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_COPY);
            if (effect != 0 && source >= 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_operand(inst, (uint32_t)source);
            }
        } else if (mnemonic == ASMKIT_X86_MOVSX || mnemonic == ASMKIT_X86_MOVSXD) {
            if (memory < 0) {
                source = asmkit_find_source_operand(inst, destination, -1);
                effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_COPY);
                if (effect != 0 && destination >= 0 && source >= 0) {
                    effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                    effect->source_count = 1u;
                    effect->sources[0] = asmkit_value_operand(inst, (uint32_t)source);
                }
            }
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_SIGN_EXTEND);
            if (effect != 0 && destination >= 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_operand(inst, (uint32_t)destination);
            }
        } else if (mnemonic == ASMKIT_X86_MOVZX) {
            if (memory < 0) {
                source = asmkit_find_source_operand(inst, destination, -1);
                effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_COPY);
                if (effect != 0 && destination >= 0 && source >= 0) {
                    effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                    effect->source_count = 1u;
                    effect->sources[0] = asmkit_value_operand(inst, (uint32_t)source);
                }
            }
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_ZERO_EXTEND);
            if (effect != 0 && destination >= 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_operand(inst, (uint32_t)destination);
            }
        } else if (mnemonic == ASMKIT_X86_ADD) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_ADD);
        else if (mnemonic == ASMKIT_X86_SUB) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SUB);
        else if (mnemonic == ASMKIT_X86_IMUL) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_MUL);
        else if (mnemonic == ASMKIT_X86_SHL) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_LEFT);
        else if (mnemonic == ASMKIT_X86_AND) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_AND);
        else if (mnemonic == ASMKIT_X86_OR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_OR);
        else if (mnemonic == ASMKIT_X86_XOR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_XOR);
        else if (mnemonic == ASMKIT_X86_SHR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_LOGICAL);
        else if (mnemonic == ASMKIT_X86_SAR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_ARITHMETIC);
        else if (mnemonic == ASMKIT_X86_ROL) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_ROTATE_LEFT);
        else if (mnemonic == ASMKIT_X86_ROR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_ROTATE_RIGHT);
        if (mnemonic == ASMKIT_X86_CMP || mnemonic == ASMKIT_X86_TEST || mnemonic == ASMKIT_X86_SUB) {
            asmkit_emit_compare(inst, out_analysis, ASMKIT_CONDITION_STATE_X86_FLAGS);
        } else if (mnemonic == ASMKIT_X86_ADD || mnemonic == ASMKIT_X86_IMUL ||
                   mnemonic == ASMKIT_X86_SHL || mnemonic == ASMKIT_X86_AND ||
                   mnemonic == ASMKIT_X86_OR || mnemonic == ASMKIT_X86_XOR ||
                   mnemonic == ASMKIT_X86_SHR || mnemonic == ASMKIT_X86_SAR ||
                   mnemonic == ASMKIT_X86_ROL || mnemonic == ASMKIT_X86_ROR ||
                   mnemonic == ASMKIT_X86_INC || mnemonic == ASMKIT_X86_DEC) {
            out_analysis->condition.defines_condition = true;
            out_analysis->condition.state = ASMKIT_CONDITION_STATE_X86_FLAGS;
        }
        out_analysis->condition.taken_relation = asmkit_x86_taken_relation(mnemonic);
        if (out_analysis->condition.taken_relation != ASMKIT_COMPARE_NONE) {
            out_analysis->condition.consumes_condition = true;
            out_analysis->condition.state = ASMKIT_CONDITION_STATE_X86_FLAGS;
        }
        break;
    case ASMKIT_ARCH_ARM:
        if ((mnemonic == ASMKIT_ARM_TBB || mnemonic == ASMKIT_ARM_TBH) && memory >= 0) {
            const uint16_t entry_width = mnemonic == ASMKIT_ARM_TBB ? 8u : 16u;
            const uint64_t pc = ((inst->address + UINT64_C(4)) & ~UINT64_C(3)) | UINT64_C(1);
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_LOAD);
            if (effect != 0) {
                effect->destination = asmkit_value_temporary(0u, entry_width);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_operand(inst, (uint32_t)memory);
            }
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_ZERO_EXTEND);
            if (effect != 0) {
                effect->destination = asmkit_value_temporary(1u, 32u);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_temporary(0u, entry_width);
            }
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_MUL);
            if (effect != 0) {
                effect->destination = asmkit_value_temporary(2u, 32u);
                effect->source_count = 2u;
                effect->sources[0] = asmkit_value_temporary(1u, 32u);
                effect->sources[1] = asmkit_value_immediate(UINT64_C(2), 32u);
            }
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_ADD);
            if (effect != 0) {
                effect->destination = asmkit_value_control_target(32u);
                effect->source_count = 2u;
                effect->sources[0] = asmkit_value_immediate(pc, 32u);
                effect->sources[1] = asmkit_value_temporary(2u, 32u);
            }
        } else if ((mnemonic == ASMKIT_ARM_ADR || control->semantics.pc_rel.is_address_materialization) && destination >= 0) {
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_ADDRESS);
            if (effect != 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_immediate(control->semantics.pc_rel.original_target, 32u);
            }
        } else if (mnemonic == ASMKIT_ARM_MOV || mnemonic == ASMKIT_ARM_MOVS) {
            source = asmkit_find_source_operand(inst, destination, -1);
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_COPY);
            if (effect != 0 && destination >= 0 && source >= 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_operand(inst, (uint32_t)source);
            }
        } else if (mnemonic == ASMKIT_ARM_ADD || mnemonic == ASMKIT_ARM_ADDW) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_ADD);
        else if (mnemonic == ASMKIT_ARM_SUB || mnemonic == ASMKIT_ARM_SUBW) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SUB);
        else if (mnemonic == ASMKIT_ARM_LSL) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_LEFT);
        else if (mnemonic == ASMKIT_ARM_AND) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_AND);
        else if (mnemonic == ASMKIT_ARM_ORR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_OR);
        else if (mnemonic == ASMKIT_ARM_EOR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_XOR);
        else if (mnemonic == ASMKIT_ARM_LSR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_LOGICAL);
        else if (mnemonic == ASMKIT_ARM_ASR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_ARITHMETIC);
        else if (mnemonic == ASMKIT_ARM_ROR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_ROTATE_RIGHT);
        if (mnemonic == ASMKIT_ARM_CMP) asmkit_emit_compare(inst, out_analysis, ASMKIT_CONDITION_STATE_ARM_FLAGS);
        if (mnemonic == ASMKIT_ARM_CBZ || mnemonic == ASMKIT_ARM_CBNZ) {
            out_analysis->condition.defines_condition = true;
            out_analysis->condition.consumes_condition = true;
            out_analysis->condition.state = ASMKIT_CONDITION_STATE_ARM_FLAGS;
            out_analysis->condition.taken_relation = mnemonic == ASMKIT_ARM_CBZ ? ASMKIT_COMPARE_EQ : ASMKIT_COMPARE_NE;
            out_analysis->condition.left = asmkit_value_operand(inst, 0u);
            out_analysis->condition.right = asmkit_value_immediate(0u, inst->operands[0].width);
        } else if (control->semantics.branch.is_conditional && control->predicate.kind == ASMKIT_EXECUTION_PREDICATE_ARM_CONDITION) {
            out_analysis->condition.consumes_condition = true;
            out_analysis->condition.state = ASMKIT_CONDITION_STATE_ARM_FLAGS;
            out_analysis->condition.taken_relation = asmkit_arm_taken_relation(control->predicate.predicate_id);
        }
        break;
    case ASMKIT_ARCH_AARCH64:
        if ((mnemonic == ASMKIT_AARCH64_ADR || mnemonic == ASMKIT_AARCH64_ADRP ||
             control->semantics.pc_rel.is_address_materialization) && destination >= 0) {
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_ADDRESS);
            if (effect != 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_immediate(control->semantics.pc_rel.original_target, 64u);
            }
        } else if (mnemonic == ASMKIT_AARCH64_MOV || mnemonic == ASMKIT_AARCH64_MOVZ || mnemonic == ASMKIT_AARCH64_MOVN) {
            source = asmkit_find_source_operand(inst, destination, -1);
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_COPY);
            if (effect != 0 && destination >= 0 && source >= 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_operand(inst, (uint32_t)source);
            }
        } else if (mnemonic == ASMKIT_AARCH64_ADD) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_ADD);
        else if (mnemonic == ASMKIT_AARCH64_SUB) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SUB);
        else if (mnemonic == ASMKIT_AARCH64_LSL) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_LEFT);
        else if (mnemonic == ASMKIT_AARCH64_AND) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_AND);
        else if (mnemonic == ASMKIT_AARCH64_ORR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_OR);
        else if (mnemonic == ASMKIT_AARCH64_EOR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_XOR);
        else if (mnemonic == ASMKIT_AARCH64_LSR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_LOGICAL);
        else if (mnemonic == ASMKIT_AARCH64_ASR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_ARITHMETIC);
        else if (mnemonic == ASMKIT_AARCH64_ROR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_ROTATE_RIGHT);
        if (mnemonic == ASMKIT_AARCH64_LDRSW || mnemonic == ASMKIT_AARCH64_LDRSH || mnemonic == ASMKIT_AARCH64_LDRSB) {
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_SIGN_EXTEND);
            if (effect != 0 && destination >= 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_operand(inst, (uint32_t)destination);
            }
        }
        if (mnemonic == ASMKIT_AARCH64_CBZ || mnemonic == ASMKIT_AARCH64_CBNZ ||
            mnemonic == ASMKIT_AARCH64_TBZ || mnemonic == ASMKIT_AARCH64_TBNZ) {
            out_analysis->condition.defines_condition = true;
            out_analysis->condition.consumes_condition = true;
            out_analysis->condition.state = ASMKIT_CONDITION_STATE_AARCH64_FLAGS;
            out_analysis->condition.taken_relation =
                (mnemonic == ASMKIT_AARCH64_CBZ || mnemonic == ASMKIT_AARCH64_TBZ) ?
                ASMKIT_COMPARE_EQ : ASMKIT_COMPARE_NE;
            out_analysis->condition.left = asmkit_value_operand(inst, 0u);
            out_analysis->condition.right = asmkit_value_immediate(0u, inst->operands[0].width);
        }
        break;
    case ASMKIT_ARCH_BPF:
        if (mnemonic == ASMKIT_BPF_MOV) {
            source = asmkit_find_source_operand(inst, destination, -1);
            effect = asmkit_value_add_effect(out_analysis, ASMKIT_VALUE_EFFECT_COPY);
            if (effect != 0 && destination >= 0 && source >= 0) {
                effect->destination = asmkit_value_operand(inst, (uint32_t)destination);
                effect->source_count = 1u;
                effect->sources[0] = asmkit_value_operand(inst, (uint32_t)source);
            }
        } else if (mnemonic == ASMKIT_BPF_ADD) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_ADD);
        else if (mnemonic == ASMKIT_BPF_SUB) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SUB);
        else if (mnemonic == ASMKIT_BPF_MUL) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_MUL);
        else if (mnemonic == ASMKIT_BPF_SLL) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_LEFT);
        else if (mnemonic == ASMKIT_BPF_AND) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_AND);
        else if (mnemonic == ASMKIT_BPF_OR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_OR);
        else if (mnemonic == ASMKIT_BPF_XOR) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_XOR);
        else if (mnemonic == ASMKIT_BPF_SRL) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_LOGICAL);
        else if (mnemonic == ASMKIT_BPF_ARSH) asmkit_emit_binary_effect(inst, out_analysis, ASMKIT_VALUE_EFFECT_SHIFT_RIGHT_ARITHMETIC);
        break;
    default:
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    return ASMKIT_OK;
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

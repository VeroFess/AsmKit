#include "test_support.h"
#include "asmkit/target/aarch64.h"
#include "asmkit/target/arm.h"
#include "asmkit/target/bpf.h"
#include "asmkit/target/wasm.h"
#include "asmkit/target/x86.h"

int asmkit_test_emit(void)
{
    asmkit_engine_t engine;
    asmkit_engine_config_t explicit_config;
    asmkit_feature_set_t explicit_features;
    asmkit_inst_t inst;
    const asmkit_instruction_info_t* inst_info;
    asmkit_emit_options_t emit_options;
    asmkit_emit_result_t result;
    asmkit_encode_result_t encode_result;
    uint8_t out[32];
    uint8_t x86_nop[] = {0x90u};
    uint8_t wasm_call[] = {0x10u, 0x00u};

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    ASMKIT_CHECK(asmkit_emit_jump(&engine, 0, 0x1000u, 0x1010u, 0, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.size == 5u);
    ASMKIT_CHECK(out[0] == 0xe9u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 1) == 0x0bu);

    ASMKIT_CHECK(asmkit_emit_jump(&engine, 0, 0x1000u, 0x1000000000ull, 0, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.size == 14u);
    ASMKIT_CHECK(out[0] == 0xffu && out[1] == 0x25u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_NOP);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 1u && out[0] == 0x90u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_RET);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 1u && out[0] == 0xc3u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_JMP);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 5u && out[0] == 0xe9u && asmkit_test_load32le(out + 1) == 0x0bu);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_CALL);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 5u && out[0] == 0xe8u && asmkit_test_load32le(out + 1) == 0x0bu);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_ECX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 2u && out[0] == 0x01u && out[1] == 0xc8u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_ADD && inst.id != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 3u && out[0] == 0x83u && out[1] == 0xc0u && out[2] == 0x05u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(0x12345678, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 5u && out[0] == 0x05u && asmkit_test_load32le(out + 1) == 0x12345678u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_ADD && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_EAX);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 0x12345678);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_LFENCE);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 3u && out[0] == 0x0fu && out[1] == 0xaeu && out[2] == 0xe8u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_LFENCE && inst.operand_count == 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_XABORT);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(0x7f, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 3u && out[0] == 0xc6u && out[1] == 0xf8u && out[2] == 0x7fu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_XABORT && inst.operand_count == 1u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_IMM && inst.operands[0].imm == 0x7f);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_SUB);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_R8D, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_R9D, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 3u && out[0] == 0x45u && out[1] == 0x29u && out[2] == 0xc8u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_R8, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_R9, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 3u && out[0] == 0x4du && out[1] == 0x89u && out[2] == 0xc8u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(0x12345678, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 5u && out[0] == 0xb8u && asmkit_test_load32le(out + 1) == 0x12345678u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_RAX, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(5, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 7u && out[0] == 0x48u && out[1] == 0xc7u && out[2] == 0xc0u && asmkit_test_load32le(out + 3) == 5u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem(ASMKIT_X86_REG_RAX, 8, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_ECX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 3u && out[0] == 0x89u && out[1] == 0x48u && out[2] == 0x08u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem_full(ASMKIT_X86_REG_RAX, ASMKIT_X86_REG_RCX, 4u, 0x20, 32u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(inst.operands[0].reg == 0u && inst.operands[0].imm == 0);
    ASMKIT_CHECK(inst.operands[0].mem.base == ASMKIT_X86_REG_RAX && inst.operands[0].mem.index == ASMKIT_X86_REG_RCX);
    ASMKIT_CHECK(inst.operands[0].mem.scale == 4u && inst.operands[0].mem.displacement == 0x20);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_EDX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && out[0] == 0x89u && out[1] == 0x54u && out[2] == 0x88u && out[3] == 0x20u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_MEM && inst.operands[0].width == 32u);
    ASMKIT_CHECK(inst.operands[0].mem.base == ASMKIT_X86_REG_RAX && inst.operands[0].mem.index == ASMKIT_X86_REG_RCX);
    ASMKIT_CHECK(inst.operands[0].mem.scale == 4u && inst.operands[0].mem.displacement == 0x20);
    ASMKIT_CHECK(inst.operands[0].reg == 0u && inst.operands[0].imm == 0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_X86_REG_EDX);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem_segment(ASMKIT_X86_REG_FS, ASMKIT_X86_REG_RAX, 0u, 1u, 8, 32u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && out[0] == 0x64u && out[1] == 0x8bu && out[2] == 0x40u && out[3] == 0x08u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_EAX);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.segment == ASMKIT_X86_REG_FS);
    ASMKIT_CHECK(inst.operands[1].mem.base == ASMKIT_X86_REG_RAX && inst.operands[1].mem.displacement == 8);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_none()) == ASMKIT_OK);
    inst.operands[0].kind = ASMKIT_OP_MEM;
    inst.operands[0].reg = ASMKIT_X86_REG_RAX;
    inst.operands[0].imm = 8;
    inst.operands[0].width = 32u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_ECX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_ERR_UNSUPPORTED_INSTRUCTION);

    {
        uint8_t x86_rip_mov[] = {0x8bu, 0x05u, 0x34u, 0x12u, 0x00u, 0x00u};
        ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_rip_mov, sizeof(x86_rip_mov), 0x1000u, &inst) == ASMKIT_OK);
        ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV && inst.operand_count == 2u);
        ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_EAX);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_X86_REG_RIP);
    ASMKIT_CHECK(inst.operands[1].mem.displacement == 0x1234 && inst.operands[1].abs_target == 0x223au);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);
    }

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem_full(0u, 0u, 1u, 0x1234, 32u, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u);
    ASMKIT_CHECK(out[0] == 0x67u && out[1] == 0x8bu && out[2] == 0x04u && out[3] == 0x25u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 4) == 0x1234u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV && inst.operands[1].kind == ASMKIT_OP_MEM);
    ASMKIT_CHECK(inst.operands[1].mem.base == ASMKIT_X86_REG_INVALID && inst.operands[1].mem.address_width == 32u);
    ASMKIT_CHECK(inst.operands[1].mem.displacement == 0x1234 && inst.operands[1].abs_target == 0u);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_PC_RELATIVE) == 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_ENTER);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(0x1234, 16u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(2, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && out[0] == 0xc8u && out[1] == 0x34u && out[2] == 0x12u && out[3] == 0x02u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_ENTER && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_IMM && inst.operands[0].imm == 0x1234);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 2);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOVSB);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem_segment(ASMKIT_X86_REG_ES, ASMKIT_X86_REG_RDI, 0u, 1u, 0, 8u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem_full(ASMKIT_X86_REG_RSI, 0u, 1u, 0, 8u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 1u && out[0] == 0xa4u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOVSB && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_MEM && inst.operands[0].mem.base == ASMKIT_X86_REG_RDI);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_X86_REG_RSI);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOVSQ);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem_segment(ASMKIT_X86_REG_ES, ASMKIT_X86_REG_RDI, 0u, 1u, 0, 64u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem_full(ASMKIT_X86_REG_RSI, 0u, 1u, 0, 64u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 2u && out[0] == 0x48u && out[1] == 0xa5u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOVSQ && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_MEM && inst.operands[0].mem.base == ASMKIT_X86_REG_RDI && inst.operands[0].width == 64u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_X86_REG_RSI && inst.operands[1].width == 64u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_RAX, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem_full(ASMKIT_X86_REG_INVALID, ASMKIT_X86_REG_INVALID, 1u, (int64_t)UINT64_C(0x1122334455667788), 64u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 10u && out[0] == 0x48u && out[1] == 0xa1u);
    ASMKIT_CHECK(asmkit_test_load64le(out + 2) == UINT64_C(0x1122334455667788));
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_RAX);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && (uint64_t)inst.operands[1].mem.displacement == UINT64_C(0x1122334455667788));

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VPADDD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_XMM0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_XMM1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_XMM2, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 5u && out[0] == 0xc4u && out[1] == 0xe1u && out[2] == 0x71u && out[3] == 0xfeu && out[4] == 0xc2u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VPADDD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_XMM0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_X86_REG_XMM1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_X86_REG_XMM2);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_ANDN);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_ECX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_EDX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 5u && out[0] == 0xc4u && out[1] == 0xe2u && out[2] == 0x70u && out[3] == 0xf2u && out[4] == 0xc2u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_ANDN && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_EAX && inst.operands[1].reg == ASMKIT_X86_REG_ECX && inst.operands[2].reg == ASMKIT_X86_REG_EDX);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VPMACSDQL);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_XMM0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_XMM1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_XMM2, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_operand_reg(ASMKIT_X86_REG_XMM3, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x8fu && out[1] == 0xe8u && out[2] == 0x70u && out[3] == 0x97u && out[4] == 0xc2u && out[5] == 0x30u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VPMACSDQL && inst.operand_count == 4u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_XMM0 && inst.operands[1].reg == ASMKIT_X86_REG_XMM1);
    ASMKIT_CHECK(inst.operands[2].reg == ASMKIT_X86_REG_XMM2 && inst.operands[3].reg == ASMKIT_X86_REG_XMM3);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VFMADDPD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_XMM0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_XMM1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_XMM2, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_operand_reg(ASMKIT_X86_REG_XMM3, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0xc4u && out[1] == 0xe3u && out[2] == 0xf1u && out[3] == 0x69u && out[4] == 0xc3u && out[5] == 0x20u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VFMADDPD && inst.operand_count == 4u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_XMM0 && inst.operands[1].reg == ASMKIT_X86_REG_XMM1);
    ASMKIT_CHECK(inst.operands[2].reg == ASMKIT_X86_REG_XMM2 && inst.operands[3].reg == ASMKIT_X86_REG_XMM3);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VPERMIL2PD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_XMM0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_XMM1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_XMM2, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_operand_reg(ASMKIT_X86_REG_XMM3, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 4u, asmkit_operand_imm(5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0xc4u && out[1] == 0xe3u && out[2] == 0x71u && out[3] == 0x49u && out[4] == 0xc2u && out[5] == 0x35u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VPERMIL2PD && inst.operand_count == 5u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_XMM0 && inst.operands[1].reg == ASMKIT_X86_REG_XMM1);
    ASMKIT_CHECK(inst.operands[2].reg == ASMKIT_X86_REG_XMM2 && inst.operands[3].reg == ASMKIT_X86_REG_XMM3);
    ASMKIT_CHECK(inst.operands[4].kind == ASMKIT_OP_IMM && inst.operands[4].imm == 5);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VADDPS);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_YMM8, 256u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_YMM9, 256u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_YMM10, 256u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 5u && out[0] == 0xc4u && out[1] == 0x41u && out[2] == 0x34u && out[3] == 0x58u && out[4] == 0xc2u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDPS && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_YMM8 && inst.operands[1].reg == ASMKIT_X86_REG_YMM9 && inst.operands[2].reg == ASMKIT_X86_REG_YMM10);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VADDPD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_ZMM0, 512u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_ZMM1, 512u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_ZMM2, 512u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x62u && out[1] == 0xf1u && out[2] == 0xf5u && out[3] == 0x48u && out[4] == 0x58u && out[5] == 0xc2u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDPD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_ZMM0 && inst.operands[1].reg == ASMKIT_X86_REG_ZMM1 && inst.operands[2].reg == ASMKIT_X86_REG_ZMM2);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VADDPD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_ZMM0, 512u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_ZMM1, 512u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_ZMM2, 512u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_x86_operand_rounding(ASMKIT_X86_ROUND_ZERO)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x62u && out[1] == 0xf1u && out[2] == 0xf5u && out[3] == 0x78u && out[4] == 0x58u && out[5] == 0xc2u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDPD && inst.operand_count == 4u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_ZMM0 && inst.operands[1].reg == ASMKIT_X86_REG_ZMM1 && inst.operands[2].reg == ASMKIT_X86_REG_ZMM2);
    ASMKIT_CHECK(inst.operands[3].kind == ASMKIT_OP_X86_ROUNDING && inst.operands[3].imm == ASMKIT_X86_ROUND_ZERO);
    ASMKIT_CHECK((inst.attributes & ASMKIT_INSTRUCTION_ATTR_X86_SAE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VADDBF16);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_XMM0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_XMM1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_XMM2, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x62u && out[1] == 0xf5u && out[2] == 0x75u && out[3] == 0x08u && out[4] == 0x58u && out[5] == 0xc2u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDBF16 && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_XMM0 && inst.operands[1].reg == ASMKIT_X86_REG_XMM1 && inst.operands[2].reg == ASMKIT_X86_REG_XMM2);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_AESKEYGENASSIST);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_XMM0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_XMM1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(0x1b, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x66u && out[1] == 0x0fu && out[2] == 0x3au && out[3] == 0xdfu && out[4] == 0xc1u && out[5] == 0x1bu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_AESKEYGENASSIST && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_XMM0 && inst.operands[1].reg == ASMKIT_X86_REG_XMM1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 0x1b);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_EXTRACTPS);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem(ASMKIT_X86_REG_RAX, 0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_XMM1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(2, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x66u && out[1] == 0x0fu && out[2] == 0x3au && out[3] == 0x17u && out[4] == 0x08u && out[5] == 0x02u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_EXTRACTPS && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_MEM && inst.operands[0].mem.base == ASMKIT_X86_REG_RAX);
    ASMKIT_CHECK(inst.operands[1].reg == ASMKIT_X86_REG_XMM1 && inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 2);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VADDBF16);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_XMM0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_K1, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_XMM1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_operand_reg(ASMKIT_X86_REG_XMM2, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x62u && out[1] == 0xf5u && out[2] == 0x75u && out[3] == 0x09u && out[4] == 0x58u && out[5] == 0xc2u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDBF16 && inst.operand_count == 4u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_XMM0 && inst.operands[1].reg == ASMKIT_X86_REG_K1);
    ASMKIT_CHECK((inst.operands[1].flags & ASMKIT_OPERAND_FLAG_X86_ZEROING) == 0u);
    ASMKIT_CHECK(inst.operands[2].reg == ASMKIT_X86_REG_XMM1 && inst.operands[3].reg == ASMKIT_X86_REG_XMM2);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VADDBF16);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_XMM0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_x86_zeroing_mask(ASMKIT_X86_REG_K1, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_XMM1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_operand_reg(ASMKIT_X86_REG_XMM2, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x62u && out[1] == 0xf5u && out[2] == 0x75u && out[3] == 0x89u && out[4] == 0x58u && out[5] == 0xc2u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDBF16 && inst.operand_count == 4u);
    ASMKIT_CHECK(inst.operands[1].reg == ASMKIT_X86_REG_K1 && (inst.operands[1].flags & ASMKIT_OPERAND_FLAG_X86_ZEROING) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VBROADCASTF32X2);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_YMM0, 256u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_x86_zeroing_mask(ASMKIT_X86_REG_K1, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_mem(ASMKIT_X86_REG_RAX, 0, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x62u && out[1] == 0xf2u && out[2] == 0x7du && out[3] == 0xa9u && out[4] == 0x19u && out[5] == 0x00u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VBROADCASTF32X2 && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_YMM0);
    ASMKIT_CHECK(inst.operands[1].reg == ASMKIT_X86_REG_K1 && (inst.operands[1].flags & ASMKIT_OPERAND_FLAG_X86_ZEROING) != 0u);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_MEM && inst.operands[2].mem.base == ASMKIT_X86_REG_RAX);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_VADDPD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_ZMM0, 512u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_ZMM1, 512u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_x86_broadcast_mem(ASMKIT_X86_REG_RAX, 0u, 1u, 0, 64u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 6u && out[0] == 0x62u && out[1] == 0xf1u && out[2] == 0xf5u && out[3] == 0x58u && out[4] == 0x58u && out[5] == 0x00u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDPD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_MEM && (inst.operands[2].flags & ASMKIT_OPERAND_FLAG_X86_BROADCAST) != 0u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_16);
    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_16, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem_full(ASMKIT_X86_REG_BX, ASMKIT_X86_REG_SI, 1u, 5, 16u, 16u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_AX, 16u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 3u && out[0] == 0x89u && out[1] == 0x40u && out[2] == 0x05u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV && inst.operands[0].kind == ASMKIT_OP_MEM);
    ASMKIT_CHECK(inst.operands[0].mem.base == ASMKIT_X86_REG_BX && inst.operands[0].mem.index == ASMKIT_X86_REG_SI);
    ASMKIT_CHECK(inst.operands[0].mem.address_width == 16u && inst.operands[0].mem.displacement == 5);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_X86_REG_AX);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_16, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_AX, 16u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem_full(0u, 0u, 1u, 0x1234, 16u, 16u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && out[0] == 0x8bu && out[1] == 0x06u);
    ASMKIT_CHECK(out[2] == 0x34u && out[3] == 0x12u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_X86_REG_INVALID);
    ASMKIT_CHECK(inst.operands[1].mem.address_width == 16u && inst.operands[1].mem.displacement == 0x1234);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_32);
    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_32, ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem_full(ASMKIT_X86_REG_BX, ASMKIT_X86_REG_SI, 1u, 5, 16u, 16u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_AX, 16u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 5u && out[0] == 0x66u && out[1] == 0x67u);
    ASMKIT_CHECK(out[2] == 0x89u && out[3] == 0x40u && out[4] == 0x05u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV && inst.operands[0].mem.address_width == 16u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_32, ASMKIT_X86_JMP);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(0xabcd, 16u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(0x12345678, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 7u && out[0] == 0xeau);
    ASMKIT_CHECK(asmkit_test_load32le(out + 1) == 0x12345678u);
    ASMKIT_CHECK(out[5] == 0xcdu && out[6] == 0xabu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_JMP && inst.inst_class == ASMKIT_INST_INDIRECT_BRANCH);
    ASMKIT_CHECK(inst.operand_count == 2u && (uint16_t)inst.operands[0].imm == 0xabcdu && (uint32_t)inst.operands[1].imm == 0x12345678u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_nop, sizeof(x86_nop), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_NOP);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 1u && out[0] == 0x90u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32);
    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_B);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xea000002u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_BL);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xeb000002u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_ADR);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe28f1008u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_CLREX);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xf57ff01fu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_CLREX);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_LDA);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem(ASMKIT_ARM_REG_R1, 0, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe1910c9fu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_LDA && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].mem.displacement == 0 && inst.operands[1].mem.address_width == 32u);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_ARM, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_LDR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem(ASMKIT_ARM_REG_R1, 4, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe5910004u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_LDR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].mem.displacement == 4 && inst.operands[1].mem.address_width == 32u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_STR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem(ASMKIT_ARM_REG_R3, -8, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe5032008u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_STR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_ARM_REG_R3);
    ASMKIT_CHECK(inst.operands[1].mem.displacement == -8 && inst.operands[1].mem.address_width == 32u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_LDC);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_mem(ASMKIT_ARM_REG_R3, 16, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xed932104u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_LDC && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_MEM && inst.operands[2].mem.base == ASMKIT_ARM_REG_R3);
    ASMKIT_CHECK(inst.operands[2].mem.displacement == 16 && inst.operands[2].mem.address_width == 32u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_LDM);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_arm_operand_reglist(0x0006u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe8900006u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_LDM && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == 0x0006u);
    ASMKIT_CHECK((inst.operands[1].flags & ASMKIT_OPERAND_FLAG_REGISTER_LIST) != 0u);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe8900006u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_R1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_ARM_REG_R2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe0810002u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADD);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R0 && inst.operands[0].width == 32u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_ARM_REG_R1 && inst.operands[1].width == 32u);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_ARM_REG_R2 && inst.operands[2].width == 32u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_SUB);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R3, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_R4, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_ARM_REG_R5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe0443005u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_SUB);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R3 && inst.operands[0].width == 32u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_ARM_REG_R4 && inst.operands[1].width == 32u);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_ARM_REG_R5 && inst.operands[2].width == 32u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_R1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe2810005u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 5);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(0x80000000ll, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe3a02102u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_MOV && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R2);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 0x80000000ll);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_R1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_arm_operand_shifted_reg_imm(ASMKIT_ARM_REG_R2, ASMKIT_ARM_SHIFT_LSL, 3u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe0810182u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_ARM_REG_R2);
    ASMKIT_CHECK(inst.operands[2].shift_kind == ASMKIT_ARM_SHIFT_LSL && inst.operands[2].shift_amount == 3u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_R1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_arm_operand_shifted_reg_reg(ASMKIT_ARM_REG_R2, ASMKIT_ARM_SHIFT_LSL, ASMKIT_ARM_REG_R3)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe0810312u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_ARM_REG_R2);
    ASMKIT_CHECK(inst.operands[2].shift_kind == ASMKIT_ARM_SHIFT_LSL && inst.operands[2].shift_reg == ASMKIT_ARM_REG_R3);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_R1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xe1a00001u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_MOV);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R0 && inst.operands[0].width == 32u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_ARM_REG_R1 && inst.operands[1].width == 32u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_AESD_8);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_Q0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_Q1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xf3b00342u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_VABA_S32);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_D0, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_D1, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_ARM_REG_D2, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xf2210712u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_VABA_S32 && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_D0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_ARM_REG_D1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_ARM_REG_D2);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB);
    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_VCADD_F16);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_Q0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_Q1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_ARM_REG_Q2, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_operand_imm(270, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_VCADD_F16 && inst.operand_count == 4u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_Q0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_ARM_REG_Q1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_ARM_REG_Q2);
    ASMKIT_CHECK(inst.operands[3].kind == ASMKIT_OP_IMM && inst.operands[3].imm == 270);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_R1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 2u && out[0] == 0x08u && out[1] == 0x46u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_MOV);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R0 && inst.operands[0].width == 32u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_ARM_REG_R1 && inst.operands[1].width == 32u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R8, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_ARM_REG_R9, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && out[0] == 0x09u && out[1] == 0xf1u && out[2] == 0x05u && out[3] == 0x08u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R8);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_ARM_REG_R9);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 5);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_ARM, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(0x80000000ll, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && out[0] == 0x4fu && out[1] == 0xf0u && out[2] == 0x00u && out[3] == 0x42u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_MOV && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R2);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 0x80000000ll);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_ARM, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R3, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(0x00ff00ff, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && out[0] == 0x4fu && out[1] == 0xf0u && out[2] == 0xffu && out[3] == 0x13u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_MOV && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R3);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 0x00ff00ff);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_LDR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem(ASMKIT_ARM_REG_R1, 4, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 2u && out[0] == 0x48u && out[1] == 0x68u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_LDR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].mem.displacement == 4 && inst.operands[1].mem.address_width == 32u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_LDR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem_full(ASMKIT_ARM_REG_R1, ASMKIT_ARM_REG_R2, 1u, 0, 0u, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 2u && out[0] == 0x88u && out[1] == 0x58u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_LDR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].mem.index == ASMKIT_ARM_REG_R2 && inst.operands[1].mem.scale == 1u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_LDR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem_full(ASMKIT_ARM_REG_R1, ASMKIT_ARM_REG_R2, 4u, 0, 0u, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && out[0] == 0x51u && out[1] == 0xf8u && out[2] == 0x22u && out[3] == 0x00u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_LDR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].mem.index == ASMKIT_ARM_REG_R2 && inst.operands[1].mem.scale == 4u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_MOV);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R4, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(0x12345678, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_ERR_UNSUPPORTED_INSTRUCTION);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_CBZ);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 2u && out[0] == 0x31u && out[1] == 0xb1u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_CBZ && inst.inst_class == ASMKIT_INST_COND_BRANCH);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_ARM, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK((inst_info->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB, ASMKIT_ARM_CBNZ);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 2u && out[0] == 0x32u && out[1] == 0xb9u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_CBNZ && inst.inst_class == ASMKIT_INST_COND_BRANCH);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R2);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_ARM, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK((inst_info->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64);
    ASMKIT_CHECK(asmkit_emit_jump(&engine, 0, 0x1000u, 0x1010u, 0, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.size == 4u);
    ASMKIT_CHECK(asmkit_test_load32le(out) == 0x14000004u);
    emit_options.mode = ASMKIT_BRANCH_AUTO;
    emit_options.allowed_clobber_mask_lo = 0u;
    emit_options.allowed_clobber_mask_hi = 0u;
    emit_options.flags = 0u;
    ASMKIT_CHECK(asmkit_emit_jump(&engine, 0, 0x1000u, 0x1000000000ull, &emit_options, out, sizeof(out), &result) == ASMKIT_ERR_FORBIDDEN_CLOBBER);
    ASMKIT_CHECK((result.clobber_mask_lo & ((uint64_t)1u << 16)) != 0u);
    emit_options.allowed_clobber_mask_lo = (uint64_t)1u << 16;
    ASMKIT_CHECK(asmkit_emit_jump(&engine, 0, 0x1000u, 0x1000000000ull, &emit_options, out, sizeof(out), &result) == ASMKIT_OK);
    ASMKIT_CHECK(result.size == 16u);
    ASMKIT_CHECK((result.clobber_mask_lo & ((uint64_t)1u << 16)) != 0u);
    ASMKIT_CHECK(asmkit_test_load32le(out) == 0x58000050u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_B);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x14000004u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_BL);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x94000004u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_B);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(0, 4u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x54000080u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_B && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_IMM && inst.operands[0].imm == 0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_CBZ);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x34000081u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_CBZ && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W1);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_TBZ);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(5, 6u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x36280082u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_TBZ && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W2);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 5);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_PC_REL && inst.operands[2].abs_target == 0x1010u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ADR);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X0, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x10000080u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ADR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_X0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_LDR);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x18000080u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_LDR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_BR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X16, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd61f0200u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_BLR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X16, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd63f0200u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_RET);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd65f03c0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x11001420u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 5);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X0, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X1, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(4096, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x91400420u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x0b020020u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ADD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W2);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_aarch64_operand_shifted_reg(ASMKIT_AARCH64_REG_W2, ASMKIT_AARCH64_SHIFT_LSL, 3u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x0b020c20u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ADD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W2);
    ASMKIT_CHECK(inst.operands[2].shift_kind == ASMKIT_AARCH64_SHIFT_LSL && inst.operands[2].shift_amount == 3u);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_AARCH64, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X0, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X1, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_aarch64_operand_extended_reg(ASMKIT_AARCH64_REG_W2, ASMKIT_AARCH64_EXTEND_UXTW, 3u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x8b224c20u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ADD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_X0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_X1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W2);
    ASMKIT_CHECK((inst.operands[2].flags & ASMKIT_OPERAND_FLAG_AARCH64_EXTENDED_REG) != 0u);
    ASMKIT_CHECK(inst.operands[2].extend_kind == ASMKIT_AARCH64_EXTEND_UXTW && inst.operands[2].extend_amount == 3u);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_AARCH64, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X0, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X1, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X2, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x8b020020u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ADD && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_X0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_X1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_X2);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_SUB);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X2, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X3, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(8, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd1002062u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_SUB);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W3, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W4, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x4b050083u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_SUB && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W3);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W4);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W5);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_SUB);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W6, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_aarch64_operand_extended_reg(ASMKIT_AARCH64_REG_W7, ASMKIT_AARCH64_EXTEND_SXTB, 1u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x4b2784c5u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_SUB && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W5);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W6);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W7);
    ASMKIT_CHECK((inst.operands[2].flags & ASMKIT_OPERAND_FLAG_AARCH64_EXTENDED_REG) != 0u);
    ASMKIT_CHECK(inst.operands[2].extend_kind == ASMKIT_AARCH64_EXTEND_SXTB && inst.operands[2].extend_amount == 1u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_AND);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x0a020020u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_AND && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W2);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_AND);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(0xff, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x12001c20u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_AND && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 0xff);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_AARCH64, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_AND);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X2, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X3, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(0xff, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x92401c62u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_AND && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_X2);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_X3);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 0xff);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_AARCH64, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_AND);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X6, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X7, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_aarch64_operand_shifted_reg(ASMKIT_AARCH64_REG_X8, ASMKIT_AARCH64_SHIFT_ROR, 9u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x8ac824e6u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_AND && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_X6);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_X7);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_X8);
    ASMKIT_CHECK(inst.operands[2].shift_kind == ASMKIT_AARCH64_SHIFT_ROR && inst.operands[2].shift_amount == 9u);
    ASMKIT_CHECK(asmkit_get_instruction_info(ASMKIT_ARCH_AARCH64, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X0, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X1, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_aarch64_operand_shifted_reg(ASMKIT_AARCH64_REG_X2, ASMKIT_AARCH64_SHIFT_ROR, 1u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_ERR_UNSUPPORTED_INSTRUCTION);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X0, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X1, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_aarch64_operand_extended_reg(ASMKIT_AARCH64_REG_W2, ASMKIT_AARCH64_EXTEND_UXTW, 5u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_ERR_UNSUPPORTED_INSTRUCTION);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ORR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W3, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W4, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x2a050083u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ORR && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W3);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W4);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W5);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_EOR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X6, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X7, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X8, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xca0800e6u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_EOR && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_X6);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_X7);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_X8);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_CSEL);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_operand_imm(0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x1a820020u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_CSEL && inst.operand_count == 4u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W2);
    ASMKIT_CHECK(inst.operands[3].kind == ASMKIT_OP_IMM && inst.operands[3].imm == 0);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ABS);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_Q0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_Q1, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x4e20b820u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ABS && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_Q0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_Q1);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_MOVZ);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X0, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(0x1234, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(16, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd2a24680u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_NOP);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd503201fu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_NOP);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_TSB);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_opaque(2u, 4u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd503225fu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_TSB && inst.operand_count == 1u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_OPAQUE && inst.operands[0].imm == 2u && inst.operands[0].width == 4u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_TSB);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_opaque(3u, 4u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_ERR_UNSUPPORTED_INSTRUCTION);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_CASPAL);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_aarch64_operand_reg_pair(ASMKIT_AARCH64_REG_W0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_aarch64_operand_reg_pair(ASMKIT_AARCH64_REG_W2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_AARCH64_REG_X3, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0x0860fc62u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_CASPAL && inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK((inst.operands[0].flags & ASMKIT_OPERAND_FLAG_REGISTER_PAIR) != 0u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W2);
    ASMKIT_CHECK((inst.operands[1].flags & ASMKIT_OPERAND_FLAG_REGISTER_PAIR) != 0u);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_MEM && inst.operands[2].mem.base == ASMKIT_AARCH64_REG_X3);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_SYSP);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(2, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_opaque(3u, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_opaque(4u, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_operand_imm(5, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 4u, asmkit_aarch64_operand_reg_pair(ASMKIT_AARCH64_REG_XZR, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd54a34bfu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_SYSP && inst.operand_count == 5u);
    ASMKIT_CHECK(inst.operands[4].kind == ASMKIT_OP_REG && inst.operands[4].reg == ASMKIT_AARCH64_REG_XZR);
    ASMKIT_CHECK((inst.operands[4].flags & ASMKIT_OPERAND_FLAG_REGISTER_PAIR) != 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ZERO);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_opaque(3u, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xc0080003u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ZERO && inst.operand_count == 1u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_OPAQUE && inst.operands[0].imm == 3);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_ZERO);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_ZT0, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xc0480001u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ZERO && inst.operand_count == 1u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_ZT0);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_MOVA);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_Z0, 128u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_AARCH64_REG_P0, 0u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_AARCH64_REG_ZAB0, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 3u, asmkit_operand_reg(ASMKIT_AARCH64_REG_W12, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 4u, asmkit_operand_imm(0, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xc0020000u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_MOVA && inst.operand_count == 5u);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_ZAB0);
    ASMKIT_CHECK(inst.operands[3].kind == ASMKIT_OP_REG && inst.operands[3].reg == ASMKIT_AARCH64_REG_W12);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_MSR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_opaque(ASMKIT_AARCH64_PSTATE_DAIFSET, 6u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(15, 4u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd5034fdfu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_MSR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_OPAQUE && inst.operands[0].imm == ASMKIT_AARCH64_PSTATE_DAIFSET);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 15);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_MSR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_opaque(ASMKIT_AARCH64_PSTATE_ALLINT, 9u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(1, 1u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u && asmkit_test_load32le(out) == 0xd501411fu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_MSR && inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_OPAQUE && inst.operands[0].imm == ASMKIT_AARCH64_PSTATE_ALLINT);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 1);

    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_MSR);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_opaque(0u, 6u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(1, 4u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_ERR_UNSUPPORTED_INSTRUCTION);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64);
    ASMKIT_CHECK(asmkit_emit_jump(&engine, 0, 0u, 8u, 0, out, sizeof(out), &result) == ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_EXIT);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0x95u && asmkit_test_load32le(out + 4) == 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_CALL);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(7, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0x85u && asmkit_test_load32le(out + 4) == 7u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_JA);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0x05u && out[2] == 0x01u && out[3] == 0x00u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(1u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm(9, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0x07u && out[1] == 0x01u && asmkit_test_load32le(out + 4) == 9u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(1u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(2u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0x0fu && out[1] == 0x21u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_JEQ);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_branch_target(0x1018u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(1u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_imm(7, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0x15u && out[1] == 0x01u && out[2] == 0x02u && asmkit_test_load32le(out + 4) == 7u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_LDDW);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(1u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_imm((int64_t)UINT64_C(0x0102030405060708), 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 16u && out[0] == 0x18u && out[1] == 0x01u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 4) == 0x05060708u);
    ASMKIT_CHECK(asmkit_test_load32le(out + 12) == 0x01020304u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_LDX);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(2u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem(1u, 4, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0x79u && out[1] == 0x12u && out[2] == 0x04u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_LDX);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(2u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_none()) == ASMKIT_OK);
    inst.operands[1].kind = ASMKIT_OP_MEM;
    inst.operands[1].reg = 1u;
    inst.operands[1].imm = 4;
    inst.operands[1].width = 64u;
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_ERR_UNSUPPORTED_INSTRUCTION);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_STX);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem(1u, 8, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(2u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0x7bu && out[1] == 0x21u && out[2] == 0x08u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_XCHG);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem(1u, 8, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(2u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0xdbu && out[1] == 0x21u && out[2] == 0x08u && asmkit_test_load32le(out + 4) == 0xe1u);

    ASMKIT_CHECK(asmkit_feature_set_clear(&explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_init(&explicit_config, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_set_features(&explicit_config, &explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_XCHG);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_mem(1u, 8, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(2u, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_ERR_UNSUPPORTED_FEATURE);
    ASMKIT_CHECK(asmkit_engine_config_enable_feature(&explicit_config, ASMKIT_BPF_FEATURE_ALU32) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u && out[0] == 0xc3u && out[1] == 0x21u && out[2] == 0x08u && asmkit_test_load32le(out + 4) == 0xe1u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32);
    asmkit_inst_init(&inst, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32, ASMKIT_WASM_RETURN);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 1u && out[0] == 0x0fu);

    asmkit_inst_init(&inst, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32, ASMKIT_WASM_CALL);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(129, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 3u);
    ASMKIT_CHECK(out[0] == 0x10u && out[1] == 0x81u && out[2] == 0x01u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_DIRECT_CALL && inst.mnemonic_id == ASMKIT_WASM_CALL && inst.operands[0].imm == 129);

    asmkit_inst_init(&inst, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32, ASMKIT_WASM_BLOCK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(0x40, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 2u && out[0] == 0x02u && out[1] == 0x40u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32, ASMKIT_WASM_I32_ADD);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 1u && out[0] == 0x6au);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_WASM_I32_ADD && inst.id != 0u && inst.operand_count == 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32, ASMKIT_WASM_I32_CONST);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_imm(-1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 2u && out[0] == 0x41u && out[1] == 0x7fu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_WASM_I32_CONST && inst.operands[0].kind == ASMKIT_OP_IMM && inst.operands[0].imm == -1);

    ASMKIT_CHECK(asmkit_feature_set_clear(&explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_init(&explicit_config, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_set_features(&explicit_config, &explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    asmkit_inst_init(&inst, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32, ASMKIT_WASM_I32X4_ADD);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_ERR_UNSUPPORTED_FEATURE);
    ASMKIT_CHECK(asmkit_engine_config_enable_feature(&explicit_config, ASMKIT_WASM_FEATURE_SIMD128) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encode_inst(&engine, 0, &inst, 0, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 3u && out[0] == 0xfdu && out[1] == 0xaeu && out[2] == 0x01u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, out, encode_result.size, 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_WASM_I32X4_ADD && inst.id != 0u);

    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, wasm_call, sizeof(wasm_call), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_WASM_CALL);

    return 0;
}

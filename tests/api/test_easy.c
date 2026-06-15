#include "test_support.h"
#include "asmkit/target/aarch64.h"
#include "asmkit/target/arm.h"
#include "asmkit/target/bpf.h"
#include "asmkit/target/x86.h"

static int asmkit_test_easy_cstr_eq(const char* left, const char* right)
{
    uint32_t i;

    i = 0u;
    while (left[i] != '\0' && right[i] != '\0') {
        if (left[i] != right[i]) {
            return 0;
        }
        ++i;
    }
    return left[i] == right[i];
}

int asmkit_test_easy_api(void)
{
    asmkit_decoder_t decoder;
    asmkit_encoder_t encoder;
    asmkit_formatter_t formatter;
    asmkit_inst_t inst;
    asmkit_encode_result_t encode_result;
    asmkit_emit_result_t emit_result;
    asmkit_text_result_t text_result;
    uint8_t out[32];
    char text[64];
#if ASMKIT_ENABLE_TEXT
    uint8_t x86_mov_rax_rbx[] = {0x48u, 0x89u, 0xd8u};
#endif
    uint8_t x86_nop[] = {0x90u};
    uint8_t bpf_add_ri[] = {0x07u, 0x01u, 0x00u, 0x00u, 0x05u, 0x00u, 0x00u, 0x00u};

    ASMKIT_CHECK(asmkit_decoder_init(&decoder, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decoder_decode_full(&decoder, x86_nop, sizeof(x86_nop), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.arch == ASMKIT_ARCH_X86);
    ASMKIT_CHECK(inst.mode == ASMKIT_MODE_X86_64);
    ASMKIT_CHECK(inst.size == 1u);

    ASMKIT_CHECK(asmkit_decode_full(ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, bpf_add_ri, sizeof(bpf_add_ri), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_BPF_ADD);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_BPF_REG_R1);
    ASMKIT_CHECK(inst.operands[1].imm == 5);

    ASMKIT_CHECK(asmkit_decoder_init(&decoder, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encoder_init(&encoder, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64) == ASMKIT_OK);
    asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_EXIT);
    ASMKIT_CHECK(asmkit_encoder_encode_inst(&encoder, &inst, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u);
    ASMKIT_CHECK(out[0] == 0x95u);

    ASMKIT_CHECK(asmkit_encode_full(ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, &inst, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 8u);
    ASMKIT_CHECK(out[0] == 0x95u);

    ASMKIT_CHECK(asmkit_encoder_init(&encoder, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decoder_init(&decoder, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encoder_emit_jump(&encoder, 0x1000u, 0x1010u, 0, out, sizeof(out), &emit_result) == ASMKIT_OK);
    ASMKIT_CHECK(emit_result.size == 5u);
    ASMKIT_CHECK(out[0] == 0xe9u);
    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_NOP);
    ASMKIT_CHECK(asmkit_encoder_encode_inst(&encoder, &inst, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 1u && out[0] == 0x90u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_EBX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_ECX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encoder_encode_inst(&encoder, &inst, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size > 0u);
    ASMKIT_CHECK(asmkit_decoder_decode_full(&decoder, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_ADD);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_EAX);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_X86_REG_EBX);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_X86_REG_ECX);
    ASMKIT_CHECK((inst.attributes & ASMKIT_INSTRUCTION_ATTR_X86_SAE) == 0u);

    asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_ADD);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_mem_full(ASMKIT_X86_REG_RBX, ASMKIT_X86_REG_INVALID, 1u, 0, 32u, 64u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_ECX, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encoder_encode_inst(&encoder, &inst, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size > 0u);
    ASMKIT_CHECK(asmkit_decoder_decode_full(&decoder, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_ADD);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_EAX);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_X86_REG_RBX);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_X86_REG_ECX);
    ASMKIT_CHECK((inst.operands[1].flags & ASMKIT_OPERAND_FLAG_X86_BROADCAST) == 0u);
    ASMKIT_CHECK((inst.attributes & ASMKIT_INSTRUCTION_ATTR_X86_SAE) == 0u);

    ASMKIT_CHECK(asmkit_encoder_init(&encoder, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decoder_init(&decoder, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32) == ASMKIT_OK);
    asmkit_inst_init(&inst, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32, ASMKIT_ARM_ADR);
    inst.address = 0x1000u;
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_ARM_REG_R1, 32u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 1u, asmkit_operand_branch_target(0x1010u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encoder_encode_inst(&encoder, &inst, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u);
    ASMKIT_CHECK(asmkit_decoder_decode_full(&decoder, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADR);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);

    ASMKIT_CHECK(asmkit_encoder_init(&encoder, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decoder_init(&decoder, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64) == ASMKIT_OK);
    asmkit_inst_init(&inst, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64, ASMKIT_AARCH64_PTRUE);
    ASMKIT_CHECK(asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_AARCH64_REG_PN8, 8u)) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_encoder_encode_inst(&encoder, &inst, out, sizeof(out), &encode_result) == ASMKIT_OK);
    ASMKIT_CHECK(encode_result.size == 4u);
    ASMKIT_CHECK(asmkit_decoder_decode_full(&decoder, out, encode_result.size, 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_PTRUE);
    ASMKIT_CHECK(inst.operand_count == 1u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_PN8);
    ASMKIT_CHECK(inst.operands[0].reg != ASMKIT_AARCH64_REG_P8);

    ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
#if ASMKIT_ENABLE_TEXT
    ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
    ASMKIT_CHECK(text_result.written > 0u);
    ASMKIT_CHECK(text[0] != '\0');
    ASMKIT_CHECK(asmkit_decode_full(ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, x86_mov_rax_rbx, sizeof(x86_mov_rax_rbx), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_test_easy_cstr_eq(text, "mov rax, rbx"));
    ASMKIT_CHECK(asmkit_decode_full(ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, bpf_add_ri, sizeof(bpf_add_ri), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_test_easy_cstr_eq(text, "add r1, 5"));
#else
    ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_ERR_FEATURE_DISABLED);
#endif

    return 0;
}

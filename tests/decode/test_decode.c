#include <string.h>

#include "test_support.h"
#include "asmkit/target/aarch64.h"
#include "asmkit/target/arm.h"
#include "asmkit/target/bpf.h"
#include "asmkit/target/wasm.h"
#include "asmkit/target/x86.h"

static int asmkit_test_x86_jcc_mnemonics(void)
{
    static const struct {
        uint8_t opcode;
        asmkit_mnemonic_id_t mnemonic;
    } cases[] = {
        {0x70u, ASMKIT_X86_JO},
        {0x71u, ASMKIT_X86_JNO},
        {0x72u, ASMKIT_X86_JB},
        {0x73u, ASMKIT_X86_JAE},
        {0x74u, ASMKIT_X86_JE},
        {0x75u, ASMKIT_X86_JNE},
        {0x76u, ASMKIT_X86_JBE},
        {0x77u, ASMKIT_X86_JA},
        {0x78u, ASMKIT_X86_JS},
        {0x79u, ASMKIT_X86_JNS},
        {0x7au, ASMKIT_X86_JP},
        {0x7bu, ASMKIT_X86_JNP},
        {0x7cu, ASMKIT_X86_JL},
        {0x7du, ASMKIT_X86_JGE},
        {0x7eu, ASMKIT_X86_JLE},
        {0x7fu, ASMKIT_X86_JG},
    };
    asmkit_engine_t engine;
    asmkit_inst_t inst;

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        uint8_t bytes[2] = {cases[i].opcode, 0x00u};
        ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bytes, sizeof(bytes), 0x1000u, &inst) == ASMKIT_OK);
        ASMKIT_CHECK(inst.size == sizeof(bytes));
        ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_COND_BRANCH);
        ASMKIT_CHECK(inst.mnemonic_id == cases[i].mnemonic);
        ASMKIT_CHECK(inst.operand_count == 1u);
        ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_PC_REL);
        ASMKIT_CHECK(inst.operands[0].abs_target == 0x1002u);
    }

    {
        uint8_t near_jne[] = {0x0fu, 0x85u, 0x00u, 0x00u, 0x00u, 0x00u};
        ASMKIT_CHECK(asmkit_decode_one(&engine, 0, near_jne, sizeof(near_jne), 0x1000u, &inst) == ASMKIT_OK);
        ASMKIT_CHECK(inst.size == sizeof(near_jne));
        ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_COND_BRANCH);
        ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_JNE);
        ASMKIT_CHECK(inst.operand_count == 1u);
        ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_PC_REL);
        ASMKIT_CHECK(inst.operands[0].abs_target == 0x1006u);
    }

    return 0;
}

static int asmkit_test_x86_register_modrm_rm_mr_forms(void)
{
    static const struct {
        uint8_t bytes[3];
        uint8_t size;
        asmkit_mnemonic_id_t mnemonic;
        uint32_t dst_reg;
        uint32_t src_reg;
        uint32_t width;
    } cases[] = {
        {{0x33u, 0xc0u, 0x00u}, 2u, ASMKIT_X86_XOR, ASMKIT_X86_REG_EAX, ASMKIT_X86_REG_EAX, 32u},
        {{0x41u, 0x0bu, 0xd1u}, 3u, ASMKIT_X86_OR, ASMKIT_X86_REG_EDX, ASMKIT_X86_REG_R9D, 32u},
        {{0x48u, 0x0bu, 0xc2u}, 3u, ASMKIT_X86_OR, ASMKIT_X86_REG_RAX, ASMKIT_X86_REG_RDX, 64u},
        {{0x48u, 0x8bu, 0xd0u}, 3u, ASMKIT_X86_MOV, ASMKIT_X86_REG_RDX, ASMKIT_X86_REG_RAX, 64u},
    };
    asmkit_engine_t engine;
    asmkit_inst_t inst;

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        ASMKIT_CHECK(asmkit_decode_one(&engine, 0, cases[i].bytes, cases[i].size, 0x1000u, &inst) == ASMKIT_OK);
        ASMKIT_CHECK(inst.size == cases[i].size);
        ASMKIT_CHECK(inst.mnemonic_id == cases[i].mnemonic);
        ASMKIT_CHECK(inst.operand_count == 2u);
        ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG);
        ASMKIT_CHECK(inst.operands[0].reg == cases[i].dst_reg);
        ASMKIT_CHECK(inst.operands[0].width == cases[i].width);
        ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG);
        ASMKIT_CHECK(inst.operands[1].reg == cases[i].src_reg);
        ASMKIT_CHECK(inst.operands[1].width == cases[i].width);
    }

    return 0;
}

static int asmkit_test_x86_setcc_mnemonics(void)
{
    static const struct {
        uint8_t opcode;
        asmkit_mnemonic_id_t mnemonic;
    } cases[] = {
        {0x90u, ASMKIT_X86_SETO},
        {0x91u, ASMKIT_X86_SETNO},
        {0x92u, ASMKIT_X86_SETB},
        {0x93u, ASMKIT_X86_SETAE},
        {0x94u, ASMKIT_X86_SETE},
        {0x95u, ASMKIT_X86_SETNE},
        {0x96u, ASMKIT_X86_SETBE},
        {0x97u, ASMKIT_X86_SETA},
        {0x98u, ASMKIT_X86_SETS},
        {0x99u, ASMKIT_X86_SETNS},
        {0x9au, ASMKIT_X86_SETP},
        {0x9bu, ASMKIT_X86_SETNP},
        {0x9cu, ASMKIT_X86_SETL},
        {0x9du, ASMKIT_X86_SETGE},
        {0x9eu, ASMKIT_X86_SETLE},
        {0x9fu, ASMKIT_X86_SETG},
    };
    asmkit_engine_t engine;
    asmkit_inst_t inst;

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        uint8_t bytes[3] = {0x0fu, cases[i].opcode, 0xc0u};
        ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bytes, sizeof(bytes), 0x1000u, &inst) == ASMKIT_OK);
        ASMKIT_CHECK(inst.size == sizeof(bytes));
        ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_STORE);
        ASMKIT_CHECK(inst.mnemonic_id == cases[i].mnemonic);
        ASMKIT_CHECK(inst.operand_count == 1u);
        ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG);
        ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_AL);
        ASMKIT_CHECK(inst.operands[0].width == 8u);
    }

    return 0;
}

static int asmkit_test_x86_lock_prefix(void)
{
    uint8_t lock_inc_rip[] = {0xf0u, 0xffu, 0x05u, 0x00u, 0x00u, 0x00u, 0x00u};
    asmkit_engine_t engine;
    asmkit_inst_t inst;

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, lock_inc_rip, sizeof(lock_inc_rip), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(lock_inc_rip));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_INC);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_X86_LOCK) != 0u);
    ASMKIT_CHECK(inst.operand_count == 1u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_MEM);
    ASMKIT_CHECK(inst.operands[0].mem.base == ASMKIT_X86_REG_RIP);
    ASMKIT_CHECK(inst.operands[0].abs_target == 0x1007u);

#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "lock inc") == 0);
    }
#endif

    return 0;
}

static int asmkit_test_x86_rep_string_prefixes(void)
{
    uint8_t rep_movsw[] = {0xf3u, 0x66u, 0xa5u};
    uint8_t repne_scasb[] = {0xf2u, 0xaeu};
    asmkit_engine_t engine;
    asmkit_inst_t inst;

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_32);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, rep_movsw, sizeof(rep_movsw), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(rep_movsw));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOVSW);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_X86_REP) != 0u);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_X86_REPNE) == 0u);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_MEM && inst.operands[0].width == 16u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].width == 16u);

#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "rep movsw") == 0);
    }
#endif

    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, repne_scasb, sizeof(repne_scasb), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(repne_scasb));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_SCASB);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_X86_REPNE) != 0u);
    ASMKIT_CHECK(inst.operand_count == 1u);

#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "repne scasb") == 0);
    }
#endif

    return 0;
}

static int asmkit_test_x86_cmovcc_mnemonics(void)
{
    static const struct {
        uint8_t opcode;
        asmkit_mnemonic_id_t mnemonic;
    } cases[] = {
        {0x40u, ASMKIT_X86_CMOVO},
        {0x41u, ASMKIT_X86_CMOVNO},
        {0x42u, ASMKIT_X86_CMOVB},
        {0x43u, ASMKIT_X86_CMOVAE},
        {0x44u, ASMKIT_X86_CMOVE},
        {0x45u, ASMKIT_X86_CMOVNE},
        {0x46u, ASMKIT_X86_CMOVBE},
        {0x47u, ASMKIT_X86_CMOVA},
        {0x48u, ASMKIT_X86_CMOVS},
        {0x49u, ASMKIT_X86_CMOVNS},
        {0x4au, ASMKIT_X86_CMOVP},
        {0x4bu, ASMKIT_X86_CMOVNP},
        {0x4cu, ASMKIT_X86_CMOVL},
        {0x4du, ASMKIT_X86_CMOVGE},
        {0x4eu, ASMKIT_X86_CMOVLE},
        {0x4fu, ASMKIT_X86_CMOVG},
    };
    asmkit_engine_t engine;
    asmkit_inst_t inst;

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        uint8_t bytes[3] = {0x0fu, cases[i].opcode, 0xc1u};
        ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bytes, sizeof(bytes), 0x1000u, &inst) == ASMKIT_OK);
        ASMKIT_CHECK(inst.size == sizeof(bytes));
        ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
        ASMKIT_CHECK(inst.mnemonic_id == cases[i].mnemonic);
        ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_CONDITIONAL) != 0u);
        ASMKIT_CHECK(inst.operand_count == 2u);
        ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_EAX && inst.operands[0].width == 32u);
        ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_X86_REG_ECX && inst.operands[1].width == 32u);
    }

    {
        uint8_t cmove64[] = {0x48u, 0x0fu, 0x44u, 0xc1u};
        ASMKIT_CHECK(asmkit_decode_one(&engine, 0, cmove64, sizeof(cmove64), 0x1000u, &inst) == ASMKIT_OK);
        ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_CMOVE);
        ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_RAX && inst.operands[0].width == 64u);
        ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_X86_REG_RCX && inst.operands[1].width == 64u);
    }

    return 0;
}

static int asmkit_test_x86_prefix_alias_mnemonics(void)
{
    uint8_t pause_bytes[] = {0xf3u, 0x90u};
    uint8_t rex_push_rbx[] = {0x48u, 0x53u};
    asmkit_engine_t engine;
    asmkit_inst_t inst;

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, pause_bytes, sizeof(pause_bytes), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(pause_bytes));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_PAUSE);
    ASMKIT_CHECK(inst.operand_count == 0u);

    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, rex_push_rbx, sizeof(rex_push_rbx), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(rex_push_rbx));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_PUSH);
    ASMKIT_CHECK(inst.operand_count == 1u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG);
    ASMKIT_CHECK(inst.operands[0].reg == ASMKIT_X86_REG_RBX);
    ASMKIT_CHECK(inst.operands[0].width == 64u);

#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "push") == 0);
    }
#endif

    return 0;
}

int asmkit_test_decode(void)
{
    asmkit_engine_t engine;
    asmkit_engine_config_t explicit_config;
    asmkit_feature_set_t explicit_features;
    asmkit_inst_t inst;
    const asmkit_instruction_info_t* inst_info;
    const asmkit_operand_info_t* operand_info;
    uint8_t x86_rip[] = {0x48u, 0x8bu, 0x05u, 0x78u, 0x56u, 0x34u, 0x12u};
    uint8_t x86_add_eax_ebx[] = {0x01u, 0xd8u};
    uint8_t x86_mov_rax_rbx[] = {0x48u, 0x89u, 0xd8u};
    uint8_t x86_movdqa_xmm0_xmm1[] = {0x66u, 0x0fu, 0x6fu, 0xc1u};
    uint8_t x86_vzeroupper[] = {0xc5u, 0xf8u, 0x77u};
    uint8_t x86_vaddpd_ymm0_ymm0_ymm1[] = {0xc5u, 0xfdu, 0x58u, 0xc1u};
    uint8_t x86_vaddpd_xmm0_xmm0_xmm1_evex[] = {0x62u, 0xf1u, 0xfdu, 0x08u, 0x58u, 0xc1u};
    uint8_t x86_vpmacsdql_xop[] = {0x8fu, 0xe8u, 0x78u, 0x97u, 0xc1u, 0x00u};
    uint8_t x86_enter[] = {0xc8u, 0x34u, 0x12u, 0x02u};
    uint8_t x86_far_jmp32[] = {0xeau, 0x78u, 0x56u, 0x34u, 0x12u, 0xcdu, 0xabu};
    uint8_t x86_call_rel32_zero[] = {0xe8u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t x86_jcc_short_back[] = {0x75u, 0xfeu};
    uint8_t x86_movsb[] = {0xa4u};
    uint8_t x86_movsq[] = {0x48u, 0xa5u};
    uint8_t x86_moffs64_load[] = {
        0x48u, 0xa1u, 0x88u, 0x77u, 0x66u, 0x55u, 0x44u, 0x33u, 0x22u, 0x11u
    };
    uint8_t a64_b[] = {0x04u, 0x00u, 0x00u, 0x14u};
    uint8_t a64_b_ne[] = {0xa1u, 0x03u, 0x00u, 0x54u};
    uint8_t a64_ret[] = {0xc0u, 0x03u, 0x5fu, 0xd6u};
    uint8_t a64_nop[] = {0x1fu, 0x20u, 0x03u, 0xd5u};
    uint8_t a64_bti[] = {0x1fu, 0x24u, 0x03u, 0xd5u};
    uint8_t a64_add_w0_imm1[] = {0x00u, 0x04u, 0x00u, 0x11u};
    uint8_t a64_add_w0_w1_w2_lsl3[] = {0x20u, 0x0cu, 0x02u, 0x0bu};
    uint8_t a64_add_x0_x1_w2_uxtw3[] = {0x20u, 0x4cu, 0x22u, 0x8bu};
    uint8_t a64_sub_w5_w6_w7_sxtb1[] = {0xc5u, 0x84u, 0x27u, 0x4bu};
    uint8_t a64_and_w0_w1_imm_ff[] = {0x20u, 0x1cu, 0x00u, 0x12u};
    uint8_t a64_and_x2_x3_imm_ff[] = {0x62u, 0x1cu, 0x40u, 0x92u};
    uint8_t a64_and_x6_x7_x8_ror9[] = {0xe6u, 0x24u, 0xc8u, 0x8au};
    uint8_t a64_movz_x0_imm1[] = {0x20u, 0x00u, 0x80u, 0xd2u};
    uint8_t a64_ldur_w0_x0_1[] = {0x00u, 0x10u, 0x40u, 0xb8u};
    uint8_t arm_b[] = {0x00u, 0x00u, 0x00u, 0xeau};
    uint8_t arm_add_r0_imm1[] = {0x01u, 0x00u, 0x80u, 0xe2u};
    uint8_t arm_lda_r0_r1[] = {0x9fu, 0x0cu, 0x91u, 0xe1u};
    uint8_t arm_aesd[] = {0x40u, 0x03u, 0xb0u, 0xf3u};
    uint8_t thumb_add_r0_imm1[] = {0x40u, 0x1cu};
    uint8_t thumb_b_zero[] = {0x00u, 0xe0u};
    uint8_t thumb_bhs[] = {0x07u, 0xd2u};
    uint8_t thumb_bne_w[] = {0x40u, 0xf0u, 0xf8u, 0x80u};
    uint8_t thumb_trap[] = {0xfeu, 0xdeu};
    uint8_t thumb_cpsid_i[] = {0x72u, 0xb6u};
    uint8_t thumb_cpsie_i[] = {0x62u, 0xb6u};
    uint8_t thumb_mov_r0_r1[] = {0x08u, 0x46u};
    uint8_t thumb_add_w_r8_r9_imm5[] = {0x09u, 0xf1u, 0x05u, 0x08u};
    uint8_t thumb_mov_w_r2_imm80000000[] = {0x4fu, 0xf0u, 0x00u, 0x42u};
    uint8_t thumb_mov_w_r3_imm00ff00ff[] = {0x4fu, 0xf0u, 0xffu, 0x13u};
    uint8_t thumb_cbz_r1_plus16[] = {0x31u, 0xb1u};
    uint8_t thumb_cbnz_r2_plus16[] = {0x32u, 0xb9u};
    uint8_t bpf_ja[] = {0x05u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t bpf_exit[] = {0x95u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t bpf_add_ri[] = {0x07u, 0x01u, 0x00u, 0x00u, 0x05u, 0x00u, 0x00u, 0x00u};
    uint8_t bpf_jeq_ri[] = {0x15u, 0x01u, 0x02u, 0x00u, 0x2au, 0x00u, 0x00u, 0x00u};
    uint8_t bpf_ld_imm64[] = {
        0x18u, 0x01u, 0x00u, 0x00u, 0x88u, 0x77u, 0x66u, 0x55u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x44u, 0x33u, 0x22u, 0x11u
    };
    uint8_t bpf_ld_acq[] = {0xdbu, 0x12u, 0x04u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u};
    uint8_t bpf_xchg[] = {0xdbu, 0x21u, 0x08u, 0x00u, 0xe1u, 0x00u, 0x00u, 0x00u};
    uint8_t bpf_xchgw32[] = {0xc3u, 0x21u, 0x08u, 0x00u, 0xe1u, 0x00u, 0x00u, 0x00u};
    uint8_t wasm_call[] = {0x10u, 0x01u};
    uint8_t wasm_block_void[] = {0x02u, 0x40u};
    uint8_t wasm_i32x4_add[] = {0xfdu, 0xaeu, 0x01u};

    ASMKIT_CHECK(asmkit_test_x86_jcc_mnemonics() == 0);
    ASMKIT_CHECK(asmkit_test_x86_register_modrm_rm_mr_forms() == 0);
    ASMKIT_CHECK(asmkit_test_x86_setcc_mnemonics() == 0);
    ASMKIT_CHECK(asmkit_test_x86_lock_prefix() == 0);
    ASMKIT_CHECK(asmkit_test_x86_rep_string_prefixes() == 0);
    ASMKIT_CHECK(asmkit_test_x86_cmovcc_mnemonics() == 0);
    ASMKIT_CHECK(asmkit_test_x86_prefix_alias_mnemonics() == 0);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_rip, sizeof(x86_rip), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == 7u);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_RAX);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_X86_REG_RIP);
    ASMKIT_CHECK(inst.operands[1].mem.displacement == 0x12345678 && inst.operands[1].abs_target == 0x1234667fu);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_add_eax_ebx, sizeof(x86_add_eax_ebx), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_ADD);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK(inst_info->opcode == 0x01u);
    ASMKIT_CHECK(inst_info->encoding_form == ASMKIT_ENCODING_FORM_X86_MODRM_REG);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_MODRM) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_mov_rax_rbx, sizeof(x86_mov_rax_rbx), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_REX_W) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_movdqa_xmm0_xmm1, sizeof(x86_movdqa_xmm0_xmm1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOVDQA);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_MANDATORY_PREFIX) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_vzeroupper, sizeof(x86_vzeroupper), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_vzeroupper));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VZEROUPPER);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_VEX) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_vaddpd_ymm0_ymm0_ymm1, sizeof(x86_vaddpd_ymm0_ymm0_ymm1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_vaddpd_ymm0_ymm0_ymm1));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDPD);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_VEX) != 0u);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_VEX_L) != 0u);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_VEX_4V) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_vaddpd_xmm0_xmm0_xmm1_evex, sizeof(x86_vaddpd_xmm0_xmm0_xmm1_evex), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_vaddpd_xmm0_xmm0_xmm1_evex));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDPD);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_EVEX) != 0u);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_REX_W) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_vpmacsdql_xop, sizeof(x86_vpmacsdql_xop), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_vpmacsdql_xop));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VPMACSDQL);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->encoding_flags & ASMKIT_ENCODING_FLAG_X86_XOP) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_enter, sizeof(x86_enter), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_enter));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_ENTER);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_IMM && inst.operands[0].imm == 0x1234 && inst.operands[0].width == 16u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 2 && inst.operands[1].width == 8u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_movsb, sizeof(x86_movsb), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_movsb));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_STORE);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOVSB);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_MEM && inst.operands[0].width == 8u);
    ASMKIT_CHECK(inst.operands[0].mem.segment == ASMKIT_X86_REG_ES);
    ASMKIT_CHECK(inst.operands[0].mem.base == ASMKIT_X86_REG_RDI && inst.operands[0].mem.address_width == 64u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].width == 8u);
    ASMKIT_CHECK(inst.operands[1].mem.base == ASMKIT_X86_REG_RSI && inst.operands[1].mem.address_width == 64u);
    ASMKIT_CHECK(asmkit_get_instruction_operand_info(inst.arch, inst.id, 0u, &operand_info) == ASMKIT_OK);
    ASMKIT_CHECK((operand_info->flags & ASMKIT_OPERAND_FLAG_READ) != 0u);
    ASMKIT_CHECK((operand_info->flags & ASMKIT_OPERAND_FLAG_WRITE) != 0u);
    ASMKIT_CHECK(asmkit_get_instruction_operand_info(inst.arch, inst.id, 1u, &operand_info) == ASMKIT_OK);
    ASMKIT_CHECK((operand_info->flags & ASMKIT_OPERAND_FLAG_READ) != 0u);
    ASMKIT_CHECK((operand_info->flags & ASMKIT_OPERAND_FLAG_WRITE) == 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_movsq, sizeof(x86_movsq), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_movsq));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOVSQ);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_MEM && inst.operands[0].width == 64u);
    ASMKIT_CHECK(inst.operands[0].mem.segment == ASMKIT_X86_REG_ES);
    ASMKIT_CHECK(inst.operands[0].mem.base == ASMKIT_X86_REG_RDI && inst.operands[0].mem.address_width == 64u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].width == 64u);
    ASMKIT_CHECK(inst.operands[1].mem.base == ASMKIT_X86_REG_RSI && inst.operands[1].mem.address_width == 64u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_moffs64_load, sizeof(x86_moffs64_load), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_moffs64_load));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_LOAD);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_MOV);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_X86_REG_RAX && inst.operands[0].width == 64u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].width == 64u);
    ASMKIT_CHECK(inst.operands[1].mem.base == ASMKIT_X86_REG_INVALID && inst.operands[1].mem.index == ASMKIT_X86_REG_INVALID);
    ASMKIT_CHECK(inst.operands[1].mem.address_width == 64u);
    ASMKIT_CHECK((uint64_t)inst.operands[1].mem.displacement == UINT64_C(0x1122334455667788));
    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_32);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_far_jmp32, sizeof(x86_far_jmp32), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_far_jmp32));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_INDIRECT_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_JMP);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_IMM && (uint16_t)inst.operands[0].imm == 0xabcdu && inst.operands[0].width == 16u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && (uint32_t)inst.operands[1].imm == 0x12345678u && inst.operands[1].width == 32u);
    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_call_rel32_zero, sizeof(x86_call_rel32_zero), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_call_rel32_zero));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_DIRECT_CALL);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_CALL);
    ASMKIT_CHECK(inst.operand_count > 0u);
    ASMKIT_CHECK(inst.operands[0].abs_target == 0x1005u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_jcc_short_back, sizeof(x86_jcc_short_back), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(x86_jcc_short_back));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_COND_BRANCH);
    ASMKIT_CHECK(inst.operand_count > 0u);
    ASMKIT_CHECK(inst.operands[0].abs_target == 0x1000u);

    ASMKIT_CHECK(asmkit_feature_set_clear(&explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_init(&explicit_config, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_set_features(&explicit_config, &explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_vaddpd_ymm0_ymm0_ymm1, sizeof(x86_vaddpd_ymm0_ymm0_ymm1), 0x1000u, &inst) == ASMKIT_ERR_UNSUPPORTED_FEATURE);
    ASMKIT_CHECK(asmkit_engine_config_enable_feature(&explicit_config, ASMKIT_X86_FEATURE_AVX) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_vaddpd_ymm0_ymm0_ymm1, sizeof(x86_vaddpd_ymm0_ymm0_ymm1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VADDPD);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_vpmacsdql_xop, sizeof(x86_vpmacsdql_xop), 0x1000u, &inst) == ASMKIT_ERR_UNSUPPORTED_FEATURE);
    ASMKIT_CHECK(asmkit_engine_config_enable_feature(&explicit_config, ASMKIT_X86_FEATURE_XOP) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, x86_vpmacsdql_xop, sizeof(x86_vpmacsdql_xop), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_X86_VPMACSDQL);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_b, sizeof(a64_b), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_DIRECT_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_B);
    ASMKIT_CHECK(inst.operands[0].abs_target == 0x1010u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_b_ne, sizeof(a64_b_ne), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_COND_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_B);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_CONDITIONAL) != 0u);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_IMM && inst.operands[0].imm == 1);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL);
#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "b.ne") == 0);
    }
#endif
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_ret, sizeof(a64_ret), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(a64_ret));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_RETURN);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_RET);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_nop, sizeof(a64_nop), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_NOP);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_NOP);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_add_w0_imm1, sizeof(a64_add_w0_imm1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_add_w0_w1_w2_lsl3, sizeof(a64_add_w0_w1_w2_lsl3), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W2);
    ASMKIT_CHECK(inst.operands[2].shift_kind == ASMKIT_AARCH64_SHIFT_LSL && inst.operands[2].shift_amount == 3u);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_add_x0_x1_w2_uxtw3, sizeof(a64_add_x0_x1_w2_uxtw3), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_ADD);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_X0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_X1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W2);
    ASMKIT_CHECK((inst.operands[2].flags & ASMKIT_OPERAND_FLAG_AARCH64_EXTENDED_REG) != 0u);
    ASMKIT_CHECK(inst.operands[2].extend_kind == ASMKIT_AARCH64_EXTEND_UXTW && inst.operands[2].extend_amount == 3u);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_sub_w5_w6_w7_sxtb1, sizeof(a64_sub_w5_w6_w7_sxtb1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_SUB);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W5);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W6);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_W7);
    ASMKIT_CHECK((inst.operands[2].flags & ASMKIT_OPERAND_FLAG_AARCH64_EXTENDED_REG) != 0u);
    ASMKIT_CHECK(inst.operands[2].extend_kind == ASMKIT_AARCH64_EXTEND_SXTB && inst.operands[2].extend_amount == 1u);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_and_w0_w1_imm_ff, sizeof(a64_and_w0_w1_imm_ff), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_AND);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_W1);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 0xff);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_and_x2_x3_imm_ff, sizeof(a64_and_x2_x3_imm_ff), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_AND);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_X2);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_X3);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 0xff);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_and_x6_x7_x8_ror9, sizeof(a64_and_x6_x7_x8_ror9), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_AND);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_X6);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_X7);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_REG && inst.operands[2].reg == ASMKIT_AARCH64_REG_X8);
    ASMKIT_CHECK(inst.operands[2].shift_kind == ASMKIT_AARCH64_SHIFT_ROR && inst.operands[2].shift_amount == 9u);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_movz_x0_imm1, sizeof(a64_movz_x0_imm1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_MOVZ);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_ldur_w0_x0_1, sizeof(a64_ldur_w0_x0_1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_LOAD);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_AARCH64_LDUR);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_AARCH64_REG_W0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_AARCH64_REG_X0);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 1);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_bti, sizeof(a64_bti), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_BTI);

    ASMKIT_CHECK(asmkit_feature_set_clear(&explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_init(&explicit_config, ASMKIT_ARCH_AARCH64, ASMKIT_MODE_AARCH64) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_set_features(&explicit_config, &explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_bti, sizeof(a64_bti), 0x1000u, &inst) == ASMKIT_ERR_UNSUPPORTED_FEATURE);
    ASMKIT_CHECK(asmkit_engine_config_enable_feature(&explicit_config, ASMKIT_AARCH64_FEATURE_BRANCHTARGETID) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, a64_bti, sizeof(a64_bti), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_BTI);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, arm_b, sizeof(arm_b), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_DIRECT_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_B);
    ASMKIT_CHECK(inst.operands[0].abs_target == 0x1008u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, arm_add_r0_imm1, sizeof(arm_add_r0_imm1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADD);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, arm_lda_r0_r1, sizeof(arm_lda_r0_r1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_LDA);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].mem.displacement == 0 && inst.operands[1].mem.address_width == 32u);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, arm_aesd, sizeof(arm_aesd), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_AESD_8);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);

    ASMKIT_CHECK(asmkit_feature_set_clear(&explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_init(&explicit_config, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_A32) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_set_features(&explicit_config, &explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, arm_aesd, sizeof(arm_aesd), 0x1000u, &inst) == ASMKIT_ERR_UNSUPPORTED_FEATURE);
    ASMKIT_CHECK(asmkit_engine_config_enable_feature(&explicit_config, ASMKIT_ARM_FEATURE_AES) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, arm_aesd, sizeof(arm_aesd), 0x1000u, &inst) == ASMKIT_ERR_UNSUPPORTED_FEATURE);
    ASMKIT_CHECK(asmkit_engine_config_enable_feature(&explicit_config, ASMKIT_ARM_FEATURE_HASV8OPS) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, arm_aesd, sizeof(arm_aesd), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_AESD_8);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_ARM, ASMKIT_MODE_ARM_THUMB);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_add_r0_imm1, sizeof(thumb_add_r0_imm1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADD);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_b_zero, sizeof(thumb_b_zero), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(thumb_b_zero));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_DIRECT_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_B);
    ASMKIT_CHECK(inst.operand_count > 0u);
    ASMKIT_CHECK(inst.operands[0].abs_target == 0x1004u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_bhs, sizeof(thumb_bhs), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(thumb_bhs));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_COND_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_B);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_CONDITIONAL) != 0u);
    ASMKIT_CHECK(inst.operand_count > 0u);
    ASMKIT_CHECK(inst.operands[0].abs_target == 0x1012u);
#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "bhs") == 0);
    }
#endif
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_bne_w, sizeof(thumb_bne_w), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(thumb_bne_w));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_COND_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_B);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_CONDITIONAL) != 0u);
#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "bne") == 0);
    }
#endif
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_trap, sizeof(thumb_trap), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(thumb_trap));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_OTHER);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_TRAP);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_CONDITIONAL) == 0u);
#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "trap") == 0);
    }
#endif
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_cpsid_i, sizeof(thumb_cpsid_i), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(thumb_cpsid_i));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_CPS);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_IMM && inst.operands[0].imm == 1);
#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "cpsid") == 0);
    }
#endif
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_cpsie_i, sizeof(thumb_cpsie_i), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(thumb_cpsie_i));
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_CPS);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_IMM && inst.operands[0].imm == 0);
#if ASMKIT_ENABLE_TEXT
    {
        asmkit_formatter_t formatter;
        asmkit_text_result_t text_result;
        char text[32];
        ASMKIT_CHECK(asmkit_formatter_init(&formatter, 0u) == ASMKIT_OK);
        ASMKIT_CHECK(asmkit_formatter_format_inst(&formatter, &inst, text, sizeof(text), &text_result) == ASMKIT_OK);
        ASMKIT_CHECK(strcmp(text, "cpsie") == 0);
    }
#endif
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_mov_r0_r1, sizeof(thumb_mov_r0_r1), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_MOV);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_add_w_r8_r9_imm5, sizeof(thumb_add_w_r8_r9_imm5), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_ADD);
    ASMKIT_CHECK(inst.operand_count == 3u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R8);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == ASMKIT_ARM_REG_R9);
    ASMKIT_CHECK(inst.operands[2].kind == ASMKIT_OP_IMM && inst.operands[2].imm == 5);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_mov_w_r2_imm80000000, sizeof(thumb_mov_w_r2_imm80000000), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_MOV);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R2);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 0x80000000ll);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_ENCODE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_mov_w_r3_imm00ff00ff, sizeof(thumb_mov_w_r3_imm00ff00ff), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_MOV);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R3);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 0x00ff00ff);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_cbz_r1_plus16, sizeof(thumb_cbz_r1_plus16), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_COND_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_CBZ);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R1);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, thumb_cbnz_r2_plus16, sizeof(thumb_cbnz_r2_plus16), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_COND_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_ARM_CBNZ);
    ASMKIT_CHECK((inst.flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == ASMKIT_ARM_REG_R2);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_PC_REL && inst.operands[1].abs_target == 0x1010u);
    ASMKIT_CHECK(asmkit_get_instruction_info(inst.arch, inst.id, &inst_info) == ASMKIT_OK);
    ASMKIT_CHECK((inst_info->metadata_flags & ASMKIT_INSTRUCTION_META_FLAG_DECODE) != 0u);
    ASMKIT_CHECK((inst_info->flags & ASMKIT_INST_FLAG_PC_RELATIVE) != 0u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bpf_ja, sizeof(bpf_ja), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_DIRECT_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_BPF_JA);
    ASMKIT_CHECK(inst.operands[0].abs_target == 0x1010u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bpf_exit, sizeof(bpf_exit), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.size == sizeof(bpf_exit));
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_RETURN);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_BPF_EXIT);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bpf_add_ri, sizeof(bpf_add_ri), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_ALU);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_BPF_ADD);
    ASMKIT_CHECK(inst.operand_count == 2u);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == 1u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_IMM && inst.operands[1].imm == 5);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bpf_jeq_ri, sizeof(bpf_jeq_ri), 0x1000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_COND_BRANCH);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_BPF_JEQ);
    ASMKIT_CHECK(inst.operands[0].abs_target == 0x1018u);
    ASMKIT_CHECK(inst.operands[1].reg == 1u && inst.operands[2].imm == 42);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bpf_ld_imm64, sizeof(bpf_ld_imm64), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_BPF_PSEUDO);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_BPF_LDDW);
    ASMKIT_CHECK(inst.size == 16u);
    ASMKIT_CHECK(inst.operands[0].reg == 1u);
    ASMKIT_CHECK((uint64_t)inst.operands[1].imm == UINT64_C(0x1122334455667788));
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bpf_ld_acq, sizeof(bpf_ld_acq), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_LOAD);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_BPF_LDX);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_REG && inst.operands[0].reg == 2u);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_MEM && inst.operands[1].mem.base == 1u && inst.operands[1].mem.displacement == 4);
    ASMKIT_CHECK(inst.operands[1].reg == 0u && inst.operands[1].imm == 0);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bpf_xchg, sizeof(bpf_xchg), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_STORE);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_BPF_XCHG);
    ASMKIT_CHECK(inst.operands[0].kind == ASMKIT_OP_MEM && inst.operands[0].mem.base == 1u && inst.operands[0].mem.displacement == 8);
    ASMKIT_CHECK(inst.operands[0].reg == 0u && inst.operands[0].imm == 0);
    ASMKIT_CHECK(inst.operands[1].kind == ASMKIT_OP_REG && inst.operands[1].reg == 2u);

    ASMKIT_CHECK(asmkit_feature_set_clear(&explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_init(&explicit_config, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_set_features(&explicit_config, &explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bpf_xchgw32, sizeof(bpf_xchgw32), 0u, &inst) == ASMKIT_ERR_UNSUPPORTED_FEATURE);
    ASMKIT_CHECK(asmkit_engine_config_enable_feature(&explicit_config, ASMKIT_BPF_FEATURE_ALU32) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, bpf_xchgw32, sizeof(bpf_xchgw32), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_STORE);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_BPF_XCHG);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, wasm_call, sizeof(wasm_call), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_DIRECT_CALL);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_WASM_CALL);
    ASMKIT_CHECK(inst.size == 2u);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, wasm_block_void, sizeof(wasm_block_void), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.inst_class == ASMKIT_INST_WASM_BLOCK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_WASM_BLOCK);
    ASMKIT_CHECK(inst.size == sizeof(wasm_block_void));

    ASMKIT_CHECK(asmkit_feature_set_clear(&explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_init(&explicit_config, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_config_set_features(&explicit_config, &explicit_features) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, wasm_i32x4_add, sizeof(wasm_i32x4_add), 0u, &inst) == ASMKIT_ERR_UNSUPPORTED_FEATURE);
    ASMKIT_CHECK(asmkit_engine_config_enable_feature(&explicit_config, ASMKIT_WASM_FEATURE_SIMD128) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_engine_init(&engine, &explicit_config) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, wasm_i32x4_add, sizeof(wasm_i32x4_add), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(inst.mnemonic_id == ASMKIT_WASM_I32X4_ADD);

    return 0;
}

/*
 * Generated neutral instruction corpus test dispatcher.
 * Do not edit by hand.
 */

int asmkit_test_instruction_corpus_aarch64(void);
int asmkit_test_instruction_corpus_arm_a32(void);
int asmkit_test_instruction_corpus_arm_thumb(void);
int asmkit_test_instruction_corpus_bpf64(void);
int asmkit_test_instruction_corpus_wasm32(void);
int asmkit_test_instruction_corpus_x86_16(void);
int asmkit_test_instruction_corpus_x86_32(void);
int asmkit_test_instruction_corpus_x86_64(void);

int asmkit_test_instruction_corpus(void)
{
    int failures;

    failures = 0;
    failures += asmkit_test_instruction_corpus_aarch64();
    failures += asmkit_test_instruction_corpus_arm_a32();
    failures += asmkit_test_instruction_corpus_arm_thumb();
    failures += asmkit_test_instruction_corpus_bpf64();
    failures += asmkit_test_instruction_corpus_wasm32();
    failures += asmkit_test_instruction_corpus_x86_16();
    failures += asmkit_test_instruction_corpus_x86_32();
    failures += asmkit_test_instruction_corpus_x86_64();
    return failures;
}

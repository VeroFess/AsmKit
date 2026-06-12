#include <stdio.h>

int asmkit_test_decode(void);
int asmkit_test_easy_api(void);
int asmkit_test_analysis(void);
int asmkit_test_emit(void);
int asmkit_test_relocate(void);
int asmkit_test_concurrency(void);
int asmkit_test_instruction_corpus(void);

int main(void)
{
    int failures;
    failures = 0;
    failures += asmkit_test_decode();
    failures += asmkit_test_easy_api();
    failures += asmkit_test_analysis();
    failures += asmkit_test_emit();
    failures += asmkit_test_relocate();
    failures += asmkit_test_concurrency();
    failures += asmkit_test_instruction_corpus();
    if (failures != 0) {
        printf("asmkit tests failed: %d\n", failures);
        return 1;
    }
    printf("asmkit tests passed\n");
    return 0;
}

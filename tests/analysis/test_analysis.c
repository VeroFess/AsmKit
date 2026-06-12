#include "test_support.h"

int asmkit_test_analysis(void)
{
    asmkit_engine_t engine;
    asmkit_inst_t inst;
    asmkit_inst_semantics_t sem;
    uint8_t call_rel[] = {0xe8u, 0x00u, 0x00u, 0x00u, 0x00u};
    uint8_t wasm_block[] = {0x02u, 0x40u};

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, call_rel, sizeof(call_rel), 0x2000u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst(&engine, 0, &inst, &sem) == ASMKIT_OK);
    ASMKIT_CHECK(sem.branch.is_call);
    ASMKIT_CHECK(sem.branch.has_target);
    ASMKIT_CHECK(sem.branch.target == 0x2005u);
    ASMKIT_CHECK((sem.hazard_flags & ASMKIT_HAZARD_PC_RELATIVE) != 0u);

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_WASM, ASMKIT_MODE_WASM32);
    ASMKIT_CHECK(asmkit_decode_one(&engine, 0, wasm_block, sizeof(wasm_block), 0u, &inst) == ASMKIT_OK);
    ASMKIT_CHECK(asmkit_analyze_inst(&engine, 0, &inst, &sem) == ASMKIT_OK);
    ASMKIT_CHECK((sem.hazard_flags & ASMKIT_HAZARD_STRUCTURED_CF) != 0u);

    return 0;
}

#include "test_support.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct asmkit_thread_arg {
    const asmkit_engine_t* engine;
    int failed;
} asmkit_thread_arg_t;

static DWORD WINAPI asmkit_decode_worker(LPVOID param)
{
    asmkit_thread_arg_t* arg;
    uint8_t code[] = {0x90u, 0x90u, 0x90u, 0x90u};
    unsigned int i;

    arg = (asmkit_thread_arg_t*)param;
    arg->failed = 0;
    for (i = 0u; i < 1000u; ++i) {
        asmkit_inst_t inst;
        asmkit_emit_result_t emit;
        uint8_t out[16];
        if (asmkit_decode_one(arg->engine, 0, code, sizeof(code), 0x1000u, &inst) != ASMKIT_OK ||
            inst.inst_class != ASMKIT_INST_NOP ||
            asmkit_emit_jump(arg->engine, 0, 0x2000u, 0x2010u, 0, out, sizeof(out), &emit) != ASMKIT_OK ||
            emit.size != 5u) {
            arg->failed = 1;
            return 1u;
        }
    }
    return 0u;
}
#endif

int asmkit_test_concurrency(void)
{
#ifdef _WIN32
    asmkit_engine_t engine;
    asmkit_thread_arg_t args[4];
    HANDLE threads[4];
    DWORD wait_status;
    unsigned int i;

    ASMKIT_INIT_ENGINE(&engine, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64);
    for (i = 0u; i < 4u; ++i) {
        args[i].engine = &engine;
        args[i].failed = 0;
        threads[i] = CreateThread(0, 0, asmkit_decode_worker, &args[i], 0, 0);
        ASMKIT_CHECK(threads[i] != 0);
    }
    wait_status = WaitForMultipleObjects(4u, threads, TRUE, 30000u);
    ASMKIT_CHECK(wait_status == WAIT_OBJECT_0);
    for (i = 0u; i < 4u; ++i) {
        CloseHandle(threads[i]);
        ASMKIT_CHECK(args[i].failed == 0);
    }
#endif
    return 0;
}

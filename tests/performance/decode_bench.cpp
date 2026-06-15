#include <cstddef>
#include <cstdint>
#include <string>

#include <benchmark/benchmark.h>

extern "C" {
#include "asmkit/asmkit.h"
}

#if ASMKIT_BENCH_HAS_CAPSTONE
#include <capstone/capstone.h>
#endif

#if ASMKIT_BENCH_HAS_ZYDIS
#include <Zydis/Zydis.h>
#endif

#if ASMKIT_BENCH_HAS_YAXPEAX_X86
extern "C" {
struct YaxpeaxOperandResult {
    std::int64_t imm;
    std::int64_t disp;
    std::uint64_t address;
    std::uint32_t flags;
    std::uint16_t width;
    std::uint16_t reg;
    std::uint16_t base;
    std::uint16_t index;
    std::uint16_t mask;
    std::uint8_t kind;
    std::uint8_t scale;
    std::uint8_t decorator;
    std::uint8_t reserved;
};

struct YaxpeaxDecodeResult {
    YaxpeaxOperandResult operands[8];
    std::uint8_t bytes[16];
    std::uint32_t opcode;
    std::uint8_t len;
    std::uint8_t operand_count;
    std::uint16_t mem_width;
};

void* asmkit_yaxpeax_x86_create();
void asmkit_yaxpeax_x86_destroy(void* context);
int asmkit_yaxpeax_x86_64_decode_one(
    const void* context,
    const std::uint8_t* data,
    std::size_t len,
    YaxpeaxDecodeResult* out_result);
int asmkit_yaxpeax_x86_32_decode_one(
    const void* context,
    const std::uint8_t* data,
    std::size_t len,
    YaxpeaxDecodeResult* out_result);
}
#endif

namespace {

struct Workload {
    const char* name;
    asmkit_arch_t asmkit_arch;
    asmkit_mode_t asmkit_mode;
    const std::uint8_t* code;
    std::size_t code_size;
    std::uint64_t address;
#if ASMKIT_BENCH_HAS_CAPSTONE
    cs_arch capstone_arch;
    cs_mode capstone_mode;
#endif
#if ASMKIT_BENCH_HAS_ZYDIS
    bool has_zydis;
    ZydisMachineMode zydis_machine_mode;
    ZydisStackWidth zydis_stack_width;
#endif
#if ASMKIT_BENCH_HAS_YAXPEAX_X86
    bool has_yaxpeax_x86;
#endif
};

constexpr std::uint8_t kX86_64Mixed[] = {
    0x55, 0x48, 0x89, 0xe5, 0x48, 0x83, 0xec, 0x20,
    0x48, 0x8b, 0x05, 0x78, 0x56, 0x34, 0x12, 0x48,
    0x85, 0xc0, 0x74, 0x0a, 0x48, 0x8d, 0x4c, 0x24,
    0x20, 0xe8, 0x12, 0x00, 0x00, 0x00, 0xc5, 0xf8,
    0x77, 0x48, 0x83, 0xc4, 0x20, 0x5d, 0xc3, 0x0f,
    0x1f, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x55, 0x48, 0x89, 0xe5, 0x48, 0x83, 0xec, 0x20,
    0x48, 0x8b, 0x05, 0x78, 0x56, 0x34, 0x12, 0x48,
    0x85, 0xc0, 0x74, 0x0a, 0x48, 0x8d, 0x4c, 0x24,
    0x20, 0xe8, 0x12, 0x00, 0x00, 0x00, 0xc5, 0xf8,
    0x77, 0x48, 0x83, 0xc4, 0x20, 0x5d, 0xc3, 0x0f,
    0x1f, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00,
};

constexpr std::uint8_t kX86_32Mixed[] = {
    0x55, 0x8b, 0xec, 0x83, 0xec, 0x10, 0x8b, 0x45,
    0x08, 0x85, 0xc0, 0x74, 0x06, 0x8d, 0x4d, 0xfc,
    0xe8, 0x12, 0x00, 0x00, 0x00, 0x83, 0xc4, 0x10,
    0x5d, 0xc3, 0x0f, 0x1f, 0x40, 0x00,
    0x55, 0x8b, 0xec, 0x83, 0xec, 0x10, 0x8b, 0x45,
    0x08, 0x85, 0xc0, 0x74, 0x06, 0x8d, 0x4d, 0xfc,
    0xe8, 0x12, 0x00, 0x00, 0x00, 0x83, 0xc4, 0x10,
    0x5d, 0xc3, 0x0f, 0x1f, 0x40, 0x00,
};

constexpr std::uint8_t kAArch64Mixed[] = {
    0x04, 0x00, 0x00, 0x14, 0xa1, 0x03, 0x00, 0x54,
    0xc0, 0x03, 0x5f, 0xd6, 0x1f, 0x20, 0x03, 0xd5,
    0x00, 0x04, 0x00, 0x11, 0x20, 0x0c, 0x02, 0x0b,
    0x20, 0x4c, 0x22, 0x8b, 0xc5, 0x84, 0x27, 0x4b,
    0x20, 0x1c, 0x00, 0x12, 0x62, 0x1c, 0x40, 0x92,
    0xe6, 0x24, 0xc8, 0x8a, 0x20, 0x00, 0x80, 0xd2,
    0x00, 0x10, 0x40, 0xb8,
};

constexpr std::uint8_t kArmA32Mixed[] = {
    0x00, 0x00, 0x00, 0xea, 0x01, 0x00, 0x80, 0xe2,
    0x9f, 0x0c, 0x91, 0xe1,
    0x00, 0x00, 0x00, 0xea, 0x01, 0x00, 0x80, 0xe2,
    0x9f, 0x0c, 0x91, 0xe1,
};

constexpr std::uint8_t kArmThumbMixed[] = {
    0x40, 0x1c, 0x00, 0xe0, 0x07, 0xd2, 0x40, 0xf0,
    0xf8, 0x80, 0x00, 0xf0, 0x00, 0xf8, 0x18, 0xbf,
    0x08, 0x46, 0x09, 0xf1, 0x05, 0x08, 0x4f, 0xf0,
    0x00, 0x42, 0x4f, 0xf0, 0xff, 0x13, 0x31, 0xb1,
    0x32, 0xb9,
};

constexpr std::uint8_t kBpf64Mixed[] = {
    0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x95, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x01, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x15, 0x01, 0x02, 0x00, 0x2a, 0x00, 0x00, 0x00,
    0xdb, 0x12, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00,
    0xdb, 0x21, 0x08, 0x00, 0xe1, 0x00, 0x00, 0x00,
};

constexpr std::uint8_t kWasm32Mixed[] = {
    0x10, 0x01, 0x02, 0x40,
    0x10, 0x01, 0x02, 0x40,
    0x10, 0x01, 0x02, 0x40,
    0x10, 0x01, 0x02, 0x40,
};

const Workload kX86_64Workload = {
    "x86_64/mixed",
    ASMKIT_ARCH_X86,
    ASMKIT_MODE_X86_64,
    kX86_64Mixed,
    sizeof(kX86_64Mixed),
    UINT64_C(0x140001000),
#if ASMKIT_BENCH_HAS_CAPSTONE
    CS_ARCH_X86,
    CS_MODE_64,
#endif
#if ASMKIT_BENCH_HAS_ZYDIS
    true,
    ZYDIS_MACHINE_MODE_LONG_64,
    ZYDIS_STACK_WIDTH_64,
#endif
#if ASMKIT_BENCH_HAS_YAXPEAX_X86
    true,
#endif
};

const Workload kX86_32Workload = {
    "x86_32/mixed",
    ASMKIT_ARCH_X86,
    ASMKIT_MODE_X86_32,
    kX86_32Mixed,
    sizeof(kX86_32Mixed),
    UINT64_C(0x401000),
#if ASMKIT_BENCH_HAS_CAPSTONE
    CS_ARCH_X86,
    CS_MODE_32,
#endif
#if ASMKIT_BENCH_HAS_ZYDIS
    true,
    ZYDIS_MACHINE_MODE_LEGACY_32,
    ZYDIS_STACK_WIDTH_32,
#endif
#if ASMKIT_BENCH_HAS_YAXPEAX_X86
    true,
#endif
};

const Workload kAArch64Workload = {
    "aarch64/mixed",
    ASMKIT_ARCH_AARCH64,
    ASMKIT_MODE_AARCH64,
    kAArch64Mixed,
    sizeof(kAArch64Mixed),
    UINT64_C(0x1000),
#if ASMKIT_BENCH_HAS_CAPSTONE
    CS_ARCH_ARM64,
    CS_MODE_LITTLE_ENDIAN,
#endif
#if ASMKIT_BENCH_HAS_ZYDIS
    false,
    ZYDIS_MACHINE_MODE_LONG_64,
    ZYDIS_STACK_WIDTH_64,
#endif
#if ASMKIT_BENCH_HAS_YAXPEAX_X86
    false,
#endif
};

const Workload kArmA32Workload = {
    "arm/a32/mixed",
    ASMKIT_ARCH_ARM,
    ASMKIT_MODE_ARM_A32,
    kArmA32Mixed,
    sizeof(kArmA32Mixed),
    UINT64_C(0x1000),
#if ASMKIT_BENCH_HAS_CAPSTONE
    CS_ARCH_ARM,
    static_cast<cs_mode>(CS_MODE_ARM | CS_MODE_LITTLE_ENDIAN),
#endif
#if ASMKIT_BENCH_HAS_ZYDIS
    false,
    ZYDIS_MACHINE_MODE_LONG_64,
    ZYDIS_STACK_WIDTH_64,
#endif
#if ASMKIT_BENCH_HAS_YAXPEAX_X86
    false,
#endif
};

const Workload kArmThumbWorkload = {
    "arm/thumb/mixed",
    ASMKIT_ARCH_ARM,
    ASMKIT_MODE_ARM_THUMB,
    kArmThumbMixed,
    sizeof(kArmThumbMixed),
    UINT64_C(0x1000),
#if ASMKIT_BENCH_HAS_CAPSTONE
    CS_ARCH_ARM,
    static_cast<cs_mode>(CS_MODE_THUMB | CS_MODE_LITTLE_ENDIAN),
#endif
#if ASMKIT_BENCH_HAS_ZYDIS
    false,
    ZYDIS_MACHINE_MODE_LONG_64,
    ZYDIS_STACK_WIDTH_64,
#endif
#if ASMKIT_BENCH_HAS_YAXPEAX_X86
    false,
#endif
};

const Workload kBpf64Workload = {
    "bpf64/mixed",
    ASMKIT_ARCH_BPF,
    ASMKIT_MODE_BPF64,
    kBpf64Mixed,
    sizeof(kBpf64Mixed),
    UINT64_C(0x1000),
#if ASMKIT_BENCH_HAS_CAPSTONE
    CS_ARCH_BPF,
    static_cast<cs_mode>(CS_MODE_LITTLE_ENDIAN | CS_MODE_BPF_EXTENDED),
#endif
#if ASMKIT_BENCH_HAS_ZYDIS
    false,
    ZYDIS_MACHINE_MODE_LONG_64,
    ZYDIS_STACK_WIDTH_64,
#endif
#if ASMKIT_BENCH_HAS_YAXPEAX_X86
    false,
#endif
};

const Workload kWasm32Workload = {
    "wasm32/mixed",
    ASMKIT_ARCH_WASM,
    ASMKIT_MODE_WASM32,
    kWasm32Mixed,
    sizeof(kWasm32Mixed),
    0,
#if ASMKIT_BENCH_HAS_CAPSTONE
    CS_ARCH_WASM,
    CS_MODE_LITTLE_ENDIAN,
#endif
#if ASMKIT_BENCH_HAS_ZYDIS
    false,
    ZYDIS_MACHINE_MODE_LONG_64,
    ZYDIS_STACK_WIDTH_64,
#endif
#if ASMKIT_BENCH_HAS_YAXPEAX_X86
    false,
#endif
};

void SetDecodeCounters(benchmark::State& state, std::uint64_t decoded_instruction_count, std::size_t byte_count)
{
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(byte_count));
    state.SetItemsProcessed(static_cast<std::int64_t>(decoded_instruction_count));
}

void BM_AsmKitDecode(benchmark::State& state, const Workload* workload)
{
    asmkit_engine_config_t config;
    asmkit_engine_t engine;
    std::uint64_t total_instructions = 0;
    std::uint64_t checksum = 0;

    if (asmkit_engine_config_init(&config, workload->asmkit_arch, workload->asmkit_mode) != ASMKIT_OK ||
        asmkit_engine_config_enable_all_features(&config) != ASMKIT_OK ||
        asmkit_engine_init(&engine, &config) != ASMKIT_OK) {
        state.SkipWithError("asmkit engine initialization failed");
        return;
    }

    for (auto _ : state) {
        std::size_t offset = 0;
        std::uint64_t iteration_instructions = 0;
        while (offset < workload->code_size) {
            asmkit_inst_t inst;
            const asmkit_status_t status = asmkit_decode_one(
                &engine,
                nullptr,
                workload->code + offset,
                workload->code_size - offset,
                workload->address + offset,
                &inst);
            if (status != ASMKIT_OK || inst.size == 0 || offset + inst.size > workload->code_size) {
                state.SkipWithError("asmkit decode failed");
                return;
            }
            benchmark::DoNotOptimize(inst);
            checksum += static_cast<std::uint64_t>(inst.id) + inst.size + inst.operand_count;
            offset += inst.size;
            ++iteration_instructions;
        }
        total_instructions += iteration_instructions;
        benchmark::DoNotOptimize(checksum);
    }

    SetDecodeCounters(state, total_instructions, workload->code_size);
}

#if ASMKIT_BENCH_HAS_CAPSTONE
void BM_CapstoneDecodeDetail(benchmark::State& state, const Workload* workload)
{
    csh handle = 0;
    cs_insn* inst = nullptr;
    std::uint64_t total_instructions = 0;
    std::uint64_t checksum = 0;

    if (cs_open(workload->capstone_arch, workload->capstone_mode, &handle) != CS_ERR_OK) {
        state.SkipWithError("capstone initialization failed");
        return;
    }
    if (cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON) != CS_ERR_OK) {
        cs_close(&handle);
        state.SkipWithError("capstone detail mode failed");
        return;
    }
    inst = cs_malloc(handle);
    if (inst == nullptr) {
        cs_close(&handle);
        state.SkipWithError("capstone instruction allocation failed");
        return;
    }

    for (auto _ : state) {
        const std::uint8_t* code = workload->code;
        std::size_t code_size = workload->code_size;
        std::uint64_t address = workload->address;
        std::uint64_t iteration_instructions = 0;
        while (code_size > 0) {
            if (!cs_disasm_iter(handle, &code, &code_size, &address, inst)) {
                cs_free(inst, 1);
                cs_close(&handle);
                state.SkipWithError("capstone decode failed");
                return;
            }
            benchmark::DoNotOptimize(*inst);
            if (inst->detail != nullptr) {
                benchmark::DoNotOptimize(*inst->detail);
            }
            checksum += static_cast<std::uint64_t>(inst->id) + inst->size;
            if (inst->detail != nullptr) {
                switch (workload->capstone_arch) {
                case CS_ARCH_X86:
                    checksum += inst->detail->x86.op_count;
                    break;
                case CS_ARCH_ARM:
                    checksum += inst->detail->arm.op_count;
                    break;
                case CS_ARCH_ARM64:
                    checksum += inst->detail->arm64.op_count;
                    break;
                case CS_ARCH_BPF:
                    checksum += inst->detail->bpf.op_count;
                    break;
                case CS_ARCH_WASM:
                    checksum += inst->detail->wasm.op_count;
                    break;
                default:
                    break;
                }
            }
            ++iteration_instructions;
        }
        total_instructions += iteration_instructions;
        benchmark::DoNotOptimize(checksum);
    }

    cs_free(inst, 1);
    cs_close(&handle);
    SetDecodeCounters(state, total_instructions, workload->code_size);
}
#endif

#if ASMKIT_BENCH_HAS_ZYDIS
void BM_ZydisDecodeFull(benchmark::State& state, const Workload* workload)
{
    ZydisDecoder decoder;
    std::uint64_t total_instructions = 0;
    std::uint64_t checksum = 0;

    if (!ZYAN_SUCCESS(ZydisDecoderInit(&decoder, workload->zydis_machine_mode, workload->zydis_stack_width))) {
        state.SkipWithError("zydis decoder initialization failed");
        return;
    }

    for (auto _ : state) {
        std::size_t offset = 0;
        std::uint64_t iteration_instructions = 0;
        while (offset < workload->code_size) {
            ZydisDecodedInstruction inst;
            ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
            const ZyanStatus status = ZydisDecoderDecodeFull(
                &decoder,
                workload->code + offset,
                workload->code_size - offset,
                &inst,
                operands);
            if (!ZYAN_SUCCESS(status) || inst.length == 0 || offset + inst.length > workload->code_size) {
                state.SkipWithError("zydis decode failed");
                return;
            }
            benchmark::DoNotOptimize(inst);
            benchmark::DoNotOptimize(operands);
            checksum += static_cast<std::uint64_t>(inst.mnemonic) + inst.length + inst.operand_count;
            if (inst.operand_count > 0) {
                checksum += operands[0].type;
            }
            offset += inst.length;
            ++iteration_instructions;
        }
        total_instructions += iteration_instructions;
        benchmark::DoNotOptimize(checksum);
    }

    SetDecodeCounters(state, total_instructions, workload->code_size);
}
#endif

#if ASMKIT_BENCH_HAS_YAXPEAX_X86
void BM_YaxpeaxX86DecodeFull(benchmark::State& state, const Workload* workload)
{
    void* context = asmkit_yaxpeax_x86_create();
    std::uint64_t total_instructions = 0;
    std::uint64_t checksum = 0;

    if (context == nullptr) {
        state.SkipWithError("yaxpeax-x86 context initialization failed");
        return;
    }

    for (auto _ : state) {
        std::size_t offset = 0;
        std::uint64_t iteration_instructions = 0;
        while (offset < workload->code_size) {
            YaxpeaxDecodeResult inst;
            const int ok = workload->asmkit_mode == ASMKIT_MODE_X86_64
                ? asmkit_yaxpeax_x86_64_decode_one(context, workload->code + offset, workload->code_size - offset, &inst)
                : asmkit_yaxpeax_x86_32_decode_one(context, workload->code + offset, workload->code_size - offset, &inst);
            if (!ok || inst.len == 0 || offset + inst.len > workload->code_size) {
                asmkit_yaxpeax_x86_destroy(context);
                state.SkipWithError("yaxpeax-x86 decode failed");
                return;
            }
            benchmark::DoNotOptimize(inst);
            checksum += static_cast<std::uint64_t>(inst.opcode) + inst.len + inst.operand_count + inst.mem_width;
            offset += inst.len;
            ++iteration_instructions;
        }
        total_instructions += iteration_instructions;
        benchmark::DoNotOptimize(checksum);
    }

    asmkit_yaxpeax_x86_destroy(context);
    SetDecodeCounters(state, total_instructions, workload->code_size);
}
#endif

void RegisterWorkload(const Workload& workload)
{
    benchmark::RegisterBenchmark(("AsmKit/" + std::string(workload.name)).c_str(), BM_AsmKitDecode, &workload)
        ->Unit(benchmark::kNanosecond);
#if ASMKIT_BENCH_HAS_ZYDIS
    if (workload.has_zydis) {
        benchmark::RegisterBenchmark(("ZydisFull/" + std::string(workload.name)).c_str(), BM_ZydisDecodeFull, &workload)
            ->Unit(benchmark::kNanosecond);
    }
#endif
#if ASMKIT_BENCH_HAS_CAPSTONE
    benchmark::RegisterBenchmark(("CapstoneDetail/" + std::string(workload.name)).c_str(), BM_CapstoneDecodeDetail, &workload)
        ->Unit(benchmark::kNanosecond);
#endif
#if ASMKIT_BENCH_HAS_YAXPEAX_X86
    if (workload.has_yaxpeax_x86) {
        benchmark::RegisterBenchmark(("YaxpeaxFull/" + std::string(workload.name)).c_str(), BM_YaxpeaxX86DecodeFull, &workload)
            ->Unit(benchmark::kNanosecond);
    }
#endif
}

[[maybe_unused]] const bool kRegistered = []() {
    RegisterWorkload(kX86_64Workload);
    RegisterWorkload(kX86_32Workload);
    RegisterWorkload(kAArch64Workload);
    RegisterWorkload(kArmA32Workload);
    RegisterWorkload(kArmThumbWorkload);
    RegisterWorkload(kBpf64Workload);
    RegisterWorkload(kWasm32Workload);
    return true;
}();

} // namespace

# ASMKIT

ASMKIT is a C99 toolkit for multi-platform disassembly and structured assembly.

## Status

ASMKIT is pre-1.0. The public C ABI can change before `1.0.0`.

The current real-world all-export validation suite passes with zero critical
mismatches and no unresolved `detail_warning` entries in the checked reports.
It cross-checks ASMKIT disassembly against LLVM MC and Capstone over exported
functions from `ntoskrnl.exe` and Debian ELF shared libraries: `libaom`,
`libatomic`, `libavcodec`, `libcrypto`, `libdav1d`, `libdnnl`, `libffi`,
`libgcc_s`, `libgmp`, `libjpeg`, `liblz4`, `liblzma`, `libpcre2-8`,
`libpixman-1`, `libssl`, `libstdc++`, `libunwind`, `libvpx`, `libx265`,
`libzstd`, and OpenBLAS images (`libblas`, `liblapack`, `libopenblas`).

| Real-world test | Images | Exported functions | Instructions | Critical mismatches |
|---|---:|---:|---:|---:|
| x86 / i386 exports | 46 | 52,405 | 3,662,582 | 0 |
| x86-64 / amd64 exports | 48 | 61,514 | 4,289,528 | 0 |
| ARM / armhf exports | 49 | 38,033 | 2,090,927 | 0 |
| AArch64 / arm64 exports | 48 | 51,522 | 3,288,230 | 0 |
| Total | 191 | 203,474 | 13,331,267 | 0 |

## Decode performance

The benchmark in [`tests/performance`](tests/performance) measures decode
throughput only; no formatter is called. The comparison uses Zydis full decode,
Capstone detail mode, and a yaxpeax-x86 C ABI shim that returns structured
instruction data.

These numbers are from a local Release build with Clang `-O3` and full LTO,
using `--benchmark_min_time=1s --benchmark_repetitions=5`. Lower is better.

| Workload | ASMKIT | Zydis Full | Capstone Detail | yaxpeax Full |
|---|---:|---:|---:|---:|
| x86-64 mixed | 697 ns | 898 ns | 2,324 ns | 752 ns |
| x86-32 mixed | 633 ns | 809 ns | 2,080 ns | 757 ns |

For the non-x86 workloads, the same benchmark compares ASMKIT with Capstone:

| Workload | ASMKIT | Capstone Detail |
|---|---:|---:|
| AArch64 mixed | 1,053 ns | 5,313 ns |
| ARM A32 mixed | 510 ns | 624 ns |
| ARM Thumb mixed | 1,177 ns | 1,749 ns |
| BPF64 mixed | 167 ns | 871 ns |
| WASM32 mixed | 118 ns | 436 ns |

Language: [简体中文](README.zh-cn.md)

## Why this exists

ASMKIT provides a C library for tools that need to inspect, encode, or relocate
machine-code instructions without linking LLVM. Target data
is derived from LLVM target records and checked in as generated C tables.

## Features

- Disassembles x86, ARM, AArch64, BPF/eBPF, and WebAssembly byte streams.
- Assembles structured instruction records through the ASMKIT Easy encoder API.
- Emits and relocates native prologue code for targets that implement native
  detour support.
- Exposes generated target metadata through public C headers.
- Builds as a static C99 library with C++-compatible headers.
- Uses caller-provided buffers and avoids dynamic allocation.
- Provides optional mnemonic text formatting with `ASMKIT_ENABLE_TEXT=ON`.

## Requirements

- CMake 3.20 or later.
- A C99 compiler supported by the local CMake generator.
- No LLVM, Python, or C++ dependency is required to build the checked-in source.

Target support is listed in [`docs/target-support.md`](docs/target-support.md).

## Build

```sh
cmake -S . -B build -D ASMKIT_ENABLE_TESTS=ON
cmake --build build
ctest --test-dir build -C Debug --output-on-failure
```

The default build creates the static library target `asmkit` and the test
executable `asmkit_core_tests`.

Common CMake options:

| Option | Default | Description |
|---|---:|---|
| `ASMKIT_ENABLE_TESTS` | `ON` | Builds `asmkit_core_tests` and the forbidden-symbol scan. |
| `ASMKIT_ENABLE_TEXT` | `OFF` | Compiles optional mnemonic text formatting tables and code. |
| `ASMKIT_FREESTANDING` | `ON` | Defines `ASMKIT_FREESTANDING=1` for the public target. |
| `ASMKIT_ENABLE_ASAN` | `OFF` | Enables AddressSanitizer for supported compilers. |
| `ASMKIT_ENABLE_TSAN` | `OFF` | Enables ThreadSanitizer for supported Clang/GNU targets. |
| `ASMKIT_ENABLE_UBSAN` | `OFF` | Enables UndefinedBehaviorSanitizer for supported Clang/GNU targets. |
| `ASMKIT_ENABLE_BENCHMARKS` | `OFF` | Builds optional decode performance tests under `tests/performance`. |

## Quick start

The recommended entry point is the ASMKIT Easy API in
[`include/asmkit/asmkit_easy.h`](include/asmkit/asmkit_easy.h), included by
[`include/asmkit/asmkit.h`](include/asmkit/asmkit.h).

```c
#include <stdint.h>
#include "asmkit/asmkit.h"

int main(void)
{
    asmkit_decoder_t decoder;
    asmkit_inst_t inst;
    uint8_t code[] = {0x90};

    if (asmkit_decoder_init(&decoder, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64) != ASMKIT_OK) {
        return 1;
    }
    if (asmkit_decoder_decode_full(&decoder, code, sizeof(code), 0x1000u, &inst) != ASMKIT_OK) {
        return 1;
    }
    return inst.size == 1u ? 0 : 1;
}
```

CMake target:

```cmake
add_subdirectory(path/to/asmkit)
target_link_libraries(my_tool PRIVATE asmkit)
```

## Documentation

- [`docs/index.md`](docs/index.md): documentation entry point.
- [`docs/easy-api.md`](docs/easy-api.md): recommended decode, encode, branch
  emit, and formatting API.
- [`docs/api-reference.md`](docs/api-reference.md): other public C APIs.
- [`docs/target-support.md`](docs/target-support.md): target modes and current
  capability boundaries.
- [`docs/generated-data.md`](docs/generated-data.md): LLVM-derived generated
  data and modified TableGen pipeline notes.
- [`docs/execution-model.md`](docs/execution-model.md): memory ownership,
  concurrency, and freestanding behavior.
- [`docs/license.md`](docs/license.md): project and generated-file licensing.
- [`docs/acknowledgements.md`](docs/acknowledgements.md): upstream projects
  acknowledged by ASMKIT.

Simplified Chinese documentation starts at
[`README.zh-cn.md`](README.zh-cn.md).

## Development

Local validation command set:

```sh
cmake -S . -B build -D ASMKIT_ENABLE_TESTS=ON
cmake --build build
ctest --test-dir build -C Debug --output-on-failure
```

The test target includes instruction corpus tests, Easy API tests, analysis,
encode, relocation, concurrency, and a CMake scan that rejects dynamic
allocation, file I/O, and common blocking primitives.

## Acknowledgements

AsmKit target data is derived from LLVM target records. Some test corpus entries
are converted by project scripts from test material in Zydis, Capstone, and
AsmJit. See [`docs/acknowledgements.md`](docs/acknowledgements.md).

## License

Project-owned source and documentation files are licensed under the MIT
License. See [`LICENSE`](LICENSE).

LLVM-derived generated files under `src/generated/` and
`include/asmkit/target/` are marked `Apache-2.0 WITH LLVM-exception`. See
[`docs/license.md`](docs/license.md) and the LLVM license at
<https://llvm.org/LICENSE.txt>.

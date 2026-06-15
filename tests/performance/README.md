# Decode Performance Tests

This directory contains optional decode-only benchmarks for ASMKIT, Zydis,
Capstone, and yaxpeax-x86. The benchmark executable does not call ASMKIT text
formatting, Zydis formatting, any explicit Capstone text output path, or
yaxpeax display formatting. Zydis and yaxpeax workloads are registered only
for x86, while Capstone workloads cover all runtime architectures represented
by this benchmark.

The dependencies are private to this performance-test target:

- Google Benchmark is used as the benchmark framework.
- Zydis is called through its C API with `ZydisDecoderDecodeFull`.
- Capstone is called through its C API with `cs_disasm_iter` and
  `CS_OPT_DETAIL=ON`.
- yaxpeax-x86 is fetched by CMake and called through a small private Rust
  `staticlib` C ABI shim built in the benchmark build directory. The shim
  materializes yaxpeax operands and instruction bytes into structured C
  fields; it still does not use display formatting.

The benchmark keeps full decode output live under LTO with Google Benchmark's
`DoNotOptimize`; the checksum itself only tracks small progress fields and is
not intended to model text formatting or full result consumption.

All third-party benchmark dependencies are fetched by CMake `FetchContent` when
`ASMKIT_ENABLE_BENCHMARKS=ON`. They are not linked by the public `asmkit`
library target and are not required for normal runtime builds.

Recommended build:

```sh
set LLVM_BIN=<path-to-clang-llvm-bin>
cmake -S . -B build-bench -G Ninja ^
  -DCMAKE_C_COMPILER=%LLVM_BIN%\clang.exe ^
  -DCMAKE_CXX_COMPILER=%LLVM_BIN%\clang++.exe ^
  -DCMAKE_LINKER=%LLVM_BIN%\lld-link.exe ^
  -DCMAKE_AR=%LLVM_BIN%\llvm-ar.exe ^
  -DCMAKE_RANLIB=%LLVM_BIN%\llvm-ranlib.exe ^
  -DCMAKE_MT=%LLVM_BIN%\llvm-mt.exe ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_FLAGS_RELEASE="-O3 -flto=full -DNDEBUG" ^
  -DCMAKE_CXX_FLAGS_RELEASE="-O3 -flto=full -DNDEBUG" ^
  -DCMAKE_EXE_LINKER_FLAGS_RELEASE="-flto=full -fuse-ld=lld" ^
  -DASMKIT_ENABLE_TESTS=OFF ^
  -DASMKIT_ENABLE_TEXT=OFF ^
  -DASMKIT_ENABLE_BENCHMARKS=ON
cmake --build build-bench --target asmkit_decode_bench
```

Recommended run:

```sh
build-bench\tests\performance\asmkit_decode_bench.exe ^
  --benchmark_min_time=1s ^
  --benchmark_repetitions=5 ^
  --benchmark_report_aggregates_only=true ^
  --benchmark_counters_tabular=true
```

# ASMKIT

ASMKIT 是一个 C99 多平台反汇编与结构化汇编工具库。

## 状态

ASMKIT 目前是 `1.0.0` 前版本。公开 C ABI 在 `1.0.0` 前可能变化。

当前现实 all-export 验证套件已经通过，已检查报告中严重不一致为 0，且没有未解决的
`detail_warning` 条目。测试会把 ASMKIT 反汇编结果与 LLVM MC 和 Capstone
交叉对比，覆盖 `ntoskrnl.exe` 以及 Debian ELF 共享库导出函数：`libaom`、
`libatomic`、`libavcodec`、`libcrypto`、`libdav1d`、`libdnnl`、`libffi`、
`libgcc_s`、`libgmp`、`libjpeg`、`liblz4`、`liblzma`、`libpcre2-8`、
`libpixman-1`、`libssl`、`libstdc++`、`libunwind`、`libvpx`、`libx265`、
`libzstd`，以及 OpenBLAS 镜像（`libblas`、`liblapack`、`libopenblas`）。

| 现实测试 | 镜像数 | 导出函数数 | 指令数 | 严重不一致 |
|---|---:|---:|---:|---:|
| x86 / i386 exports | 46 | 52,405 | 3,662,582 | 0 |
| x86-64 / amd64 exports | 48 | 61,514 | 4,289,528 | 0 |
| ARM / armhf exports | 49 | 38,033 | 2,090,927 | 0 |
| AArch64 / arm64 exports | 48 | 51,522 | 3,288,230 | 0 |
| 总计 | 191 | 203,474 | 13,331,267 | 0 |

语言：[English](README.md)

## 项目目标

ASMKIT 为需要检查、编码或重定位机器码指令的工具提供 C 库。
目标数据来自 LLVM target records，并以生成后的 C 表随源码提交。

## 功能

- 反汇编 x86、ARM、AArch64、BPF/eBPF 和 WebAssembly 字节流。
- 通过 ASMKIT Easy encoder API 汇编结构化指令记录。
- 为实现原生 detour 支持的目标生成并重定位 prologue 代码。
- 通过公开 C 头文件暴露生成的目标元数据。
- 构建为静态 C99 库，头文件兼容 C++。
- 使用调用方提供的缓冲区，不做动态分配。
- 通过 `ASMKIT_ENABLE_TEXT=ON` 提供可选助记符文本格式化。

## 要求

- CMake 3.20 或更高版本。
- 当前 CMake generator 支持的 C99 编译器。
- 构建已提交的源码不需要 LLVM、Python 或 C++。

目标支持见 [`docs/target-support.zh-cn.md`](docs/target-support.zh-cn.md)。

## 构建

```sh
cmake -S . -B build -D ASMKIT_ENABLE_TESTS=ON
cmake --build build
ctest --test-dir build -C Debug --output-on-failure
```

默认构建会生成静态库目标 `asmkit` 和测试可执行文件 `asmkit_core_tests`。

常用 CMake 选项：

| 选项 | 默认值 | 说明 |
|---|---:|---|
| `ASMKIT_ENABLE_TESTS` | `ON` | 构建 `asmkit_core_tests` 和禁用符号扫描。 |
| `ASMKIT_ENABLE_TEXT` | `OFF` | 编译可选助记符文本格式化表和代码。 |
| `ASMKIT_FREESTANDING` | `ON` | 为公开目标定义 `ASMKIT_FREESTANDING=1`。 |
| `ASMKIT_ENABLE_ASAN` | `OFF` | 对支持的编译器启用 AddressSanitizer。 |

## 快速开始

推荐入口是 [`include/asmkit/asmkit_easy.h`](include/asmkit/asmkit_easy.h) 中的
ASMKIT Easy API。该头文件会被 [`include/asmkit/asmkit.h`](include/asmkit/asmkit.h)
包含。

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

## 文档

- [`docs/index.zh-cn.md`](docs/index.zh-cn.md)：文档入口。
- [`docs/easy-api.zh-cn.md`](docs/easy-api.zh-cn.md)：推荐的反汇编、汇编、分支生成和格式化 API。
- [`docs/api-reference.zh-cn.md`](docs/api-reference.zh-cn.md)：其他公开 C API。
- [`docs/target-support.zh-cn.md`](docs/target-support.zh-cn.md)：目标模式和当前能力边界。
- [`docs/generated-data.zh-cn.md`](docs/generated-data.zh-cn.md)：LLVM 派生生成数据和修改后的 TableGen 管线说明。
- [`docs/execution-model.zh-cn.md`](docs/execution-model.zh-cn.md)：内存所有权、并发和 freestanding 行为。
- [`docs/license.zh-cn.md`](docs/license.zh-cn.md)：项目和生成文件许可证。
- [`docs/acknowledgements.zh-cn.md`](docs/acknowledgements.zh-cn.md)：ASMKIT 致谢的上游项目。

英文文档入口见 [`README.md`](README.md)。

## 开发

本地验证命令集：

```sh
cmake -S . -B build -D ASMKIT_ENABLE_TESTS=ON
cmake --build build
ctest --test-dir build -C Debug --output-on-failure
```

测试目标包含指令语料测试、Easy API 测试、analysis、encode、relocation、
concurrency，以及一个 CMake 扫描，用于拒绝动态分配、文件 I/O
和常见阻塞原语。

## 致谢

ASMKIT 目标数据来自 LLVM target records。部分测试语料由项目脚本从 Zydis、
Capstone 和 AsmJit 的测试材料转换而来。见
[`docs/acknowledgements.zh-cn.md`](docs/acknowledgements.zh-cn.md)。

## 许可证

项目自有源码和文档文件使用 MIT License。见 [`LICENSE`](LICENSE)。

`src/generated/` 和 `include/asmkit/target/` 下 LLVM 派生的生成文件标记为
`Apache-2.0 WITH LLVM-exception`。见
[`docs/license.zh-cn.md`](docs/license.zh-cn.md) 和 LLVM 许可证
<https://llvm.org/LICENSE.txt>。

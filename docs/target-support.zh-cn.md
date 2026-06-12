# 目标支持

下表描述 AsmKit 已实现的指令目标。

## 模式

公开目标和模式枚举声明在
[`include/asmkit/asmkit_arch.h`](../include/asmkit/asmkit_arch.h)。

| 目标 | 公开模式 | 本仓库语料测试 |
|---|---|---|
| `ASMKIT_ARCH_X86` | `ASMKIT_MODE_X86_16`, `ASMKIT_MODE_X86_32`, `ASMKIT_MODE_X86_64` | x86 16-bit、32-bit、64-bit |
| `ASMKIT_ARCH_ARM` | `ASMKIT_MODE_ARM_A32`, `ASMKIT_MODE_ARM_THUMB` | ARM A32 和 Thumb |
| `ASMKIT_ARCH_AARCH64` | `ASMKIT_MODE_AARCH64` | AArch64 |
| `ASMKIT_ARCH_BPF` | `ASMKIT_MODE_BPF64` | BPF64 |
| `ASMKIT_ARCH_WASM` | `ASMKIT_MODE_WASM32`, `ASMKIT_MODE_WASM64` | Wasm32 |

`ASMKIT_MODE_DEFAULT` 会按架构映射到 x86-64、ARM A32、AArch64、BPF64 或 Wasm32。

## 能力

| 目标 | 反汇编 | 结构化汇编 | 原生分支/调用生成 | Prologue 重定位 |
|---|---|---|---|---|
| x86 | 生成的 decode 表和 x86 overlay 处理 | 生成的结构化 encode | 相对分支和 x86-64 absolute stub | copy、直接分支/调用、x86-64 RIP-relative 调整、x86-64 分支扩展 |
| ARM A32 | 生成的 decode 表和 ARM overlay 处理 | 生成的结构化 encode | 直接 `B`/`BL` 和 A32 absolute stub | copy 和直接分支/调用重定位 |
| ARM Thumb | 生成的 decode 表和 Thumb overlay 处理 | 生成的结构化 encode | 短分支生成 | 保守重定位；IT 指令是 relocation-reject hazard |
| AArch64 | 生成的 decode 表和 AArch64 overlay 处理 | 生成的结构化 encode | 直接 `B`/`BL` 和 x16 literal absolute stub | PC-relative 调整，以及越界分支的 x16 absolute 扩展 |
| BPF/eBPF | 生成的 decode 表 | 生成的结构化 encode | 不支持 | 不支持 |
| WebAssembly | 生成的 opcode stream decode | 生成的结构化 encode | 不支持 | 不支持 |

BPF 和 WebAssembly 对原生 detour 操作返回
`ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION`，因为这些目标需要当前 C ABI 之外的
embedding-specific rewriter。

## 文本格式化

文本格式化由 `ASMKIT_ENABLE_TEXT` 控制。启用后，AsmKit 可以为所有目标输出生成的
助记符名称。当前仅 BPF 操作数实现了类型化文本输出。关闭时，格式化返回
`ASMKIT_ERR_FEATURE_DISABLED`。

## 目标元数据

[`include/asmkit/target/`](../include/asmkit/target/) 下的每个公开目标头文件都包含
生成的目标本地 ID，例如助记符、寄存器、特性、谓词、CPU 和操作数类型枚举。
普通 encode/decode 代码使用这些公开目标 ID，例如 `ASMKIT_X86_ADD`、
`ASMKIT_X86_REG_RAX` 或 `ASMKIT_BPF_EXIT`。

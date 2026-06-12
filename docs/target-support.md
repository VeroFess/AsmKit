# Target support

The tables below describe instruction targets implemented by AsmKit.

## Modes

Public target and mode enums are declared in
[`include/asmkit/asmkit_arch.h`](../include/asmkit/asmkit_arch.h).

| Target | Public modes | Corpus tests in this repository |
|---|---|---|
| `ASMKIT_ARCH_X86` | `ASMKIT_MODE_X86_16`, `ASMKIT_MODE_X86_32`, `ASMKIT_MODE_X86_64` | x86 16-bit, 32-bit, and 64-bit |
| `ASMKIT_ARCH_ARM` | `ASMKIT_MODE_ARM_A32`, `ASMKIT_MODE_ARM_THUMB` | ARM A32 and Thumb |
| `ASMKIT_ARCH_AARCH64` | `ASMKIT_MODE_AARCH64` | AArch64 |
| `ASMKIT_ARCH_BPF` | `ASMKIT_MODE_BPF64` | BPF64 |
| `ASMKIT_ARCH_WASM` | `ASMKIT_MODE_WASM32`, `ASMKIT_MODE_WASM64` | Wasm32 |

`ASMKIT_MODE_DEFAULT` maps to x86-64, ARM A32, AArch64, BPF64, or Wasm32
depending on the selected architecture.

## Capabilities

| Target | Decode | Structured encode | Native branch/call emit | Prologue relocation |
|---|---|---|---|---|
| x86 | Generated decode tables plus x86 overlay handling | Generated structured encode | Relative branches and x86-64 absolute stubs | Copy, direct branch/call, x86-64 RIP-relative adjustment, and x86-64 branch expansion |
| ARM A32 | Generated decode tables plus ARM overlay handling | Generated structured encode | Direct `B`/`BL` and A32 absolute stubs | Copy and direct branch/call relocation |
| ARM Thumb | Generated decode tables plus Thumb overlay handling | Generated structured encode | Short branch emission | Conservative relocation; IT instructions are relocation-reject hazards |
| AArch64 | Generated decode tables plus AArch64 overlay handling | Generated structured encode | Direct `B`/`BL` and x16 literal absolute stubs | PC-relative adjustment and x16 absolute expansion for out-of-range branches |
| BPF/eBPF | Generated decode tables | Generated structured encode | Unsupported | Unsupported |
| WebAssembly | Generated opcode-stream decode | Generated structured encode | Unsupported | Unsupported |

BPF and WebAssembly return `ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION` for native
detour operations because those targets need an embedding-specific rewriter
outside the current C ABI.

## Text formatting

Text formatting is controlled by `ASMKIT_ENABLE_TEXT`. When enabled, AsmKit can emit
generated mnemonic names for all targets. Typed operand text is currently
implemented for BPF operands. When disabled, formatting returns
`ASMKIT_ERR_FEATURE_DISABLED`.

## Target metadata

Each public target header under [`include/asmkit/target/`](../include/asmkit/target/)
contains generated target-local IDs such as mnemonic, register, feature,
predicate, CPU, and operand-type enums. Normal encode/decode code uses those
public target IDs, for example `ASMKIT_X86_ADD`, `ASMKIT_X86_REG_RAX`, or
`ASMKIT_BPF_EXIT`.

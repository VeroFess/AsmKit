# Execution model

AsmKit is a C99 static library. It does not depend on LLVM or a C++ runtime.

## Memory ownership

AsmKit writes into caller-provided output buffers. It does not allocate or free
memory.

`asmkit_engine_t` is initialized by value and is immutable after
`asmkit_engine_init` succeeds. It can be shared by concurrent callers after
initialization.

`asmkit_workspace_t` is caller-owned mutable scratch state. Concurrent calls use
distinct workspaces. APIs that do not need scratch accept `workspace == NULL`.

## Freestanding build

`ASMKIT_FREESTANDING` defaults to `ON` in
[`CMakeLists.txt`](../CMakeLists.txt). The test suite includes a CMake scan that
rejects allocation, file I/O, `printf`-style formatting, and common blocking
primitive names in AsmKit sources.

Text formatting is optional and controlled by `ASMKIT_ENABLE_TEXT`. When text is
disabled, `asmkit_format_inst` and `asmkit_formatter_format_inst` return
`ASMKIT_ERR_FEATURE_DISABLED`.

## Bounds

Public fixed bounds include:

| Constant | Value | Use |
|---|---:|---|
| `ASMKIT_MAX_INST_BYTES` | `16` | Maximum bytes stored in `asmkit_inst_t`. |
| `ASMKIT_MAX_OPERANDS` | `8` | Maximum operands stored in `asmkit_inst_t`. |
| `ASMKIT_MAX_PREFIX_BYTES` | `16` | Prefix storage bound. |
| `ASMKIT_MAX_BLOCK_INSTRUCTIONS` | `32` | Block decode instruction bound. |

Metadata lookup functions return pointers into immutable generated tables.
Name-based metadata lookups are bounded scans over those generated tables.

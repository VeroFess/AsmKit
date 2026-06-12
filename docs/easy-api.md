# AsmKit Easy API

The AsmKit Easy API is the recommended public entry point for normal decode,
structured assembly, branch emission, and optional formatting. It is declared in
[`include/asmkit/asmkit_easy.h`](../include/asmkit/asmkit_easy.h) and included by
[`include/asmkit/asmkit.h`](../include/asmkit/asmkit.h).

## Include files

```c
#include "asmkit/asmkit.h"
```

Target-local mnemonic, register, feature, CPU, predicate, and operand-type enum
values live in target headers.

```c
#include "asmkit/target/x86.h"
#include "asmkit/target/bpf.h"
```

## Decode one instruction

```c
asmkit_decoder_t decoder;
asmkit_inst_t inst;
uint8_t code[] = {0x90};

asmkit_status_t status = asmkit_decoder_init(
    &decoder,
    ASMKIT_ARCH_X86,
    ASMKIT_MODE_X86_64);
if (status == ASMKIT_OK) {
    status = asmkit_decoder_decode_full(
        &decoder,
        code,
        sizeof(code),
        0x1000u,
        &inst);
}
```

`asmkit_decoder_decode_full` writes the decoded instruction into `out_inst`.
The `address` argument is the runtime address used for PC-relative targets.

## Decode a block

```c
asmkit_inst_t insts[ASMKIT_MAX_BLOCK_INSTRUCTIONS];
asmkit_decode_block_result_t result;

asmkit_status_t status = asmkit_decoder_decode_block_until(
    &decoder,
    code,
    code_size,
    address,
    5u,
    insts,
    ASMKIT_MAX_BLOCK_INSTRUCTIONS,
    &result);
```

The block decoder stops when it reaches `min_size`, runs out of input, fills the
caller-provided instruction array, or hits the internal
`ASMKIT_MAX_BLOCK_INSTRUCTIONS` bound.

## One-shot decode

```c
asmkit_inst_t inst;

asmkit_status_t status = asmkit_decode_full(
    ASMKIT_ARCH_BPF,
    ASMKIT_MODE_BPF64,
    code,
    code_size,
    address,
    &inst);
```

## Encode a structured instruction

Encoding input is a structured `asmkit_inst_t`.

```c
#include "asmkit/target/bpf.h"

asmkit_encoder_t encoder;
asmkit_inst_t inst;
asmkit_encode_result_t result;
uint8_t out[16];

asmkit_inst_init(&inst, ASMKIT_ARCH_BPF, ASMKIT_MODE_BPF64, ASMKIT_BPF_EXIT);

asmkit_status_t status = asmkit_encoder_init(
    &encoder,
    ASMKIT_ARCH_BPF,
    ASMKIT_MODE_BPF64);
if (status == ASMKIT_OK) {
    status = asmkit_encoder_encode_inst(
        &encoder,
        &inst,
        out,
        sizeof(out),
        &result);
}
```

`result.size` contains the number of bytes written when the call returns
`ASMKIT_OK`.

## Set operands

Operand helper examples from
[`include/asmkit/asmkit_decode.h`](../include/asmkit/asmkit_decode.h):

```c
#include "asmkit/target/x86.h"

asmkit_inst_t inst;
asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_ADD);
asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u));
asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_EBX, 32u));
asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_ECX, 32u));
```

## Emit a native branch or call

```c
asmkit_encoder_t encoder;
asmkit_emit_result_t result;
uint8_t out[32];

asmkit_status_t status = asmkit_encoder_init(
    &encoder,
    ASMKIT_ARCH_X86,
    ASMKIT_MODE_X86_64);
if (status == ASMKIT_OK) {
    status = asmkit_encoder_emit_jump(
        &encoder,
        0x1000u,
        0x1010u,
        0,
        out,
        sizeof(out),
        &result);
}
```

Native branch and call emission is target-dependent. BPF and WebAssembly return
`ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION` for native detour operations.

## Format instruction text

Text formatting is disabled by default. Build with `ASMKIT_ENABLE_TEXT=ON` to
enable it.

```c
asmkit_formatter_t formatter;
asmkit_text_result_t result;
char text[64];

asmkit_formatter_init(&formatter, 0u);
asmkit_status_t status = asmkit_formatter_format_inst(
    &formatter,
    &inst,
    text,
    sizeof(text),
    &result);
```

When text support is disabled, formatting returns `ASMKIT_ERR_FEATURE_DISABLED`.
When text support is enabled, mnemonic names are available for all targets.
Typed operand text is currently implemented for BPF operands.

## Status handling

Every Easy API operation returns `asmkit_status_t`. Use
`asmkit_status_string(status)` for a stable diagnostic string.

```c
if (status != ASMKIT_OK) {
    const char* message = asmkit_status_string(status);
    (void)message;
}
```

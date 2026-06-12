# AsmKit Easy API

AsmKit Easy API 是普通反汇编、结构化汇编、分支生成和可选格式化的推荐公开入口。
它声明在 [`include/asmkit/asmkit_easy.h`](../include/asmkit/asmkit_easy.h)，并由
[`include/asmkit/asmkit.h`](../include/asmkit/asmkit.h) 包含。

## 头文件

```c
#include "asmkit/asmkit.h"
```

目标本地助记符、寄存器、特性、CPU、谓词和操作数类型枚举值位于目标头文件。

```c
#include "asmkit/target/x86.h"
#include "asmkit/target/bpf.h"
```

## 反汇编一条指令

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

`asmkit_decoder_decode_full` 会把反汇编结果写入 `out_inst`。`address`
参数是用于解析 PC-relative 目标的运行时地址。

## 反汇编代码块

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

代码块反汇编会在达到 `min_size`、输入耗尽、调用方指令数组写满，或达到内部
`ASMKIT_MAX_BLOCK_INSTRUCTIONS` 边界时停止。

## 一次性反汇编

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

## 汇编结构化指令

编码输入是结构化的 `asmkit_inst_t`。

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

调用返回 `ASMKIT_OK` 时，`result.size` 是写入字节数。

## 设置操作数

[`include/asmkit/asmkit_decode.h`](../include/asmkit/asmkit_decode.h) 中的操作数 helper 示例：

```c
#include "asmkit/target/x86.h"

asmkit_inst_t inst;
asmkit_inst_init(&inst, ASMKIT_ARCH_X86, ASMKIT_MODE_X86_64, ASMKIT_X86_ADD);
asmkit_inst_set_operand(&inst, 0u, asmkit_operand_reg(ASMKIT_X86_REG_EAX, 32u));
asmkit_inst_set_operand(&inst, 1u, asmkit_operand_reg(ASMKIT_X86_REG_EBX, 32u));
asmkit_inst_set_operand(&inst, 2u, asmkit_operand_reg(ASMKIT_X86_REG_ECX, 32u));
```

## 生成原生分支或调用

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

原生分支和调用生成取决于目标。BPF 和 WebAssembly 对原生 detour 操作返回
`ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION`。

## 格式化指令文本

文本格式化默认关闭。构建时设置 `ASMKIT_ENABLE_TEXT=ON` 可启用。

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

未启用文本支持时，格式化返回 `ASMKIT_ERR_FEATURE_DISABLED`。启用文本支持后，
所有目标都有助记符名称。当前仅 BPF 操作数实现了类型化文本输出。

## 状态处理

每个 Easy API 操作都返回 `asmkit_status_t`。使用
`asmkit_status_string(status)` 获取稳定诊断字符串。

```c
if (status != ASMKIT_OK) {
    const char* message = asmkit_status_string(status);
    (void)message;
}
```

# 公开 API 参考

多数调用方应优先使用 [`easy-api.zh-cn.md`](easy-api.zh-cn.md) 中的
AsmKit Easy API。本文列出 [`include/asmkit/`](../include/asmkit/) 下头文件暴露的其他公开 C API。

公开 API 指 `include/asmkit/` 中的声明。`src/` 下的文件都是实现细节。

## 目标和模式

[`asmkit_arch.h`](../include/asmkit/asmkit_arch.h) 声明：

- `asmkit_arch_t`: `ASMKIT_ARCH_X86`, `ASMKIT_ARCH_ARM`, `ASMKIT_ARCH_AARCH64`,
  `ASMKIT_ARCH_BPF`, `ASMKIT_ARCH_WASM`。
- `asmkit_mode_t`: `ASMKIT_MODE_X86_16`, `ASMKIT_MODE_X86_32`,
  `ASMKIT_MODE_X86_64`, `ASMKIT_MODE_ARM_A32`, `ASMKIT_MODE_ARM_THUMB`,
  `ASMKIT_MODE_AARCH64`, `ASMKIT_MODE_BPF64`, `ASMKIT_MODE_WASM32`,
  `ASMKIT_MODE_WASM64`, `ASMKIT_MODE_DEFAULT`。

## Engine 和特性配置

[`asmkit.h`](../include/asmkit/asmkit.h) 声明低层 engine 和特性配置 API。

| API | 用途 |
|---|---|
| `asmkit_engine_config_init` | 用架构和模式初始化 `asmkit_engine_config_t`。 |
| `asmkit_engine_config_set_features` | 应用调用方构造的 `asmkit_feature_set_t`。 |
| `asmkit_engine_config_enable_feature` | 启用一个生成的 feature ID。 |
| `asmkit_engine_config_enable_cpu_features` | 应用一个生成 CPU ID 对应的生成特性。 |
| `asmkit_engine_config_enable_all_features` | 启用所有生成的目标特性。 |
| `asmkit_engine_init` | 初始化不可变的 `asmkit_engine_t`。 |
| `asmkit_workspace_required_size` | 返回所需 workspace 字节数。当前 AsmKit 返回 `0`。 |
| `asmkit_workspace_init` | 初始化调用方拥有的可变 workspace 存储。 |
| `asmkit_workspace_reset` | 重置调用方拥有的 workspace 状态。 |

除非提供显式特性，engine 默认启用所有目标特性。

## 反汇编数据结构

[`asmkit_decode.h`](../include/asmkit/asmkit_decode.h) 声明结构化指令模型：

- `asmkit_inst_t`：反汇编得到或调用方构造的指令记录。
- `asmkit_operand_t`：寄存器、立即数、内存、PC-relative、WebAssembly 和目标特定操作数形式。
- `asmkit_inst_class_t`：分支、调用、返回、load、store、ALU 等宽分类。
- `ASMKIT_MAX_INST_BYTES`, `ASMKIT_MAX_OPERANDS`,
  `ASMKIT_MAX_BLOCK_INSTRUCTIONS`：固定 AsmKit 边界。

Helper 函数：

| Helper | 用途 |
|---|---|
| `asmkit_inst_init` | 初始化结构化指令记录。 |
| `asmkit_inst_set_operand` | 设置一个操作数并更新 `operand_count`。 |
| `asmkit_operand_reg` | 构造寄存器操作数。 |
| `asmkit_operand_imm` | 构造立即数操作数。 |
| `asmkit_operand_mem` / `asmkit_operand_mem_full` | 构造内存操作数。 |
| `asmkit_operand_branch_target` | 构造绝对 PC-relative 分支目标操作数。 |

低层反汇编函数：

| API | 用途 |
|---|---|
| `asmkit_decode_one` | 使用显式 engine 和 workspace 反汇编一条指令。 |
| `asmkit_decode_block_until` | 使用显式 engine 和 workspace 反汇编有界代码块。 |

## 汇编和分支生成

[`asmkit_encode.h`](../include/asmkit/asmkit_encode.h) 声明结构化二进制编码和原生
分支/调用生成。

| API | 用途 |
|---|---|
| `asmkit_encode_inst` | 使用显式 engine 和 workspace 编码结构化 `asmkit_inst_t`。 |
| `asmkit_emit_jump` | 在目标支持时生成原生 jump 序列。 |
| `asmkit_emit_call` | 在目标支持时生成原生 call 序列。 |

`asmkit_emit_options_t` 包含 `asmkit_branch_mode_t` 和允许 clobber 的掩码。
`asmkit_encode_result_t` 与 `asmkit_emit_result_t` 报告生成大小和 clobber。

## 分析和重定位

[`asmkit_analysis.h`](../include/asmkit/asmkit_analysis.h) 声明
`asmkit_analyze_inst`，通过 `asmkit_inst_semantics_t` 报告分支、PC-relative、
重定位和 hazard 信息。

[`asmkit_relocate.h`](../include/asmkit/asmkit_relocate.h) 声明：

| API | 用途 |
|---|---|
| `asmkit_plan_detour` | 规划原生 detour 的最小覆盖大小和分支策略。 |
| `asmkit_relocate_prologue` | 把有界原生 prologue 重定位到调用方提供的输出缓冲区。 |

## 元数据

[`asmkit_metadata.h`](../include/asmkit/asmkit_metadata.h) 暴露不可变的生成元数据表。
返回的指针归 AsmKit 所有，不得释放或修改。

元数据分组：

- 寄存器。
- 指令。
- 操作数和操作数类型。
- 隐式寄存器读写效果。
- 谓词。
- 特性。
- CPU model 和 CPU feature 关系。

每个分组都有 count 和 lookup API，例如 `asmkit_get_register_count`、
`asmkit_get_register_info_by_index`、`asmkit_get_register_info`、
`asmkit_find_register_by_name`。指令操作数 schema 和隐式寄存器效果通过
`asmkit_get_instruction_operand_info` 与
`asmkit_get_instruction_register_effect` 暴露。

## BPF opcode-field 元数据

[`asmkit_bpf.h`](../include/asmkit/asmkit_bpf.h) 暴露类型化 BPF opcode-field
literal 元数据。使用：

- `asmkit_bpf_get_opcode_field_count`。
- `asmkit_bpf_get_opcode_field_info_by_index`。
- `asmkit_bpf_find_opcode_field_by_name`。
- `asmkit_bpf_find_opcode_field_by_value`。

## 文本格式化

[`asmkit_text.h`](../include/asmkit/asmkit_text.h) 声明 `asmkit_format_inst`。
除非构建时设置 `ASMKIT_ENABLE_TEXT=ON`，否则格式化关闭。关闭时函数返回
`ASMKIT_ERR_FEATURE_DISABLED`。

## 状态码

[`asmkit_error.h`](../include/asmkit/asmkit_error.h) 声明 `asmkit_status_t` 和
`asmkit_status_string`。

| 状态 | 含义 |
|---|---|
| `ASMKIT_OK` | 操作成功。 |
| `ASMKIT_ERR_INVALID_ARGUMENT` | 必需指针或参数无效。 |
| `ASMKIT_ERR_UNSUPPORTED_ARCH` | 操作不支持该架构。 |
| `ASMKIT_ERR_UNSUPPORTED_MODE` | 不支持该架构/模式组合。 |
| `ASMKIT_ERR_UNSUPPORTED_FEATURE` | 生成的 feature ID 超出支持范围。 |
| `ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION` | 目标未实现请求的操作。 |
| `ASMKIT_ERR_DECODE_FAILED` | 输入字节无法反汇编为受支持指令。 |
| `ASMKIT_ERR_ENCODE_FAILED` | 结构化指令无法编码。 |
| `ASMKIT_ERR_OUTPUT_TOO_SMALL` | 调用方输出缓冲区过小。 |
| `ASMKIT_ERR_WORKSPACE_TOO_SMALL` | 调用方 workspace 过小。 |
| `ASMKIT_ERR_UNSUPPORTED_INSTRUCTION` | 操作不支持该指令。 |
| `ASMKIT_ERR_UNSUPPORTED_RELOCATION` | 指令或上下文不支持重定位。 |
| `ASMKIT_ERR_BRANCH_OUT_OF_RANGE` | 分支目标超出可编码范围。 |
| `ASMKIT_ERR_REQUIRES_BRANCH_ISLAND` | 操作需要 nearby branch island。 |
| `ASMKIT_ERR_FORBIDDEN_CLOBBER` | 所需序列会 clobber 调用方未允许的寄存器。 |
| `ASMKIT_ERR_FEATURE_DISABLED` | 功能在编译时关闭。 |
| `ASMKIT_ERR_INTERNAL` | 内部不变量失败。 |

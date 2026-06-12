# Public API reference

The recommended API for most callers is the AsmKit Easy API in
[`easy-api.md`](easy-api.md). This page lists the other public C APIs exposed by
headers under [`include/asmkit/`](../include/asmkit/).

Public API means declarations in `include/asmkit/`. Files under `src/` are
implementation details.

## Targets and modes

[`asmkit_arch.h`](../include/asmkit/asmkit_arch.h) declares:

- `asmkit_arch_t`: `ASMKIT_ARCH_X86`, `ASMKIT_ARCH_ARM`, `ASMKIT_ARCH_AARCH64`,
  `ASMKIT_ARCH_BPF`, `ASMKIT_ARCH_WASM`.
- `asmkit_mode_t`: `ASMKIT_MODE_X86_16`, `ASMKIT_MODE_X86_32`,
  `ASMKIT_MODE_X86_64`, `ASMKIT_MODE_ARM_A32`, `ASMKIT_MODE_ARM_THUMB`,
  `ASMKIT_MODE_AARCH64`, `ASMKIT_MODE_BPF64`, `ASMKIT_MODE_WASM32`,
  `ASMKIT_MODE_WASM64`, and `ASMKIT_MODE_DEFAULT`.

## Engine and feature configuration

[`asmkit.h`](../include/asmkit/asmkit.h) declares the lower-level engine and feature
configuration APIs.

| API | Use |
|---|---|
| `asmkit_engine_config_init` | Initialize `asmkit_engine_config_t` with an architecture and mode. |
| `asmkit_engine_config_set_features` | Apply a caller-built `asmkit_feature_set_t`. |
| `asmkit_engine_config_enable_feature` | Enable one generated feature ID. |
| `asmkit_engine_config_enable_cpu_features` | Apply generated features for one generated CPU ID. |
| `asmkit_engine_config_enable_all_features` | Enable all generated target features. |
| `asmkit_engine_init` | Initialize an immutable `asmkit_engine_t`. |
| `asmkit_workspace_required_size` | Return required workspace bytes. Current AsmKit returns `0`. |
| `asmkit_workspace_init` | Initialize caller-owned mutable workspace storage. |
| `asmkit_workspace_reset` | Reset caller-owned workspace state. |

Engines default to all target features unless explicit features are supplied.

## Decode data structures

[`asmkit_decode.h`](../include/asmkit/asmkit_decode.h) declares the structured
instruction model:

- `asmkit_inst_t`: decoded or caller-constructed instruction record.
- `asmkit_operand_t`: register, immediate, memory, PC-relative, WebAssembly, and
  target-specific operand forms.
- `asmkit_inst_class_t`: broad instruction class such as branch, call, return,
  load, store, or ALU.
- `ASMKIT_MAX_INST_BYTES`, `ASMKIT_MAX_OPERANDS`, and
  `ASMKIT_MAX_BLOCK_INSTRUCTIONS`: fixed AsmKit bounds.

Helper functions:

| Helper | Use |
|---|---|
| `asmkit_inst_init` | Initialize a structured instruction record. |
| `asmkit_inst_set_operand` | Set one operand and update `operand_count`. |
| `asmkit_operand_reg` | Build a register operand. |
| `asmkit_operand_imm` | Build an immediate operand. |
| `asmkit_operand_mem` / `asmkit_operand_mem_full` | Build memory operands. |
| `asmkit_operand_branch_target` | Build an absolute PC-relative branch target operand. |

Lower-level decode functions:

| API | Use |
|---|---|
| `asmkit_decode_one` | Decode one instruction with an explicit engine and workspace. |
| `asmkit_decode_block_until` | Decode a bounded block with explicit engine and workspace. |

## Encode and branch emission

[`asmkit_encode.h`](../include/asmkit/asmkit_encode.h) declares structured binary
encoding and native branch/call emission.

| API | Use |
|---|---|
| `asmkit_encode_inst` | Encode a structured `asmkit_inst_t` with an explicit engine and workspace. |
| `asmkit_emit_jump` | Emit a native jump sequence when the target supports it. |
| `asmkit_emit_call` | Emit a native call sequence when the target supports it. |

`asmkit_emit_options_t` contains `asmkit_branch_mode_t` and allowed clobber masks.
`asmkit_encode_result_t` and `asmkit_emit_result_t` report emitted sizes and
clobbers.

## Analysis and relocation

[`asmkit_analysis.h`](../include/asmkit/asmkit_analysis.h) declares
`asmkit_analyze_inst`, which reports branch, PC-relative, relocation, and hazard
information in `asmkit_inst_semantics_t`.

[`asmkit_relocate.h`](../include/asmkit/asmkit_relocate.h) declares:

| API | Use |
|---|---|
| `asmkit_plan_detour` | Plan the minimum overwrite size and branch strategy for a native detour. |
| `asmkit_relocate_prologue` | Relocate a bounded native prologue into caller-provided output. |

## Metadata

[`asmkit_metadata.h`](../include/asmkit/asmkit_metadata.h) exposes immutable generated
metadata tables. Returned pointers are owned by AsmKit and must not be freed or
mutated.

Metadata groups:

- Registers.
- Instructions.
- Operands and operand types.
- Implicit register effects.
- Predicates.
- Features.
- CPU models and CPU feature relationships.

Each group has count and lookup APIs such as `asmkit_get_register_count`,
`asmkit_get_register_info_by_index`, `asmkit_get_register_info`, and
`asmkit_find_register_by_name`. Instruction operand schemas and implicit register
effects are exposed through `asmkit_get_instruction_operand_info` and
`asmkit_get_instruction_register_effect`.

## BPF opcode-field metadata

[`asmkit_bpf.h`](../include/asmkit/asmkit_bpf.h) exposes typed BPF opcode-field
literal metadata. Use:

- `asmkit_bpf_get_opcode_field_count`.
- `asmkit_bpf_get_opcode_field_info_by_index`.
- `asmkit_bpf_find_opcode_field_by_name`.
- `asmkit_bpf_find_opcode_field_by_value`.

## Text formatting

[`asmkit_text.h`](../include/asmkit/asmkit_text.h) declares `asmkit_format_inst`.
Formatting is disabled unless the library is built with `ASMKIT_ENABLE_TEXT=ON`.
When disabled, the function returns `ASMKIT_ERR_FEATURE_DISABLED`.

## Status codes

[`asmkit_error.h`](../include/asmkit/asmkit_error.h) declares `asmkit_status_t` and
`asmkit_status_string`.

| Status | Meaning |
|---|---|
| `ASMKIT_OK` | Operation succeeded. |
| `ASMKIT_ERR_INVALID_ARGUMENT` | A required pointer or argument is invalid. |
| `ASMKIT_ERR_UNSUPPORTED_ARCH` | The architecture is not supported by the operation. |
| `ASMKIT_ERR_UNSUPPORTED_MODE` | The architecture/mode combination is not supported. |
| `ASMKIT_ERR_UNSUPPORTED_FEATURE` | A generated feature ID is outside the supported range. |
| `ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION` | The target does not implement the requested operation. |
| `ASMKIT_ERR_DECODE_FAILED` | The input bytes did not decode as a supported instruction. |
| `ASMKIT_ERR_ENCODE_FAILED` | The structured instruction could not be encoded. |
| `ASMKIT_ERR_OUTPUT_TOO_SMALL` | The caller output buffer is too small. |
| `ASMKIT_ERR_WORKSPACE_TOO_SMALL` | The caller workspace is too small. |
| `ASMKIT_ERR_UNSUPPORTED_INSTRUCTION` | The instruction is not supported by the operation. |
| `ASMKIT_ERR_UNSUPPORTED_RELOCATION` | Relocation is not supported for the instruction or context. |
| `ASMKIT_ERR_BRANCH_OUT_OF_RANGE` | A branch target is outside the encodable range. |
| `ASMKIT_ERR_REQUIRES_BRANCH_ISLAND` | The operation needs a near branch island. |
| `ASMKIT_ERR_FORBIDDEN_CLOBBER` | The required sequence clobbers a register the caller did not allow. |
| `ASMKIT_ERR_FEATURE_DISABLED` | The feature is compiled out. |
| `ASMKIT_ERR_INTERNAL` | Internal invariant failure. |

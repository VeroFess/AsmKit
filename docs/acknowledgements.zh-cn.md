# 致谢

AsmKit 使用了以下公开项目的成果。

## LLVM

AsmKit 目标数据来自 LLVM target records。仓库中提交的生成文件由项目修改后的
TableGen-based 生成器 `asmkit-tblgen` 生成，并标记为
`Apache-2.0 WITH LLVM-exception`。

- 项目：<https://llvm.org/>
- 许可证文本：<https://llvm.org/LICENSE.txt>

## Zydis

部分指令语料测试由项目脚本从 Zydis 的测试材料转换而来。

- 项目：<https://zydis.re/>
- 仓库：<https://github.com/zyantific/zydis>

## Capstone

部分指令语料测试由项目脚本从 Capstone 的测试材料转换而来。

- 项目：<https://www.capstone-engine.org/>
- 仓库：<https://github.com/capstone-engine/capstone>

## AsmJit

部分指令语料测试由项目脚本从 AsmJit 的测试材料转换而来。

- 项目：<https://asmjit.com/>
- 仓库：<https://github.com/asmjit/asmjit>

## 说明

转换后的测试用于验证 AsmKit 行为。它们不是上游项目的 API 契约。

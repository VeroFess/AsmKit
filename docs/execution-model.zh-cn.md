# 执行模型

AsmKit 是 C99 静态库。它不依赖 LLVM，也不需要 C++ runtime。

## 内存所有权

AsmKit 写入调用方提供的输出缓冲区。它不分配或释放内存。

`asmkit_engine_t` 按值初始化。`asmkit_engine_init` 成功后它不可变，可在初始化后被并发
调用方共享。

`asmkit_workspace_t` 是调用方拥有的可变 scratch 状态。并发调用使用不同 workspace。
不需要 scratch 的 API 接受 `workspace == NULL`。

## Freestanding 构建

[`CMakeLists.txt`](../CMakeLists.txt) 中 `ASMKIT_FREESTANDING` 默认为 `ON`。
测试包含一个 CMake 扫描，用于拒绝 AsmKit 源码中的动态分配、文件 I/O、`printf`
风格格式化和常见阻塞原语名称。

文本格式化是可选功能，由 `ASMKIT_ENABLE_TEXT` 控制。关闭文本时，
`asmkit_format_inst` 和 `asmkit_formatter_format_inst` 返回
`ASMKIT_ERR_FEATURE_DISABLED`。

## 边界

公开固定边界包括：

| 常量 | 值 | 用途 |
|---|---:|---|
| `ASMKIT_MAX_INST_BYTES` | `16` | `asmkit_inst_t` 中保存的最大字节数。 |
| `ASMKIT_MAX_OPERANDS` | `8` | `asmkit_inst_t` 中保存的最大操作数数。 |
| `ASMKIT_MAX_PREFIX_BYTES` | `16` | prefix 存储边界。 |
| `ASMKIT_MAX_BLOCK_INSTRUCTIONS` | `32` | 代码块反汇编指令数边界。 |

元数据 lookup 函数返回指向不可变生成表的指针。基于名称的元数据 lookup 是对生成表的
有界扫描。

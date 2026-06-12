#ifndef ASMKIT_ARCH_H
#define ASMKIT_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum asmkit_arch {
    ASMKIT_ARCH_X86,
    ASMKIT_ARCH_ARM,
    ASMKIT_ARCH_AARCH64,
    ASMKIT_ARCH_BPF,
    ASMKIT_ARCH_WASM
} asmkit_arch_t;

typedef enum asmkit_mode {
    ASMKIT_MODE_DEFAULT = 0,
    ASMKIT_MODE_X86_16,
    ASMKIT_MODE_X86_32,
    ASMKIT_MODE_X86_64,
    ASMKIT_MODE_ARM_A32,
    ASMKIT_MODE_ARM_THUMB,
    ASMKIT_MODE_AARCH64,
    ASMKIT_MODE_BPF64,
    ASMKIT_MODE_WASM32,
    ASMKIT_MODE_WASM64
} asmkit_mode_t;

#ifdef __cplusplus
}
#endif

#endif

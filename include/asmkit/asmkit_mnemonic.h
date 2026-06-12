#ifndef ASMKIT_MNEMONIC_H
#define ASMKIT_MNEMONIC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t asmkit_mnemonic_id_t;

#define ASMKIT_MNEMONIC_INVALID 0u
#define ASMKIT_MNEMONIC_X86_BASE UINT32_C(0x01000000)
#define ASMKIT_MNEMONIC_ARM_BASE UINT32_C(0x02000000)
#define ASMKIT_MNEMONIC_AARCH64_BASE UINT32_C(0x03000000)
#define ASMKIT_MNEMONIC_BPF_BASE UINT32_C(0x04000000)
#define ASMKIT_MNEMONIC_WASM_BASE UINT32_C(0x05000000)

#ifdef __cplusplus
}
#endif

#endif

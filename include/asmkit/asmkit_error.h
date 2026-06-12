#ifndef ASMKIT_ERROR_H
#define ASMKIT_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum asmkit_status {
    ASMKIT_OK = 0,
    ASMKIT_ERR_INVALID_ARGUMENT,
    ASMKIT_ERR_UNSUPPORTED_ARCH,
    ASMKIT_ERR_UNSUPPORTED_MODE,
    ASMKIT_ERR_UNSUPPORTED_FEATURE,
    ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION,
    ASMKIT_ERR_DECODE_FAILED,
    ASMKIT_ERR_ENCODE_FAILED,
    ASMKIT_ERR_OUTPUT_TOO_SMALL,
    ASMKIT_ERR_WORKSPACE_TOO_SMALL,
    ASMKIT_ERR_UNSUPPORTED_INSTRUCTION,
    ASMKIT_ERR_UNSUPPORTED_RELOCATION,
    ASMKIT_ERR_BRANCH_OUT_OF_RANGE,
    ASMKIT_ERR_REQUIRES_BRANCH_ISLAND,
    ASMKIT_ERR_FORBIDDEN_CLOBBER,
    ASMKIT_ERR_FEATURE_DISABLED,
    ASMKIT_ERR_INTERNAL
} asmkit_status_t;

typedef struct asmkit_error_info {
    asmkit_status_t status;
    unsigned int target;
    unsigned int detail;
} asmkit_error_info_t;

const char* asmkit_status_string(asmkit_status_t status);

#ifdef __cplusplus
}
#endif

#endif

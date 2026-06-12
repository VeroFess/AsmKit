#include "asmkit/asmkit_error.h"

const char* asmkit_status_string(asmkit_status_t status)
{
    switch (status) {
    case ASMKIT_OK: return "ok";
    case ASMKIT_ERR_INVALID_ARGUMENT: return "invalid argument";
    case ASMKIT_ERR_UNSUPPORTED_ARCH: return "unsupported architecture";
    case ASMKIT_ERR_UNSUPPORTED_MODE: return "unsupported mode";
    case ASMKIT_ERR_UNSUPPORTED_FEATURE: return "unsupported feature";
    case ASMKIT_ERR_UNSUPPORTED_TARGET_OPERATION: return "unsupported target operation";
    case ASMKIT_ERR_DECODE_FAILED: return "decode failed";
    case ASMKIT_ERR_ENCODE_FAILED: return "encode failed";
    case ASMKIT_ERR_OUTPUT_TOO_SMALL: return "output too small";
    case ASMKIT_ERR_WORKSPACE_TOO_SMALL: return "workspace too small";
    case ASMKIT_ERR_UNSUPPORTED_INSTRUCTION: return "unsupported instruction";
    case ASMKIT_ERR_UNSUPPORTED_RELOCATION: return "unsupported relocation";
    case ASMKIT_ERR_BRANCH_OUT_OF_RANGE: return "branch out of range";
    case ASMKIT_ERR_REQUIRES_BRANCH_ISLAND: return "requires branch island";
    case ASMKIT_ERR_FORBIDDEN_CLOBBER: return "forbidden clobber";
    case ASMKIT_ERR_FEATURE_DISABLED: return "feature disabled";
    case ASMKIT_ERR_INTERNAL: return "internal error";
    default: return "unknown status";
    }
}

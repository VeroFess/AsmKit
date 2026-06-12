#include "core/asmkit_internal.h"

size_t asmkit_workspace_required_size(const asmkit_engine_t* engine)
{
    (void)engine;
    return 0u;
}

asmkit_status_t asmkit_workspace_init(asmkit_workspace_t* workspace, void* memory, size_t memory_size)
{
    if (workspace == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    workspace->memory = (uint8_t*)memory;
    workspace->size = memory_size;
    workspace->used = 0u;
    workspace->flags = 0u;
    workspace->last_error.status = ASMKIT_OK;
    workspace->last_error.target = 0u;
    workspace->last_error.detail = 0u;
    return ASMKIT_OK;
}

void asmkit_workspace_reset(asmkit_workspace_t* workspace)
{
    if (workspace == 0) {
        return;
    }
    workspace->used = 0u;
    workspace->last_error.status = ASMKIT_OK;
    workspace->last_error.target = 0u;
    workspace->last_error.detail = 0u;
}

#include "core/asmkit_internal.h"

asmkit_status_t asmkit_analyze_inst(
    const asmkit_engine_t* ASMKIT_RESTRICT engine,
    asmkit_workspace_t* ASMKIT_RESTRICT workspace,
    const asmkit_inst_t* ASMKIT_RESTRICT inst,
    asmkit_inst_semantics_t* ASMKIT_RESTRICT out_semantics)
{
    const asmkit_target_ops_t* ops;

    if (engine == 0 || inst == 0 || out_semantics == 0) {
        return ASMKIT_ERR_INVALID_ARGUMENT;
    }
    ops = asmkit_engine_ops(engine);
    if (ops == 0 || ops->analyze_inst == 0) {
        return ASMKIT_ERR_UNSUPPORTED_ARCH;
    }
    return ops->analyze_inst(engine, workspace, inst, out_semantics);
}

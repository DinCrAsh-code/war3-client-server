//============================================================================
//  See ctaskpointandtargetinstancegenerator.h for why this one
//  two-instruction leaf lives in its own translation unit rather than
//  alongside its three callers in taskpointandtargetinstancegenerator.cpp,
//  and for why this address is CTaskPointAndTarget's own, not shared with
//  any other InstanceGenerator<CTaskXxx>.
//============================================================================
#include "ctaskpointandtargetinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F271B30.
//----------------------------------------------------------------------------
CTaskPointAndTargetInstanceGenerator* GetCTaskPointAndTargetGeneratorSingleton()
{
    return &g_ctaskPointAndTargetInstanceGenerator;
}

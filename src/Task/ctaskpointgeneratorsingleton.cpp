//============================================================================
//  See ctaskpointinstancegenerator.h for why this one two-instruction leaf
//  lives in its own translation unit rather than alongside its three
//  callers in taskpointinstancegenerator.cpp, and for why this address is
//  CTaskPoint's own, not shared with any other InstanceGenerator<CTaskXxx>.
//============================================================================
#include "ctaskpointinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F271A30.
//----------------------------------------------------------------------------
CTaskPointInstanceGenerator* GetCTaskPointGeneratorSingleton()
{
    return &g_ctaskPointInstanceGenerator;
}

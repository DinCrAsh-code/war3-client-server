//============================================================================
//  See ctaskpoint2instancegenerator.h for why this one two-instruction leaf
//  lives in its own translation unit rather than alongside its three
//  callers in taskpoint2instancegenerator.cpp, and for why this address is
//  CTaskPoint2's own, not shared with any other InstanceGenerator<CTaskXxx>.
//============================================================================
#include "ctaskpoint2instancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F271AB0.
//----------------------------------------------------------------------------
CTaskPoint2InstanceGenerator* GetCTaskPoint2GeneratorSingleton()
{
    return &g_ctaskPoint2InstanceGenerator;
}

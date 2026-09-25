//============================================================================
//  See ctaskrotateinstancegenerator.h for why this one two-instruction leaf
//  lives in its own translation unit rather than alongside its three
//  callers in taskrotateinstancegenerator.cpp.
//============================================================================
#include "ctaskrotateinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F2718B0.
//----------------------------------------------------------------------------
CTaskRotateInstanceGenerator* GetCTaskRotateGeneratorSingleton()
{
    return &g_ctaskRotateInstanceGenerator;
}

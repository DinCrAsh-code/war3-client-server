//============================================================================
//  See ctasktargetinstancegenerator.h for why this one two-instruction leaf
//  lives in its own translation unit rather than alongside its three
//  callers in tasktargetinstancegenerator.cpp.
//============================================================================
#include "ctasktargetinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F271BB0.
//----------------------------------------------------------------------------
CTaskTargetInstanceGenerator* GetCTaskTargetGeneratorSingleton()
{
    return &g_ctaskTargetInstanceGenerator;
}

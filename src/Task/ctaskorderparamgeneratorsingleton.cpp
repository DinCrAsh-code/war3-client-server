//============================================================================
//  See ctaskorderparaminstancegenerator.h for why this one two-instruction
//  leaf lives in its own translation unit rather than alongside its three
//  callers in taskorderparaminstancegenerator.cpp - same reasoning as
//  ctaskparamgeneratorsingleton.cpp (InstanceGenerator<CTaskParam>'s own
//  equivalent).
//============================================================================
#include "ctaskorderparaminstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F2719B0.
//----------------------------------------------------------------------------
CTaskOrderParamInstanceGenerator* GetCTaskOrderParamGeneratorSingleton()
{
    return &g_ctaskOrderParamInstanceGenerator;
}

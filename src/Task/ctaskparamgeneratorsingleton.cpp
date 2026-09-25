//============================================================================
//  See ctaskparaminstancegenerator.h for why this one two-instruction leaf
//  lives in its own translation unit rather than alongside its three
//  callers in taskparaminstancegenerator.cpp - same reasoning as
//  ctaskgeneratorsingleton.cpp (InstanceGenerator<CTask>'s own equivalent).
//============================================================================
#include "ctaskparaminstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F271830.
//----------------------------------------------------------------------------
CTaskParamInstanceGenerator* GetCTaskParamGeneratorSingleton()
{
    return &g_ctaskParamInstanceGenerator;
}

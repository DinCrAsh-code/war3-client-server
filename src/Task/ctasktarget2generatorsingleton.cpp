//============================================================================
//  See ctasktarget2instancegenerator.h for why this one two-instruction leaf
//  lives in its own translation unit rather than alongside its three
//  callers in tasktarget2instancegenerator.cpp.
//============================================================================
#include "ctasktarget2instancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F271C30.
//----------------------------------------------------------------------------
CTaskTarget2InstanceGenerator* GetCTaskTarget2GeneratorSingleton()
{
    return &g_ctaskTarget2InstanceGenerator;
}

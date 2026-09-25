//============================================================================
//  See ctaskactioninstancegenerator.h for why this one two-instruction leaf
//  lives in its own translation unit rather than alongside its three
//  callers in taskactioninstancegenerator.cpp - same reasoning as
//  ctaskparamgeneratorsingleton.cpp (InstanceGenerator<CTaskParam>'s own
//  equivalent).
//============================================================================
#include "ctaskactioninstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F271930.
//----------------------------------------------------------------------------
CTaskActionInstanceGenerator* GetCTaskActionGeneratorSingleton()
{
    return &g_ctaskActionInstanceGenerator;
}

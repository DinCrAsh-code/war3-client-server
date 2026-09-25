//============================================================================
//  See jassfuncinstancegenerator.h for why this one two-instruction leaf
//  lives in its own translation unit rather than alongside its three
//  callers in jassfunc.cpp.
//============================================================================
#include "jassfuncinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F3B1470.
//----------------------------------------------------------------------------
JassFuncInstanceGenerator* GetJassFuncGeneratorSingleton()
{
    return &g_jassFuncInstanceGenerator;
}

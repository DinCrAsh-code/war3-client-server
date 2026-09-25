//============================================================================
//  See ctaskinstancegenerator.h for why this one two-instruction leaf lives
//  in its own translation unit rather than alongside its three callers in
//  taskinstancegenerator.cpp - and for the correction to this wave's own
//  starting plan about this address being shared across all 33
//  InstanceGenerator<CTaskXxx> classes (it is not; it is CTask's own).
//============================================================================
#include "ctaskinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F2717B0.
//----------------------------------------------------------------------------
CTaskInstanceGenerator* GetCTaskGeneratorSingleton()
{
    return &g_ctaskInstanceGenerator;
}

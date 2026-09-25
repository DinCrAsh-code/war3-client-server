//============================================================================
//  See ctaskghostinstancegenerator.h for why this one two-instruction
//  leaf lives in its own translation unit rather than alongside its three
//  callers in taskghostinstancegenerator.cpp - same reasoning as
//  ctaskorderparamgeneratorsingleton.cpp (InstanceGenerator<CTaskOrderParam>'s
//  own equivalent).
//============================================================================
#include "ctaskghostinstancegenerator.h"

//----------------------------------------------------------------------------
//  0x6F271CB0.
//----------------------------------------------------------------------------
CTaskGhostInstanceGenerator* GetCTaskGhostGeneratorSingleton()
{
    return &g_ctaskGhostInstanceGenerator;
}

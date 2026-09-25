//============================================================================
//  Three one-instruction leaves the NIpse::CLrPath/CLrPathingAcc/
//  CLrPathingSys cluster's own `ReturnToStaticPool` vtable slots each call
//  once: `mov eax, g_pTimeSync; add eax, <offset>; retn` - no `this` read at
//  all, just "here is where this class's own static free-list pool header
//  lives".  Same fixed-block free-list shape (and the same
//  `SAgentChainPoolHeader` layout) as Agent/agentbasechain_staticpools.h's
//  own family of these leaves for the CPresence/CRlAgent base chain - kept
//  in a file of its own rather than added there, since that file is shared
//  by several concurrent sessions' own base-chain batches and touching it
//  here would just be merge churn for three lines with no real coupling.
//
//  `g_pTimeSync` (CTimeSync*, Game/game.h, 0x6FAB73D8) is the same global
//  the base-chain leaves already read from at other offsets.
//
//  Own translation unit, deliberately (CLAUDE.md's own "put a callee in its
//  own TU"): each of these is called from a different one of this batch's
//  own `ReturnToStaticPool` bodies (Pathfinding/pathclient_returntopool.cpp,
//  Pathfinding/lrpathingacc_returntopool.cpp,
//  Pathfinding/lrpathingsys_returntopool.cpp), and the real dump shows each
//  call staying out of line.
//============================================================================
#ifndef LRPATH_STATICPOOLS_H
#define LRPATH_STATICPOOLS_H

#include "agentbasechain_staticpools.h"  // SAgentChainPoolHeader

//  0x6F49B040 - NIpse::CLrPath's own static pool: g_pTimeSync + 0x958.
void* CLrPath_GetStaticPool();

//  0x6F497D00 - NIpse::CLrPathingAcc's own static pool: g_pTimeSync + 0x938.
void* CLrPathingAcc_GetStaticPool();

//  0x6F4A0970 - NIpse::CLrPathingSys's own static pool: g_pTimeSync + 0x918.
void* CLrPathingSys_GetStaticPool();

#endif  // LRPATH_STATICPOOLS_H

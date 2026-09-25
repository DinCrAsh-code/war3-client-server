//============================================================================
//  0x6F423D40 - the per-player "async refresh subgroups" notification
//  CAbilityInventory::DetachAndNotify (abilityinventorydispatch.cpp) fires
//  once a unit's inventory is left with no valid handles at all.  See
//  unitsubgrouprefresh.cpp for the body.
//============================================================================
#ifndef UNITSUBGROUPREFRESH_H
#define UNITSUBGROUPREFRESH_H

#include "agent.h"   // CAgentPtr

//----------------------------------------------------------------------------
//  The object at the player-slot table's own +0x34 - not otherwise
//  identified past the members this call tree reaches.
//----------------------------------------------------------------------------
struct SPlayerSubgroupTarget
{
    //  0x6F423D40, retn 4.  `flag` is always 1 at this call tree's one
    //  call site; kept as a real parameter rather than hard-coded since
    //  the shipped function reads it as its own stack argument.
    void NotifyAsyncRefreshSubgroups(int flag);

    char       m_reserved000[0x1AC];
    //  +0x1AC - the player index the log message names.  Not otherwise
    //  identified.
    int        m_playerIndex;
    char       m_reserved1B0[0x1D8 - 0x1B0];
    //  +0x1D8 - a cached, ref-counted agent reference, released/acquired
    //  with the same test-decrement-call-slot0 / test-increment shape
    //  CAgentPtr's own AssignChecked already uses (agent.h) - written as a
    //  real CAgentPtr assignment rather than the raw four instructions.
    CAgentPtr  m_cachedAgent;
    //  +0x1DC - a plain (non-refcounted) pointer cache, copied from the
    //  same field-walker head object's own +0x30 the +0x1D8 assignment
    //  above reads its replacement candidate from.
    void*      m_cachedField30;
};

#endif

//============================================================================
//  Three more no-/OPT:ICF duplicates of Agent/agent_writehandlepair.cpp's
//  own WriteAgentHandlePair (0x6F2D0350): "append a CAgent's {handle,
//  typeTag} pair to a save stream, -1/-1 when the slot is empty."  Reached
//  from three different places inside CPlayerWar3::Save's own closure
//  (playerwar3_save.cpp, selectionwar3_save.cpp) - each at its own address,
//  same reasoning agentdefaults.cpp's own header note gives for every other
//  duplicated writer in this repo (WriteDwordAlt/Alt2/Alt3 and friends).
//
//  All three are the SAME compiled body (down to the instruction) at three
//  different addresses - unlike 0x6F2D0350 itself, all three explicitly
//  reload `ecx` from the saved store pointer before the *second* WriteDword
//  call, where 0x6F2D0350's own body does not (its own header comment
//  explains why: the compiler keeps ecx live across the untouched call).
//  Nothing here found a source spelling that reproduces that one-instruction
//  difference against an otherwise identical function, so all three are
//  IDENTICAL rather than EXACT - see the BEHAVIOUR entries.
//============================================================================
#include "agent.h"
#include "cdatastorescratch.h"

//  0x6F40F130 - called from CPlayerWar3::Save (playerwar3_save.cpp) for the
//  +0x38 field (an SCheckedAllySlot, agenttypedslots.h - a CAgentPtr-shaped
//  wrapper, so its own raw pointer is exactly a `CAgent**` read).
CDataStoreScratch* __fastcall WriteAgentHandlePairDupA(CDataStoreScratch* store,
                                                        CAgent** slot)
{
    CAgent* agent = *slot;

    store->WriteDword(agent != 0 ? agent->m_handle : 0xFFFFFFFFu);
    store->WriteDword(agent != 0 ? agent->m_typeTag : 0xFFFFFFFFu);
    return store;
}

//  0x6F41F940 - called from CSelectionWar3::Save (selectionwar3_save.cpp)
//  for its own +0x1A8/+0x1E4 fields.
CDataStoreScratch* __fastcall WriteAgentHandlePairDupB(CDataStoreScratch* store,
                                                        CAgent** slot)
{
    CAgent* agent = *slot;

    store->WriteDword(agent != 0 ? agent->m_handle : 0xFFFFFFFFu);
    store->WriteDword(agent != 0 ? agent->m_typeTag : 0xFFFFFFFFu);
    return store;
}

//  0x6F2D0310 - called both from CPlayerWar3::Save itself (the +0x2B0/+0x2B4
//  "dead hero" TSGrowableArray<CAgentPtr>'s own elements, playerwar3_save.cpp)
//  and from CSelectionWar3::Save's own inner list-write helper
//  (sub_6F420440, selectionwar3_save.cpp) for each list node's candidate.
CDataStoreScratch* __fastcall WriteAgentHandlePairDupC(CDataStoreScratch* store,
                                                        CAgent** slot)
{
    CAgent* agent = *slot;

    store->WriteDword(agent != 0 ? agent->m_handle : 0xFFFFFFFFu);
    store->WriteDword(agent != 0 ? agent->m_typeTag : 0xFFFFFFFFu);
    return store;
}

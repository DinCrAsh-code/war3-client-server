//============================================================================
//  0x6F40FEB0 - PlayerRecordChecksumHash, CGameWar3::ComputeChecksum's own
//  per-player-record vtable-slot-0 contribution (Game/gamewar3.cpp).  `this`
//  is one player record - the same object playerscore.h's own SPlayerScore
//  already views for the end-of-game score fields, IndexedArrayHolder_
//  6F3A1650::At(i) hands back.
//
//  Five fields, none of them touched by any already-scored view of this
//  record:
//
//    +0x29C - a seed dword, XORed in first.
//    +0x2D0 - SPlayerScore::m_pTallies (CPlayerTallyList*) - the *same*
//      field playerscore.h already names, folded through its own new
//      ComputeChecksum() (playerscore.h/playerroster.cpp).
//    +0x2A4 - a second seed-shaped dword, folded in right after.
//    +0x2B0/+0x2B4 - a count and an array pointer: the loop this batch was
//      specifically asked about.  See below for what its elements are and
//      are not shown to be.
//    +0x34  - a pointer to this record's own "ability agent tracker"
//      sub-object (PlayerAbilityAgentTrackerChecksum,
//      playerabilityagenttracker.cpp).
//
//  The +0x2B0/+0x2B4 array and the CUnit* question
//  --------------------------------------------------------------------
//  Each non-null element is hashed as `[elem+0x10] ^ [elem+0xC]` - and
//  those two offsets are exactly CAgent::m_handle (+0x0C) and
//  CAgent::m_typeTag (+0x10), agent.h's own already-established field
//  layout, inherited unchanged by every CAgent-derived class in this
//  binary including the whole CUnit chain (CUnit -> CSelectable ->
//  CWidget -> CWar3Image -> CAgentWar3 -> CAgent).  So every element this
//  loop reads is, at minimum, *some* CAgent-derived object with a real,
//  live identity (handle+typeTag are assigned once at construction, not
//  recomputed) - not a placeholder, not a static table row, not a cheap
//  counter.
//
//  What this loop does **not** do, and what nothing in this 36-function
//  dump settles, is filter that array by type at all: contrast
//  PlayerAbilityAgentTrackerChecksum's own third list (record+0x34's
//  sub-object), which drains through SAbilityAgentCollector::CollectAgents
//  -> SCheckedUnitSlot::Assign -> GetUnitTypeFourCC/AgileTypeIsDerivedFrom
//  and is therefore *provably* CUnit-only (SCheckedUnitSlot "*is* the
//  binary's CAgentPtr<CUnit>" per agenttypedslots.h).  This loop's own
//  array has no such gate anywhere in this dump, and nothing else in
//  `src/` writes to record+0x2B0/+0x2B4 (grepped; no hit), so there is no
//  evidence in hand for *what specifically* populates it - it could be a
//  per-player unit list, but it could equally be a mixed bag of any
//  CAgent-derived object the player record tracks (abilities, items,
//  anything else CAgent covers).  Handle/typeTag are identity fields, not
//  live state either way: moving a unit does not change its own handle or
//  type tag, so this specific hash is sensitive to which agents are
//  *present* in the list, not to their position or other mutable state -
//  a real per-instance signal, but a narrower one than "this reflects a
//  unit's current position."  See
//  docs/notes/checksum-provider-registry.md's own section on this batch
//  for the full writeup.
//============================================================================
#include "playerscore.h"

unsigned int __fastcall PlayerAbilityAgentTrackerChecksum(void* self);   // 0x6F4244E0

unsigned int __fastcall PlayerRecordChecksumHash(void* recordPtr)
{
    char* record = (char*)recordPtr;

    unsigned int hash = *(unsigned int*)(record + 0x29C);

    CPlayerTallyList* tallies = *(CPlayerTallyList**)(record + 0x2D0);
    hash ^= tallies->ComputeChecksum();
    hash = (hash << 3) | (hash >> 29);

    hash ^= *(unsigned int*)(record + 0x2A4);
    hash = (hash << 3) | (hash >> 29);

    unsigned int count = *(unsigned int*)(record + 0x2B0);
    void** array = *(void***)(record + 0x2B4);

    if (count != 0)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            void* elem = array[i];
            if (elem)
            {
                unsigned int contribution = *(unsigned int*)((char*)elem + 0x10) ^ *(unsigned int*)((char*)elem + 0xC);
                contribution = (contribution << 3) | (contribution >> 29);

                hash ^= contribution;
                hash = (hash << 3) | (hash >> 29);
            }
        }
    }

    unsigned int trackerHash = PlayerAbilityAgentTrackerChecksum(*(void**)(record + 0x34));
    hash = (trackerHash ^ hash);
    hash = (hash << 3) | (hash >> 29);

    return hash;
}

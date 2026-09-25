//============================================================================
//  CUnit batch-16 (slot-101 closure).
//
//  0x6F049740 - given a preferred player-slot index (bounded < 12, one per
//  `SPlayerHandleSlotTable::m_slots` entry), try to resolve that slot's own
//  handle first; if that comes up empty and `scanAllPlayers` is set, walk
//  every one of the 12 slots, skipping any whose relation to the preferred
//  index is not "visible" (CGameWorld::TestVisibleRelationBit, with the
//  loop's own player index as the first argument and the *caller's*
//  preferred index reused as the bit to test - not the slot being resolved)
//  or whose own handle pair is the empty sentinel, and return the first
//  resolved candidate - immediately if `checkAddable` is clear, or only
//  once `CUnit::CheckItemAddable` accepts it otherwise.
//
//  `(handle & typeTag) != 0xFFFFFFFFu` as the "has a real record" test
//  (not `handle != -1` alone) is read directly off the shipped
//  `and`/`cmp -1` pair, not assumed.
//============================================================================
#include "unit.h"
#include "gameui.h"
#include "itemhandleresolve.h"

extern void* g_unk6FAB65F4;

struct SPlayerHandleSlot
{
    unsigned int m_handle;      // +0x00
    int          m_typeTag;     // +0x04
    unsigned int m_reserved08;  // +0x08, untouched here
};

struct SPlayerHandleSlotTable
{
    char              m_reserved00[0xC0];
    SPlayerHandleSlot m_slots[12];   // +0xC0

    void* FindAddableCandidate(unsigned int preferredIndex, int scanAllPlayers,
                               int checkAddable, int isPowerup);
};

void* SPlayerHandleSlotTable::FindAddableCandidate(unsigned int preferredIndex,
    int scanAllPlayers, int checkAddable, int isPowerup)
{
    if (preferredIndex >= 0x0C)
        return 0;

    SPlayerHandleSlot* preferred = &m_slots[preferredIndex];
    void* candidate = 0;
    if ((preferred->m_handle & preferred->m_typeTag) != 0xFFFFFFFF)
        candidate = QueryHandleField0x54((SOptionalHandleRef*)preferred);

    if (candidate != 0 || scanAllPlayers == 0)
        return candidate;

    CGameWorld* world = (CGameWorld*)g_unk6FAB65F4;
    SPlayerHandleSlot* slot = m_slots;
    for (unsigned int player = 0; player < 0x0C; player++, slot++)
    {
        if (!world->TestVisibleRelationBit(player, preferredIndex))
            continue;

        if ((slot->m_handle & slot->m_typeTag) == 0xFFFFFFFF)
            continue;

        void* resolved = QueryHandleField0x54((SOptionalHandleRef*)slot);
        if (resolved == 0)
            continue;

        if (checkAddable == 0)
            return resolved;

        if (((CUnit*)resolved)->CheckItemAddable(0, isPowerup, 0) != 0)
            return resolved;
    }

    return 0;
}

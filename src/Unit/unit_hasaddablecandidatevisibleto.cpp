//============================================================================
//  CUnit batch-16 (slot-101 closure).
//
//  0x6F057CC0 - true if any of the 12 players (0..11) has, at their own
//  preferred slot, a resolvable candidate this table's own
//  `FindAddableCandidate` (unit_findaddablecandidate.cpp, scanAllPlayers/
//  checkAddable/isPowerup all clear - so this only ever resolves that
//  player's own slot, never scans) accepts, *and* that player's relation
//  to `bit` (this function's one argument) is "visible"
//  (CGameWorld::TestVisibleRelationBit).  The combined `!found && player
//  < 12` loop condition is read directly off the shipped stream's own
//  shape: `found` is tested at the top of the loop (so a hit on player N
//  stops the scan before player N+1 runs) and the bound is tested at the
//  bottom.
//============================================================================
#include "gameui.h"

extern void* g_unk6FAB65F4;

struct SPlayerHandleSlotTable
{
    void* FindAddableCandidate(unsigned int preferredIndex, int scanAllPlayers,
                               int checkAddable, int isPowerup);

    int HasAddableCandidateVisibleTo(int bit);
};

int SPlayerHandleSlotTable::HasAddableCandidateVisibleTo(int bit)
{
    CGameWorld* world = (CGameWorld*)g_unk6FAB65F4;
    int found = 0;

    for (unsigned int player = 0; player < 0x0C; player++)
    {
        if (found)
            break;
        void* candidate = FindAddableCandidate(player, 0, 0, 0);
        found = (candidate != 0) && world->TestVisibleRelationBit(player, bit);
    }

    return found;
}

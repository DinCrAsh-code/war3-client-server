//============================================================================
//  0x6F423FC0 - CSelectionWar3::Save, reached from CPlayerWar3::Save's own
//  +0x34 field (playerwar3_save.cpp).  CSelectionWar3 itself is never
//  modelled concretely (playerwar3.h's own class comment) - every field
//  this file touches is a raw offset read straight off the dump, the same
//  way playerwar3_ctorsubobjects.cpp already treats this class.
//
//  Own translation unit: every function in it belongs to the same
//  0x6F42xxxx module (CSelectionWar3's own Save-side helpers) - one
//  wrapper adapter (0x6F4207D0), a filtered-list writer reused six times
//  at six different fields plus once more nested inside the
//  ability-entry-list writer (0x6F420440), its count-only twin
//  (0x6F421630), the ability-entry-list-of-lists writer (0x6F4208A0), and
//  a second "index of current" helper at a different pair of offsets than
//  Player/playerabilityagenttracker.cpp's own (0x6F423850).
//============================================================================
#include "game.h"                 // LookupHandle, CHandleObject
#include "cdatastorescratch.h"
#include "agent.h"
#include "playerwar3.h"           // CSelectionWar3's own declaration

//  playerwar3_savewritehandlepair.cpp.
CDataStoreScratch* __fastcall WriteAgentHandlePairDupB(
        CDataStoreScratch* store, CAgent** slot);
CDataStoreScratch* __fastcall WriteAgentHandlePairDupC(
        CDataStoreScratch* store, CAgent** slot);

//  Same '+agl' tag every handle-registered object carries - see
//  Pathfinding/pathrefarrayresize.cpp for the fuller note on this constant.
static const unsigned int kAgentTagPathable = 0x2B61676Cu;

//  0x6F420440 - append `count` = however many nodes of the intrusive list
//  rooted at `listOwner+0xC` (node+4 = next, node+8 = a CAgent* slot) have
//  a live, '+agl'-tagged, flag-clear resolved target, writing each such
//  node's own {handle,typeTag} pair (WriteAgentHandlePairDupC) as it goes -
//  the count itself is a placeholder reserved up front and patched
//  afterwards (WriteDwordAt), the same "reserve then patch" shape
//  CAgent::Save's own preamble (agent_save.cpp) already established.  Also
//  called with `listOwner` = a per-entry node's own +8 field, from
//  CSelectionWar3WriteAbilityEntryLists below - the same function, reused
//  for a nested list.  `retn 0`.
CDataStoreScratch* __fastcall CSelectionWar3WriteFilteredAgentList(
        CDataStoreScratch* store, void* listOwner)
{
    unsigned int placeholderOffset = (unsigned int)store->m_field10;
    store->WriteDword(0);

    int matched = 0;
    int node = *(int*)((char*)listOwner + 0xC);
    if (node > 0)
    {
        do
        {
            CAgent* candidate = *(CAgent**)(node + 8);
            if (candidate != 0)
            {
                CHandleObject* resolved =
                        LookupHandle(candidate->m_handle, (int)candidate->m_typeTag);
                if (resolved != 0 &&
                    resolved->m_kindTag == (int)kAgentTagPathable &&
                    *(int*)((char*)resolved + 0x20) == 0)
                {
                    WriteAgentHandlePairDupC(store, (CAgent**)(node + 8));
                    matched++;
                }
            }

            node = *(int*)(node + 4);
        } while (node > 0);
    }

    store->WriteDwordAt(placeholderOffset, (unsigned int)matched);
    return store;
}

//  0x6F4207D0 - the one-line adapter CSelectionWar3::Save's own first call
//  needs: its own list lives at `this+0xC` directly (`this` itself is the
//  `listOwner` CSelectionWar3WriteFilteredAgentList reads `+0xC` off of),
//  so this just swaps the (this=ecx, store=stack) member-call shape into
//  the free function's (store=ecx, listOwner=edx) one.  `retn 4`.
CDataStoreScratch* CSelectionWar3::SaveMainList(CDataStoreScratch* store)
{
    return CSelectionWar3WriteFilteredAgentList(store, this);
}

//  0x6F421630 - the count-only twin of CSelectionWar3WriteFilteredAgentList:
//  same filter, no store, no write.  `ecx` = the list owner directly (no
//  store argument at all).  `retn 0`.
int __fastcall CSelectionWar3CountFilteredAgentList(void* listOwner)
{
    int matched = 0;
    int node = *(int*)((char*)listOwner + 0xC);
    if (node > 0)
    {
        do
        {
            CAgent* candidate = *(CAgent**)(node + 8);
            if (candidate != 0)
            {
                CHandleObject* resolved =
                        LookupHandle(candidate->m_handle, (int)candidate->m_typeTag);
                if (resolved != 0 &&
                    resolved->m_kindTag == (int)kAgentTagPathable &&
                    *(int*)((char*)resolved + 0x20) == 0)
                {
                    matched++;
                }
            }

            node = *(int*)(node + 4);
        } while (node > 0);
    }

    return matched;
}

//  0x6F4208A0 - a list of "ability entries" rooted at `listOwner+0x8`
//  (node+4 = next, same node shape as above) - for each entry, treat
//  entry+8 as a nested list owner and write its own filtered agent list
//  through CSelectionWar3WriteFilteredAgentList above, counting how many
//  entries were actually walked (capped at 0xFFFF0, the shipped bound) into
//  a placeholder patched at the end - the same "list of lists" shape
//  Player/playerabilityagenttracker.cpp's own PlayerAbilityAgentTracker-
//  Checksum already established for a different pair of offsets.  The
//  "entry==0 -> reload the sentinel" fallback is the same dead branch that
//  file's own IndexOfCurrent already documents (unreachable given the
//  entry `> 0` loop guard, reproduced because the shipped stream has it).
//  `retn 0`.
CDataStoreScratch* __fastcall CSelectionWar3WriteAbilityEntryLists(
        CDataStoreScratch* store, void* listOwner)
{
    unsigned int placeholderOffset = (unsigned int)store->m_field10;
    store->WriteDword(0);

    char* sentinel = (char*)listOwner + 4;
    int entry = *(int*)((char*)listOwner + 8);
    unsigned int matched = 0;

    if (entry > 0)
    {
        do
        {
            if (matched >= 0xFFFF0)
                break;

            CSelectionWar3WriteFilteredAgentList(store, (void*)(entry + 8));
            matched++;

            if (entry == 0)
                entry = (int)sentinel;
            entry = *(int*)(entry + 4);
        } while (entry > 0);
    }

    store->WriteDwordAt(placeholderOffset, matched);
    return store;
}

//  0x6F423850 - a second "index of current" helper, the same shape as
//  Player/playerabilityagenttracker.cpp's own PlayerAbilityAgentTracker-
//  IndexOfCurrent but at CSelectionWar3's own +0x200/+0x204/+0x20C rather
//  than +0x1B4/+0x1B8/+0x1C0 - a distinct compiled body (the offsets are
//  literals baked into it), not a reusable address.  `retn 0`.
int __fastcall CSelectionWar3IndexOfCurrentB(void* selfPtr)
{
    char* self = (char*)selfPtr;

    void* current = *(void**)(self + 0x20C);
    if (current == 0)
        return -1;

    int raw = *(int*)(self + 0x204);
    int entry = raw > 0 ? raw : 0;
    if (entry <= 0)
        return -1;

    int sentinel = (int)(self + 0x200);

    int index = 0;
    for (;;)
    {
        if ((void*)(entry + 8) == current)
            return index;

        index++;

        if (entry == 0)
            entry = sentinel;

        entry = *(int*)(entry + 4);
        if (entry <= 0)
            return -1;
    }
}

//  0x6F422D10 - Player/playerabilityagenttracker.cpp.  Reused verbatim: the
//  same address, called here on CSelectionWar3's own +0x1B4/+0x1B8/+0x1C0
//  fields (the same relative shape as its own PlayerAbilityAgentTracker
//  namesake - the OTHER "index of current", CSelectionWar3IndexOfCurrentB
//  above, is a genuinely different compiled body at +0x200/+0x204/+0x20C).
int __fastcall PlayerAbilityAgentTrackerIndexOfCurrent(void* selfPtr);

//  0x6F423FC0 - see the file header above for the class-level notes.  `void`
//  return matches playerwar3.h's own declaration - nothing sets `eax`
//  explicitly before the shipped `retn 4` either (whatever the last call
//  left there is never read back).
void CSelectionWar3::Save(CDataStoreScratch* store)
{
    char* selfBytes = (char*)this;

    SaveMainList(store);

    store->WriteDword(0x0A);
    //  Ten fixed 0x14-byte slots at +0xDC and, in lock-step, ten more at
    //  +0x14 (the loop's own `ebx-0xC8` addressing) - each an independent
    //  filtered-agent-list write.
    for (int i = 0; i < 0x0A; i++)
    {
        char* highSlot = selfBytes + 0xDC + i * 0x14;
        char* lowSlot = highSlot - 0xC8;
        CSelectionWar3WriteFilteredAgentList(store, lowSlot);
        CSelectionWar3WriteFilteredAgentList(store, highSlot);
    }

    WriteAgentHandlePairDupC(store, (CAgent**)(selfBytes + 0x1A4));
    WriteAgentHandlePairDupB(store, (CAgent**)(selfBytes + 0x1A8));
    CSelectionWar3WriteFilteredAgentList(store, selfBytes + 0x1E8);

    //  Reserve a count placeholder, then walk the ability-node list at
    //  +0x1B4/+0x1B8 (the same intrusive list PlayerAbilityAgentTracker-
    //  IndexOfCurrent below reads), counting only the nodes whose own
    //  +8-offset candidate list (CSelectionWar3CountFilteredAgentList) is
    //  non-empty and writing exactly those (CSelectionWar3WriteFiltered-
    //  AgentList) - the same "list of lists" shape
    //  CSelectionWar3WriteAbilityEntryLists uses below, just counting
    //  matches instead of a fixed 0xFFFF0 cap.
    {
        unsigned int placeholderOffset = (unsigned int)store->m_field10;
        store->WriteDword(0);

        unsigned int matchedEntries = 0;
        int rawCount = *(int*)(selfBytes + 0x1B8);
        int abilityNode = rawCount > 0 ? rawCount : 0;
        if (abilityNode > 0)
        {
            do
            {
                void* candidateBase = (void*)(abilityNode + 8);
                if (CSelectionWar3CountFilteredAgentList(candidateBase) != 0)
                {
                    CSelectionWar3WriteFilteredAgentList(store, candidateBase);
                    matchedEntries++;
                }

                if (abilityNode == 0)
                    abilityNode = (int)(selfBytes + 0x1B4);
                abilityNode = *(int*)(abilityNode + 4);
            } while (abilityNode > 0);
        }

        store->WriteDwordAt(placeholderOffset, matchedEntries);
    }

    store->WriteDword((unsigned int)PlayerAbilityAgentTrackerIndexOfCurrent(this));

    CSelectionWar3WriteFilteredAgentList(store, selfBytes + 0x1C4);
    WriteAgentHandlePairDupC(store, (CAgent**)(selfBytes + 0x1E0));
    WriteAgentHandlePairDupB(store, (CAgent**)(selfBytes + 0x1E4));
    CSelectionWar3WriteAbilityEntryLists(store, selfBytes + 0x1FC);

    store->WriteDwordAlt(*(unsigned int*)(selfBytes + 0x208));

    store->WriteDword((unsigned int)CSelectionWar3IndexOfCurrentB(this));

    store->WriteDwordAlt(*(unsigned int*)(selfBytes + 0x1AC));
    WriteAgentHandlePairDupC(store, (CAgent**)(selfBytes + 0x1D8));
    store->WriteDword(*(unsigned int*)(selfBytes + 0x1DC));
}

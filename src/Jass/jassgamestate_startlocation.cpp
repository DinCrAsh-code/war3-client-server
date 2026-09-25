//============================================================================
//  0x6F3A1320 and 0x6F3A3240 - the two members of the JASS game state that
//  own a start-location slot: place one, and pin a player to one.
//
//  Both were naked `jmp` thunks in jassnatives_thunks.cpp.  They are the
//  two functions between JASS_DefineStartLocationLoc / JASS_SetPlayerStart-
//  Location / JASS_ForcePlayerStartLocation and the record their handles
//  ultimately name, so leaving them redirected meant three finished natives
//  still ran shipped code the moment they did anything.
//
//  One translation unit: both are in the 0x6F3Axxxx game-state module and
//  neither calls the other.
//============================================================================
#include "jassnatives.h"

//  Re-declared to match misc_field_getters.cpp's own struct exactly, so this
//  call site mangles the same way and reaches the real symbol - the same
//  re-declaration jassgamestatemasks.cpp makes for the identical reason
//  (docs/notes/verifier-gate-link-failures.md).
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//----------------------------------------------------------------------------
//  0x6F3A1320 - place start location `index` at (*x, *y) and mark the slot
//  as defined.  `retn 0Ch`, `this` in ecx.
//
//  Both reals are read out before either is stored, and the second one
//  first: the shipped code loads `y`'s pointer, dereferences it, then does
//  the same for `x` and only then writes the pair.  That is what naming the
//  two values does and what storing straight through the arguments does not
//  - the same shape JASS_MoveLocation's own body records
//  (jassnatives_location.cpp).
//----------------------------------------------------------------------------
void CJassGameState::DefineStartLocation(int index, const int* x, const int* y)
{
    SStartLocation* slot = &m_startLocations[index];

    int newY = *y;
    int newX = *x;

    slot->m_x = newX;
    slot->m_y = newY;
    slot->m_defined = 1;
}

//----------------------------------------------------------------------------
//  0x6F3A3240 - pin `playerIndex` to `startLocation`.  `retn 8`, `this` in
//  ecx.
//
//  The player record comes out of the singleton's own indexed array, and
//  `At` is called through a cast rather than re-declared as a member of this
//  class for the same reason jassgamestatemasks.cpp gives: MSVC puts the
//  class name in the mangled name, so a second declaration would be a second
//  symbol.  It leaves ecx alone (its whole body is three instructions
//  through eax), which is why the shipped code can still form the
//  start-location slot address off `this` straight after the call.
//
//  The two flag words go in before the player's own field, and the
//  publication through SetField78 is last; the shipped code hoists the
//  `[eax+274h]` store in between that call's `push` and the call itself,
//  which is scheduling, not a different order.
//----------------------------------------------------------------------------
void CJassGameState::ForcePlayerStartLocation(int playerIndex, int startLocation)
{
    SJassPlayer* player =
        (SJassPlayer*)((IndexedArrayHolder_6F3A1650*)this)->At(playerIndex);

    SStartLocation* slot = &m_startLocations[startLocation];

    slot->m_field18 = 1;
    slot->m_field28 = 1;

    player->m_startLocation = startLocation;

    //  +0xD0 on the player record: a published field the rest of the game
    //  watches, set to 1 here.  Reached by offset rather than as a named
    //  member of SJassPlayer because that is all this call tree establishes
    //  about it - the same HandleRefFieldOwner_6F473170 view every other
    //  caller of 0x6F473140 takes (jassnatives.h).
    ((HandleRefFieldOwner_6F473170*)((char*)player + 0xD0))->SetField78(1);
}

//----------------------------------------------------------------------------
//  0x6F3A4F70 - resize start location `index`'s ally-priority list.
//  `retn 8`, `this` in ecx.
//
//  The record is *not* named first: the shipped code forms one address,
//  `lea ecx,[edx+ecx+0B4h]`, straight from the scaled index, which is what
//  reaching through the subscript in a single expression produces.  0xB4 is
//  0xAC (the array of records) + 0x08 (the list inside one).
//----------------------------------------------------------------------------
void CJassGameState::SetStartLocPrioCount(int index, unsigned int count)
{
    m_startLocations[index].m_priorities.SetCount(count);
}

//----------------------------------------------------------------------------
//  0x6F3A12B0 - write one entry of that list.  `retn 10h`, `this` in ecx.
//
//  The entry pointer is named, because the shipped code forms it once
//  (`lea eax,[ecx+edx*8]`) and stores through it twice; both values are
//  loaded off the stack only after that address exists.
//----------------------------------------------------------------------------
void CJassGameState::SetStartLocPrio(int index, int slot,
                                     int otherStartLocation, int priority)
{
    AllySlot* entry = &m_startLocations[index].m_priorities[slot];

    entry->m_startLocation = otherStartLocation;
    entry->m_priority = priority;
}

//----------------------------------------------------------------------------
//  0x6F3A1300 - read one entry's priority back.  `retn 8`, `this` in ecx.
//----------------------------------------------------------------------------
int CJassGameState::GetStartLocPrio(int index, int slot)
{
    return m_startLocations[index].m_priorities[slot].m_priority;
}

//----------------------------------------------------------------------------
//  0x6F3A12E0 - and the other word of that same entry, the start location it
//  names.  `retn 8`, `this` in ecx.  Six instructions that differ from
//  0x6F3A1300's six by the `+4` in the final addressing mode alone, which is
//  what fixes the entry as the {start location, priority} pair the AllySlot
//  constructor initialises to {-1, 0}.
//----------------------------------------------------------------------------
int CJassGameState::GetStartLocPrioSlot(int index, int slot)
{
    return m_startLocations[index].m_priorities[slot].m_startLocation;
}

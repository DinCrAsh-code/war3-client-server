//============================================================================
//  The start-location natives - the seven registered at
//  0x6F3BAF70..0x6F3BB080 in the 0x6F3BAxxx/0x6F3BBxxx native module.
//
//      DefineStartLocation   "(IRR)V"                  0x6F3BAF70
//      SetStartLocPrioCount  "(II)V"                   0x6F3BAF90
//      SetStartLocPrio       "(IIIHstartlocprio;)V"    0x6F3BAFC0
//      GetStartLocPrioSlot   "(II)I"                   0x6F3BB000
//      GetStartLocPrio       "(II)Hstartlocprio;"      0x6F3BB030
//      GetStartLocationX     "(I)R"                    0x6F3BB060
//      GetStartLocationY     "(I)R"                    0x6F3BB080
//
//  Every one of them is a range check in front of one CJassGameState member
//  (jassgamestate_startlocation.cpp).  The bound is twelve on a start
//  location index and on a priority slot index alike, and three on the
//  priority itself, which is the count of `startlocprio` values JASS has
//  (LOW / HIGH / NOT_SET).
//
//  Four of the seven begin by loading the singleton and testing it; the two
//  coordinate getters check their index *first* and never test the singleton
//  at all, and DefineStartLocation tests neither.  That asymmetry is the
//  shipped code's.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F3BAF70 - "(IRR)V".  A JASS `real` crosses the native boundary as a
//  pointer to its encoded value, so all three arguments are plain dwords and
//  the native is a bare forward to the game state's own member
//  (jassgamestate_startlocation.cpp) - the same member
//  `DefineStartLocationLoc` (0x6F3C0EA0) reaches after unpacking a location
//  handle.  Nothing is range-checked here; the member indexes the record
//  array with whatever it is given.
//----------------------------------------------------------------------------
void __cdecl JASS_DefineStartLocation(int whichStartLoc, const int* x,
                                      const int* y)
{
    g_pJassGameState->DefineStartLocation(whichStartLoc, x, y);
}

//----------------------------------------------------------------------------
//  0x6F3BAF90 - "(II)V".
//----------------------------------------------------------------------------
void __cdecl JASS_SetStartLocPrioCount(int whichStartLoc, int prioSlotCount)
{
    CJassGameState* state = g_pJassGameState;

    if (state && (unsigned int)whichStartLoc < 12 &&
                 (unsigned int)prioSlotCount < 12)
    {
        state->SetStartLocPrioCount(whichStartLoc, prioSlotCount);
    }
}

//----------------------------------------------------------------------------
//  0x6F3BAFC0 - "(IIIHstartlocprio;)V".
//----------------------------------------------------------------------------
void __cdecl JASS_SetStartLocPrio(int whichStartLoc, int prioSlotIndex,
                                  int otherStartLocIndex, int priority)
{
    CJassGameState* state = g_pJassGameState;

    if (state && (unsigned int)whichStartLoc < 12 &&
                 (unsigned int)prioSlotIndex < 12 &&
                 (unsigned int)otherStartLocIndex < 12 &&
                 (unsigned int)priority < 3)
    {
        state->SetStartLocPrio(whichStartLoc, prioSlotIndex,
                               otherStartLocIndex, priority);
    }
}

//----------------------------------------------------------------------------
//  0x6F3BB000 - "(II)I".  The other half of the same 8-byte priority entry
//  GetStartLocPrio reads: which start location the slot names, rather than
//  what priority it was given.  Same two bounds, same zero answer.
//----------------------------------------------------------------------------
int __cdecl JASS_GetStartLocPrioSlot(int whichStartLoc, int prioSlotIndex)
{
    CJassGameState* state = g_pJassGameState;

    if (state && (unsigned int)whichStartLoc < 12 &&
                 (unsigned int)prioSlotIndex < 12)
    {
        return state->GetStartLocPrioSlot(whichStartLoc, prioSlotIndex);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F3BB030 - "(II)Hstartlocprio;".  A `startlocprio` handle is the raw
//  priority value, which is why the failure answer is a bare zero rather
//  than the default-bound fallback the rect getters use.
//----------------------------------------------------------------------------
int __cdecl JASS_GetStartLocPrio(int whichStartLoc, int prioSlotIndex)
{
    CJassGameState* state = g_pJassGameState;

    if (state && (unsigned int)whichStartLoc < 12 &&
                 (unsigned int)prioSlotIndex < 12)
    {
        return state->GetStartLocPrio(whichStartLoc, prioSlotIndex);
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F3BB060 - "(I)R".  The shipped code forms the record's address and
//  dereferences it in two instructions - `lea eax,[eax+ecx+0ACh]` /
//  `mov eax,[eax]` - where this build folds both into one addressing mode,
//  and that one instruction is the whole difference.  Seven spellings have
//  now been measured against it: a named slot pointer, the record read as an
//  int, the singleton named first, the bare subscript below, the record's
//  address cast to `const int*`, and an inline helper returning `const int&`
//  - MSVC folds every one of them.  Its twin GetStartLocationY at the bottom
//  of this file is exact precisely because its 0xB0 *is* four strides, so
//  there is no displacement left to fold.
//----------------------------------------------------------------------------
int __cdecl JASS_GetStartLocationX(int whichStartLoc)
{
    if ((unsigned int)whichStartLoc >= 12)
        return 0;

    SStartLocation* slot = &g_pJassGameState->m_startLocations[whichStartLoc];
    return slot->m_x;
}

//----------------------------------------------------------------------------
//  0x6F3BB080 - "(I)R", GetStartLocationX's twin one dword further into the
//  same 0x2C-byte record.  It is *not* written the same way, and that is the
//  shipped code's own arithmetic: 0xB0 is exactly four strides of 0x2C, so
//  MSVC folds the field offset into the index (`add eax,4` / `imul eax,2Ch`
//  / `mov eax,[eax+ecx]`) where the X native's 0xAC is not a multiple of the
//  stride and has to keep its displacement.  Reaching the record through a
//  second subscript rather than through a named pointer is what asks for
//  that fold - see the note on JASS_GetStartLocationLoc
//  (jassnatives_startlocationloc.cpp), where the same rewrite is the thing
//  to be *avoided*.
//----------------------------------------------------------------------------
int __cdecl JASS_GetStartLocationY(int whichStartLoc)
{
    if ((unsigned int)whichStartLoc >= 12)
        return 0;

    return g_pJassGameState->m_startLocations[whichStartLoc].m_y;
}

//============================================================================
//  0x6F314B60 - GetPlayerColorValue.
//
//  The address of a player's colour inside the game's colour table.  The
//  palette slot is resolved first (ResolvePlayerColorSlot, 0x6F314A30,
//  Widget/playercolor.cpp) and then passed as the *fallback* to
//  ResolvePlayerColorIndex, which is what the `mov edx, eax` between the two
//  calls is.
//
//  Own translation unit: both callees are real calls, and one of them
//  (0x6F314A70) is in this same shipped module - it must not be in a
//  position to inline into this.
//============================================================================
#include "gameui.h"
#include "playercolor.h"

//: dword_6FAB5954 - a *pointer* to the table of ARGB colours, not the table
//: itself: the shipped code loads the dword and then indexes off it.
extern unsigned int* g_pPlayerColorTable;

const unsigned int* __fastcall GetPlayerColorValue(unsigned int player)
{
    unsigned int index =
        ResolvePlayerColorIndex(player, ResolvePlayerColorSlot(player));

    return &g_pPlayerColorTable[index];
}

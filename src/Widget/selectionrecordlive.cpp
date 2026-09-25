//============================================================================
//  0x6F416560 - "is this player selection record the local player's".
//
//  CItem's slot 102 (item_selectionrefresh.cpp) asks this of every one of
//  the sixteen records in the world object's +0x58 array; it was a
//  `mov eax,<address> / jmp eax` redirect and is seven instructions of
//  field compare.
//
//  The record's own owner index at +0x30 is a *byte*, and the world
//  object's local-player index at +0x28 is a *word*: the shipped code
//  widens the byte into ax (`movzx ax, byte ptr [ecx+30h]`) and compares 16
//  bits wide (`cmp [edx+28h], ax`) rather than promoting either side to a
//  dword.  Both casts below are load-bearing - promoting to `int` makes
//  MSVC use `movzx eax` and a 32-bit compare instead.
//
//  Own translation unit: its one caller must reach it with a real call.
//============================================================================
#include "playercolor.h"    // g_unk6FAB65F4 - the world-object singleton

struct SSelectionRecord
{
    int IsLive();

    char          m_reserved00[0x30];
    unsigned char m_ownerIndex;     // +0x30
};

int SSelectionRecord::IsLive()
{
    return *(const unsigned short*)((const char*)g_unk6FAB65F4 + 0x28)
           == (unsigned short)m_ownerIndex;
}

//============================================================================
//  0x6F314A30 - see playercolor.cpp.  In a header so the thunk translation
//  unit and the call site declare SPlayerSlotTable::Find identically.
//============================================================================
#ifndef PLAYERCOLOR_H
#define PLAYERCOLOR_H

struct SPlayerSlot
{
    //  0x6F40AFF0 - GameUI/gameuiplayerlabel.cpp: the slot's display name,
    //  built once from the game-string table and cached in the counted
    //  string at +0x24.
    const char* __thiscall EnsureDisplayName(int build);

    char         m_reserved000[0x24];
    char         m_displayName[0x0C];   // +0x24, a counted string
    unsigned char m_slotNumber;         // +0x30
    char         m_reserved031[0x233];
    unsigned int m_color;      // +0x264, -1 when the player has none
};

//  0x6F3A1650 is IndexedArrayHolder_6F3A1650::At (Misc/misc_field_getters.cpp),
//  already reconstructed and EXACT.  This header used to declare a second
//  name for it (`SPlayerSlotTable::Find`) backed by its own naked thunk in
//  Misc/unreconstructed_thunks.cpp - two symbols for one function, which
//  meant ResolvePlayerColorSlot reached the shipped code and that EXACT
//  three-instruction body was never exercised by it.  Both are gone; the
//  call site declares the real class locally, the way every other caller of
//  it already does.
//
//  It is still a thiscall member and not a __fastcall free function, and
//  that part was never a style question - see playercolor.cpp.

extern unsigned int g_playerColorSlotCount;   // dword_6FAB5944

//  dword_6FAB65F4, and deliberately spelled with the name itemhandlemain.h
//  already gave it rather than a second one of this target's own: funcmap.py
//  binds one C++ name per address, gen_rename_from_funcmap.py reports two as
//  a conflict, and that file references this symbol from naked asm.  What
//  this target adds is what it *is* - a table indexed by player index whose
//  records carry a colour at +0x264 - which is recorded in funcmap.py's own
//  DATA comment rather than by renaming it out from under a matched target.
extern void* g_unk6FAB65F4;

unsigned int __fastcall ResolvePlayerColorSlot(unsigned int player);

#endif

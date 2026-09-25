//============================================================================
//  0x6F2B6F80 - CItem's vtable slot 102 (+0x198).
//
//  Run slot 100's own refresh first, then ask every one of the sixteen
//  player selection records whether it is holding *this* item, and if any
//  one of them is, tail-jump into the global selection-overlay refresh.
//
//  The sixteen are the world object's own +0x58 array, the same one
//  CItem::Method_0x188 (item_selected.cpp) indexes by the local player -
//  this slot walks all of them instead, which is what makes it "is anyone
//  at all selecting me" where slot 98 is "am *I* selected locally".
//
//  Own translation unit: four real calls, one of them virtual and one of
//  them a tail jump, and it shares nothing with any other file here.
//============================================================================
#include "item.h"
#include "playercolor.h"    // g_unk6FAB65F4 - the world-object singleton

//  0x6F3A1650 - already reconstructed as IndexedArrayHolder_6F3A1650::At
//  (misc_field_getters.cpp).  Re-declared, not re-thunked, for the reason
//  item_selected.cpp spells out at length: a second naked function at the
//  same address is two symbols for one function and costs the original its
//  score.
struct IndexedArrayHolder_6F3A1650
{
    char pad[0x58];
    int  m_data[1];
    int  At(unsigned int index);
};

//----------------------------------------------------------------------------
//  One player's selection record.  Only +0x34 is reached, and only to be
//  handed to the membership test below as its `this`.
//----------------------------------------------------------------------------
struct SSelectionRecord
{
    //  0x6F416560 - "is this selection record the local player's".
    //  thiscall, no arguments; reconstructed in selectionrecordlive.cpp.
    int IsLive();

    char  m_reserved00[0x34];
    void* m_pSet;       // +0x34
};

//----------------------------------------------------------------------------
//  What that +0x34 names: the set of agents the record has selected.
//----------------------------------------------------------------------------
struct SSelectionSet
{
    //  0x6F41FE20 - "am I holding that agent, and if so let go of it".
    //  thiscall, one stack argument; reconstructed in selectionholder.cpp.
    int Contains(CAgent* agent);
};

//  0x6F332700 - the global "somebody's selection changed, redraw it"
//  refresh.  __fastcall with a single argument, which this caller always
//  passes as 0 (the shipped `xor ecx,ecx` immediately before the jump).
//  Reconstructed in selectionoverlayrefresh.cpp.
void __fastcall RefreshSelectionOverlay(int which);

//  Slot 100 (+0x190) reached through the vtable rather than by name: this
//  is CSelectable's own body (0x6F2C74E0), which CItem does not override,
//  and the shipped code dispatches it (`mov eax,[ebx]` /
//  `mov edx,[eax+190h]` / `call edx`) instead of calling it directly.
typedef void(__thiscall* Slot190Fn)(void* self, int a, int b);

void CItem::RefreshSelectionState()
{
    ((Slot190Fn)(*(void***)this)[0x190 / 4])(this, 0, 0);

    //  The singleton read once into a callee-saved register before the loop
    //  (the shipped `mov ebp, dword_6FAB65F4`), not re-read per iteration:
    //  a `((IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4)->At(i)` inside the
    //  body makes MSVC reload the global every time round, because none of
    //  the three calls in the body can be proven to leave it alone.
    IndexedArrayHolder_6F3A1650* world =
        (IndexedArrayHolder_6F3A1650*)g_unk6FAB65F4;

    //  No early exit: the shipped loop runs all sixteen iterations even
    //  after the flag is set, so this is a flag and not a `return 1`.
    int anySelecting = 0;
    for (unsigned int i = 0; i < 0x10; i++)
    {
        SSelectionRecord* rec = (SSelectionRecord*)world->At(i);
        if (rec == 0)
            continue;
        if (!((SSelectionSet*)rec->m_pSet)->Contains((CAgent*)this))
            continue;
        if (!rec->IsLive())
            continue;
        anySelecting = 1;
    }

    if (anySelecting)
        RefreshSelectionOverlay(0);
}

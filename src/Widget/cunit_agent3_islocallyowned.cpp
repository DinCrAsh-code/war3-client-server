//============================================================================
//  0x6F285AA0 - `retn`: is this widget's own owning player (slot 59,
//  GetOwningPlayerIndex, widget.h/unit.h) the local acting player
//  (g_unk6FAB65F4's own +0x28 word - the same "acting (local) player" slot
//  singleton unit_selectionhighlight.cpp's SActingPlayerSlotHolder and
//  gameuicursorcolor.cpp's own copy already read, re-declared locally the
//  same way both of those do)?
//============================================================================
#include "widget.h"

extern void* g_unk6FAB65F4;
struct SActingPlayerSlotHolder
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;   // +0x28
};

//  A local, non-virtual extension of CWidget - not added to widget.h
//  itself, since nothing in this call tree says this address is reached
//  through CWidget's own vtable (the shipped `call [ecx+0ECh]` dispatches
//  slot 59 dynamically, so any CWidget-derived receiver works the same
//  way; adding a new non-virtual member here changes CWidget's own vtable
//  not at all).
struct CWidgetLocalOwnerCheck : CWidget
{
    int IsOwnedByLocalPlayer();
};

int CWidgetLocalOwnerCheck::IsOwnedByLocalPlayer()
{
    //  Read *before* the virtual call and kept in a callee-saved register
    //  across it - the shipped `movzx esi,[eax+28h]` runs ahead of
    //  `call [edx+0ECh]`, not after it.
    int localSlot = ((SActingPlayerSlotHolder*)g_unk6FAB65F4)->m_actingSlot;
    return GetOwningPlayerIndex() == localSlot;
}

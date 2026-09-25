//============================================================================
//  0x6F2B7C20 - CItem's vtable slot 33 (+0x84), where CAgentWar3 only sets
//  the bit (0x6F0520B0, agentwar3_flagbits.cpp).
//
//  An item going "pending" does the whole teardown: the bit, then drop the
//  pathing footprint through slot 83, then a rendering-side gate, then
//  clear the notify handle, then slot 102, then finally hand two -1s to the
//  target-lost notifier CWidget keeps at 0x6F2AB310.
//
//  Own translation unit: five real calls, none of them to each other.
//============================================================================
#include "item.h"

//  See itemrendergate.cpp.
struct SItemRenderGate
{
    void OnPending();
};

//  slot 83 (+0x14C) - CWidget::RemoveFootprint (widgetfootprintremove.cpp).
typedef void (__thiscall *Slot0x14CFn)(void* self, int blocking, int useAlternate);
//  slot 102 (+0x198) - CItem::RefreshSelectionState (item_selectionrefresh.cpp).
typedef void (__thiscall *Slot0x198Fn)(void* self);

//  0x6F333290 - a rendering-side gate, reconstructed in
//  itemrendergate.cpp.  A thiscall member with no arguments; declared, not
//  defined, here so this call site reaches it with a real call.

void CItem::SetPending()
{
    m_flags |= 1;

    ((Slot0x14CFn)(*(void***)this)[0x14C / 4])(this, 1, 1);
    ((SItemRenderGate*)this)->OnPending();
    StoreAndNotifyHandle(0);
    ((Slot0x198Fn)(*(void***)this)[0x198 / 4])(this);
    NotifyTargetLost(-1, -1);
}

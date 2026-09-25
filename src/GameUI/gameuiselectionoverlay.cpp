//============================================================================
//  0x6F37A3B0 - CGameUISelectionOverlay::ClearSelection.
//
//  Tell whatever the overlay is holding that it has been deselected, then
//  drop the reference.  The body is almost entirely reference-count
//  bookkeeping: the held object is pinned for the whole function, a checked
//  copy of it is taken and released, and *that* copy - not the original - is
//  what receives the notification when it resolves to a live, non-pending
//  handle carrying flag 0x1000.
//
//  Two scoped smart pointers are what puts the `__except_handler4`-shaped
//  frame this toolchain cannot reproduce around the body
//  (docs/msvc-vc8-idioms.md); the release sequences themselves are the same
//  `+= -1` then separate `== 0` test agenttypedslots.h's Release() already
//  spells, inlined at each exit.
//
//  Own translation unit: 0x6F34D760 is a real call and a different shipped
//  module.
//============================================================================
#include "gameui.h"
#include "agenttypedslots.h"   // SCheckedDestructableSlot (0x6F012CF0)
#include "gameuithunks.h"
#include "gameuiagenthandle.h"

//  The reference-counted object the overlay holds: vtable slot 0 is
//  ReleaseSelf and the count is at +0x04.
struct SOverlaySubject
{
    void*        m_vtable;          // +0x00
    int          m_refcount;        // +0x04
    char         m_reserved08[0x18];
    unsigned int m_flags;           // +0x20
};

//  0x6F34D760 - gameuiagentrefcopy.cpp.
struct SAgentRefHolder
{
    void* m_pAgent;
    SCheckedDestructableSlot* __thiscall CopyRef(SCheckedDestructableSlot* out) const;
};

//  slot 0x190 of the subject: "you are no longer selected".
typedef void (__thiscall *NotifyDeselectedFn)(void* self, int a, int b);

enum { kOverlayNotifyFlag = 0x1000 };

static void ReleaseSubject(SOverlaySubject* subject)
{
    subject->m_refcount += -1;
    if (subject->m_refcount == 0)
        ((void (__thiscall *)(void*))((void**)subject->m_vtable)[0])(subject);
}

void CGameUISelectionOverlay::ClearSelection()
{
    SOverlaySubject* held = m_pSelected;

    if (held != 0)
        held->m_refcount += 1;

    if (held != 0)
    {
        SAgentRefHolder holder;
        holder.m_pAgent = held;

        SCheckedDestructableSlot checkedSlot;
        holder.CopyRef(&checkedSlot);

        SOverlaySubject* checked = (SOverlaySubject*)checkedSlot.m_value;

        if (checked != 0)
            checked->m_refcount += 1;

        if (checkedSlot.m_value != 0)
            ReleaseSubject((SOverlaySubject*)checkedSlot.m_value);

        SOverlaySubject* target = held;

        if (checked != 0)
        {
            SItemHandleObject* handle = (SItemHandleObject*)
                LookupHandle(((SAgentHandlePair*)checked)->m_handle,
                             ((SAgentHandlePair*)checked)->m_handleType);

            SItemHandleObject* resolved =
                (handle != 0 && handle->m_kindTag == kAgentTagPathable)
                    ? handle : 0;

            if (resolved != 0 && resolved->m_pendingFlag == 0
                && (checked->m_flags & kOverlayNotifyFlag) != 0)
                target = checked;
        }

        ((NotifyDeselectedFn)(*(void***)target)[0x190 / 4])(target, 0, 0);

        if (checked != 0)
            ReleaseSubject(checked);
    }

    SOverlaySubject* current = m_pSelected;

    if (current != 0)
    {
        ReleaseSubject(current);
        m_pSelected = 0;
    }

    if (held != 0)
        ReleaseSubject(held);
}

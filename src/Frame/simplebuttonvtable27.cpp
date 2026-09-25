//============================================================================
//  0x6F603440 - CSimpleButton's own slot 27 (+0x6C): FireClickEvent.  If
//  m_124 (the button's own click observer) is set, build a
//  CSimpleButtonClickEvent on the stack (id 0, m_128 as its extra data,
//  this button, and the raw message id slots 20/21 pass) and dispatch it
//  through the observer's own slot 3 - the shape CObserver::Method_0x0C
//  (Agent/observer.h) declares, though nothing in this batch confirms
//  CObserver is the real base rather than a sibling with the same slot.
//
//  Wrapped in an __except_handler4 frame in the shipped build (/GS- /EHs-c-
//  removes it here, per every other constructor/destructor in this file -
//  docs/msvc-vc8-idioms.md).
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleButton::Method_0x6Cb(int id)
{
    if (m_124 == 0)
        return;

    CSimpleButtonClickEvent event;
    event.m_vtable    = (void*)kCSimpleButtonClickEventVftable;
    event.m_eventId   = 0;
    event.m_extraData = m_128;
    event.m_pButton   = this;
    event.m_buttonId  = id;

    typedef int (__thiscall *Fn)(void*, void*);
    ((Fn)(*(void***)m_124)[3])(m_124, &event);
}

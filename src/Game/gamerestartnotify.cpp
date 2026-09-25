//============================================================================
//  0x6F426AE0 - announce that a load has been requested.
//
//  Two halves and both are the shape the unit-order path already has: a
//  four-word `CEvent` record built on the stack and handed to the object's
//  own vtable slot 4, then a timer armed on the same object at +0x2A8.  The
//  record's destructor is only in the shipped function's unwind funclet -
//  the main body never re-stamps the vptr - so there is nothing to write out
//  here (see docs/msvc-vc8-idioms.md, "A stack `CEvent`'s base-class vtable
//  stamp belongs in the source", for the case where there is).
//============================================================================
#include "unitorder.h"      // SOrderEvent, g_vftCEvent, PostAgentEventFn
#include "cfloat.h"

//  0x6F4778F0 - Item/item_remove.cpp already defines this redirect; the
//  declaration is repeated rather than a second name invented for one
//  shipped address.
struct SAgentTimerArm
{
    void Arm(const CFloat* delay, int eventId, void* subject, int a, int b);
};

//: dword_6FAAE470 - the shared zero CFloat.
extern "C" const CFloat g_CFloatZero;

void __fastcall PostReloadRequestedEvent(void* host)
{
    SOrderEvent event;
    event.m_reserved04 = 0;
    event.m_vtable = &g_vftCEvent;
    event.m_msgId = 0x80201;
    event.m_pOrder = 0;

    ((PostAgentEventFn)(*(void***)host)[0x10 / 4])(host, &event);

    ((SAgentTimerArm*)((char*)host + 0x2A8))->Arm(&g_CFloatZero, 0x80271, host, 0, 0);
}

//============================================================================
//  The four event-context accessors the order and spell natives share, all
//  in the 0x6F3C2xxx native module and none calling another:
//
//      0x6F3C2BA0  GetOrderEventOwner   ids 0x80226..0x80228
//      0x6F3C2BE0  GetOrderEventData    ids 0x8024B..0x8024D
//      0x6F3C2C40  GetSpellEventOwner   ids 0x80310..0x80314
//      0x6F3C2C10  GetSpellEventData    ids 0x80321..0x80325
//
//  Each pair is the same body against a different id range: an "owner" one
//  follows the event's own +0x24 handle ref, a "data" one takes the
//  CScriptEventData straight off context slot 5.  See jassevents.h for the
//  0x80200 bias every comparison here is made under.
//
//  All four start their answer at null and overwrite it, which is what the
//  `xor esi,esi` before the id test and the `mov eax,esi` at the shared exit
//  are; the "no event" arm returns the null the call has just produced in
//  eax, so it costs no instruction of its own.
//============================================================================
#include "jassevents.h"

//----------------------------------------------------------------------------
//  0x6F3C2BA0.
//----------------------------------------------------------------------------
void* GetOrderEventOwner()
{
    void* owner = 0;

    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) - 0x80226 <= 2)
    {
        CScriptEvent* event = GetCurrentScriptEvent();
        if (!event)
            return 0;

        SOptionalHandleRef* ref = &event->m_ownerRef;
        owner = ((ref->m_typeTag & ref->m_handle) == -1)
                    ? 0
                    : QueryHandleField0x54(ref);
    }

    return owner;
}

//----------------------------------------------------------------------------
//  0x6F3C2BE0.
//----------------------------------------------------------------------------
void* GetOrderEventData()
{
    void* data = 0;

    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) - 0x8024B <= 2)
    {
        if (!GetCurrentScriptEvent())
            return 0;

        data = GetCurrentScriptEventData();
    }

    return data;
}

//----------------------------------------------------------------------------
//  0x6F3C2C10.
//----------------------------------------------------------------------------
void* GetSpellEventData()
{
    void* data = 0;

    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) - 0x80321 <= 4)
    {
        if (!GetCurrentScriptEvent())
            return 0;

        data = GetCurrentScriptEventData();
    }

    return data;
}

//----------------------------------------------------------------------------
//  0x6F3C2C40.
//----------------------------------------------------------------------------
void* GetSpellEventOwner()
{
    void* owner = 0;

    if ((unsigned int)(GetCurrentEventId() + kEventIdBias) - 0x80310 <= 4)
    {
        CScriptEvent* event = GetCurrentScriptEvent();
        if (!event)
            return 0;

        SOptionalHandleRef* ref = &event->m_ownerRef;
        owner = ((ref->m_typeTag & ref->m_handle) == -1)
                    ? 0
                    : QueryHandleField0x54(ref);
    }

    return owner;
}

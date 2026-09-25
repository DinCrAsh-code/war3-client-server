//============================================================================
//  0x6F3C1D80 - `GetExpiredTimer` "()Htimer;".  Own translation unit: the
//  0x6F3C1D80 neighbourhood, calling nothing else in this batch.
//
//  When the current event has event-data, the timer object sits ready-made
//  at the event data's own +0x20 word (read as a live pointer, not a
//  handle needing resolution - see jassevents.h's own note on that field's
//  several interpretations) and is registered as an agent directly.
//  Otherwise it falls back to the plain "current timer" event-context slot
//  (slot 5) and validates it through ResolveTimerHandle.
//============================================================================
#include "jassevents.h"
#include "jassnatives.h"   // ResolveTimerHandle
#include "itemhandlemain.h"
#include "agentregistry.h"

int GetExpiredTimer()
{
    SItemSlotTableHolder* slots = g_pItemSlotHost->GetSlotTable();
    if (!slots)
        return 0;

    CScriptEventData* data = GetCurrentScriptEventData();
    if (data)
    {
        void* obj = (void*)data->m_ownerRef.m_handle;

        CAgentRegistry* registry = (CAgentRegistry*)g_pItemSlotHost->GetSlotTable();
        return (int)registry->Register((CUnitAgent*)obj, 0);
    }

    int handle = slots->PeekEventSlot(5);
    if (ResolveTimerHandle(handle))
        return handle;

    return 0;
}

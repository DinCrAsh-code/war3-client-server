//============================================================================
//  0x6F3C1D40 and 0x6F3C1D60 - event-context slots 4 and 5, resolved.
//
//  Both are the same five instructions plus a tail jump into their own
//  resolver, and both are in the 0x6F3C1xxx native module; neither calls the
//  other, so they share a translation unit.
//============================================================================
#include "jassevents.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"

//----------------------------------------------------------------------------
//  0x6F3C1D40 - slot 4.
//----------------------------------------------------------------------------
CScriptEvent* GetCurrentScriptEvent()
{
    return ResolveEventHandle(g_pItemSlotHost->GetSlotTable()->PeekEventSlot(4));
}

//----------------------------------------------------------------------------
//  0x6F3C1D60 - slot 5.
//----------------------------------------------------------------------------
CScriptEventData* GetCurrentScriptEventData()
{
    return ResolveEventDataHandle(
        g_pItemSlotHost->GetSlotTable()->PeekEventSlot(5));
}

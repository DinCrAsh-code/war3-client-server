//============================================================================
//  The `timer` natives at 0x6F3C1370..0x6F3C1420 and the two handle-registry
//  destroyers next to them.  See jassnatives.h.
//============================================================================
#include "jassnatives.h"
#include "itemhandlemain.h"

//----------------------------------------------------------------------------
//  0x6F3C1370 - `TimerStart` "(Htimer;RBC)V": a real by pointer, a boolean
//  and a JASS code handle, all passed on unchanged.
//----------------------------------------------------------------------------
void __cdecl JASS_TimerStart(int hTimer, const int* seconds, int periodic, int callback)
{
    SJassTimer* timer = ResolveTimerHandle(hTimer);

    if (timer)
        timer->Start(seconds, periodic, callback);
}

//----------------------------------------------------------------------------
//  0x6F3C13A0 - `TimerGetElapsed` "(Htimer;)R".  GetElapsed writes the real
//  into the caller's own one-dword buffer and hands its address back; the
//  native then loads that dword as the result, which is why there is both a
//  local and a dereference of the returned pointer.
//----------------------------------------------------------------------------
int __cdecl JASS_TimerGetElapsed(int hTimer)
{
    SJassTimer* timer = ResolveTimerHandle(hTimer);

    if (!timer)
        return 0;

    int elapsed;
    return *timer->GetElapsed(&elapsed);
}

//----------------------------------------------------------------------------
//  0x6F3C1420 - `ResumeTimer` "(Htimer;)V", a tail jump into Resume.
//----------------------------------------------------------------------------
void __cdecl JASS_ResumeTimer(int hTimer)
{
    SJassTimer* timer = ResolveTimerHandle(hTimer);

    if (timer)
        timer->Resume();
}

//----------------------------------------------------------------------------
//  0x6F3C1440 / 0x6F3C1470 - `DestroyCondition` "(Hconditionfunc;)V" and
//  `DestroyFilter` "(Hfilterfunc;)V".  Two separate entry points with
//  byte-identical bodies: JASS keeps `conditionfunc` and `filterfunc` as
//  distinct types, and the two natives are the same code compiled twice
//  rather than one alias, which is what two addresses in the registration
//  table means.
//
//  Both fetch the registry singleton *before* resolving the handle - the
//  shipped code calls GetSlotTable first and keeps it in esi across the
//  resolve - so the singleton is named first here too.
//----------------------------------------------------------------------------
void __cdecl JASS_DestroyCondition(int hCondition)
{
    CJassHandleRegistry* registry = (CJassHandleRegistry*)g_pItemSlotHost->GetSlotTable();

    SJassBoolExpr* condition = ResolveFilterHandle(hCondition);

    if (condition)
        registry->ReleaseObject(condition);
}

void __cdecl JASS_DestroyFilter(int hFilter)
{
    CJassHandleRegistry* registry = (CJassHandleRegistry*)g_pItemSlotHost->GetSlotTable();

    SJassBoolExpr* filter = ResolveFilterHandle(hFilter);

    if (filter)
        registry->ReleaseObject(filter);
}

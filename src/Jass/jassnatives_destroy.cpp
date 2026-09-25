//============================================================================
//  The "destroy this handle" natives - six entry points across five handle
//  types, every one of them the same five instructions: resolve, null-check,
//  tail jump through vtable slot 23.  Six separate addresses in the
//  registration table, so six separate functions in the shipped source, not
//  one shared helper.  See jassnatives.h.
//============================================================================
#include "jassnatives.h"

//  0x6F3C1350 - `DestroyTimer` "(Htimer;)V".
void __cdecl JASS_DestroyTimer(int hTimer)
{
    SJassTimer* timer = ResolveTimerHandle(hTimer);

    if (timer)
        JassDestroySelf(timer);
}

//  0x6F3C14A0 - `DestroyBoolExpr` "(Hboolexpr;)V".
void __cdecl JASS_DestroyBoolExpr(int hBoolExpr)
{
    SJassBoolExpr* expr = ResolveBoolExprHandle(hBoolExpr);

    if (expr)
        JassDestroySelf(expr);
}

//  0x6F3C14C0 - `RemoveRegion` "(Hregion;)V".
void __cdecl JASS_RemoveRegion(int hRegion)
{
    SJassRegion* region = ResolveRegionHandle(hRegion);

    if (region)
        JassDestroySelf(region);
}

//  0x6F3C16F0 - `RemoveRect` "(Hrect;)V".
void __cdecl JASS_RemoveRect(int hRect)
{
    SJassRect* rect = ResolveRectHandle(hRect);

    if (rect)
        JassDestroySelf(rect);
}

//  0x6F3C1BA0 - `DestroyFogModifier` "(Hfogmodifier;)V".
void __cdecl JASS_DestroyFogModifier(int hFogModifier)
{
    SJassFogModifier* modifier = ResolveFogModifierHandle(hFogModifier);

    if (modifier)
        JassDestroySelf(modifier);
}

//  0x6F3C1C00 - `DestroyTrigger` "(Htrigger;)V".
void __cdecl JASS_DestroyTrigger(int hTrigger)
{
    SJassTrigger* trigger = ResolveTriggerHandle(hTrigger);

    if (trigger)
        JassDestroySelf(trigger);
}

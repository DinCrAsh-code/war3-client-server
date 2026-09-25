//============================================================================
//  The `trigger` state/property natives at 0x6F3C1C40..0x6F3C1D36, right
//  after JASS_ResetTrigger (0x6F3C1C20, jassnatives_destroy.cpp's sibling
//  module) and before the JASS `timer` expiry native.  None of these calls
//  another, so one translation unit.  See jassnatives.h for
//  SJassTrigger's own +0x64/+0x78/+0x58/+0x5C fields.
//============================================================================
#include "jassnatives.h"

//----------------------------------------------------------------------------
//  0x6F3C1C40 - `EnableTrigger` "(Htrigger;)V".
//----------------------------------------------------------------------------
void __cdecl JASS_EnableTrigger(int hTrigger)
{
    SJassTrigger* trigger = ResolveTriggerHandle(hTrigger);

    if (trigger)
        trigger->m_enabledOwner.SetField78(1);
}

//----------------------------------------------------------------------------
//  0x6F3C1C60 - `DisableTrigger` "(Htrigger;)V".
//----------------------------------------------------------------------------
void __cdecl JASS_DisableTrigger(int hTrigger)
{
    SJassTrigger* trigger = ResolveTriggerHandle(hTrigger);

    if (trigger)
        trigger->m_enabledOwner.SetField78(0);
}

//----------------------------------------------------------------------------
//  0x6F3C1C80 - `IsTriggerEnabled` "(Htrigger;)B".  Tail jump into
//  QueryField78 when the trigger resolves; the shipped `jmp` is what makes
//  the failure arm's `retn` return whatever the failed resolve already left
//  in eax (0) instead of an explicit `xor eax,eax`.
//----------------------------------------------------------------------------
int __cdecl JASS_IsTriggerEnabled(int hTrigger)
{
    SJassTrigger* trigger = ResolveTriggerHandle(hTrigger);

    if (!trigger)
        return 0;

    return trigger->m_enabledOwner.QueryField78();
}

//----------------------------------------------------------------------------
//  0x6F3C1CA0 - `TriggerWaitOnSleeps` "(HtriggerB)V".  A plain field store,
//  not a vtable dispatch - +0x78 is a raw int, unlike +0x64's owner object.
//----------------------------------------------------------------------------
void __cdecl JASS_TriggerWaitOnSleeps(int hTrigger, int waitOnSleeps)
{
    SJassTrigger* trigger = ResolveTriggerHandle(hTrigger);

    if (trigger)
        trigger->m_waitOnSleeps = waitOnSleeps;
}

//----------------------------------------------------------------------------
//  0x6F3C1CC0 - `IsTriggerWaitOnSleeps` "(Htrigger;)B".  Reads +0x78 straight
//  back, no dispatch.
//----------------------------------------------------------------------------
int __cdecl JASS_IsTriggerWaitOnSleeps(int hTrigger)
{
    SJassTrigger* trigger = ResolveTriggerHandle(hTrigger);

    if (!trigger)
        return 0;

    return trigger->m_waitOnSleeps;
}

//----------------------------------------------------------------------------
//  0x6F3C1CE0 - `GetTriggerEvalCount` "(Htrigger;)I".
//----------------------------------------------------------------------------
int __cdecl JASS_GetTriggerEvalCount(int hTrigger)
{
    SJassTrigger* trigger = ResolveTriggerHandle(hTrigger);

    if (!trigger)
        return 0;

    return trigger->m_evalCount;
}

//----------------------------------------------------------------------------
//  0x6F3C1D00 - `GetTriggerExecCount` "(Htrigger;)I".
//----------------------------------------------------------------------------
int __cdecl JASS_GetTriggerExecCount(int hTrigger)
{
    SJassTrigger* trigger = ResolveTriggerHandle(hTrigger);

    if (!trigger)
        return 0;

    return trigger->m_execCount;
}

//----------------------------------------------------------------------------
//  0x6F3C1D20 - `GetTriggerDestructable` "()Hdestructable;".  Takes no
//  handle argument at all: it reads the *current* destructable event slot
//  (event-context slot 1, 0x6F3BB260) and validates it through
//  ResolveDestructableHandle (0x6F3BE010, jassresolvedestructablehandle.cpp)
//  - the same nine-in-the-family resolver shape, used here purely as a
//  boolean check.  `neg eax` / `sbb eax,eax` / `and eax,esi` is a checked
//  cast written as a conditional expression, the same idiom
//  agenthandlelive.cpp's own header comment names.
//----------------------------------------------------------------------------
int __cdecl JASS_GetTriggerDestructable()
{
    int handle = GetDestructableEventSlotHandle();

    return ResolveDestructableHandle(handle) ? handle : 0;
}

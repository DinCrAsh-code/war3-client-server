//============================================================================
//  0x6F2CFE10 - SAbilityHostRecord::NotifyObserverRegistrations
//  (abilityhostrecord.h).
//============================================================================
#include "abilityhostrecord.h"
#include "missile_thunderbolt.h"
#include "widget.h"

//  0x6F2AB3E0 - the real sibling registration entry point, reconstructed by
//  a different parallel agent as a standalone leaf (Misc/misc_notifyorforward_
//  event.cpp, EXACT) rather than as a CMissileThunderBoltRefs member; called
//  through this locally-redeclared dispatch struct, same convention as that
//  file's own Misc/cunit_agent8_batch2.cpp caller.
struct SEventNotifyOrForwardHost
{
    void NotifyOrForwardEvent(void* target, int flag);
};

void SAbilityHostRecord::NotifyObserverRegistrations(void* target, int add,
                                                       int doThunderBolt,
                                                       int doAlt,
                                                       int doWidgetEvent)
{
    if (doThunderBolt)
        ((CMissileThunderBoltRefs*)target)->SetObserverRegistration(this, add);
    if (doAlt)
        ((SEventNotifyOrForwardHost*)target)->NotifyOrForwardEvent(this, add);
    if (doWidgetEvent)
        ((CWidget*)target)->PostAgentEvent2(this, add);
}

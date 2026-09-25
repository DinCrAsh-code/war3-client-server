//============================================================================
//  0x6F291790 - CUnit's vtable slot 33 (+0x84), SetPending.  See unit.h.
//
//  CWar3Image's own body at this slot (0x6F0520B0, agentwar3_flagbits.cpp)
//  sets a flag bit; CUnit's does not touch the flags word at all.  It
//  notifies the visibility side through slot 38 and then re-runs its own
//  pending-state refresh with (1, 0).
//============================================================================
#include "unit.h"

//  slot 38 (+0x98) - CWar3Image::NotifyWidgetVisibilityChanged
//  (widgetvisibility.cpp), reached through the vtable and with no
//  arguments.  Same dispatch widget_spriteanim.cpp already makes.
typedef void (__thiscall *Slot0x98Fn)(void* self);

void CUnit::SetPending()
{
    ((Slot0x98Fn)(*(void***)this)[0x98 / 4])(this);
    RefreshPendingState(1, 0);
}

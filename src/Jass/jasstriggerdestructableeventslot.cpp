//============================================================================
//  0x6F3BB260 - the destructable event slot's raw handle: event-context
//  slot 1 of the singleton's push-down stack (jassnatives.h's
//  SItemSlotTableHolder::PeekEventSlot, the same member
//  jasseventcontext.cpp calls against slots 4/5).  GetTriggerDestructable
//  (jassnatives_trigger.cpp) is the one caller in this batch.
//
//  Own translation unit: in the 0x6F3Bxxxx JASS-resolver module, calling
//  nothing else in this batch.
//============================================================================
#include "jassnatives.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"

int GetDestructableEventSlotHandle()
{
    return g_pItemSlotHost->GetSlotTable()->PeekEventSlot(1);
}

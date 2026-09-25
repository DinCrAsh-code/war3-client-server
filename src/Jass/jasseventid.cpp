//============================================================================
//  0x6F3BB2C0 - the current event id: the top of event-context slot 6.
//
//  Own translation unit in the 0x6F3Bxxxx native module.  The singleton is
//  loaded at *this* call site rather than inside GetSlotTable - see
//  itemhandlemain.h, which records why that load belongs to the caller.
//============================================================================
#include "jassevents.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"

int GetCurrentEventId()
{
    return g_pItemSlotHost->GetSlotTable()->PeekEventSlot(6);
}

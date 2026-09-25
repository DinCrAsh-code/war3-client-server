//============================================================================
//  0x6F3BE010 - ResolveDestructableHandle: turn the current destructable
//  event slot's raw handle into a validated object, or null.
//
//  The tenth member of the same 115-instruction JASS-resolver family
//  jassresolvetriggerhandle.cpp documents - see that file for the two
//  load-bearing shapes (singleton fetched before the null test; the
//  failure path really does re-run Construct(0) rather than returning a
//  bare 0).  This one is not a JASS handle resolver at all (its slot is
//  SCheckedDestructableSlot, agenttypedslots.h's '+w3d' entry, called
//  Construct rather than a converting constructor because its own callers
//  treat the slot as raw storage - see that struct's own comment) - it is
//  reached from GetTriggerDestructable (jassnatives_trigger.cpp), the one
//  caller in this batch, purely for its truthiness.
//
//  Own translation unit: in the 0x6F3Bxxxx JASS-resolver module and calls
//  nothing else in this batch.
//============================================================================
#include "jassnatives.h"
#include "agenttypedslots.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"
#include "game.h"

CAgent* __fastcall ResolveDestructableHandle(int handle)
{
    SItemSlotTableHolder* singleton = g_pItemSlotHost->GetSlotTable();

    if (!handle)
        return 0;

    CAgent* candidate = singleton->QueryItemSlot(handle);
    SCheckedDestructableSlot checked;
    checked.Construct(candidate);

    CAgent* object = checked.m_value;
    bool validated = false;
    if (object)
    {
        SItemHandleObject* handleObj =
            (SItemHandleObject*)LookupHandle(object->m_handle, object->m_typeTag);
        if (handleObj)
        {
            SItemHandleObject* tagOk =
                (handleObj->m_kindTag != 0x2B61676C) ? 0 : handleObj;
            if (tagOk)
                validated = (tagOk->m_pendingFlag == 0);
        }
    }

    if (validated)
        return checked.m_value;

    SCheckedDestructableSlot fallback;
    fallback.Construct(0);
    return fallback.m_value;
}

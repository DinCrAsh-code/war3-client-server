//============================================================================
//  0x6F3BE250 - ResolveEventDataHandle: turn a JASS event-data handle into
//  the CScriptEventData behind it, or null.
//
//  The twelfth copy of the same 115-instruction resolver body, and the type
//  comes from the dump the same way: both of its constructor calls are
//  0x6F27C350, whose Assign (0x6F271E90) checks 0x77736364.  That tag is
//  CScriptEventData's own - ??_7CScriptEventData@@6B@'s slot 7 is literally
//  `mov eax, 77736364h` / `retn` - so the class name here is the binary's
//  and not an inference from the shape.
//
//  Own translation unit, like every other resolver.
//============================================================================
#include "jassevents.h"
#include "agenttypedslots.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"
#include "game.h"

CScriptEventData* __fastcall ResolveEventDataHandle(int hEventData)
{
    SItemSlotTableHolder* singleton = g_pItemSlotHost->GetSlotTable();

    if (!hEventData)
        return 0;

    CAgent* candidate = singleton->QueryItemSlot(hEventData);
    SCheckedEventDataSlot checked(candidate);

    CAgent* data = checked.m_value;
    bool validated = false;
    if (data)
    {
        SItemHandleObject* handleObj =
            (SItemHandleObject*)LookupHandle(data->m_handle, data->m_typeTag);
        if (handleObj)
        {
            SItemHandleObject* tagOk =
                (handleObj->m_kindTag != 0x2B61676C) ? 0 : handleObj;
            if (tagOk)
                validated = (tagOk->m_pendingFlag == 0);
        }
    }

    if (validated)
        return (CScriptEventData*)checked.m_value;

    SCheckedEventDataSlot fallback(0);
    return (CScriptEventData*)fallback.m_value;
}

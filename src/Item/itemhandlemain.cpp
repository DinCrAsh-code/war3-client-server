//============================================================================
//  See itemhandlemain.h.
//============================================================================
#include "itemhandlemain.h"
#include "itemtypedassign.h"
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F3BEB50.  Two local SCheckedItemSlot temporaries, exactly matching
//  the dump's own two release-if-last cleanups at scope exit: `checked`
//  always exists; `fallback` is only constructed on the path where
//  `checked`'s own candidate did not pass the second (global-handle-table)
//  validation, and always holds null (it is constructed from 0).
//----------------------------------------------------------------------------
CAgent* __fastcall ResolveItemHandle(int hItem)
{
    SItemSlotTableHolder* singleton = g_pItemSlotHost->GetSlotTable();

    if (!hItem)
        return 0;

    CAgent* candidate = singleton->QueryItemSlot(hItem);
    SCheckedItemSlot checked(candidate);

    CAgent* item = checked.m_value;
    bool validated = false;
    if (item)
    {
        SItemHandleObject* handleObj = (SItemHandleObject*)LookupHandle(item->m_handle, item->m_typeTag);
        if (handleObj)
        {
            SItemHandleObject* tagOk = (handleObj->m_kindTag != 0x2B61676C) ? 0 : handleObj;
            if (tagOk)
                validated = (tagOk->m_pendingFlag == 0);
        }
    }

    if (validated)
        return checked.m_value;

    SCheckedItemSlot fallback(0);
    return fallback.m_value;
}

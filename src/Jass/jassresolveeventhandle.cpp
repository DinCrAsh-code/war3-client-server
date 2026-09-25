//============================================================================
//  0x6F3BE130 - ResolveEventHandle: turn a JASS `event` handle into the
//  CScriptEvent behind it, or null.
//
//  The eleventh copy of the 115-instruction resolver body
//  docs/targets/JASS_Location.md tabulates, and the dump settles which type
//  it is without any guessing: both of its constructor calls are 0x6F3BA320,
//  whose Assign (0x6F3B7560) checks '+evt' (0x6F4327D0).
//
//  Own translation unit, next to the other ten: all of them are in the
//  0x6F3Bxxxx JASS-resolver module and none calls another.  The two
//  load-bearing shapes are the ones jassresolvelocationhandle.cpp records -
//  the singleton is fetched *before* the null test on the handle, and the
//  failure path constructs a second slot from 0 rather than returning a
//  bare zero.
//============================================================================
#include "jassevents.h"
#include "agenttypedslots.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"
#include "game.h"

CScriptEvent* __fastcall ResolveEventHandle(int hEvent)
{
    SItemSlotTableHolder* singleton = g_pItemSlotHost->GetSlotTable();

    if (!hEvent)
        return 0;

    CAgent* candidate = singleton->QueryItemSlot(hEvent);
    SCheckedEventSlot checked(candidate);

    CAgent* event = checked.m_value;
    bool validated = false;
    if (event)
    {
        SItemHandleObject* handleObj =
            (SItemHandleObject*)LookupHandle(event->m_handle,
                                             event->m_typeTag);
        if (handleObj)
        {
            SItemHandleObject* tagOk =
                (handleObj->m_kindTag != 0x2B61676C) ? 0 : handleObj;
            if (tagOk)
                validated = (tagOk->m_pendingFlag == 0);
        }
    }

    if (validated)
        return (CScriptEvent*)checked.m_value;

    SCheckedEventSlot fallback(0);
    return (CScriptEvent*)fallback.m_value;
}

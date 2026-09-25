//============================================================================
//  0x6F3BDDD0 - ResolveFogModifierHandle: turn a JASS `fogmodifier` handle into the object
//  behind it, or null.
//
//  Was a naked `jmp` thunk.  One of nine copies of a single 115-instruction
//  body, one per handle type, differing only in which type-checked slot they
//  park the candidate in - and the dump settles which that is without a
//  guess, because both of its constructor calls are 0x6F095D70, the '+fgm'
//  constructor (checkedfogmodifierslot.cpp).
//
//  See jassresolvelocationhandle.cpp for the two load-bearing shapes: the
//  singleton is fetched *before* the null test on the handle, and the
//  failure path really does construct a second slot from 0 and return its
//  null rather than returning a bare 0.
//
//  Own translation unit: every member of this family is in the 0x6F3Bxxxx
//  JASS-resolver module and none calls another.
//============================================================================
#include "jassnatives.h"
#include "agenttypedslots.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"
#include "game.h"

SJassFogModifier* __fastcall ResolveFogModifierHandle(int handle)
{
    SItemSlotTableHolder* singleton = g_pItemSlotHost->GetSlotTable();

    if (!handle)
        return 0;

    CAgent* candidate = singleton->QueryItemSlot(handle);
    SCheckedFogModifierSlot checked(candidate);

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
        return (SJassFogModifier*)checked.m_value;

    SCheckedFogModifierSlot fallback(0);
    return (SJassFogModifier*)fallback.m_value;
}

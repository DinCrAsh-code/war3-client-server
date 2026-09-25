//============================================================================
//  0x6F3BDCB0 - ResolveUnitHandle: turn a JASS `unit` handle into the
//  CUnit behind it, or null.
//
//  Was a naked `jmp` thunk.  It is the unit twin of ResolveItemHandle
//  (0x6F3BEB50, itemhandlemain.cpp) and its body is that function line for
//  line, with SCheckedItemSlot replaced by SCheckedUnitSlot
//  (agenttypedslots.h) - the same singleton bootstrap, the same index
//  lookup, the same type-checked local, the same second validation
//  through the *global* handle table, and the same "return the fallback's
//  null" tail.
//
//  Own translation unit, next to the item one: both are in the 0x6F3Bxxxx
//  JASS-resolver module and neither calls the other.
//
//  Two shapes are load-bearing, both inherited from the item twin:
//
//   * the singleton is fetched *before* the null test on the handle.  The
//     shipped code calls GetSlotTable and only then does `test esi,esi`,
//     so the call is unconditional and the early-out is not.
//
//   * `fallback` is a second SCheckedUnitSlot constructed from 0, not a
//     bare `return 0;`.  The shipped code really does construct a second
//     one on the failure path and release it - that is the second
//     `call sub_6F038450` at 0x6F3BDD5D and the `bl` flag that guards its
//     own scope-exit release.
//============================================================================
#include "jassnatives.h"
#include "agenttypedslots.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"
#include "game.h"

CUnit* __fastcall ResolveUnitHandle(int hUnit)
{
    SItemSlotTableHolder* singleton = g_pItemSlotHost->GetSlotTable();

    if (!hUnit)
        return 0;

    CAgent* candidate = singleton->QueryItemSlot(hUnit);
    SCheckedUnitSlot checked(candidate);

    CAgent* unit = checked.m_value;
    bool validated = false;
    if (unit)
    {
        SItemHandleObject* handleObj =
            (SItemHandleObject*)LookupHandle(unit->m_handle, unit->m_typeTag);
        if (handleObj)
        {
            //  The `setz`/`neg`/`sbb`/`and` chain in the shipped code is
            //  MSVC's branchless spelling of exactly this: a bool from the
            //  tag comparison, widened to a mask, and'd with the pointer.
            SItemHandleObject* tagOk =
                (handleObj->m_kindTag != 0x2B61676C) ? 0 : handleObj;
            if (tagOk)
                validated = (tagOk->m_pendingFlag == 0);
        }
    }

    if (validated)
        return (CUnit*)checked.m_value;

    SCheckedUnitSlot fallback(0);
    return (CUnit*)fallback.m_value;
}

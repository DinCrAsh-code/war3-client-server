//============================================================================
//  0x6F3BD3B0 - ResolveLocationHandle: turn a JASS `location` handle into
//  the object behind it, or null.
//
//  Was a naked `jmp` thunk, and the most-used one in this family: seven of
//  the location natives go through it (RemoveLocation, MoveLocation,
//  GetLocationX/Y/Z, IsLocationInRegion, DefineStartLocationLoc), so it was
//  115 instructions of shipped code standing between every one of them and
//  a hookable tree.
//
//  It is the location twin of ResolveUnitHandle (0x6F3BDCB0,
//  jassresolveunithandle.cpp) and ResolveItemHandle (0x6F3BEB50,
//  itemhandlemain.cpp): the same body line for line with SCheckedUnitSlot
//  replaced by SCheckedLocationSlot (agenttypedslots.h).  The dump settles
//  which one it is without any guessing - both of its `call sub_6F3B8FA0`
//  sites are the '+loc' constructor the `Location` native itself uses
//  (checkedlocationslot.cpp).
//
//  Own translation unit, next to the item and unit ones: all three are in
//  the 0x6F3Bxxxx JASS-resolver module and none calls another.
//
//  The two load-bearing shapes are inherited from those twins:
//
//   * the singleton is fetched *before* the null test on the handle.  The
//     shipped code calls GetSlotTable and only then does `test esi,esi`, so
//     the call is unconditional and the early-out is not.
//
//   * `fallback` is a second SCheckedLocationSlot constructed from 0, not a
//     bare `return 0;`.  The shipped code really does construct a second one
//     on the failure path and release it - the second `call sub_6F3B8FA0`
//     at 0x6F3BD45D and the `bl` flag that guards its own scope-exit
//     release.
//============================================================================
#include "jassnatives.h"
#include "agenttypedslots.h"
#include "itemhandlemain.h"
#include "itemhandletable.h"
#include "game.h"

SJassLocation* __fastcall ResolveLocationHandle(int hLocation)
{
    SItemSlotTableHolder* singleton = g_pItemSlotHost->GetSlotTable();

    if (!hLocation)
        return 0;

    CAgent* candidate = singleton->QueryItemSlot(hLocation);
    SCheckedLocationSlot checked(candidate);

    CAgent* location = checked.m_value;
    bool validated = false;
    if (location)
    {
        SItemHandleObject* handleObj =
            (SItemHandleObject*)LookupHandle(location->m_handle,
                                             location->m_typeTag);
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
        return (SJassLocation*)checked.m_value;

    SCheckedLocationSlot fallback(0);
    return (SJassLocation*)fallback.m_value;
}

//============================================================================
//  0x6F3C7780 - JASS_GetUnitCurrentOrder, `(Hunit;)I`.
//
//  Two validations and then one field read.  Both validations are shapes
//  this repo has already matched elsewhere, which is the only reason this
//  is a short file:
//
//   1. the unit must still be *live in the agent table* - resolve its own
//      {handle, typeTag} pair at CAgent+0x0C/+0x10, require the 'lga+' tag
//      (0x2B61676C) and a clear pending flag.  That is
//      jassresolveunithandle.cpp's own tail, and the `setnz`/`sub`/`and`
//      chain is MSVC's branchless spelling of the tag test - a bool from
//      the comparison, widened to a mask, and'd with the pointer.
//
//   2. the order-holder ref at CUnit+0x19C must be set - both words
//      all-ones is the "unset" sentinel, tested as a single AND against -1
//      exactly the way itemhandleresolve.h documents.
//
//  Then the order id is the resolved object's +0x24.  Every refusal
//  answers 0, which is JASS's ORDER_ID_NONE.
//
//  All six refusals share one `xor eax,eax` / `pop esi` / `retn` at the
//  very end, reached only by falling through - so this is the *nested*
//  guard shape, not the early-return one.  See docs/msvc-vc8-idioms.md.
//
//  `this + 0x19C` is formed before the sentinel test (`lea ecx,[esi+19Ch]`
//  at 0x6F3C77C6, the `cmp` at 0x6F3C77CC), which is what taking the
//  member's address into a named pointer does.
//============================================================================
#include "jassnatives.h"
#include "unit.h"
#include "agent.h"
#include "game.h"
#include "itemhandleresolve.h"

//  The object CUnit+0x19C names.  Only the order id is read here.
struct SUnitOrderHolder
{
    char m_reserved00[0x24];
    int  m_orderId;       // +0x24
};

int __cdecl JASS_GetUnitCurrentOrder(int hUnit)
{
    CUnit* unit = ResolveUnitHandle(hUnit);
    if (unit)
    {
        CAgent* agent = (CAgent*)unit;
        SItemHandleObject* handleObj =
            (SItemHandleObject*)LookupHandle(agent->m_handle, agent->m_typeTag);
        if (handleObj)
        {
            SItemHandleObject* tagOk =
                (handleObj->m_kindTag != 0x2B61676C) ? 0 : handleObj;
            if (tagOk && tagOk->m_pendingFlag == 0)
            {
                SOptionalHandleRef* ref = (SOptionalHandleRef*)&unit->m_ref19C;
                if ((ref->m_typeTag & (int)ref->m_handle) != -1)
                {
                    SUnitOrderHolder* holder =
                        (SUnitOrderHolder*)QueryHandleField0x54(ref);
                    if (holder)
                        return holder->m_orderId;
                }
            }
        }
    }
    return 0;
}

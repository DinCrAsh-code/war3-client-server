//============================================================================
//  0x6F2832E0 - CUnit::IsOrderAlreadyActive: is the unit already carrying
//  this order out?
//
//  Three questions, each of which answers no on its own: is there a current
//  order at all (+0x19C set and still resolvable), is there an ability that
//  would take the new one (CUnit::FindAbilityForOrder), and does that
//  ability say it is already running it (its own slot 0x22C).  Only the
//  third can answer yes.
//
//  Own translation unit: FindAbilityForOrder is a real call out of it.
//============================================================================
#include "unit.h"
#include "unitorder.h"

int CUnit::IsOrderAlreadyActive(void* order)
{
    //  Early return, not a nested guard: the shipped stream has the
    //  `xor eax,eax` / `ret 4` physically between the two tests and the
    //  ability lookup, which is what a short-circuited `||` followed by a
    //  fall-through `return 0` produces.  Written as nested `if`s the two
    //  refusals get tail-merged past the whole body instead.
    SOptionalHandleRef* head = (SOptionalHandleRef*)&m_ref19C;
    if ((head->m_typeTag & (int)head->m_handle) == -1
        || QueryHandleField0x54(head) == 0)
        return 0;

    void* ability = FindAbilityForOrder(order);
    if (ability == 0)
        return 0;

    return ((AbilityIsRunningOrderFn)
            (*(void***)ability)[0x22C / 4])(ability, order);
}

//============================================================================
//  0x6F279560 - CUnit::CheckPointOrder: could any of this unit's abilities
//  take this order, aimed at a point on the ground?
//
//  Walks the intrusive ability list at +0x1DC and asks each candidate in
//  turn, keeping the *lowest* refusal code anybody gave and starting from
//  0xDD ("no ability of yours does that").  A candidate whose own source
//  object is not the one the caller asked about is skipped with that same
//  0xDD; one that accepts is then gated on its three counters at +0x3C,
//  +0x40 and +0x44, and the first candidate that clears all three makes the
//  whole function return 0 without looking at the rest.
//
//  The 0x3C and 0x44 tests share one `eax = 0xB9` here where the widget
//  version (unitchecktargetorder.cpp) gives each its own - that is the
//  shipped difference between the two, not a spelling choice.
//
//  Own translation unit: nothing here calls anything in the sibling.
//============================================================================
#include "unit.h"
#include "unitorder.h"

int CUnit::CheckPointOrder(int orderId, int player, void* abilitySource,
                           const void* x, const void* y)
{
    int best = 0xDD;

    SOptionalHandleRef* head = (SOptionalHandleRef*)&m_ref1DC;
    if ((head->m_typeTag & (int)head->m_handle) != -1)
    {
        SUnitAbility* ability = (SUnitAbility*)QueryHandleField0x54(head);
        while (ability != 0)
        {
            int result;
            if (abilitySource != ((AbilityGetSourceAgentFn)
                                  (*(void***)ability)[0xA0 / 4])(ability))
            {
                result = 0xDD;
            }
            else
            {
                result = ((AbilityCheckPointOrderFn)
                          (*(void***)ability)[0x25C / 4])(ability, orderId,
                                                          player, x, y, 3);
                if (result == 0)
                {
                    if (ability->m_int40 > 0)
                        result = 0xDD;
                    else if (ability->m_int3C > 0 || ability->m_int44 > 0)
                        result = 0xB9;
                    else
                        return 0;
                }
            }

            if (result < best)
                best = result;

            ability = (SUnitAbility*)ResolveHandleRefOrNull(&ability->m_next);
        }
    }

    return best;
}

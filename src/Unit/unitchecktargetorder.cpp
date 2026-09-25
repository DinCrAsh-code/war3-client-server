//============================================================================
//  0x6F279630 - CUnit::CheckTargetOrder: could any of this unit's abilities
//  take this order, aimed at a widget?
//
//  The widget twin of CUnit::CheckPointOrder (unitcheckpointorder.cpp) and
//  the same walk over the same +0x1DC ability list, with two differences
//  that are both in the shipped stream:
//
//    * it dispatches through slot 0x260 rather than 0x25C, and passes the
//      target widget where the point version passes two coordinates;
//    * a candidate whose answer is 0xDC - the one refusal code the JASS
//      order-result gate (0x6F3B2ED0, jassorderresult.cpp) still counts as
//      success - raises a module-wide flag, and the two "some counter is
//      positive" tests get an assignment each instead of sharing one.
//
//  Own translation unit: nothing here calls anything in the sibling.
//============================================================================
#include "unit.h"
#include "unitorder.h"

//: dword_6FAB4258 - set whenever a candidate ability answered 0xDC.  Only
//: written here; whoever reads it is outside this tree.
extern int g_targetOrderDeferred;

int CUnit::CheckTargetOrder(int orderId, int player, void* abilitySource,
                            void* target)
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
                result = ((AbilityCheckTargetOrderFn)
                          (*(void***)ability)[0x260 / 4])(ability, orderId,
                                                          player, target, 3);
                if (result == 0)
                {
                    if (ability->m_int40 > 0)
                        result = 0xDD;
                    else if (ability->m_int3C > 0)
                        result = 0xB9;
                    else if (ability->m_int44 > 0)
                        result = 0xB9;
                    else
                        return 0;
                }
                else if (result == 0xDC)
                {
                    g_targetOrderDeferred = 1;
                }
            }

            if (result < best)
                best = result;

            ability = (SUnitAbility*)ResolveHandleRefOrNull(&ability->m_next);
        }
    }

    return best;
}

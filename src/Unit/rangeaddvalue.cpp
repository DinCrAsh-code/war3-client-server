//============================================================================
//  0x6F4A92B0 - SHandleObjectRangeImpl::AddToValue: move the published
//  value by `delta`, clamped into the window, and tell the listeners.
//
//  The one mutator the whole life/mana pool goes through.  Four steps, and
//  the shipped body does them in this order:
//
//    1. PrepareRangeUpdate() - settle the ramp so m_value means "now"
//       rather than "whenever it was last written".  It has to come first:
//       everything after reads m_value.
//    2. remember the old m_value, before it moves.
//    3. m_value + delta, clamped into [m_low, m_high].
//    4. NotifyRangeChanged(&m_value, &previous, 0).
//
//  The clamp is the same *pointer select* as CPathTrace::RampValueAt
//  (pathtraceramp.cpp), instruction for instruction, and is spelled the
//  same way here for the same reason: the shipped code keeps the winning
//  candidate's address in edx, steps it from the low bound to the high one
//  with `add edx,4`, and dereferences it once at a single exit.  Three
//  `return`s, or an if/else over the value, make MSVC copy each candidate
//  instead.  The comparison senses are the shipped ones and are not
//  symmetric: a value equal to either bound is the value, not the bound.
//
//  `previous` is a real named local whose address is taken, and the sum is
//  bound as a reference to operator+'s own buffer rather than copied - the
//  shipped code pushes the pointer operator+ returned.
//
//  Own translation unit: it calls PrepareRangeUpdate (0x6F4A90B0) and
//  NotifyRangeChanged (0x6F4A9230) for real, and both are their own TUs
//  already for exactly that reason - see handleobjectrange.h.
//============================================================================
#include "handleobjectrange.h"

void SHandleObjectRangeImpl::AddToValue(const CFloat* delta)
{
    PrepareRangeUpdate();

    CFloat previous = m_value;

    //  Bound as a reference, not copied: the shipped code reads the sum
    //  through the pointer operator+ handed back (`mov ecx, eax`) and uses
    //  that same pointer as the last clamp candidate.
    const CFloat& sum = m_value + *delta;

    const CFloat* pick = &m_low;
    if (*(const float*)&sum.m_bits < *(const float*)&pick->m_bits)
        goto done;

    ++pick;
    if (*(const float*)&sum.m_bits > *(const float*)&pick->m_bits)
        goto done;

    pick = &sum;

done:
    m_value = *pick;
    NotifyRangeChanged(&m_value, &previous, 0);
}

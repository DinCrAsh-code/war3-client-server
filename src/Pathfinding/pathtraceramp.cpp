//============================================================================
//  0x6F477180 - evaluate this trace's ramp at `t`, clamped into its window.
//
//  `base + rate * t`, held between the two bounds - and held by *pointer
//  select*, not by two conditional assignments: the shipped code keeps the
//  address of whichever of the three values wins in edx and dereferences it
//  once at the end.  Starting from the low bound's address and moving it
//  only when a test says so is what produces that, where three `return`s or
//  a pair of `if`s over the value itself would copy each candidate.
//
//  The comparison senses are the shipped ones and they are not symmetric:
//  the low bound wins when it is strictly greater than the value, the high
//  bound when it is strictly less.  A value equal to either bound is the
//  value, not the bound.
//============================================================================
#include "game.h"
#include "boxmath.h"

void CPathTrace::RampValueAt(CFloat* out, const CFloat* t)
{
    //  Both intermediates bound as references, not copied into named
    //  CFloats: the shipped code keeps the pointer operator+ returned in ecx
    //  and uses it as the value's address *and* as the load source, which a
    //  named local turns into a copy plus a fresh `lea`.
    const CFloat& scaled = RampRate() * *t;
    const CFloat& value = RampBase() + scaled;

    //  A `goto` chain over one pointer, and `++pick` rather than
    //  `&RampMax()` for the second candidate.  Both are the shipped shape:
    //  it keeps the winning candidate's address in edx, steps it with
    //  `add edx, 4` from the low bound to the high one, and dereferences it
    //  once at a single exit.  An `if`/`else` over the value - or a
    //  conditional expression - makes MSVC duplicate the load and the store
    //  into three separate exits instead.
    const CFloat* pick = &RampMin();
    if (*(const float*)&value.m_bits < *(const float*)&pick->m_bits)
        goto done;

    ++pick;
    if (*(const float*)&value.m_bits > *(const float*)&pick->m_bits)
        goto done;

    pick = &value;

done:
    *out = *pick;
}

//============================================================================
//  0x6F4A8FD0 - where `t` sits along the ramp, as a fraction of its rate.
//
//  `t` outside the ramp's own window is not clamped, it is refused: the
//  answer is -1, which is what the shipped code's `dword_6FAAE464` fallback
//  is.  Inside it, the ramp is evaluated at however far this trace has
//  travelled and `t` is measured from there.
//
//  The two window tests are the shipped pair and they are not the same
//  sense: the low bound fails on strictly greater, the high bound on
//  strictly less, so a `t` sitting exactly on a bound is accepted.
//============================================================================
void CPathTrace::RampFractionAt(CFloat* out, const CFloat* t)
{
    //  `t` on the left of both tests: MSVC emits the two `fld`s in source
    //  order and the shipped body loads `t` first for each bound.
    if (*(const float*)&t->m_bits < *(const float*)&RampMin().m_bits ||
        *(const float*)&t->m_bits > *(const float*)&RampMax().m_bits)
    {
        *out = g_CFloatMinusOne;
        return;
    }

    //  The travelled distance reached through the pointer GetTravelDistance
    //  returned rather than through a named copy - the shipped code pushes
    //  that pointer straight on as RampValueAt's argument.
    const CFloat& travelled = GetTravelDistance();

    //  kCFloatNoInit: RampValueAt fills this before anything reads it, and
    //  the zeroing default constructor is one store the shipped code has
    //  not got.
    CFloat at(kCFloatNoInit);
    RampValueAt(&at, &travelled);

    *out = (*t - at) / RampRate();
}

//============================================================================
//  0x6F4771E0 - the ramp evaluated at this trace's own current travel
//  distance. Same pair, same shape as SUnitTrackedRef::GetValue
//  (Unit/unittrackedref.cpp) and CDrivenRelation::Resample's own naked
//  body (Agent/agentrelationresample.cpp): the pointer GetTravelDistance
//  hands back is passed straight into RampValueAt rather than dereferenced
//  into a second local first.
//============================================================================
CFloat* CPathTrace::RampValueAtCurrent(CFloat* out)
{
    const CFloat& travelled = GetTravelDistance();
    RampValueAt(out, &travelled);
    return out;
}

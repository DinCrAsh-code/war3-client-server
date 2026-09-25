//============================================================================
//  0x6F28B1F0 - CUnit's vtable slot 73 (+0x124), SetLife.  See
//  unit.h.
//
//  Publish the caller's value onto the +0x98 tracked reference, but never
//  a positive one: the shipped body compares it against g_CFloatZero and
//  substitutes the zero's own address when the value is greater.  That is
//  a min, and it is a min of *pointers* - whichever of the two addresses
//  wins is what gets passed on, so no copy is made.
//
//  The compare is the `fld a` / `fld b` / `fcompp` / `test ah, 5` / `jnp`
//  shape docs/msvc-vc8-idioms.md catalogues as `!(a > b)`; the CFloat's
//  bits are loaded as a hardware float, which is what `fld dword ptr` on a
//  CFloat means everywhere else in this repo (cmatrix3_tofloat.cpp).
//
//  Then tell the widget layer, through CWidget's own 0x6F2AB460.
//============================================================================
#include "unit.h"
#include "CFloat.h"

void CUnit::SetLife(const CFloat* value)
{
    //  Written as the *negation* of `>` rather than as `<=`.  Both read the
    //  same for ordinary numbers and this compiler keeps them apart: `<=`
    //  is `test ah, 1`, `!(a > b)` is `test ah, 5`, and the shipped body
    //  has `test ah, 5`.  They differ only for NaN, which is exactly why
    //  the compiler does not fold one into the other -
    //  docs/msvc-vc8-idioms.md has the table.
    //
    //  So the clamp is a floor at zero, not a ceiling: a value that is not
    //  greater than zero is replaced by zero's own address, and no copy is
    //  made either way - whichever of the two pointers wins is what gets
    //  published.
    const CFloat* clamped = value;
    if (!(*(const float*)value > *(const float*)&g_CFloatZero))
        clamped = &g_CFloatZero;

    m_life.SetValue(clamped);
    NotifyLifeChanged();
}

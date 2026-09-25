//============================================================================
//  0x6F26F320 - CUnit's vtable slot 56 (+0xE0), GetPosition.  See unit.h.
//
//  Copy the unit's own three position floats into the caller's FVec3, and
//  optionally hand back a single bit (bit 1 of +0x280) through the fourth
//  argument, which is a pointer and not the int the base's declaration
//  suggests - the shipped code stores through it.
//
//  With `wantGround` clear the z component is overwritten with zero and
//  that is the whole answer.  With it set, and only when the third
//  argument is not the -1 "no particular index" sentinel, the result is
//  handed to this unit's own slot 57 (+0xE4) - whose float return is
//  discarded with a bare `fstp st`, which is what calling a
//  float-returning function for its side effect and ignoring the value
//  looks like.
//
//  Both exits return the caller's own buffer.
//============================================================================
#include "unit.h"
#include "fvec3.h"

FVec3* CUnit::GetPosition(FVec3* out, int wantGround, int a3, int a4)
{
    if (a4 != 0)
        *(int*)a4 = (int)((m_flags280 >> 1) & 1);

    //  One struct assignment, not three float ones: the shipped code moves
    //  the three words with integer `mov`s, which is FVec3's own implicit
    //  copy assignment.  Component-by-component and this compiler routes
    //  each through the x87 stack instead.
    *out = *(const FVec3*)&m_posX284;

    //  Positive test with the zero-z arm last, not `if (wantGround == 0)`
    //  first: the shipped code falls through into the slot-57 call and
    //  puts `fldz` at a label past the return, which is the way round this
    //  produces.
    if (wantGround != 0)
    {
        if (a3 != -1)
            Method_0xE4(out, a3, a4, 0);
        return out;
    }
    out->m_z = 0.0f;
    return out;
}

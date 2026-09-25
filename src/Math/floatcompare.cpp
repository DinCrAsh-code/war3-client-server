//============================================================================
//  0x6F00C110 / 0x6F01FD40 - the two bare CFloat comparisons.
//
//  Neither goes through CFloat::operator- the way CFloatNearlyEqual
//  (floatnearlyequal.cpp) does: both `fld` the two stored patterns straight
//  out of the objects and read the answer off one `fcompp`, which is what
//  makes them two instructions rather than a call.
//
//  They are members rather than free functions because every shipped call
//  site loads ecx with the left-hand operand's address and pushes only the
//  right-hand one.  Both return an `int`, not a `bool`: the shipped bodies
//  materialise a whole-register 0 or 1 and do *not* narrow it through
//  `movzx eax, al` (compare CPathClient::CheckSlotDistance, which does).
//
//  Own translation unit, and separate from CFloat's inline operators: next
//  to a caller MSVC folds the whole body into the `fcompp` and neither
//  survives as a function.
//============================================================================
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F00C110 - `*this < other`.
//----------------------------------------------------------------------------
int CFloat::IsLess(const CFloat* other) const
{
    return *(const float*)&m_bits < *(const float*)&other->m_bits;
}

//----------------------------------------------------------------------------
//  0x6F01FD40 - `*this > other`.
//----------------------------------------------------------------------------
int CFloat::IsGreater(const CFloat* other) const
{
    return *(const float*)&m_bits > *(const float*)&other->m_bits;
}

//----------------------------------------------------------------------------
//  0x6F01FD80 - `*this >= other`, immediately after IsGreater above in the
//  shipped binary.
//----------------------------------------------------------------------------
int CFloat::IsGreaterOrEqual(const CFloat* other) const
{
    return *(const float*)&m_bits >= *(const float*)&other->m_bits;
}

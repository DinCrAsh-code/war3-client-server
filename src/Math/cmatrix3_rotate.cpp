//============================================================================
//  0x6F4B00C0 - pre-multiply this matrix by a rotation about `axis`.
//
//  Both calls it makes are real calls in the shipped build
//  (MakeAxisRotation at 0x6F4AFCD0, CMatrix3::operator* at 0x6F4AF4D0), so
//  all three live in separate translation units.
//
//  The assignment back into *this is a plain 36-byte struct copy, which is
//  where the shipped `mov ecx,9 / rep movsd` comes from - not a member-wise
//  loop and not nine stores.
//
//  Note the operand order: the new rotation goes on the *left*.  The
//  shipped code passes operator*'s stack argument (its `b`) as `this` and
//  its edx argument (its `a`) as the freshly built rotation.
//============================================================================
#include "cmatrix3.h"

void CMatrix3::RotateAxis(const CFloat& angle, const CWorldVec3& axis,
                          int alreadyNormalized)
{
    *this = MakeAxisRotation(angle, axis, alreadyNormalized) * *this;
}

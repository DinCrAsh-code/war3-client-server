//============================================================================
//  0x6F479810 - normalize a CFloat 3-vector in place: the deterministic twin
//  of FVec3::Normalize (0x6F011550, fvec3normalize.cpp).
//
//  It does not divide.  The reciprocal length comes back from a single call
//  at 0x6F2BAE20 (CFloatReciprocalLength3, Math/cfloatreciprocallength3.cpp)
//  - which takes the three components as three separate arguments (x in
//  edx, y and z on the stack) rather than the vector's own address - and
//  each component is then *multiplied* by it, in place, one
//  CFloat::operator* per component.
//
//  Each component is written back before the next one is read, which is why
//  the three multiplies cannot be reordered or hoisted: they are three
//  statements, not one vector expression.
//============================================================================
#include "cmatrix3.h"

//  0x6F2BAE20 - Math/cfloatreciprocallength3.cpp.  Declared here rather than
//  through a shared header: a re-declaration that differs in any part of the
//  type is a second mangled name and a link failure verify.py cannot see, so
//  this one is character for character what that file gives it.
CFloat __fastcall CFloatReciprocalLength3(const CFloat& x, const CFloat& y,
                                          const CFloat& z);

void __fastcall NormalizeWorldVec3(CWorldVec3* v)
{
    CFloat scale = CFloatReciprocalLength3(v->m_x, v->m_y, v->m_z);
    v->m_x = v->m_x * scale;
    v->m_y = v->m_y * scale;
    v->m_z = v->m_z * scale;
}

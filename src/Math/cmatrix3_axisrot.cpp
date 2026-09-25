//============================================================================
//  0x6F4AFCD0 - build a CFloat rotation matrix from an axis and an angle:
//  Rodrigues' rotation formula, in the row-vector (transposed) convention.
//
//      | t*x*x + c    t*x*y + z*s  t*z*x - y*s |
//      | t*x*y - z*s  t*y*y + c    t*y*z + x*s |     t = 1 - c
//      | t*z*x + y*s  t*y*z - x*s  t*z*z + c   |
//
//  Own translation unit: CMatrix3::RotateAxis (0x6F4B00C0) calls it for
//  real, and CMatrix3::operator* (0x6F4AF4D0) is called from here for real
//  in turn - neither may be inlined into this one.
//
//  Two orderings in here are load-bearing and are what the disassembly
//  dictates, not stylistic choices:
//
//  * The nine products (xx, yy, zz, xy, yz, zx, xs, ys, zs) are named
//    locals declared in exactly that order, and `t` comes after all nine.
//    The shipped code computes them in that order, ahead of any matrix
//    element.
//  * The nine matrix elements are the nine arguments of one CMatrix3
//    constructor call.  MSVC evaluates arguments right to left, and the
//    shipped code computes element 8 (`t*z*z + c`) first and element 0
//    last, so the constructor-argument spelling is what reproduces it -
//    nine assignments to a local would run the other way.
//
//  The angle is copied into a local before the sin/cos call rather than
//  passed straight through: the shipped code loads `*angle` into a register
//  and stores it into its own stack slot (0x6F4AFD0C-0x6F4AFD12) before
//  taking that slot's address, which is what a genuine local, not a
//  forwarded reference, produces.
//============================================================================
#include "cmatrix3.h"

//  dword_6FAAE4F4.  The name is a misnomer inherited from the CPathTrace
//  target that first reached it (pathtracegrid.cpp's ClassifySlopeAxis used
//  it as a slope threshold); it is simply the simulation's encoded CFloat
//  1.0, which floatfloorsplit.cpp already says in as many words and which
//  this file needs twice over - as Rodrigues' `1 - cos` minuend here, and
//  as the z component of CWidget::InitSprite's own (0, 0, 1) rotation axis.
//  Left under the established name rather than renamed, since renaming a
//  DATA row changes the symbol every other translation unit binds through.
extern const CFloat g_slopeThreshold1;

CMatrix3 __fastcall MakeAxisRotation(const CFloat& angle,
                                     const CWorldVec3& axis,
                                     int alreadyNormalized)
{
    CWorldVec3 n = axis;
    if (!alreadyNormalized)
        NormalizeWorldVec3(&n);

    CFloat theta = angle;
    CFloat s(kCFloatNoInit);
    CFloat c(kCFloatNoInit);
    CFloatSinCos(theta, &s, &c);

    CFloat xx = n.m_x * n.m_x;
    CFloat yy = n.m_y * n.m_y;
    CFloat zz = n.m_z * n.m_z;
    CFloat xy = n.m_x * n.m_y;
    CFloat yz = n.m_y * n.m_z;
    CFloat zx = n.m_z * n.m_x;
    CFloat xs = n.m_x * s;
    CFloat ys = n.m_y * s;
    CFloat zs = n.m_z * s;
    CFloat t = g_slopeThreshold1 - c;

    return CMatrix3(t * xx + c,  t * xy + zs, t * zx - ys,
                    t * xy - zs, t * yy + c,  t * yz + xs,
                    t * zx + ys, t * yz - xs, t * zz + c);
}

//============================================================================
//  CMatrix3 - a 3x3 orientation matrix built out of CFloat, i.e. the
//  deterministic ("software float") twin of itemplacementdispatch.h's
//  SOrientationMatrix.
//
//  The binary carries both spellings of the same three routines, one pair
//  per arithmetic type, at two separate address neighbourhoods:
//
//      concat / multiply       0x6F4AF4D0 (CFloat)   0x6F4B16F0 (float)
//      build from axis+angle   0x6F4AFCD0 (CFloat)   0x6F4B2C50 (float)
//      rotate in place         0x6F4B00C0 (CFloat)   0x6F4B2DA0 (float)
//
//  They are not one template instantiated twice - the two `build from
//  axis+angle` entries take their arguments in a different order (the
//  CFloat one takes the angle in edx and the axis on the stack, the float
//  one the other way round), which a single template could not produce.
//  Two hand-maintained copies of the same algorithm, which is also why the
//  float pair reaches _CIsin/_CIcos while the CFloat pair goes through the
//  simulation's own table-driven CFloatSinCos (0x6F6EF990).
//
//  Storage is row-major, m[row*3 + col], matching SOrientationMatrix - see
//  cmatrix3_mul.cpp for the multiply that pins the convention down.
//============================================================================
#ifndef CMATRIX3_H
#define CMATRIX3_H

#include "CFloat.h"
#include "game.h"       // CWorldVec3 - the CFloat 3-vector the axis arrives in
#include "itemplacementdispatch.h"   // SOrientationMatrix

class CMatrix3
{
public:
    //  The default constructor is the *identity*, not a zero matrix - see
    //  CWidget::InitSprite (widget.cpp), whose own local is set up with the
    //  literal bit pattern 0x3F800000 loaded once into a register and stored
    //  into the three diagonal slots while the six off-diagonal ones take a
    //  register that already held zero.  No call, no zeroing pass.
    CMatrix3()
        : m_00(CFloat(kCFloatExpBias)), m_01(), m_02(),
          m_10(), m_11(CFloat(kCFloatExpBias)), m_12(),
          m_20(), m_21(), m_22(CFloat(kCFloatExpBias)) {}

    //  Nine separate members rather than a `CFloat m[9]` array, and a
    //  mem-initialiser list rather than nine assignments in the body: an
    //  array member is default-constructed element by element before the
    //  body runs, and CFloat's own default constructor zeroes, so the array
    //  spelling emits ten dead instructions ahead of every construction and
    //  grows the constructor past what /Ob2 will inline.  The shipped code
    //  inlines it at both call sites, and this is the spelling that does.
    CMatrix3(const CFloat& m0, const CFloat& m1, const CFloat& m2,
             const CFloat& m3, const CFloat& m4, const CFloat& m5,
             const CFloat& m6, const CFloat& m7, const CFloat& m8)
        : m_00(m0), m_01(m1), m_02(m2),
          m_10(m3), m_11(m4), m_12(m5),
          m_20(m6), m_21(m7), m_22(m8) {}

    //  0x6F2ABDC0 - cmatrix3_tofloat.cpp.  Returned by value: 36 bytes go
    //  back through a hidden buffer, whose pointer the callee leaves in eax
    //  for the caller to `rep movsd` straight out of.
    SOrientationMatrix ToOrientationMatrix() const;

    //  0x6F4B00C0 - cmatrix3_rotate.cpp.
    void RotateAxis(const CFloat& angle, const CWorldVec3& axis,
                    int alreadyNormalized);

    CFloat m_00, m_01, m_02;    // +0x00 +0x04 +0x08
    CFloat m_10, m_11, m_12;    // +0x0C +0x10 +0x14
    CFloat m_20, m_21, m_22;    // +0x18 +0x1C +0x20
};

//  0x6F4AF4D0 - cmatrix3_mul.cpp.
CMatrix3 __fastcall operator*(const CMatrix3& a, const CMatrix3& b);

//  0x6F4AFCD0 - cmatrix3_axisrot.cpp.
CMatrix3 __fastcall MakeAxisRotation(const CFloat& angle,
                                     const CWorldVec3& axis,
                                     int alreadyNormalized);

//  0x6F479810 - cworldvec3normalize.cpp.  Scales the vector by the
//  reciprocal of its own length, in place.
void __fastcall NormalizeWorldVec3(CWorldVec3* v);

//  0x6F6EF990 - cfloatsincos.cpp.  Both at once, off one table lookup.
void __fastcall CFloatSinCos(const CFloat& angle, CFloat* outSin,
                             CFloat* outCos);

#endif

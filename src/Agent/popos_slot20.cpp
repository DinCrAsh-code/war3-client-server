//============================================================================
//  NIpse::CPoPos::Slot20 (0x6F4A74A0), vtable slot 20 - real now (third
//  follow-up session on this family). Computes the velocity vector's own
//  angle (SCFloatVec2::AngleFromLength/::ComputeAngle,
//  Math/cfloatvec2angle.cpp) into +0x8C (m_fieldE), but only when the
//  velocity's own squared length differs from zero by more than
//  g_velocityAngleEpsilon (flt_6FAAE618, a plain hardware-float constant
//  this module reads with a bare `fld`, never through a CFloat operation -
//  same convention Math/cfloatnearlyequal.cpp's own g_CFloatEqualityTolerance
//  already documents) - skipped entirely (leaving m_fieldE untouched) when
//  the velocity is that close to stationary, matching
//  Math/cfloatnearlyequal.cpp's own `(a - b).m_bits & 0x7FFFFFFF` /
//  `*(float*)&magnitude < tolerance` idiom with `b = g_CFloatZero`.
//
//  This is NOT the same "CFloat division/threshold helper web"
//  (sub_6F493910/sub_6F6EFE50) Slot19 (popos_slots1920_thunk.cpp) still
//  cannot close - Slot20 reaches a completely different, fully independent
//  and fully closed acos-based angle pair instead
//  (SCFloatVec2::AngleFromLength/::ComputeAngle, sub_6F1D2AB0/sub_6F4B2DE0).
//============================================================================
#include "popos.h"

//  Math/cfloatvec2angle.cpp - not declared in a shared header since this
//  is its only caller; the struct itself is the same "flat, offset-
//  addressed" reinterpret-cast convention this file's own header already
//  documents for CRlProperty.
struct SCFloatVec2
{
    CFloat x;
    CFloat y;

    void ComputeAngle(CFloat* out, const CFloat* length) const;
    CFloat* AngleFromLength(CFloat* out) const;
};

//  flt_6FAAE618 - a plain hardware `float`, read with a bare `fld` exactly
//  like Math/cfloatnearlyequal.cpp's own g_CFloatEqualityTolerance
//  (flt_6FAAE614, four bytes below this one).
extern const float g_velocityAngleEpsilon;

namespace NIpse {

void CPoPos::Slot20()
{
    SCFloatVec2* vel = reinterpret_cast<SCFloatVec2*>(&m_velX);

    CFloat lenSq = vel->x * vel->x + vel->y * vel->y;
    int magnitude = (lenSq - g_CFloatZero).m_bits & 0x7FFFFFFF;

    if (!(*(const float*)&magnitude < g_velocityAngleEpsilon))
    {
        CFloat angle(kCFloatNoInit);
        m_fieldE = *vel->AngleFromLength(&angle);
    }
}

}  // namespace NIpse

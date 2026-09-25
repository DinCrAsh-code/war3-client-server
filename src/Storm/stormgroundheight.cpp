//============================================================================
//  0x6F011780 - SStormSingletonB::QueryGroundHeightAt: drop a vertical
//  segment through this point and answer where it hit.
//
//  A member and not a free function, even though nothing in the body reads
//  `this`: ecx is never set up here and is handed to
//  QueryNearestObstruction (0x6F0116E0, stormobstructionquery.cpp) exactly
//  as it arrived, which is only true of a `this` passed straight through.
//
//  The two probe points are the incoming point with its z replaced - the
//  copy really is the whole three-float point and *then* the z store, which
//  is what puts three `mov`s and one `fstp` in the shipped stream for each
//  of them rather than two `mov`s.
//
//  The answer is the top probe's own z minus the distance the query hands
//  back, i.e. the height of the first thing under the point, and it is only
//  written when something was hit *and* the caller asked for it.
//
//  Own translation unit: the obstruction query is a real call, and it is a
//  member of the same singleton.
//============================================================================
#include "stormsingletonb.h"
#include "fvec3.h"

//: flt_6F8768A4 / flt_6F8768A0 - the top and bottom of the probe segment,
//: the world's own z bounds.  Declared and not spelled: the operand the
//: canonicaliser compares is the symbol.
extern const float g_groundProbeTopZ;
extern const float g_groundProbeBottomZ;

void SStormSingletonB::QueryGroundHeightAt(const float* point,
                                            float* outHeight)
{
    FVec3 from = *(const FVec3*)point;
    from.m_z = g_groundProbeTopZ;

    FVec3 to = *(const FVec3*)point;
    to.m_z = g_groundProbeBottomZ;

    float distance;
    if (QueryNearestObstruction(&from.m_x, &to.m_x, &distance) != 0
        && outHeight != 0)
    {
        *outHeight = from.m_z - distance;
    }
}

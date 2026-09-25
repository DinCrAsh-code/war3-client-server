//============================================================================
//  0x6F32D350 / 0x6F32D330 - two more members of the GetFootprintZOffset
//  family (footprinttype.h): a type's "elevRadius" property (+0xA4, a
//  hardware float, 0.0f fallback - same shape as GetFootprintZOffset) and
//  its "elevPoints" property (+0xA0, a plain int/pointer word, 0 fallback -
//  same shape as GetFootprintShape's null fallback, just not a pointer
//  type here since nothing in this call tree ever dereferences it).
//
//  Both are reached from sub_6F276AD0 (CUnit's slot 57 callee, footprint
//  elevation-outline sampling): elevPoints picks which corner-count branch
//  runs, elevRadius scales the rotated corner offsets.  Own translation
//  unit for the same reason every other member of this family gets one:
//  the call into GetOrCreateFootprintTypeData has to stay a real call
//  rather than being inlined into a three-line body.
//============================================================================
#include "footprinttype.h"

float __fastcall GetFootprintElevRadius(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "elevRadius");
    if (data == 0)
        return 0.0f;
    return data->m_elevRadius;
}

int __fastcall GetFootprintElevPoints(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "elevPoints");
    if (data == 0)
        return 0;
    return data->m_elevPoints;
}

//----------------------------------------------------------------------------
//  0x6F32D2F0 / 0x6F32D310 - "maxPitch"/"maxRoll", the two angle callees
//  sub_6F282160 (CUnit's slot 110) reads while building its own 3x3
//  orientation-basis output.  Same shape and fallback as GetFootprintZOffset.
//----------------------------------------------------------------------------
float __fastcall GetFootprintMaxPitch(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "maxPitch");
    if (data == 0)
        return 0.0f;
    return data->m_maxPitch;
}

float __fastcall GetFootprintMaxRoll(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "maxRoll");
    if (data == 0)
        return 0.0f;
    return data->m_maxRoll;
}

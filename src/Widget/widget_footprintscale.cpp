//============================================================================
//  0x6F32D070 - a footprint type's "scaleFactor" property as a hardware
//  float, or 1.0 when the type has no property record at all.
//
//  Own translation unit for the same reason footprintshape.cpp
//  (0x6F32D0B0) and footprintshapealt.cpp are: the call into
//  GetOrCreateFootprintTypeData has to stay a real call rather than being
//  inlined into this three-line body.  The three are the same function
//  three times over with a different property name and, here, a different
//  "type not registered" answer.
//
//  __fastcall with only ecx used: `mov edx, offset "scaleFactor"`
//  overwrites edx before the call, so the incoming edx is dead and the one
//  real parameter is the footprint type id in ecx.
//============================================================================
#include "footprinttype.h"

float __fastcall GetFootprintScaleFactor(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "scaleFactor");
    if (data == 0)
        return 1.0f;
    return data->m_scaleFactor;
}

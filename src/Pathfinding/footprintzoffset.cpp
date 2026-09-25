//============================================================================
//  0x6F32D090 - a footprint type's "zOffset" property as a hardware float,
//  or 0.0f when the type has no property record at all.
//
//  Own translation unit for the same reason footprintshape.cpp (0x6F32D0B0)
//  and widget_footprintscale.cpp (0x6F32D070) are: the call into
//  GetOrCreateFootprintTypeData has to stay a real call rather than being
//  inlined into this three-line body.  Same shape as
//  GetFootprintScaleFactor, with a 0.0f ("not registered") fallback instead
//  of 1.0f.
//
//  __fastcall with only ecx used: `mov edx, offset "zOffset"` overwrites
//  edx before the call, so the incoming edx is dead and the one real
//  parameter is the footprint type id in ecx.
//============================================================================
#include "footprinttype.h"

float __fastcall GetFootprintZOffset(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "zOffset");
    if (data == 0)
        return 0.0f;
    return data->m_zOffset;
}

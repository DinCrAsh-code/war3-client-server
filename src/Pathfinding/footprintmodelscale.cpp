//============================================================================
//  0x6F32D3C0 - a footprint type's "modelScale" property as a hardware
//  float, or 1.0f when the type has no property record at all.
//
//  Own translation unit for the same reason footprintzoffset.cpp (0x6F32D090)
//  and widget_footprintscale.cpp (0x6F32D070) are: the call into
//  GetOrCreateFootprintTypeData has to stay a real call rather than being
//  inlined into this three-line body.  Same shape as GetFootprintScaleFactor,
//  with a 1.0f ("not registered") fallback - the identity scale, same as
//  that sibling rather than GetFootprintZOffset's 0.0f.
//
//  __fastcall with only ecx used: `mov edx, offset "modelScale"` overwrites
//  edx before the call, so the incoming edx is dead and the one real
//  parameter is the footprint type id in ecx.
//============================================================================
#include "footprinttype.h"

float __fastcall GetFootprintModelScale(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "modelScale");
    if (data == 0)
        return 1.0f;
    return data->m_modelScale;
}

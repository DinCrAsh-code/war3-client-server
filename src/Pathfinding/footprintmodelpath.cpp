//============================================================================
//  0x6F32CFD0 - a footprint type's "modelPath" property, a plain pointer,
//  or null when the type has no property record at all.
//
//  Own translation unit for the same reason footprintshape.cpp (0x6F32D0B0)
//  and widget_footprintscale.cpp (0x6F32D070) are: the call into
//  GetOrCreateFootprintTypeData has to stay a real call rather than being
//  inlined into this four-line body.
//
//  __fastcall with only ecx used: `mov edx, offset "modelPath"` overwrites
//  edx before the call, so the incoming edx is dead and the one real
//  parameter is the footprint type id in ecx.
//============================================================================
#include "footprinttype.h"

const char* __fastcall GetFootprintModelPath(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "modelPath");
    if (data == 0)
        return 0;
    return data->m_modelPath;
}

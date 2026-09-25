//============================================================================
//  0x6F32D010 - a footprint type's "uberSplat" record, or null if it has
//  none or is not registered at all.  citem-batch-L, off CUnit::Reposition's
//  own call tree (asm/sub_6F2A5D50_0x6F2A5D50_calltree_asm.md), reached
//  from a still-thunked helper (0x6F28E170) with `ecx = m_footprintType`
//  carried straight through from its own caller.
//
//  Same GetOrCreateFootprintTypeData(id, debugName) lookup and null-check
//  shape as GetFootprintShape/GetFootprintScaleFactor/GetFootprintZOffset
//  (footprintshape.cpp / widget_footprintscale.cpp / footprintzoffset.cpp),
//  with a pointer fallback (0) instead of a float one.
//============================================================================
#include "footprinttype.h"

void* __fastcall GetFootprintUberSplat(unsigned int id)
{
    CFootprintTypeData* data =
        GetOrCreateFootprintTypeData(id, "uberSplat");
    if (data == 0)
        return 0;
    return data->m_uberSplat;
}

//============================================================================
//  CUnit batch-14 (slot-3 closure, off the message dispatcher 0x6F2A7E60).
//
//  0x6F32D030 - a footprint type's "shadow" property, or null when the type
//  has no record at all.  Same shape as footprintubersplat.cpp's m_uberSplat
//  getter one dword over: `mov edx, offset "shadow"` overwrites edx before
//  the call, so the incoming edx is dead and the one real parameter is the
//  footprint type id in ecx.
//
//  0x6F32CBE0 - the type's "ShadowOnWater" flag, read back raw off the node
//  with no null check of its own (same unguarded-read shape
//  GetSelCircleOnWaterFlag/GetFootprintIsStructureFlag use) - the caller
//  that reaches it has already gone through a registered type by the time
//  it asks.
//============================================================================
#include "footprinttype.h"

void* __fastcall GetFootprintShadow(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "shadow");
    if (data == 0)
        return 0;
    return data->m_shadow;
}

unsigned int __fastcall GetFootprintShadowOnWaterFlag(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "ShadowOnWater");
    return data->m_shadowOnWater;
}

//----------------------------------------------------------------------------
//  CUnit batch-15.  0x6F32D050 - a footprint type's "structureShadow"
//  property, or null when the type has no record at all.  Same shape as
//  GetFootprintShadow one dword over.
//----------------------------------------------------------------------------
void* __fastcall GetFootprintStructureShadow(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "structureShadow");
    if (data == 0)
        return 0;
    return data->m_structureShadow;
}

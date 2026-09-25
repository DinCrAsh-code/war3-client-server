//============================================================================
//  0x6F32D0B0 - a type's footprint shape.  Its own translation unit so the
//  call into GetOrCreateFootprintTypeData (defined in footprinttype.cpp)
//  stays a real call, as the shipped code has it, instead of being inlined.
//============================================================================
#include "footprinttype.h"

CFootprintShape* __fastcall GetFootprintShape(unsigned int id)
{
    CFootprintTypeData* data =
        GetOrCreateFootprintTypeData(id, "pPathingFootprint");
    if (data == 0)
        return 0;
    return data->m_footprint;
}

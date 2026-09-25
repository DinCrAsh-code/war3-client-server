//============================================================================
//  0x6F32CC20 - a type's "SelCircleOnWater" flag.  Its own translation unit
//  so the call into GetOrCreateFootprintTypeData (defined in
//  footprinttype.cpp) stays a real call, as the shipped code has it,
//  instead of being inlined - same reasoning as footprintshape.cpp.
//
//  Unlike GetFootprintShape/GetFootprintScaleFactor, the shipped body does
//  not null-check the node before reading the field: no `test eax,eax`
//  between the `call` and the `mov eax,[eax+94h]`.  GetOrCreateFootprintTypeData
//  never actually answers null in practice (Ptr() either finds a node or
//  NewNode() makes one), but this call site's own C++ simply never wrote the
//  check the other two do - reproduced as written, not "fixed" to match its
//  siblings.
//============================================================================
#include "footprinttype.h"

unsigned int __fastcall GetSelCircleOnWaterFlag(unsigned int footprintTypeId)
{
    return GetOrCreateFootprintTypeData(footprintTypeId, "SelCircleOnWater")
        ->m_selCircleOnWater;
}

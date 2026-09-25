//============================================================================
//  0x6F32CC80 - a type's "IsStructure" flag.  Its own translation unit so
//  the call into GetOrCreateFootprintTypeData (defined in footprinttype.cpp)
//  stays a real call, as the shipped code has it, instead of being inlined -
//  same reasoning as footprintshape.cpp / footprintselcircleonwater.cpp.
//
//  Same no-null-check shape as GetSelCircleOnWaterFlag, reproduced as
//  written: no `test eax,eax` between the `call` and the `mov eax,[eax+0B4h]`.
//  Its only caller in this dump is CUnit's per-template initialiser
//  (0x6F2A0E30), which asks with the CUnit's own +0x30 template-id
//  argument (already in ecx from the call site) and uses the answer to
//  set/clear CUnit's own +0x5C flag bit 0x10000.
//============================================================================
#include "footprinttype.h"

unsigned int __fastcall GetFootprintIsStructureFlag(unsigned int footprintTypeId)
{
    return GetOrCreateFootprintTypeData(footprintTypeId, "IsStructure")
        ->m_isStructure;
}

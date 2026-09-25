//============================================================================
//  0x6F00F5D0 - the water-height query behind JASS_GetLocationZ: six
//  instructions that forward the point and the out-parameter to a member of
//  the large Storm singleton.
//
//  Its own `retn` is bare, so the two words it pushes are cleaned by the
//  callee - which is what makes that callee a `__thiscall` member with two
//  stack arguments rather than anything this function has to clean itself.
//  The push order settles which argument is which: `push edx` then
//  `push ecx` puts the point (ecx) at the lower address, so it is the first
//  argument.
//
//  Own translation unit: two real calls out of it, and it is the only thing
//  this target reaches in the 0x6F00Fxxxx module.
//============================================================================
#include "stormsingletona.h"

SStormSingletonA* __fastcall GetStormSingletonA();

//  0x6F7527F0 - inside Storm's own address range; not reconstructed.  A
//  __thiscall member with two stack arguments (`retn 8`), which is what this
//  caller's own bare `retn` after two pushes proves.
struct SStormWaterQuery
{
    int QueryHeightAt(const FVec3* point, float* outHeight);
};

int __fastcall QueryWaterHeightAt(const FVec3* point, float* outHeight)
{
    return ((SStormWaterQuery*)GetStormSingletonA())
               ->QueryHeightAt(point, outHeight);
}

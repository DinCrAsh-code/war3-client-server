//============================================================================
//  0x6F00BF60 / 0x6F00F7D0 - the two one-line front ends onto the large
//  Storm singleton that CWidget's vtable slot 65 (widget_slot65.cpp) uses to
//  find the terrain tint under a widget and to push it back at the terrain
//  table.
//
//  Both are `push the arguments, fetch the singleton, dispatch` and nothing
//  else, which is why the two argument pushes come *before* the
//  GetStormSingletonA call in the shipped code - that is what MSVC emits for
//  `GetStormSingletonA()->Method(a, b)` written as one expression.
//
//  Own translation unit: GetStormSingletonA lives in lazysingleton.cpp and
//  both of these reach it with a real `call`.
//============================================================================
#include "stormsingletona.h"

//  0x6F01F5A0 - lazysingleton.cpp.
SStormSingletonA* __fastcall GetStormSingletonA();

//----------------------------------------------------------------------------
//  0x6F00BF60 - the terrain tint at a world point.
//
//  The point comes in through edx and the output dword through ecx, and the
//  function hands the *output* pointer back rather than whatever the
//  singleton's own method returned - the shipped `mov eax, esi` after the
//  call, which is an explicit `return out;` and not a forwarded result.
//----------------------------------------------------------------------------
unsigned int* __fastcall QueryTerrainTintAt(unsigned int* out, const FVec3* point)
{
    GetStormSingletonA()->QueryTintAt(out, point);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F00F7D0 - push a tint back at one terrain record.
//----------------------------------------------------------------------------
void __fastcall StoreTerrainTint(int recordIndex, const unsigned int* tint)
{
    GetStormSingletonA()->StoreTint(recordIndex, tint);
}

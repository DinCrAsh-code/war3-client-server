//============================================================================
//  0x6F4E8370 - set one slot from a single int, by handing the apply path
//  the address of this function's own argument slot.
//
//  Five instructions, and the exact twin of SetControllerSlotFloat
//  (0x6F4E8380, controllerslotsetfloat.cpp) sixteen bytes further on: same
//  shape, same `lea ecx,[esp+4]`, different apply.
//
//  Own translation unit: the apply is a real call.
//============================================================================
#include "controllerslot.h"

//  0x6F4E8160 - the single-int apply, another private-ABI body (host in
//  eax).  Redirected in GameUI/controllerslot_thunks.cpp.
void __fastcall ApplyControllerSlotInt(void* host, int slot, const int* value);

void __fastcall SetControllerSlotInt(void* host, int slot, int value)
{
    ApplyControllerSlotInt(host, slot, &value);
}

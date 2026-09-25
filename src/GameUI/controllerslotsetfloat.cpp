//============================================================================
//  0x6F4E8380 - set one slot from a single float, by handing the apply path
//  the address of this function's own argument slot.
//
//  Five instructions, and the `lea ecx,[esp+4]` really is the incoming
//  argument's own address: the value is never copied anywhere first.
//
//  Own translation unit: the apply is a real call.
//============================================================================
#include "controllerslot.h"

//  0x6F4E8200 - the single-float apply, another private-ABI body (host in
//  eax).  Redirected in GameUI/controllerslot_thunks.cpp.
void __fastcall ApplyControllerSlotFloat(void* host, int slot,
                                         const float* value);

void __fastcall SetControllerSlotFloat(void* host, int slot, float value)
{
    ApplyControllerSlotFloat(host, slot, &value);
}

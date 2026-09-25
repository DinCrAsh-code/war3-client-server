//============================================================================
//  0x6F4E8260 - the public face of the slot read: the same three arguments
//  in the registers a __fastcall puts them in, handed to the private-ABI
//  body next door.
//
//  Own translation unit: the call is real, and this address had been a
//  redirect in gameui_thunks.cpp until its body landed here.
//============================================================================
#include "controllerslot.h"

void __fastcall QuerySlotVec3(void* host, int slot, float* out)
{
    ReadControllerSlotValue((SControllerSlotHost*)host, slot, out);
}

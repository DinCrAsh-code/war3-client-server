//============================================================================
//  0x6F4E80D0 - read one slot's three floats, updating the slot first when
//  its flag byte says the value is stale.
//
//  **The shipped ABI is not reachable from C++.**  The host arrives in eax
//  and the destination in edi, with only the slot number in a register
//  MSVC would use, and the body cleans nothing (`retn 0`); its one shipped
//  caller (0x6F4E8260) sets both up by hand.  So this reconstruction is
//  spelled as an ordinary `__fastcall` - which is consistent with the one
//  caller in this repo and therefore correct inside the `.mix` - and the
//  address is listed in tools/unhookable.py, because patching it would hand
//  the real game's eax/edi convention to a function reading ecx and the
//  stack.
//
//  Which vtable slot the update goes through is the flag byte's bit 1: set
//  picks slot 2 and clear slot 1, and both take one float, which the
//  shipped code always passes as zero.
//
//  Own translation unit: 0x6F4E8260 calls it for real.
//============================================================================
#include "controllerslot.h"
#include "fvec3.h"

void __fastcall ReadControllerSlotValue(SControllerSlotHost* host, int slot,
                                        float* out)
{
    SControllerSlot* entry = host->m_slots[slot];
    unsigned char flags = entry->m_flags;

    if ((flags & 4) != 0)
    {
        typedef void (__thiscall *UpdateSlotFn)(SControllerSlot*, float);

        //  The vftable named once and the slot picked out of it, not two
        //  separate loads: the shipped code has one `mov eax,[esi]` with
        //  `mov edx,[eax+8]` / `mov edx,[eax+4]` on the two arms.
        void** vtable = *(void***)entry;
        UpdateSlotFn update = (UpdateSlotFn)(((flags & 2) != 0) ? vtable[2]
                                                                : vtable[1]);
        update(entry, 0.0f);
    }

    //  Copied as one three-float object and not field by field: the shipped
    //  code moves the three words with integer `mov`s, which a struct
    //  assignment gives and three float assignments do not (those are
    //  `fld`/`fstp` pairs).
    *(FVec3*)out = *(const FVec3*)entry->m_value;
}

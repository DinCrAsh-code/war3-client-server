//============================================================================
//  0x6F4E82D0 - write a three-float value into a slot, keeping whichever
//  components the mask marks as "leave alone" at whatever the slot already
//  holds.
//
//  The slot's current value is read first, into a local the shipped code
//  zeroes before the call - all three words, with `fst`/`fst`/`fstp` off one
//  `fldz` - and each component is then picked between that and the caller's
//  value by one bit of the mask byte: bit 0 for x, bit 1 for y, bit 2 for z.
//  The three picks happen z, y, x and the outgoing vector is filled x, y, z,
//  which is why there are three separate temporaries in between rather than
//  one straight store.
//
//  Own translation unit: the read and the apply are both real calls.
//============================================================================
#include "controllerslot.h"
#include "fvec3.h"

//  0x6F4E81C0 - the apply proper, and one of this layer's private-ABI
//  bodies: the host arrives in eax.  Redirected in
//  GameUI/controllerslot_thunks.cpp.
void __fastcall ApplyControllerSlotVec3(void* host, int slot,
                                        const float* value);

void __fastcall MergeControllerSlotVec3(void* host, int slot,
                                        const float* value, unsigned int mask)
{
    FVec3 current;
    current.m_x = 0.0f;
    current.m_y = 0.0f;
    current.m_z = 0.0f;

    QuerySlotVec3(host, slot, &current.m_x);

    float z = (mask & 4) ? current.m_z : value[2];
    float y = (mask & 2) ? current.m_y : value[1];
    float x = (mask & 1) ? current.m_x : value[0];

    FVec3 merged;
    merged.m_x = x;
    merged.m_y = y;
    merged.m_z = z;

    ApplyControllerSlotVec3(host, slot, &merged.m_x);
}

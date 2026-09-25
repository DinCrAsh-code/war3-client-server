//============================================================================
//  0x6F449CF0 - the free-function wrapper that reaches the *running*
//  thread's own m_scripts array through GetThreadLocalSlot(5).
//
//  0x6F44D8D0/0x6F44D960 - park a JassInstance at a slot of this thread's
//  own +0x88 handle array (SJassThread**/JassInstance**, see
//  jassthreadstate.h): an explicit `index` places it directly and stamps
//  the instance's own +0x2850 with that index; `index == 0` auto-assigns
//  the next free slot - off the +0x98 free-slot stack if one is parked
//  there, or by growing the array (jassthreadinstancearray.cpp's own
//  Grow()) - and hands the assigned index back in eax without stamping
//  +0x2850 itself (every explicit-index caller in this closure already
//  owns that stamp, so the auto-assign arm leaves it to its own caller).
//  An instance that already carries a nonzero +0x2850 is left alone
//  either way.
//============================================================================
#include "storm.h"
#include "jassthreadstate.h"
#include "jassinstance.h"
#include "gamecontext.h"    // GetThreadLocalSlot - 0x6F4C34D0

//  0x6F449CF0 - `retn 0` (__fastcall, index arrives in ecx).
void* __fastcall JassThreadGetScript(unsigned int index)
{
    JassThreadLocal* self = (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);
    return self->GetScript(index);
}

//  0x6F44D8D0
unsigned int JassThreadLocal::AssignInstanceSlot(JassInstance* obj, unsigned int index)
{
    if (index != 0)
    {
        m_handleArrayData[index] = obj;
        *(unsigned int*)((char*)obj + 0x2850) = index;
        return index;
    }

    unsigned int existing = *(unsigned int*)((char*)obj + 0x2850);
    if (existing != 0)
        return existing;

    if (m_freeSlotCount != 0)
    {
        unsigned int freeIndex = m_freeSlotData[m_freeSlotCount - 1];
        ((TSGrowableArray<void*>*)&m_freeSlotAlloc)->SetCount(m_freeSlotCount - 1);
        m_handleArrayData[freeIndex] = obj;
        return freeIndex;
    }

    unsigned int newIndex = m_handleArrayN;
    void** slot = ((TSGrowableArray<void*>*)&m_handleArrayAlloc)->Grow();
    *slot = obj;
    return newIndex;
}

//  0x6F44D960 - `retn 0` (__fastcall, both arguments in registers).
//  Returns AssignInstanceSlot's own assigned-index - see this function's
//  own updated declaration in jassthreadstate.h for why this was `void`
//  and the caller that proves it should not be.
unsigned int __fastcall JassThreadRegisterInstance(JassInstance* obj, unsigned int index)
{
    JassThreadLocal* self = (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);
    return self->AssignInstanceSlot(obj, index);
}

//  0x6F449D10 - `retn 0` (__fastcall, index arrives in ecx) - the
//  GetInstanceAtSlot() twin of JassThreadGetScript() above, against the
//  running thread's own instance-handle array instead of its script array.
JassInstance* __fastcall JassThreadGetInstance(unsigned int index)
{
    JassThreadLocal* self = (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);
    return self->GetInstanceAtSlot(index);
}

//  0x6F44B2E0 - a second, address-distinct copy of the same
//  JassThreadGetInstance() body (a tail `jmp sub_6F449D10` in the shipped
//  binary, not a byte-identical duplicate - this build's toolchain does no
//  link-time /OPT:ICF, so the jump itself cannot be reproduced, only the
//  instruction sequence it jumps into; see jassmoduleteardown.cpp's own
//  note on the same class of gap for 0x6F454680).  Kept as its own real
//  function rather than a naked redirect to the address above, since a
//  thunk in this codebase redirects to the *shipped, unhooked* function,
//  never to another one of this repo's own reconstructions.
JassInstance* __fastcall LookupInstanceBySlot(unsigned int index)
{
    JassThreadLocal* self = (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);
    return self->GetInstanceAtSlot(index);
}

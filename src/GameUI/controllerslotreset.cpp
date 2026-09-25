//============================================================================
//  0x6F4E82C0 - clear one slot back to zero and hand it to its own apply
//  hook.
//
//  IDA folds the tail-jumped block at 0x6F4E81A0 into this function, and
//  that is what it is: 0x6F4E81A0 has no other caller and no heading of its
//  own, so the twelve instructions really are one function - the `mov
//  eax,ecx` at the top is the receiver being moved into the register that
//  block reads.
//
//  The last step is a hand-written slot dispatch: vtable slot 3 of the slot
//  itself, tail-jumped with the caller's own stack argument, which is what
//  fixes its one dword (`retn 4`).
//
//  Own translation unit: the dispatch is a real indirect call.
//============================================================================
#include "controllerslot.h"

void __fastcall ResetControllerSlot(void* host, int slot, void* arg)
{
    SControllerSlot* entry = ((SControllerSlotHost*)host)->m_slots[slot];

    entry->m_field18 = 0.0f;
    entry->m_field10 = 0;
    entry->m_field14 = 0;

    typedef void (__thiscall *ApplySlotFn)(SControllerSlot*, void*);
    ((ApplySlotFn)(*(void***)entry)[3])(entry, arg);
}

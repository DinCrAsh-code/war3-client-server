//============================================================================
//  0x6F495030 - NIpse::CPoSeparate::Slot4, vtable slot 4 ("Slot4" - column
//  name, see poseparate.h's own header comment). CPoSeparate's own real
//  override of the column NTempest::CPresence::Slot4
//  (Agent/presence_slot4.cpp) leaves a bare indirect tail-call: this one
//  unlinks `this` from its own +0x04/+0x08 live-instance list and zeroes
//  m_field14 FIRST, then still tail-calls through `this`'s own vtable slot
//  1 (ReturnToStaticPool) exactly the way CPresence::Slot4 does. A live
//  call therefore unlinks the (already just-emptied) +0x04/+0x08 pair a
//  second time, harmlessly, inside ReturnToStaticPool itself before the
//  pool push runs - transcribed exactly as the shipped double-unlink
//  rather than "cleaned up" into one, per CLAUDE.md's "say why when the
//  source looks odd" rule. `flags` is never read, same as Apply above and
//  CPresence::Slot4 itself.
//============================================================================
#include "poseparate.h"

namespace NIpse {

typedef void* (__thiscall *ReturnToPoolFn)(void*);

void* CPoSeparate::Slot4(unsigned int /*flags*/)
{
    char* self = (char*)this;
    void* prev = *(void**)(self + 4);
    void* next = *(void**)(self + 8);
    if (prev != 0)
        *(void**)((char*)prev + 8) = next;
    if (next != 0)
        *(void**)((char*)next + 4) = prev;
    *(void* volatile*)(self + 4) = 0;
    *(void* volatile*)(self + 8) = 0;

    //  The vtable/slot-1 fetch happens AFTER the two zero-stores above but
    //  BEFORE m_field14 is zeroed - the dump's own
    //  `mov eax,[ecx] / mov edx,[eax+4]` sits between the +0x04/+0x08
    //  zero-stores and `mov dword ptr [ecx+14h],0`. The +0x04/+0x08
    //  stores are marked volatile purely to pin them ahead of the vtable
    //  read in program order (no aliasing would otherwise stop the
    //  optimiser hoisting the read above them, since the two never touch
    //  the same offset) - not a claim that the shipped code itself used
    //  volatile, just the smallest C++ spelling that reproduces the
    //  dump's own instruction order exactly.
    void* volatile* vtbl = *(void* volatile**)this;
    ReturnToPoolFn fn = (ReturnToPoolFn)vtbl[1];
    m_field14 = 0;
    return fn(this);
}

}  // namespace NIpse

//============================================================================
//  Two small forwarders that reuse an already-reconstructed leaf from
//  elsewhere in this dump rather than reimplementing it.
//============================================================================
#include "vslot4thunk.h"
#include "refcnt.h"

//----------------------------------------------------------------------------
//  0x6F6F1A20 - `mov ecx,[ecx+4] / call CallVSlot4 / sub eax,1 / retn`:
//  read a vtable-4 target out of `this+4` and return one less than
//  whatever it reports. Nothing in this call tree names the owning class
//  or the +4 member beyond "holds an IVSlot4Target*".
//----------------------------------------------------------------------------
struct SVSlot4Holder
{
    char           m_reserved00[4];
    IVSlot4Target* m_target;   // +4
};

int __fastcall ForwardVSlot4MinusOne(SVSlot4Holder* self)
{
    return CallVSlot4(self->m_target) - 1;
}

//----------------------------------------------------------------------------
//  0x6F6F1A00 - `cmp [ecx+1A0h],0 / jnz skip / mov ecx,[esp+arg_0] / call
//  ReleaseForward_6F4C8870 / retn 4`: release the given ref-counted object
//  through the shared 0x6F4C8870 forwarder, but only when `this`'s dword at
//  +0x1A0 is still zero. Neither the owning class nor what a nonzero flag
//  means is named anywhere in this call tree.
//
//  `obj` arrives on the stack, not in edx - the same "middle fastcall
//  register skipped, real argument one slot further out" shape
//  ParseConfigInt's crash fix (see parseconfigint.cpp/funcmap.py) already
//  documents, mirrored here as an unused `edx` rather than an unused `ecx`.
//  It also matters for the shape of the call: with `obj` genuinely on the
//  stack, this wrapper's own `retn 4` has to pop a dword
//  ReleaseForward_6F4C8870 itself never pushed, so the compiler cannot fold
//  the call into a tail `jmp` the way ReleaseForward_6F4C8870 itself does
//  for TRefCnt::Release - it needs a real `call` and its own explicit
//  `retn 4` afterward, exactly what the target shows.
//----------------------------------------------------------------------------
struct SReleaseGate
{
    char m_reserved00[0x1A0];
    int  m_suppressRelease;   // +0x1A0 - nonzero skips the release
};

void __fastcall ReleaseIfNotSuppressed(SReleaseGate* self, void* /*unused_edx*/, TRefCnt* obj)
{
    if (self->m_suppressRelease == 0)
        ReleaseForward_6F4C8870(obj);
}

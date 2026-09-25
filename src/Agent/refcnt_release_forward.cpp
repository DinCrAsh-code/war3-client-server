//============================================================================
//  0x6F4C8870 / 0x6F4C8880 - two more addresses, both a single `jmp
//  sub_6F4C40A0` in the target: some other class's vtable slot (or slots -
//  two different call sites want the same operation) forwarding straight to
//  TRefCnt::Release() with no adjustment. This call tree does not reach
//  either forwarder's own class, only that both addresses are a pure tail
//  call to Release() with `this` passed on unchanged - the same "two
//  identical addresses, one shape" situation the five TSGrowableArray<T>
//  destructors already in this codebase are in, just with a forward instead
//  of a body. Named generically since nothing here identifies which derived
//  class either slot belongs to.
//
//  Kept in their own translation unit, separate from refcnt.cpp: the target
//  is a real cross-module `jmp`, not an inlined copy of Release()'s body -
//  this codebase's own /O2 /Ob2 will otherwise inline Release() on sight
//  from within the same TU, same as TRefCnt::Self() needed splitting out for
//  (see refcnt_self.cpp).
//============================================================================
#include "refcnt.h"

void __fastcall ReleaseForward_6F4C8870(TRefCnt* self)
{
    self->Release();
}

void __fastcall ReleaseForward_6F4C8880(TRefCnt* self)
{
    self->Release();
}

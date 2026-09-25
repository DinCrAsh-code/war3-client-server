//============================================================================
//  0x6F26A4C0 - CDestructable's own override of vtable slot 72 (+0x120),
//  NegateAndForwardOffset.
//
//  A naked redirect, not a reconstruction: the shipped body walks a
//  regrowth/lookup helper keyed by `unk_6FAB4204`, this object's own
//  sprite pointer, and a small {4, 0x1C} descriptor handed to a function
//  named nowhere else in this codebase (0x6F4D7E50), then conditionally
//  plays a looping animation - real behaviour this vtable-slot pass did
//  not chase down with enough confidence to hand-write correctly (getting
//  an argument count or a field's real meaning wrong here is exactly the
//  class of live crash abi_audit.py exists to catch, and this slot's
//  argument shapes were not independently confirmed the way
//  thunk_abi_audit.py requires for a hand-written body).  `retn 10h`
//  matches the four-stack-argument declaration in destructable.h exactly,
//  so the ABI is safe even though the body is not yet reconstructed - see
//  docs/targets/CDestructable__vtable.md for what is left.
//============================================================================
#include "destructable.h"

__declspec(naked) void* CDestructable::NegateAndForwardOffset(
    void* /*target*/, SWidgetOffsetSource* /*src*/, int /*a3*/, int /*a4*/)
{
    __asm { mov eax, 06F26A4C0h }
    __asm { jmp eax }
}

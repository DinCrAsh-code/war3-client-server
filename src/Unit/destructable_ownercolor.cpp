//============================================================================
//  0x6F26A170 - CDestructable's own override of vtable slot 16 (+0x40),
//  RefreshOwningPlayerColor.
//
//  A naked redirect, not a reconstruction: 136 instructions deep into
//  handle-object resolution (through this object's own slot 46, +0xB8),
//  Position::RecordSlot8-shaped record building, sprite tint application
//  and pathing-mask refresh, none of it independently confirmed to this
//  session's own confidence bar (see destructable_offset.cpp's own note on
//  why an under-confirmed argument shape here is exactly the class of live
//  crash abi_audit.py exists to catch).  `retn` with no operand matches the
//  zero-argument declaration in destructable.h exactly, so the ABI is safe
//  even though the body is not yet reconstructed - see
//  docs/targets/CDestructable__vtable.md for what is left.
//============================================================================
#include "destructable.h"

__declspec(naked) void CDestructable::RefreshOwningPlayerColor()
{
    __asm { mov eax, 06F26A170h }
    __asm { jmp eax }
}

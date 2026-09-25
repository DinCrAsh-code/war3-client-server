//============================================================================
//  0x6F269120 - CDestructable's own override of vtable slot 65 (+0x104),
//  RefreshTerrainTint.
//
//  A naked redirect, not a reconstruction: 215 instructions, the largest
//  of the 34 slots this session's sweep covers, and out of reach of this
//  pass's own confidence bar for the same reason
//  destructable_ownercolor.cpp's slot 16 is - see
//  docs/targets/CDestructable__vtable.md for what is left.  `retn` with no
//  operand matches the zero-argument declaration in destructable.h
//  exactly, so the ABI is safe even though the body is not yet
//  reconstructed.
//============================================================================
#include "destructable.h"

__declspec(naked) void CDestructable::RefreshTerrainTint()
{
    __asm { mov eax, 06F269120h }
    __asm { jmp eax }
}

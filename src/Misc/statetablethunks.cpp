//============================================================================
//  The two arms of SStateTable::ApplyByKind and the row applier under
//  ApplyIndexed, none of which is in this batch.  Naked redirects to the
//  shipped bodies; each goes away as its own module lands.
//
//  The calling conventions are the shipped ones and are load-bearing: two of
//  the three take `this` in ecx and clean their own stack arguments, and
//  0x6F7415D0 takes nothing in a register at all.
//============================================================================
#include "statetable.h"

#define STATE_THUNK(addr)        \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//  0x6F741700 - `retn 4`.  Hands ApplyStateRow a fixed row: `this+0x7A4`,
//  which is `m_pRows+8` (statetable.h) - one row in from the table's own
//  base, not `value`'s own index.  __thiscall with one stack argument, so
//  `this` (ecx) never appears in the dump as a read - only the arithmetic
//  on it does.
void SStateTable::ApplyDefault(void* value)
{
    char* row = (char*)this + 0x7A4;
    ApplyStateRow(row, value);
}

//  0x6F763D50 - `retn 8`.
__declspec(naked) void SStateTable::ApplyPreset(void*, int) STATE_THUNK(0x6F763D50)

//  0x6F7415D0 - `retn 8`, __stdcall.
__declspec(naked) void __stdcall ApplyStateRow(void*, void*) STATE_THUNK(0x6F7415D0)

#undef STATE_THUNK

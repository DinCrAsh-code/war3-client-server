//============================================================================
//  0x6F4C4630 and its 0x6F4C4660 alias - read a handle-bearing object's own
//  recorded handle, or zero when it has none.
//
//  0x6F4C4660 is a one-instruction `jmp` to 0x6F4C4630 and IDA gives it its
//  own name, so it is a genuine second entry point rather than a chunk -
//  the linker's ICF-style alias for a second inline expansion of the same
//  member, most likely.  It is reproduced as a naked tail jump rather than a
//  duplicate body, because two bodies would be two different functions and
//  the shipped alias is one instruction.
//============================================================================

#include "handleobject.h"

unsigned int __fastcall GetHandleOrZero(const SHandleHolder* self)
{
    if (self->m_pOwner != 0)
        return self->m_pOwner->m_handle;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4C4660.
//----------------------------------------------------------------------------
__declspec(naked) unsigned int __fastcall GetHandleOrZeroAlias(
        const SHandleHolder*)
{
    __asm { jmp GetHandleOrZero }
}

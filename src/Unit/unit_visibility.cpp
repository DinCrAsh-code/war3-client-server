//============================================================================
//  0x6F28C510 - CUnit's vtable slot 62 (+0xF8).  See unit.h.
//
//  Three gates, then a masked compare.  The unit has to say it is visible
//  (its own slot 60 with 1), its slot 111 has to say *no*, and bit 0x20 of
//  CAgentWar3::m_flags has to be clear; any of those and the answer is 0.
//  Past them, take a 16-bit word off whatever 0x6F2834E0 hands back, AND
//  it with the caller's second argument, and answer whether the result is
//  *empty*.
//
//  The inversion is not a reading of `neg ax / sbb eax, eax / add eax, 1`
//  that could go either way: `neg ax` sets the carry exactly when ax is
//  non-zero, `sbb eax, eax` turns carry into -1 and no-carry into 0, and
//  `add eax, 1` makes that 0 and 1 - so 1 means the masked word came out
//  zero.
//
//  The first parameter is never read.  It is still declared, because the
//  slot's `retn 8` says two dwords are pushed and cleaned, and dropping it
//  would leave every caller four bytes out (docs/notes/hook-abi-crash-
//  classes.md).
//============================================================================
#include "unit.h"

//  0x6F2834E0 (`retn 0`) - CUnit's own; hands back the object the mask
//  comes from.  0x6F40B1E0 (`retn 0`) - that object's own 16-bit accessor.
void* __fastcall UnitVisibilityMaskOwner_6F2834E0(void* self);
unsigned short __fastcall VisibilityMaskWord_6F40B1E0(void* self);

int CUnit::IsVisibilityMaskLive(unsigned int a, unsigned int b)
{
    (void)a;
    //  One short-circuiting condition, not three early returns: written as
    //  three `if (...) return 0;` this compiler puts the shared
    //  `xor eax, eax` block in the middle and branches back to it, where
    //  the shipped code has it at the end with three forward jumps.
    if (QueryVisible(1) != 0 && Method_0x1BC() == 0 && (m_flags & 0x20) == 0)
    {
        unsigned short masked = (unsigned short)
            (VisibilityMaskWord_6F40B1E0(UnitVisibilityMaskOwner_6F2834E0(this)) & b);
        return masked == 0;
    }
    return 0;
}

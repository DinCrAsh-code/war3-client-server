//============================================================================
//  0x6F4BC3A0 - draw one uniform-[0,1) CFloat out of an SHashState and
//  scale it by `factor`: the classic float-PRNG bit trick - force the
//  state's low 23 bits into a [1,2) float's mantissa (`| 0x3F800000`),
//  then subtract 1.0 (g_CFloatMinusOne) to land in [0,1).
//
//  No register argument at all (`__stdcall`, three plain stack dwords,
//  `retn 0Ch`) - not this codebase's usual __fastcall.  The one incoming
//  `ecx` in the shipped code is a bare `push ecx` used as four bytes of
//  scratch stack space, not a parameter.
//============================================================================
#include "CFloat.h"
#include "hashstate.h"

extern const CFloat g_CFloatMinusOne;   // dword_6FAAE464

void __stdcall RandomCFloat(CFloat* dest, const CFloat* factor,
                            SHashState* state)
{
    int bits = (state->Advance() & 0x7FFFFF) | 0x3F800000;
    CFloat uniform = CFloat(bits) + g_CFloatMinusOne;
    *dest = *factor * uniform;
}

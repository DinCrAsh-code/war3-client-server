//============================================================================
//  0x6F4BBFE0 - draw one uniform-[0,1) float out of an SHashState (the same
//  bit trick RandomCFloat uses - force the low 23 bits into a [1,2) float's
//  mantissa, then subtract 1.0), scale it by `range`, and truncate to int.
//
//  __stdcall, two plain stack dwords, `retn 8` - not this codebase's usual
//  __fastcall, same as RandomCFloat.
//============================================================================
#include "hashstate.h"

int __stdcall RandomInt(int range, SHashState* state)
{
    int bits = (state->Advance() & 0x7FFFFF) | 0x3F800000;
    float uniform = *(float*)&bits - 1.0f;
    return (int)(uniform * range);
}

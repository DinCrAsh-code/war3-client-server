//============================================================================
//  0x6F266760 - ModulateColors: multiply two packed colours channel by
//  channel, each product scaled back down by 255.
//
//  `/ 255` on a plain `int` and not `>> 8` or a reciprocal multiply written
//  out by hand: the shipped code is MSVC's own signed divide-by-255
//  sequence (`imul 80808081h` / `add` / `sar 7` / `shr 31` / `add`), which
//  is what the compiler emits for the division and nothing else produces.
//  The operands are `unsigned char`, so each product promotes to a signed
//  int and can never actually be negative - the sign correction is dead
//  every time, and it is still in the binary because the source divided an
//  `int`.
//
//  The channels are done from 3 down to 0, and the whole dword is zeroed
//  first even though all four bytes are written afterwards - both are the
//  shipped order and both are in the source rather than the scheduler.
//
//  Own translation unit: its one caller in this tree (CItem's slot 65,
//  item_slot65.cpp) reaches it with a real `call`.
//============================================================================
#include "packedcolor.h"

SPackedColor* __fastcall ModulateColors(SPackedColor* out,
                                        const SPackedColor* a,
                                        const SPackedColor* b)
{
    *(unsigned int*)out = 0;
    out->m_c3 = (unsigned char)(a->m_c3 * b->m_c3 / 255);
    out->m_c2 = (unsigned char)(a->m_c2 * b->m_c2 / 255);
    out->m_c1 = (unsigned char)(a->m_c1 * b->m_c1 / 255);
    out->m_c0 = (unsigned char)(a->m_c0 * b->m_c0 / 255);
    return out;
}

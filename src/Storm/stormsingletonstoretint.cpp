//============================================================================
//  0x6F7424B0 - SStormSingletonA::StoreTint.
//
//  Was a `mov eax,<address> / jmp eax` redirect on the grounds that it is
//  "~270 instructions of fixed-point blending in Storm's own address
//  range".  It is 85 instructions, and 60 of them are one idiom repeated
//  four times.
//
//  What it does: fade the caller's colour towards the singleton's own
//  target colour (+0x3C) by the blend weight the terrain record carries,
//  then modulate the caller's colour by the result in place.  Per channel
//
//      blend[i] = 255 - (255 - target[i]) * weight / 255
//
//  so a weight of 0 would leave 255 (fully transparent, no change) - which
//  is exactly the case the early-out above skips, and why the shipped code
//  can afford to test it before doing any of this.
//
//  `/ 255` on a plain `int`, not `>> 8`: the shipped code is MSVC's own
//  signed divide-by-255 (`imul 80808081h` / `add` / `sar 7` / `shr 31` /
//  `add`), the same sequence ModulateColors (colormodulate.cpp) carries and
//  the same dead sign correction - the operands are bytes, so the product
//  can never be negative.  Nothing but the division produces it.
//
//  Channels run from 3 down to 0, and the local is zeroed as a whole dword
//  before any of the four bytes lands in it, both of which are in the
//  source rather than in the scheduler - see colormodulate.cpp for the same
//  pair.
//
//  Own translation unit: it reaches ModulateColors with a real call, and
//  stormsingleton_terrain.cpp must reach this with one.
//============================================================================
#include "stormsingletona.h"
#include "packedcolor.h"

void SStormSingletonA::StoreTint(int recordIndex, const unsigned int* tint)
{
    if (m_tintTableA == 0)
        return;
    if (m_tintTableB == 0)
        return;
    if (recordIndex < 0)
        return;
    if (recordIndex >= m_tintRecordCount)
        return;

    //  24-byte records; the blend weight is the byte at +0x14 of each.  Read
    //  as a byte and tested as a byte (`mov al,[...]` / `test al,al`), not
    //  widened first - widening here costs the `movzx` its position.
    unsigned char weight = m_tintRecords[recordIndex * 24 + 0x14];
    if (weight == 0)
        return;

    SPackedColor blend;
    *(unsigned int*)&blend = 0;
    blend.m_c3 = (unsigned char)(255 - (255 - m_tintTarget.m_c3) * weight / 255);
    blend.m_c2 = (unsigned char)(255 - (255 - m_tintTarget.m_c2) * weight / 255);
    blend.m_c1 = (unsigned char)(255 - (255 - m_tintTarget.m_c1) * weight / 255);
    blend.m_c0 = (unsigned char)(255 - (255 - m_tintTarget.m_c0) * weight / 255);

    //  In place: the modulate writes into its own out-parameter and this
    //  copies the whole dword back over the caller's colour, which is what
    //  the shipped `mov ecx,[eax]` / `mov [esi],ecx` pair is.  Handing
    //  ModulateColors the caller's own pointer as `out` would be a
    //  different function - it zeroes `out` before it reads `a`.
    //  Read back through the *returned* pointer, not through the local:
    //  the shipped code is `mov ecx,[eax]` off ModulateColors' own return
    //  value, and naming the local instead makes MSVC address the frame
    //  slot directly.
    SPackedColor modulated;
    *(unsigned int*)tint =
        *(const unsigned int*)ModulateColors(&modulated,
                                             (const SPackedColor*)tint,
                                             &blend);
}

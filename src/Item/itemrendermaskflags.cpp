//============================================================================
//  0x6F2C6200 / 0x6F2C6210 / 0x6F2C6220 / 0x6F2C6230 - four of the six leaf
//  helpers behind SItemMaskNotify::OnMaskChanged (0x6F38EA30,
//  itemsinkmaskchanged.cpp, still a thunk: three of its own six callees -
//  0x6F2C6240, 0x6F60FCE0, 0x6F2C64D0 - are hash-table/footprint-cell
//  lookups whose owning layout is not established, so that caller cannot
//  be finished from these four alone).  Reconstructed independently
//  because each is a complete, self-contained leaf that only ever reads
//  or writes one field of its own `this` and needs no assumption about
//  the rest of that object's layout.
//
//  The two fields sit back-to-back (+0x4A, +0x4C) on whatever render-side
//  object 0x6F38EA30 passes as `this`; nothing in this depth-3 cut says what
//  that class is, so it is named generically here rather than guessed.  Both
//  are plain `unsigned short`s: `mov ax, [.]` / `mov [.], ax` never widens
//  through eax.
//
//  0x6F2C6200 (GetMaskLow) is the getter twin of SetMaskLow one address
//  below it, reached by Widget/widget_ghostimagemaskclear.cpp
//  (0x6F2AF7E0) - `mov eax, [ecx+4Ah]` widened to 32 bits by the caller's
//  own `and ax, di`, not here.
//============================================================================
#include "itemrendermaskflags.h"

//  0x6F2C6200 - thiscall, no stack arguments (`retn`).
unsigned short SItemRenderMaskFields::GetMaskLow() const
{
    return m_maskLow;
}

//  0x6F2C6210 - thiscall, one stack argument (`retn 4`).
void SItemRenderMaskFields::SetMaskLow(unsigned short value)
{
    m_maskLow = value;
}

//  0x6F2C6220 - thiscall, no stack arguments (`retn`).
unsigned short SItemRenderMaskFields::GetMaskHigh() const
{
    return m_maskHigh;
}

//  0x6F2C6230 - thiscall, one stack argument (`retn 4`).
void SItemRenderMaskFields::OrMaskHigh(unsigned short bits)
{
    m_maskHigh |= bits;
}

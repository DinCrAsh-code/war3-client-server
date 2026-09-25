//============================================================================
//  0x6F611630 - CTextFrame::HasStyleBits.
//
//  "Are *all* of `mask`'s bits set in the style word +0x1F0 points at?" - an
//  and-then-compare-against-the-mask, not a plain `!= 0` test, which is what
//  the `and ecx,eax` / `cmp ecx,eax` pair says.  Both call sites ask for
//  0x100 and both use the answer to pick between the raw string and the
//  masked one, so 0x100 is the "draw this as a password" bit.
//
//  Its own translation unit: FreeText and SetString both call it for real in
//  the shipped stream, and at nine instructions it would inline into either
//  of them on sight.
//============================================================================
#include "frame.h"

int CTextFrame::HasStyleBits(int mask)
{
    return (*m_pStyle & mask) == mask;
}

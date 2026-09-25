//============================================================================
//  0x6F60DAF0 - CSimpleFontString::SetJustify.
//
//  Three bits, highest wins, and each arm clears a *different* pair before
//  setting its own: bit 2 clears bits 0 and 1, bit 1 clears bits 0 and 2, bit
//  0 clears bits 1 and 2.  An argument with none of the three set leaves the
//  style word alone entirely - the shipped code jumps past the store, not to
//  it, which is why the comparison at the bottom is against the value read at
//  the top rather than against the argument.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleFontString::SetJustify(int justify)
{
    int before = m_style;

    if (justify & 4)
        m_style = (before & ~3) | 4;
    else if (justify & 2)
        m_style = (before & ~5) | 2;
    else if (justify & 1)
        m_style = (before & ~6) | 1;

    if (m_style != before)
        Rebuild(0);
}

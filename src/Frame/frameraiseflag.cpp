//============================================================================
//  0x6F6088F0 - CSimpleFrame::RaiseFlagBit: set one bit of the frame's flag
//  word, and the first time it goes up, tell the CSimpleTop that owns it.
//
//  Its own translation unit because CSimpleButton's constructor calls it.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleFrame::RaiseFlagBit(int bit, int arg)
{
    int mask = 1 << bit;
    if (m_flags & mask)
        return;

    if (m_attached)
        m_pTop->AddFrameToLayer(this, bit, arg);

    m_flags |= mask;
}

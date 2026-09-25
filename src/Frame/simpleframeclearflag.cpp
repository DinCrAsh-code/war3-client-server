//============================================================================
//  0x6F608930 - CSimpleFrame::ClearFlagBit, RaiseFlagBit's opposite number.
//
//  Each bit doubles as a draw-layer index: raising one for the first time
//  adds this frame to CSimpleTop's list for that layer, and clearing it takes
//  it out again.  Its own translation unit because RaiseFlagBit
//  (frameraiseflag.cpp) is one, and neither calls the other.
//
//  Restored after the frame-library merge: the batch that reconstructed both
//  halves had them in one file, and the merge dropped that file in favour of
//  an upstream one that only carried the raise, leaving 0x6F608930 as a
//  redirect with a funcmap row pointing at it.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleFrame::ClearFlagBit(int bit)
{
    unsigned int mask = 1u << bit;

    if ((m_flags & mask) != 0)
    {
        if (m_attached != 0)
            m_pTop->RemoveFrameFromLayer(this, bit);

        m_flags &= ~mask;
    }
}

//============================================================================
//  0x6F608550 - CSimpleFrame::SetOwnedFrame: hand this frame to the
//  generated backdrop and remember it.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleFrame::SetOwnedFrame(void* frame)
{
    ((CBackdropGenerator*)frame)->Generate(this);
    m_pOwnedFrame = frame;
}

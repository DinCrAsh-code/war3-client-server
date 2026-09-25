//============================================================================
//  0x6F6058F0 - CSimpleFrame's own override of slot 1 (+0x04).  No
//  arguments.  Build a zeroed SFrameRect on the stack, ask
//  CLayoutFrame::ComputeContentRect to fill it in, and - only if it
//  produced one - apply it through slot 3 (SetRect).
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleFrame::UpdateContentRect()
{
    SFrameRect rect = {0.0f, 0.0f, 0.0f, 0.0f};

    if (ComputeContentRect(&rect))
        SetRect(&rect);
}

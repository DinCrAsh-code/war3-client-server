//============================================================================
//  0x6F605CC0 - CLayoutFrame::QueueForLayout, the module's own tail: six of
//  its neighbours end in a jump into it.  Its own translation unit for
//  exactly that reason.
//
//  The queue is one global TSExplicitList (dword_6FA9A990) and the body is
//  its LinkToHead, inlined - Unlink first, then splice at the head.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  0x6F605CC0 - put this frame on the global re-layout queue.
void CLayoutFrame::QueueForLayout(int checkQueued)
{
    if (checkQueued && IsLayoutQueued())
        return;

    g_layoutQueue.LinkToHead(this);
}


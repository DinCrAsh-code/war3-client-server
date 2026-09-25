//============================================================================
//  0x6F45AD70 - the other half of jassscriptframepool.h's own
//  ReleaseScriptFrameContents note ("does not itself return `frame` to
//  g_jassFramePool ... whatever calls this is presumably what does that
//  next"): this is that caller.  Tears the frame's contents down, then
//  pushes the frame back onto the pool's own free list, LIFO, through its
//  link word 4 bytes before the JassScriptFrame* AcquireFrame hands out
//  (jassframepool.h's own note on that offset).
//============================================================================
#include "jassframepool.h"
#include "jassscriptframepool.h"

void JassFramePool::ReleaseFrame(JassScriptFrame* frame)
{
    ReleaseScriptFrameContents(frame);

    void* link = (char*)frame - 4;
    *(void**)link = m_frameListHead;
    m_liveFrameCount--;
    m_frameListHead = link;
}

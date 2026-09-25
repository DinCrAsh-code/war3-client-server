//============================================================================
//  0x6F60A450 - CScreenFrame::ReleaseRefs: drop all four of the frame's
//  counted references, backwards through the array.
//
//  The release is the "last one out destroys it" shape written out rather
//  than a call to TRefCnt::Release - the shipped code decrements the count in
//  place and only dispatches slot 0 when it hits zero, four times over, which
//  is what an inlined loop body over a four-element array looks like.
//
//  Its own translation unit because the constructor calls it.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#include "refcnt.h"

void CScreenFrame::ReleaseRefs()
{
    SFrameRef* slot = m_refs;
    int remaining = 4;

    do
    {
        TRefCnt* ref = (TRefCnt*)slot->m_pRef;
        if (ref && --ref->m_refcount == 0)
        {
            typedef void (__thiscall *DestroyFn)(void*);
            ((DestroyFn)(*(void***)ref)[0])(ref);
        }
        slot->m_pRef = 0;
        ++slot;
    }
    while (--remaining);
}

//============================================================================
//  0x6F6089D0 - CSimpleFrame's own override of CLayoutFrame::DestroyLayout,
//  slot 0 (+0x00) of CSimpleButton's vtable (0x6F96DC8C) and, by
//  inheritance, of every other CLayoutFrame-derived class this session did
//  not check individually.
//
//  Despite the inherited name, the real body never treats `deleteFlags` as
//  a delete-flags word: every caller this session found (CLayoutFrame::
//  SetRect, slot 3, Frame/simpleframevtable3.cpp) passes its own
//  `const SFrameRect*` straight through, so it is reinterpreted as one
//  here.  It cannot be renamed or re-typed on the signature itself -
//  overriding a base `virtual` requires the identical name and parameter
//  types - so this comment carries the correction instead.
//
//  Body: recompute this frame's queued-layout dependants against the new
//  rect (RecomputeQueuedLayout) and the cached screen rect
//  (RecomputeScreenRect) unconditionally, then - only if this frame owns
//  another frame (CSimpleFrame::SetOwnedFrame) - cascade the same rect into
//  it, *unless* both the height and the width changed at once.  That "both
//  changed -> skip" branch is not a rewrite artefact: the shipped code
//  jumps straight past the width test the instant the height test alone
//  says "equal" and only bothers with width when height differed, and
//  either equal test on its own is enough to cascade - the one case that
//  does *not* cascade is both axes changing together.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleFrame::DestroyLayout(int deleteFlags)
{
    const SFrameRect* rect = (const SFrameRect*)deleteFlags;

    RecomputeQueuedLayout(rect);
    RecomputeScreenRect(rect);

    void* owned = m_pOwnedFrame;
    if (owned)
    {
        //  Argument order matters here: __stdcall evaluates right-to-left,
        //  and the shipped stream computes the m_rect side first - so the
        //  m_rect subtraction has to be the *second* source argument.
        int heightEqual = FloatNearlyEqual(rect->y1 - rect->y0, m_rect[3] - m_rect[1]);
        if (!heightEqual)
        {
            int widthEqual = FloatNearlyEqual(rect->x1 - rect->x0, m_rect[2] - m_rect[0]);
            if (!widthEqual)
                return;
        }
        ApplyOwnedFrameRect(owned, 0, rect);
    }
}

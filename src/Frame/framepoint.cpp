//============================================================================
//  CFramePoint - ".\\CFramePoint.cpp", 0x6F629440 / 0x6F629490.
//
//  Two pools, one per flavour: the absolute point comes out of the pool at
//  unk_6FACE9AC and the relative one out of unk_6FACE9C0, and the only way to
//  tell a point's flavour apart at release time is to ask slot 3 for the
//  frame it is anchored to.  That is why Release() below is a *dispatch* and
//  not a branch on a stored flag.
//
//  The dump's block for 0x6F629440 runs three shipped functions together -
//  the dispatcher and the two tails it jumps to (0x6F6292A0 and 0x6F6292C0,
//  which have no headings of their own and live in framepointrelease.cpp) -
//  so the heading's score covers the dispatcher's own eleven instructions
//  and the twenty-six after them belong to the other two.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  0x6F629440 - hand this point back to whichever pool it came out of.  Both
//  arms are tail jumps in the shipped stream.
void CFramePoint::Release()
{
    if (GetRelativeFrame())
        ReleaseRelativeFramePoint(this);
    else
        ReleaseAbsoluteFramePoint(this);
}

//  0x6F629490 - make a point anchored to `otherPoint` of `frame`, at (x, y)
//  from it.  A genuine __fastcall: the frame arrives in ecx and the point
//  index in edx, with only the two floats on the stack.  Returns null when
//  the pool is empty - the shipped code checks and its callers do not.
CFramePoint* __fastcall MakeRelativeFramePoint(CLayoutFrame* frame,
                                               int otherPoint,
                                               float x, float y)
{
    CFramePointRelative* point = (CFramePointRelative*)
        g_framePointRelativePool.Alloc(0, ".?AVCFramePointRelative@@", -2);
    if (point)
        new (point) CFramePointRelative(frame, otherPoint, x, y);
    return point;
}

//  0x6F629460 - make a point pinned to an absolute screen position.
//  `__stdcall`: both parameters are floats and MSVC never puts a float in a
//  register, so nothing is left for ecx to carry and the callee cleans all
//  eight bytes itself.  Returns null when the pool is empty, exactly as the
//  relative factory above does.
CFramePoint* __stdcall MakeAbsoluteFramePoint(float x, float y)
{
    CFramePointAbsolute* point = (CFramePointAbsolute*)
        g_framePointPool.Alloc(0, ".?AVCFramePointAbsolute@@", -2);
    if (point)
        new (point) CFramePointAbsolute(x, y);
    return point;
}

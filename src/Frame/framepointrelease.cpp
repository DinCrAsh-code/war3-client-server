//============================================================================
//  0x6F6292A0 / 0x6F6292C0 - the two halves of a frame point's release, one
//  per pool.  The shipped build has them as two functions and
//  CFramePoint::Release tail-jumps into whichever one slot 3's answer picks,
//  so they get a translation unit of their own: folded into the dispatcher
//  they would inline and both tail jumps would go.
//
//  They differ only in the pool and in the __LINE__ the allocator is handed
//  (0x28 against 0x3F), which is itself the evidence that the shipped source
//  really is two functions rather than one with a pool argument.
//============================================================================
#include "frame.h"

//  0x6F6292A0 - the absolute point's pool.
void __fastcall ReleaseAbsoluteFramePoint(CFramePoint* point)
{
    point->Destroy(0);
    g_framePointPool.Free(point, ".\\CFramePoint.cpp", 0x28);
}

//  0x6F6292C0 - the relative point's.
void __fastcall ReleaseRelativeFramePoint(CFramePoint* point)
{
    point->Destroy(0);
    g_framePointRelativePool.Free(point, ".\\CFramePoint.cpp", 0x3F);
}

//============================================================================
//  0x6F608490 - CFrame::Create, ".\\CFrame.cpp"'s own factory: 0x168 bytes
//  from Storm and a CFrame constructed into them, or null.
//
//  A genuine __fastcall static: two of its three arguments arrive in ecx and
//  edx and only the third is pushed (`retn 4`).  The null test in front of the
//  constructor call is placement-new's own, not a hand-written one.
//
//  Score capped by the same unified EH frame CFrame::CFrame carries; see
//  framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CFrame* __fastcall CFrame::Create(int a, int b, int c)
{
    CFrame* frame = (CFrame*)SMemAlloc(0x168, ".\\CFrame.cpp", 0xE2, 0);
    if (frame)
        return new (frame) CFrame(a, b, c);
    return 0;
}

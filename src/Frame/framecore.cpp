//============================================================================
//  0x6F608200 - CFrame::CFrame, the two-base constructor at the root of the
//  whole library.
//
//  Its first base is CLayer (0x6F600720, clayerctor.cpp) and its
//  second is CLayoutFrame at +0xB4 (0x6F6065C0, clayoutframector.cpp), which is
//  what makes MSVC stamp *two* vtables - ??_7CFrame@@6B@ at +0 and
//  ??_7CFrame@@6B@_0 at +0xB4, exactly as the shipped stream does.
//
//  **The score is capped here and the shortfall is not source-addressable.**
//  The shipped function carries the unified `__CxxFrameHandler3` EH frame
//  docs/msvc-vc8-idioms.md describes ("An `__except_handler4`-shaped frame
//  this toolchain cannot reproduce"), and the dump's block for this heading
//  runs the whole appended scope table and its five unwind stubs in with the
//  body.  Nothing this compiler can be told to do emits that frame.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CFrame::CFrame(int a, int b, int c)
    : CLayer((CLayer*)a, b, c)
{
    m_11C = 0;
    m_120 = 0;
    m_124 = 0;
    m_128 = 0;
    m_12C = 0.0f;
    m_130 = 0.0f;
    m_134 = 0.0f;
    m_138 = 0.0f;
    m_13C = 0.0f;
    m_140 = 0;
    m_lightCount = 0;
    m_160 = 0;
    m_164 = 0;

    //  The owning layer's own +0x114 is the scale every frame under it
    //  starts at, and it goes down the whole subtree (0x6F607930), not just
    //  onto this frame.
    if (m_pOwner)
        SetScaleRecursive(*(const float*)((const char*)m_pOwner + 0x114));
}

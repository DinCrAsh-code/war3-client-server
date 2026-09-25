//============================================================================
//  0x6F609A20 - CSimpleFrame's own slot 5 (+0x14) override.  Not
//  CLayoutFrame::SetScaleRecursive (0x6F607930, layoutscaletree.cpp, a
//  different address that walks a CFrame's own children list) - this one
//  re-checks the same epsilon inline, calls CLayoutFrame::SetScale
//  directly, and then pushes the same scale into two of CSimpleFrame's own
//  lists at +0xD8 and +0x120 through each entry's own slot 5
//  (LayoutSetScaleFn).  Neither list is a typed member yet - +0xD8 falls
//  inside m_regions (CRegionNodeList) and +0x120 inside m_children
//  (TSList<SIMPLEFRAMENODE, 0>), and nothing else in this batch's closure
//  needed either list's own internal layout pinned down - so both walks
//  read the shipped node shape (next @ +4, frame @ +8) through raw byte
//  offsets rather than a named field.
//============================================================================
#include "frame.h"
#include "framethunks.h"

extern "C" double __cdecl fabs(double x);
#pragma intrinsic(fabs)

namespace {

//  Every node in either list has the same shape this file's other list
//  walks use: next link at +4, owning frame pointer at +8 (matches
//  FRAMEPRIORITY and the SIMPLEFRAMENODE shape alike).
void PropagateOneList(void* listHead, float scale)
{
    typedef void (__thiscall *Fn)(void*, float);

    for (char* n = (char*)listHead; n; n = *(char**)(n + 4))
    {
        void* child = *(void**)(n + 8);
        ((Fn)(*(void***)child)[5])(child, scale);
    }
}

}  // namespace

void CSimpleFrame::PropagateScaleToChildren(float scale)
{
    float delta = scale - m_scale;
    float mag   = (float)fabs(delta);

    if (mag < g_frameScaleEpsilon)
        return;

    SetScale(scale);

    PropagateOneList(*(void**)((char*)this + 0xD8), scale);
    PropagateOneList(*(void**)((char*)this + 0x120), scale);
}

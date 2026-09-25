//============================================================================
//  0x6F607930 - CLayoutFrame::SetScaleRecursive: take a new scale and push
//  it down the owning frame's whole subtree.
//
//  `this` is a CFrame's *second* base subobject, so the children the walk
//  needs are the CLayer half's, 0xB4 bytes below it - which is exactly what
//  the shipped code's `mov eax, [esi-98h]` reads (0xB4 - 0x98 = 0x1C, the
//  children list's own terminator).
//============================================================================
#include "frame.h"
#include "framethunks.h"

extern "C" double __cdecl fabs(double x);
#pragma intrinsic(fabs)

void CLayoutFrame::SetScaleRecursive(float scale)
{
    float delta = scale - m_scale;
    float mag   = (float)fabs(delta);

    if (!(mag < g_frameScaleEpsilon))
    {
        SetScale(scale);

        CFrame* frame = (CFrame*)((char*)this - 0xB4);
        int link = (int)frame->m_children.Tail();

        while (link > 0)
        {
            LAYERNODE* node = (LAYERNODE*)link;
            CLayoutFrame* child =
                (CLayoutFrame*)((char*)node->m_pLayer + 0xB4);

            ((LayoutSetScaleFn)(*(void***)child)[0x14 / 4])(child, scale);

            link = (int)frame->m_children.Prev(node);
        }
    }
}

//============================================================================
//  0x6F605D40 - CLayoutFrame::SetScale.
//
//  Two named float locals, because the shipped stream stores and reloads
//  twice - once after the subtraction and once after the `fabs`
//  (docs/msvc-vc8-idioms.md, "Named locals for every intermediate, when the
//  shipped code stores and reloads").  `fabs` only becomes the bare x87
//  instruction on a C-linkage declaration; a C++ one warns C4162 and emits a
//  call.
//============================================================================
#include "frame.h"
#include "framethunks.h"

extern "C" double __cdecl fabs(double x);
#pragma intrinsic(fabs)

void CLayoutFrame::SetScale(float scale)
{
    float delta = scale - m_scale;
    float mag   = (float)fabs(delta);

    if (!(mag < g_frameScaleEpsilon))
    {
        m_scale   = scale;
        m_54 = 0;
        QueueForLayout(0);
    }
}

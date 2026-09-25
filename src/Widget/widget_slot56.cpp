//============================================================================
//  0x6F267F00 - CWidget's vtable slot 56 (+0xE0).  See widget.h.
//
//  Own translation unit so the call into GetWorldPositionWithGround
//  (widget_worldpos.cpp) survives, which is what the shipped code has.
//
//  The three-float copy out of the helper's answer is done through the FPU
//  (`fld`/`fstp` per component) rather than with integer moves, unlike the
//  copy inside the helper itself - so this level really is treating the
//  result as hardware floats, and FVec3 is the right type for it.
//============================================================================
#include "widget.h"
#include "fvec3.h"

FVec3* CWidget::GetPosition(FVec3* out, int wantGround, int a3, int a4)
{
    FVec3 scratch;
    FVec3* v = GetWorldPositionWithGround(&scratch, wantGround, a3, a4);
    out->m_x = v->m_x;
    out->m_y = v->m_y;
    out->m_z = v->m_z;
    return out;
}

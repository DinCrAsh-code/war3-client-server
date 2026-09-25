//============================================================================
//  0x6F278190 - CWidget::GetOffsetPosition.
//
//  Slot 56's answer with the module's fixed world offset folded in:
//  (-16, -16, 0), read out of 0x6FA70EB8 with `ida_query bytes`.  The offset
//  is applied only when m_flags carries 0x1000, and the result is the
//  caller's own buffer either way - the shipped code has `mov eax, esi`
//  before the test, so `out` is the return value on both paths.
//
//  The shipped stream loads the *value* first for x and y and the
//  *constant* first for z.  Both spellings of each were tried and this
//  compiler emits the same thing either way, so the two-instruction gap in
//  y and z is the x87 operand-order normalisation
//  docs/msvc-vc8-idioms.md records as not source-addressable, not a
//  spelling still to be found.
//
//  Own translation unit: the slot dispatch through `this` is a real
//  indirect call, and 0x6F333FB0 calls this one for real.
//============================================================================
#include "widget.h"
#include "fvec3.h"

//: flt_6FA70EB8 / flt_6FA70EBC / flt_6FA70EC0 - the module's world offset,
//: (-16, -16, 0), read with `ida_query bytes`.  Three separate `extern`s and
//: three funcmap.DATA rows rather than three file-scope constants: the x and
//: y values are identical, and MSVC folds two equal float literals into one
//: constant and then keeps it on the x87 stack across both components -
//: which is a different instruction stream from the shipped two loads.  Same
//: reasoning as docs/msvc-vc8-idioms.md's "Three separate byte arrays for one
//: three-byte-stride table".
extern const float g_widgetWorldOffsetX;
extern const float g_widgetWorldOffsetY;
extern const float g_widgetWorldOffsetZ;

FVec3* CWidget::GetOffsetPosition(FVec3* out, int wantGround, int a3, int a4)
{
    GetPosition(out, wantGround, a3, a4);

    if ((m_flags & 0x1000) != 0)
    {
        out->m_x = out->m_x + g_widgetWorldOffsetX;
        out->m_y = g_widgetWorldOffsetY + out->m_y;
        out->m_z = out->m_z + g_widgetWorldOffsetZ;
    }

    return out;
}

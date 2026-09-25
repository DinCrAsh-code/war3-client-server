//============================================================================
//  0x6F00C9F0 - CWidget::GetWorldPositionWithGround.  See widget.h.
//
//  Own translation unit: its one named caller (CWidget::GetPosition,
//  vtable slot 56, widget_slot56.cpp) reaches it with a real `call`, and it
//  itself calls CPathRef::SmartPtrToWorldVec3 (CPathRef.cpp) for real.
//
//  The handle-bearing sub-object comes back from vtable slot 46 (+0xB8) and
//  is treated as a CPathRef - which is what it is: SmartPtrToWorldVec3
//  reads exactly the {handle, typeTag} pair at +0x08/+0x0C that
//  SHandleWithType and CPathRef both describe (game.h, timesyncbounds.h).
//============================================================================
#include "widget.h"
#include "fvec3.h"

//  slot 46 (+0xB8) - the handle-bearing sub-object.  Reached through the
//  vtable and not as CWidget::GetHandleObject(): every derived class in
//  this family overrides it, and a plain CWidget's own answer is null.
typedef CPathRef* (__thiscall *GetHandleObjectFn)(void* self);
//  slot 57 (+0xE4) - the ground-height query (CWidget::Method_0xE4, item.cpp).
typedef float (__thiscall *GroundHeightFn)(void* self, FVec3* probe, int a3,
                                           int a4, int zero);

FVec3* CWidget::GetWorldPositionWithGround(FVec3* out, int wantGround,
                                            int a3, int a4)
{
    //  One expression, so the return buffer for SmartPtrToWorldVec3 is
    //  pushed before the vtable[0xB8] call rather than after it - the same
    //  ordering point CWidget::Method_0x15C makes
    //  (widget_vtable_leaves.cpp), and the shape jasssetitemposition.cpp
    //  already documents at its own copy of this call pair.
    CWorldVec3 scratch =
        ((GetHandleObjectFn)(*(void***)this)[0xB8 / 4])(this)
            ->SmartPtrToWorldVec3();

    //  A whole-object copy, three integer `mov`s: CWorldVec3 is three
    //  CFloats and the shipped code moves the bits, it does not go through
    //  the FPU here.  It is only the *next* block that reinterprets the
    //  same bytes as hardware floats, which is sound because CFloat's
    //  storage already is an IEEE single (widget.h).
    *(CWorldVec3*)out = scratch;

    if (wantGround != 0)
    {
        //  The *same* twelve bytes again, now as three hardware floats:
        //  the shipped frame is 0xC bytes total, so the return buffer and
        //  the probe share one slot.  A second, separately-declared local
        //  makes MSVC allocate a second one and the frame comes out twice
        //  the size.
        FVec3* probe = (FVec3*)&scratch;
        probe->m_x = out->m_x;
        probe->m_y = out->m_y;
        probe->m_z = 0.0f;
        out->m_z = ((GroundHeightFn)(*(void***)this)[0xE4 / 4])(
            this, probe, a3, a4, 0);
    }

    return out;
}

//============================================================================
//  0x6F2AD400 - CWidget vtable[0x150] (index 84): put this widget's
//  footprint onto the pathing grid.
//
//  The smallest of the three footprint slots and the clearest statement of
//  the shape all three share:
//
//      shape    = my footprint shape for my type
//      position = where my handle sub-object says I am
//      facing   = what it says I am facing, as one of four quadrants
//      snap the position to the shape's own cell grid for that quadrant
//      visit every covered cell, marking it through the +0x34 grid
//      and, once the grid is up, invalidate the footprint's bounding box
//
//  Own translation unit: every call it makes is real.
//
//  The `+0x34` field it hands to CFootprintShape::VisitCells as the visit context
//  is the same one AdjustWidgetPathRefs (widgetpathref.cpp) walks as an
//  array of pinned path data, and the same one the other two footprint
//  slots gate on - the widget's own pathing-grid registration.
//============================================================================
#include "widget.h"
#include "widgetfootprint.h"
#include "quadrant.h"
#include "game.h"

typedef CPathRef* (__thiscall *GetHandleObjectFn)(void*);
typedef void (__thiscall *GetFacingFn)(void*, CFloat*);

//  0x6F2AB0F0 - the per-cell callback.  Its address is taken and never
//  called from here; below this dump's own depth cut.
void __fastcall FootprintMarkCell();

//  A member, not the `__fastcall F(CWidget*)` free function a previous
//  session wrote: it is vtable slot 84, and a vtable entry has to name a
//  member or the emitted vtable references a symbol nothing defines.  The
//  two spellings compile to the same code - `this` in ecx either way - so
//  only the mangled name changes.
void CWidget::ApplyWidgetFootprint()
{
    CWidget* self = this;
    void* vself = self;

    if (self->m_pathGridRegistration == 0)
        return;

    CFootprintShape* shape = self->GetOwnFootprintShape(1);
    if (shape == 0)
        return;

    CWorldVec3 world =
        ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself)
            ->SmartPtrToWorldVec3();

    //  x and y copied into their own pair of slots: everything downstream
    //  takes the position as a two-CFloat block, not as the three-component
    //  vector SmartPtrToWorldVec3 hands back.
    SFootprintPos pos;
    pos.m_x = world.m_x;
    pos.m_y = world.m_y;

    //  The handle sub-object is asked twice - once for the position and
    //  again for the facing - rather than kept in a register across the
    //  first call.  That is the shipped code, not an oversight here.
    CPathRef* ref = ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself);
    //  Uninitialised - the slot-0x1C call writes it before anything reads
    //  it, and the zeroing default constructor's `mov [esp+12], 0` is one
    //  instruction the shipped code does not have.  See CFloat.h's
    //  kCFloatNoInit.
    CFloat facing(kCFloatNoInit);
    ((GetFacingFn)(*(void***)ref)[0x1C / 4])(ref, &facing);

    int direction = ClassifyQuadrant(facing);

    shape->ApplyDirection(&pos.m_x, &pos.m_y, direction);
    shape->VisitCells(&pos, direction, (void*)&FootprintMarkCell,
                        self->m_pathGridRegistration);

    if (g_pathGridReady != 0)
    {
        shape->CellBox(&world, &pos, direction);
        PathGridInvalidateBox(&world);
    }
}

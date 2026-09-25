//============================================================================
//  0x6F2AE690 - CWidget vtable[0x154] (index 85): re-apply this widget's
//  footprint and tell the pathing grid about the region it now occupies.
//
//  The same opening as ApplyWidgetFootprint (0x6F2AD400) - shape, position,
//  facing, quadrant, snap, visit - with two differences and a long tail:
//
//  * the shape comes from GetFootprintShape directly, not through
//    CWidget::GetOwnFootprintShape, so this slot does not consult
//    vtable[0xB0] and does not have an "alternate" form;
//  * the per-cell callback is a different one (0x6F2ADE80's neighbour
//    0x6F2ABEE0 rather than 0x6F2AB0F0);
//  * and after visiting, it builds a 0x30-byte region descriptor on the
//    stack out of the footprint's own cell box, the terrain type at the
//    snapped cell, the position and `this`, and hands that to the grid.
//
//  Own translation unit: every call it makes is real.
//
//  The region descriptor is zero-initialised and then filled: the shipped
//  code stores a known-zero register into six of its fields and immediately
//  overwrites four of them, which is what `= {0}` plus field assignments
//  produces and a plain uninitialised local does not.  The cell box is
//  copied into a second set of fields inside that same descriptor rather
//  than being read from where CFootprintShape::CellBox left it.
//============================================================================
#include "widget.h"
#include "widgetfootprint.h"
#include "boxmath.h"        // SCellBox - the zeroing cell-box constructor
#include "quadrant.h"
#include "rowrangetable.h"
#include "game.h"

typedef CPathRef* (__thiscall *GetHandleObjectFn)(void*);
typedef void (__thiscall *GetFacingFn)(void*, CFloat*);

//  0x6F2ABEE0 / 0x6F2ADE80 - the per-cell and per-region callbacks.  Both
//  address-taken only; below this dump's own depth cut.
void __fastcall FootprintMarkCellRegion();
void __fastcall FootprintRegionVisit();

//  0x6F4693D0 - hand the finished region descriptor to the pathing grid.
//  `this` is the descriptor, edx a fixed selector, four stack arguments.
void __fastcall PathGridSubmitRegion(void* region, unsigned int selector,
                                     int a, int b, void* visit, int* terrain);

//  The descriptor.  Only what this function writes is named.
//
//  Which fields have a *type with a constructor* is read straight off the
//  shipped stream: it zeroes exactly six dwords before filling the record -
//  the four of the copied cell box (+0x14..+0x20) and the two CFloats at
//  +0x28/+0x2C - and leaves +0x00..+0x0F, +0x10 and +0x24 alone.  That is
//  `SCellBox`'s zeroing default constructor (boxmath.h) plus `CFloat`'s,
//  running on an ordinary uninitialised local; it is not `= {0}`, which
//  would zero all twelve, and it is not a plain `int[4]`/`int`/`void*`
//  triple, which would zero none.  The box CellBox fills stays a raw
//  `int[4]` for the same reason - the shipped code never zeroes it.
struct SFootprintRegion
{
    int      m_box[4];       // +0x00  CellBox fills this
    int      m_terrain;      // +0x10
    SCellBox m_boxCopy;      // +0x14  zeroed by its own constructor
    void*    m_pWidget;      // +0x24
    CFloat   m_x;            // +0x28  zeroed by CFloat's
    CFloat   m_y;            // +0x2C  zeroed by CFloat's
};

//  A member, not the `__fastcall F(CWidget*)` free function a previous
//  session wrote: it is vtable slot 85, and a vtable entry has to name a
//  member or the emitted vtable references a symbol nothing defines.  The
//  two spellings compile to the same code - `this` in ecx either way - so
//  only the mangled name changes.
void CWidget::UpdateWidgetFootprintRegion()
{
    CWidget* self = this;
    void* vself = self;

    if (self->m_pathGridRegistration == 0)
        return;

    CFootprintShape* shape = GetFootprintShape(self->m_footprintType);
    if (shape == 0)
        return;

    CWorldVec3 world =
        ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself)
            ->SmartPtrToWorldVec3();

    SFootprintPos pos;
    pos.m_x = world.m_x;
    pos.m_y = world.m_y;

    CPathRef* ref = ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself);
    //  Uninitialised: the slot-0x1C call writes it before anything reads
    //  it, and the zeroing default constructor is one store the shipped
    //  code does not have.  See CFloat.h's kCFloatNoInit.
    CFloat facing(kCFloatNoInit);
    ((GetFacingFn)(*(void***)ref)[0x1C / 4])(ref, &facing);

    int direction = ClassifyQuadrant(facing);

    shape->ApplyDirection(&pos.m_x, &pos.m_y, direction);
    shape->VisitCells(&pos, direction,
                        (void*)&FootprintMarkCellRegion,
                        self->m_pathGridRegistration);

    //  Plain declaration - the constructors named above are the whole of
    //  the zeroing the shipped code does.
    SFootprintRegion region;
    shape->CellBox(&region, &pos, direction);

    //  Each subtraction into a named local before it is converted: written
    //  as one expression MSVC reads the difference back *through*
    //  operator-'s returned pointer (`mov eax, [eax]`), where the shipped
    //  code reads the temporary's own slot (`mov eax, [esp+10h]`).
    CFloat scratch = pos.m_x - g_pGameData->m_originX;
    int gx = CFloatToInt(WorldToGrid(scratch));
    scratch = pos.m_y - g_pGameData->m_originY;
    int gy = CFloatToInt(WorldToGrid(scratch));
    region.m_terrain = RowMaxWithTerrainType(gx, gy, 0);

    region.m_boxCopy.m_y0 = region.m_box[0];
    region.m_boxCopy.m_x0 = region.m_box[1];
    region.m_boxCopy.m_y1 = region.m_box[2];
    region.m_boxCopy.m_x1 = region.m_box[3];
    region.m_pWidget = self;
    region.m_x = pos.m_x;
    region.m_y = pos.m_y;

    PathGridSubmitRegion(&region, 0x4FD0000, 0, 0,
                         (void*)&FootprintRegionVisit, &region.m_terrain);

    if (g_pathGridReady != 0)
        PathGridInvalidateBox(&region);
}

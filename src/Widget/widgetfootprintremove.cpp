//============================================================================
//  0x6F2AD300 - CWidget vtable[0x14C] (index 83): take this widget's
//  footprint back off the pathing grid and free its registration.
//
//  The exact counterpart of slot 0x148 (0x6F2AD0C0), which creates that
//  registration - the two allocate and free the same block, and 0x148's own
//  Storm allocation tag is what identifies this module as `.\CWidget.cpp`.
//  Together with 0x150 (apply) and 0x154 (re-apply and submit the region),
//  the four make one family.
//
//  Order matters and is the shipped order: clear the three path flags on
//  the handle sub-object *first*, then tear down and free the registration
//  and null the field, and only then recompute the footprint's cell box -
//  which is why the box work happens with `m_pathGridRegistration` already
//  zero and does not visit any cells.  Removal only has to invalidate what
//  the footprint used to cover, not re-mark it.
//
//  Own translation unit: every call it makes is real.
//============================================================================
#include "widget.h"
#include "widgetfootprint.h"
#include "widgetpathrefadjust.h"
#include "quadrant.h"
#include "storm.h"
#include "game.h"

typedef SPathRefHandle* (__thiscall *GetHandleObjectFn)(void*);
typedef void (__thiscall *GetFacingFn)(void*, CFloat*);

//  0x6F47C100 - below this dump's depth cut; thunked.
void __fastcall ReleasePathRegistration(void* registration);

void CWidget::RemoveFootprint(int blocking, int useAlternate)
{
    void* vself = this;

    SPathRefHandle* ref =
        ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself);
    ref->SetPathFlagBlocking(blocking);
    ref->SetPathFlagWalkable(1);
    ref->SetPathFlagVisible(1);

    void* registration = m_pathGridRegistration;
    if (registration != 0)
    {
        ReleasePathRegistration(registration);
        SMemFree(registration, "delete", -1, 0);
    }
    m_pathGridRegistration = 0;

    CFootprintShape* shape = GetOwnFootprintShape(useAlternate);
    if (shape == 0)
        return;

    //  Same object as CPathRef (game.h); cast rather than duplicating the
    //  accessor - see widgetpathrefadjust.h for why the layout is spelled
    //  twice in this repo.
    CWorldVec3 world = ((CPathRef*)
        ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself))
            ->SmartPtrToWorldVec3();

    SFootprintPos pos;
    pos.m_x = world.m_x;
    pos.m_y = world.m_y;

    SPathRefHandle* r2 =
        ((GetHandleObjectFn)(*(void***)vself)[0xB8 / 4])(vself);
    //  Uninitialised: slot 0x1C writes it before anything reads it, and
    //  the zeroing default constructor's `mov [esp+8], 0` is one
    //  instruction the shipped code does not have.  See CFloat.h's
    //  kCFloatNoInit.
    CFloat facing(kCFloatNoInit);
    ((GetFacingFn)(*(void***)r2)[0x1C / 4])(r2, &facing);

    int direction = ClassifyQuadrant(facing);
    shape->ApplyDirection(&pos.m_x, &pos.m_y, direction);

    if (g_pathGridReady != 0)
    {
        shape->CellBox(&world, &pos, direction);
        PathGridInvalidateBox(&world);
    }
}

//============================================================================
//  0x6F2AC220 / 0x6F2ABFB0 - see widget.h.
//============================================================================
#include "widget.h"
#include "storm.h"   // placement new
#include "quadrant.h"
#include "timesyncbounds.h"
#include "gamebounds.h"
#include "pathfind.h"
#include "footprinttype.h"
#include "rowrangetable.h"

//  vtable[0xB8] - hand back the handle-bearing sub-object.
typedef SHandleWithType* (__thiscall *GetHandleObjectFn)(void*);
//  vtable[0xDC] - forward a full move on to the concrete widget type.  Same
//  eleven-argument shape as MoveWidget itself.
typedef void (__thiscall *ForwardMoveFn)(void*, CFloat*, CFloat*, int, int,
                                         int, int, int, int, int, int, int);
//  The sub-object's own vtable[0x1C] - write its facing into *out.
typedef void (__thiscall *GetFacingFn)(void*, CFloat*);

void CWidget::MoveTo(CFloat* posX, CFloat* posY, int forward, int a4, int a5,
                     int a6, int a7, int noDelta, const CFloat* facing,
                     int a10, int a11)
{
    void* self = this;
    CGridVec2 pos;
    pos.m_x = *posX;
    pos.m_y = *posY;
    ClampToGameBounds(&pos);

    if (forward != 0)
    {
        ForwardMoveFn fn = (ForwardMoveFn)(*(void***)self)[0xDC / 4];
        fn(self, &pos.m_x, &pos.m_y, a4, a5, 1, a6, a7, 0, 0, a10, a11);
    }

    CFootprintShape* shape =
        GetFootprintShape(m_footprintType);
    if (shape != 0)
    {
        //  With no facing supplied, ask the handle-bearing sub-object for its
        //  own.  The shipped code writes that answer into MoveWidget's first
        //  argument slot, which is dead by then - a temporary VC8 coalesced,
        //  not something the source names.
        //  Raw bits rather than a CFloat: CFloat's default constructor
        //  zeroes, and the shipped code leaves this scratch uninitialised
        //  (it is written by the call before it is read).
        int ownFacing;
        const CFloat* which = facing;
        if (facing == 0)
        {
            SHandleWithType* sub =
                ((GetHandleObjectFn)(*(void***)self)[0xB8 / 4])(self);
            ((GetFacingFn)(*(void***)sub)[0x1C / 4])(sub, (CFloat*)&ownFacing);
            which = (const CFloat*)&ownFacing;
        }

        shape->ApplyDirection(&pos.m_x, &pos.m_y, ClassifyQuadrant(*which));
    }

    //  The move is reported in world space, with a zero height.
    CWorldVec3 world;
    world.m_x = pos.m_x;
    world.m_y = pos.m_y;
    world.m_z = g_CFloatZero;

    ((GetHandleObjectFn)(*(void***)self)[0xB8 / 4])(self)
        ->FlushedOriginDelta(&world, noDelta == 0);

    *posX = pos.m_x;
    *posY = pos.m_y;
}

//============================================================================
//  0x6F2ABFB0 - CWidget::MoveForward.  See widget.h for the shape and its
//  relationship to MoveTo() above.
//============================================================================

//  Outside this call tree; mapped so the `offset` operand that installs it
//  as the default filter resolves on both sides (see PathPlacementFilter's
//  own thunk, src/pathplacementfilter_thunk.cpp, for the same shape).
__declspec(naked) int __fastcall DefaultMoveFilter(const CFloat*, void*)
{
    __asm
    {
        mov eax, 06F2AB130h
        jmp eax
    }
}

//  vtable[0xAC] - default "centre on whole cells" flag when the caller does
//  not name one explicitly.
typedef int (__thiscall *GetDefaultCellCentresFn)(void*);
//  vtable[0xD0] - bracket a move: 1 on entry (only when `lockFlag` is set),
//  0 on the way out (unconditionally paired with the entry call).  Return
//  value, if any, is never read at either call site.
typedef void (__thiscall *SetMoveLockFn)(void*, int);
//  vtable[0x158] - this widget's own placement mask, no explicit arguments.
typedef int (__thiscall *GetPlacementMaskFn)(void*);
//  vtable[0x168] - this widget's own placement radius, returned through a
//  caller-supplied CFloat* out-buffer (the class-return-by-pointer ABI
//  CFloat's own operator+/- use) and handed back again in eax.
typedef CFloat* (__thiscall *GetPlacementRadiusFn)(void*, CFloat*);

int CWidget::MoveForward(CFloat* posX, CFloat* posY, int useBoundedSearch,
                         int terrainType, int lockFlag, PathFilterFn filter,
                         void* context, const SCellBoxF* bounds,
                         int cellCentresOverride, int unitType, int player)
{
    void* self = this;
    CGridVec2 pos;
    pos.m_x = *posX;
    pos.m_y = *posY;
    ClampToGameBounds(&pos);

    if (lockFlag != 0)
        ((SetMoveLockFn)(*(void***)self)[0xD0 / 4])(self, 1);

    //  -1 is "look it up": the terrain-type nibble or CRowRangeTable max at
    //  the (now-clamped) grid cell, whichever is larger.  Anything else is
    //  taken as given.  A separate local, not the parameter reassigned in
    //  place - the shipped code leaves arg_C's own stack slot untouched and
    //  writes the answer to a fresh one.
    //  Frame differs from the target by one dword here (the terrainOut
    //  local's own eager store ahead of the -1 test rather than only at
    //  the branch's join point) - a compiler scheduling choice, not a
    //  source shape this session found a spelling around; see
    //  docs/targets/CWidget__MoveForward.md.
    int terrainOut = terrainType;
    if (terrainOut == -1)
    {
        int gx = CFloatToInt(WorldToGrid(pos.m_x - g_pGameData->m_originX));
        int gy = CFloatToInt(WorldToGrid(pos.m_y - g_pGameData->m_originY));
        terrainOut = RowMaxWithTerrainType(gx, gy, 0);
    }

    //  A degenerate box at the point itself when the caller supplies no
    //  rectangle, otherwise the caller's own - copied into a local either
    //  way, since the ring-search branch below needs its own addressable
    //  copy regardless of which source it came from.
    SCellBoxF pointBox(pos.m_y, pos.m_x, pos.m_y, pos.m_x);
    const SCellBoxF* srcBox = bounds ? bounds : &pointBox;
    SCellBoxF box = *srcBox;

    //  No separate scratch local: by the time the radius out-buffer and,
    //  later, the sub-object's own facing need a throwaway CFloat-sized
    //  slot, terrainOut is dead (already read into searchContext's
    //  fallback, or never taken at all in the caller-supplied-context
    //  case) - reusing it here rather than declaring a fresh local matches
    //  the shipped frame's local count (11 dwords, 44 bytes) instead of
    //  adding a twelfth.  The shipped code actually reuses two *different*
    //  dead parameter slots for these two purposes (arg_18's own stack
    //  slot for the facing write, a separate one for the radius
    //  out-buffer) - stack-slot-for-stack-slot fidelity is not
    //  source-addressable (CLAUDE.md), only getting the frame size right
    //  is.
    int success;
    if (useBoundedSearch != 0)
    {
        int cellCentres = cellCentresOverride
            ? (cellCentresOverride == 1)
            : ((GetDefaultCellCentresFn)(*(void***)self)[0xAC / 4])(self);
        //  Discarded on purpose - the shipped code calls it and throws the
        //  result away, presumably for a lazy-init side effect the sub-
        //  object's own accessor has.
        ((GetHandleObjectFn)(*(void***)self)[0xB8 / 4])(self);

        void* searchContext = context ? context : &terrainOut;
        PathFilterFn searchFilter = filter ? filter : DefaultMoveFilter;
        //  mask and radius are read directly off the vtable calls as call
        //  arguments, not stored to named locals first - MSVC evaluates a
        //  call's arguments right to left, so this is what schedules the
        //  two virtual calls between the already-pushed context/filter/
        //  cellCentres/startSide and the still-to-push py, exactly as the
        //  shipped code has it.
        success = FindFreeWorldPoint(
            &box, &pos.m_x, &pos.m_y,
            ((GetPlacementRadiusFn)(*(void***)self)[0x168 / 4])(
                self, (CFloat*)&terrainOut),
            ((GetPlacementMaskFn)(*(void***)self)[0x158 / 4])(self),
            2, cellCentres, searchFilter, searchContext, unitType, player, 0,
            -1);
    }
    else
    {
        //  Discarded on purpose - same dead-getter idiom as the bounded
        //  branch above.
        ((GetHandleObjectFn)(*(void***)self)[0xB8 / 4])(self);

        void* searchContext = context ? context : &terrainOut;
        PathFilterFn searchFilter = filter ? filter : DefaultMoveFilter;
        success = FindFreeWorldPointNearby(
            &pos.m_x, &pos.m_y, &g_CFloatZero,
            ((GetPlacementMaskFn)(*(void***)self)[0x158 / 4])(self),
            searchFilter, searchContext);
    }

    CFootprintShape* shape = GetFootprintShape(m_footprintType);
    if (shape != 0 && success != 0)
    {
        SHandleWithType* sub =
            ((GetHandleObjectFn)(*(void***)self)[0xB8 / 4])(self);
        ((GetFacingFn)(*(void***)sub)[0x1C / 4])(sub, (CFloat*)&terrainOut);
        shape->ApplyDirection(&pos.m_x, &pos.m_y, ClassifyQuadrant(*(CFloat*)&terrainOut));
    }

    if (lockFlag != 0)
        ((SetMoveLockFn)(*(void***)self)[0xD0 / 4])(self, 0);

    *posX = pos.m_x;
    *posY = pos.m_y;
    return success;
}

//----------------------------------------------------------------------------
//  0x6F2ACAE0 (root) - see widget.h and dataallocator.h.
//----------------------------------------------------------------------------
CWidget* CDataAllocator::ConstructCWidget(int zeroMemory)
{
    void* raw = Alloc(zeroMemory, ".?AVCWidget@@", -2);
    CWidget* w = 0;
    if (raw != 0)
        w = new (raw) CWidget();
    return w;
}

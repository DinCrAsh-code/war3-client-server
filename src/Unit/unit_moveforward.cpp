//============================================================================
//  0x6F2A5FB0 - CUnit's vtable slot 55 (+0xDC), overriding CWidget::MoveForward
//  (widget.cpp, 0x6F2ABFB0).  Confirmed as the same slot by the vtable index
//  (both are slot 55 / +0xDC) and by the eleven-argument shape matching
//  exactly, retn-for-retn (0x2C both sides).
//
//  Unlike CWidget's own body, this override does **not** clamp posX/posY to
//  the game bounds first - it reads them straight off the caller's pointers.
//  What it adds on top of a plain forward is:
//
//   * a visibility check (QueryVisibleImpl(1), already reconstructed -
//     unit_visibilitymask.cpp) that gates everything below;
//   * three globals it fills in only when the unit is visible - its own
//     footprint type, its owning player index (through the already-
//     reconstructed GetOwningPlayerIndex virtual, slot 59) and the terrain
//     type the search will use (either the caller's own, or - on the
//     caller's -1 "look it up" sentinel - the same
//     WorldToGrid/CFloatToInt/RowMaxWithTerrainType lookup CWidget::MoveForward
//     inlines, run here on the *unclamped* posX/posY rather than a clamped
//     local copy);
//   * a pair of unit-specific defaults for `filter`/`context`, used only
//     when the caller passed neither: `&g_unitMoveFilterFootprint` and
//     `DefaultUnitMoveFilter` (0x6F2A5C90 - CWidget's own default filter is
//     a different function, 0x6F2AB130) when the unit is visible, or a
//     plain null for both when it is not.  Each default is computed
//     *inline*, only inside the branch where the caller left the slot
//     empty - not hoisted into an eager local ahead of the null tests,
//     which would compute both `neg`/`sbb`/`and` triples unconditionally.
//
//  The whole thing then tail-hands off to the base CWidget::MoveForward with
//  those two defaults resolved and every other argument passed straight
//  through - which is why this file needs no search logic of its own.
//
//  0x6F2A5C90 has no dump in this session's asm/ (an unrelated call tree
//  would have to be pulled to reach it) - out of scope, same as
//  widget.cpp's own DefaultMoveFilter thunk for 0x6F2AB130, and redirected
//  the same way.
//============================================================================
#include "unit.h"
#include "game.h"
#include "rowrangetable.h"

//  0x6FAB425C/0x6FAB4260/0x6FAB4264 - three scratch globals this function
//  fills in only when the unit is visible, then hands the first's address
//  on as the default search context.  Nothing in this call tree reads
//  0x6FAB4260/0x6FAB4264 back (only 0x6FAB425C's *address* is passed on,
//  as an opaque context pointer for 0x6F2A5C90 - which has no dump - to
//  interpret), so their names describe what fills them rather than a
//  confirmed purpose.
//  funcmap.DATA globals - declared extern, not defined, so build_mix.py
//  binds them to the real game addresses instead of giving the .mix a
//  private zero-filled copy (see CLAUDE.md's link_check.py DATA BINDING
//  note; this is the exact CObserver::Dispatch crash class).
extern unsigned int g_unitMoveFilterFootprint;  // 0x6FAB425C
extern unsigned int g_unitMoveFilterOwner;      // 0x6FAB4260
extern int          g_unitMoveFilterTerrain;    // 0x6FAB4264

//  0x6F2A5C90 - CUnit's own default move filter, the counterpart to
//  widget.cpp's DefaultMoveFilter (0x6F2AB130).  Out of this session's
//  scope (no dump); redirected.
__declspec(naked) int __fastcall DefaultUnitMoveFilter(const CFloat*, void*)
{
    __asm
    {
        mov eax, 06F2A5C90h
        jmp eax
    }
}

int CUnit::MoveForward(CFloat* posX, CFloat* posY, int useBoundedSearch,
                       int terrainType, int lockFlag, PathFilterFn filter,
                       void* context, const SCellBoxF* bounds,
                       int cellCentresOverride, int unitType, int player)
{
    int visible = QueryVisibleImpl(1);

    if (visible != 0)
    {
        g_unitMoveFilterFootprint = m_footprintType;
        g_unitMoveFilterOwner = GetOwningPlayerIndex();

        if (terrainType == -1)
        {
            int gx = CFloatToInt(WorldToGrid(*posX - g_pGameData->m_originX));
            int gy = CFloatToInt(WorldToGrid(*posY - g_pGameData->m_originY));
            g_unitMoveFilterTerrain = RowMaxWithTerrainType(gx, gy, 0);
        }
        else
        {
            g_unitMoveFilterTerrain = terrainType;
        }
    }

    return CWidget::MoveForward(
        posX, posY, useBoundedSearch, terrainType, lockFlag,
        filter ? filter : (visible ? DefaultUnitMoveFilter : (PathFilterFn)0),
        context ? context : (visible ? (void*)&g_unitMoveFilterFootprint : 0),
        bounds, cellCentresOverride, unitType, player);
}

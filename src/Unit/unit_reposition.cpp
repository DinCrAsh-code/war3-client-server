//============================================================================
//  0x6F2A5D50 - CUnit's own override of vtable slot 96 (+0x180),
//  CUnit::Reposition.  Reached from JASS_SetUnitPosition
//  (Jass/jassnatives_unitpositionxy.cpp, through CWidget's own public
//  Reposition entry point) and from CUnit::ClearPendingAndNotify
//  (Unit/unit_clearpendingnotify.cpp) on every ordinary movement tick.
//
//  This is the desync-investigation target: does an ordinary Reposition
//  call (even one that never crosses a spatial-grid cell boundary) touch
//  any state shared with the tempest presence/checksum table
//  (g_unk6FAB778C, Net/tempestproviderlisthash.cpp,
//  Pathfinding/gridregistrationteardown.cpp)?  See
//  docs/notes/reposition-desync-investigation.md for the full write-up -
//  the short answer this reconstruction's own call tree gives is **yes,
//  through its footprint teardown/rebuild pair** (RemoveFootprint/
//  AddFootprint below).
//
//  Shape, in shipped order:
//
//    1. `visible = QueryVisibleImpl(1)` - the same call CUnit::MoveForward
//       (0x6F2A5FB0, Unit/unit_moveforward.cpp, immediately below this
//       function in the image) makes first, for the same reason.
//    2. If `visible != 0` OR `m_int248` bit 9 (`0x200`) is set: tear the
//       footprint off (`RemoveFootprint(0, 1)`), dispatch the position-
//       notify state machine with `0`, then fill the same three "unit
//       move filter" scratch globals CUnit::MoveForward fills
//       (g_unitMoveFilterFootprint/Owner/Terrain, 0x6FAB425C/60/64) -
//       looking the terrain type up from the (unclamped) new position
//       when the caller passed `-1` for it, the exact WorldToGrid/
//       CFloatToInt/RowMaxWithTerrainType idiom CUnit::MoveForward and
//       CWidget::MoveForward both already have.
//    3. Both branches rejoin and forward the move through the base class,
//       `CWidget::MoveTo` (0x6F2AC220, a direct non-virtual call in the
//       shipped code - the same "call the base body without reloading the
//       vtable" shape CUnit::ClearPendingAndNotify's own
//       CWar3Image::ClearPendingAndNotify call already has), supplying
//       filter/context defaults for MoveTo's own trailing a6/a7 slots the
//       same "arg-or-default, only when visible" way CUnit::MoveForward
//       computes its own.  `a6`/`a7` matter only once MoveTo's own
//       `forward != 0` branch calls through vtable[0xDC] into (ultimately)
//       CWidget::MoveForward's search, which is what actually
//       re-registers this widget's pathing/collision grid boxes
//       (Pathfinding/pathtracegrid.cpp) through
//       CGridRegistration::UpdateBox (Pathfinding/spatialgrid.cpp) -
//       confirmed, per that file's own header, to touch only CCellGrid's
//       per-cell chains, never the tempest presence table.
//    4. RefreshAttachmentPlacement (vtable[0x1A0]) is called twice, with
//       RefreshPositionDeltaState/RefreshPositionNotifyState/
//       ReleaseAndRefreshFootprintOrientation between and after - none of
//       these three are reconstructed for real (STORM_THUNK,
//       Misc/citemclosure_batchL_thunks.cpp), and that file's own header
//       already establishes the whole family as visual-attachment/terrain-
//       splat bookkeeping, not spatial-grid or presence-table state.
//    5. `RefreshUnitTypeSplatIfPresent()` if `m_pSprite != 0` -
//       same family, same header, purely a terrain decal.
//    6. **The footprint rebuild.** If visible, mid-teardown
//       (`m_int248 & 0x200`), or `field1EC == 0` (see below): force-set
//       the widget's path flags again (`AddFootprint(1, 1, 1)`), dispatch
//       the notify state machine with `1`, refresh the uber-splat
//       attachment if visible, and - only if `m_pSprite != 0` and
//       `field1EC == 0` - republish this unit's own controller-slot
//       position (`GetPosition`/`MergeControllerSlotVec3`/
//       `ResetFloatTargetSlotForRecord`).  Paired with step 2's own
//       `RemoveFootprint(0, 1)`, this is a genuine teardown-then-rebuild
//       of `m_pathGridRegistration` on an ordinary Reposition call, not
//       only on one that crosses a grid cell - see
//       Widget/widgetfootprintremove.cpp (RemoveFootprint always frees and
//       nulls an existing registration through
//       CGridRegistration::TeardownRegistration, which
//       `STwoIndexedFreeLists::Push`-es this object's own presence slot
//       back into g_unk6FAB778C) and Widget/widget_addfootprint.cpp
//       (AddFootprint only re-builds - allocates fresh, re-registers
//       cells - when `m_pathGridRegistration == 0`, which step 2 just
//       made true).  The allocation side's own presence-table `AllocateAt`
//       call is still not found in *this* call tree (Construct is a
//       thunk, 0x6F47C350) - see the write-up doc for what would close
//       that gap.
//    7. If `m_atk1E8 != 0` (the ability-agent sub-object,
//       Unit/unit_findability.cpp's own name for this field):
//       `SPositionRepublishTarget::RepublishOwnerPosition`
//       (Widget/attachmentpositionrepublish.cpp) - out-of-band position
//       republishing for whatever is attached to this unit, unrelated to
//       the presence table.
//    8. Grant `GetFourCC_6F110BC0()`'s ability if this unit does not
//       already carry it (`FindAbility`), and if newly found, run a
//       follow-up this reconstruction does not chase further
//       (`sub_6F12D520`, its own SEH frame and four more callees - well
//       past this pass's depth-2/3 budget; thunked below, `this` = the
//       ability record just found, not this CUnit).
//
//  Indirect calls resolved as plain (non-virtual or virtual, matching the
//  shipped `call`) member calls rather than raw `(*(void***)this)[...]`:
//  `RemoveFootprint`/`AddFootprint` (CWidget's own slots 82/83, not
//  overridden by CUnit), `RefreshAttachmentPlacement`/`GetPosition`/
//  `GetOwningPlayerIndex` (CUnit's own).  `QueryVisibleImpl` and
//  `DispatchPositionNotifyState` are plain (non-virtual) members, matching
//  the shipped direct `call`.
//
//  Two calls (GetPosition/MergeControllerSlotVec3's own argument split
//  across the two adjacent call sites, and ResetFloatTargetSlotForRecord's
//  own real argument shape) are transcribed from a careful read of the
//  interleaved push sequence but not independently re-verified against a
//  live capture - see the inline comments at each call.  Neither changes
//  this file's own answer to the desync question, which turns on step 6.
//============================================================================
#include "unit.h"
#include "game.h"
#include "rowrangetable.h"
#include "widget.h"
#include "controllerslot.h"
#include "fvec3.h"

//  0x6FAB425C/0x6FAB4260/0x6FAB4264 - the same three "unit move filter"
//  scratch globals CUnit::MoveForward (Unit/unit_moveforward.cpp) already
//  declares and fills; Reposition fills them independently on its own
//  visible/teardown path before ever reaching MoveTo (which, if it takes
//  the `forward != 0` branch, ends up back in CUnit::MoveForward's own
//  vtable override, refilling them a second time - a real, shipped
//  redundancy, not a mistake in either reconstruction).
extern unsigned int g_unitMoveFilterFootprint;  // 0x6FAB425C
extern unsigned int g_unitMoveFilterOwner;      // 0x6FAB4260
extern int          g_unitMoveFilterTerrain;    // 0x6FAB4264

//  0x6F2A5C90 - CUnit's own default move filter (Unit/unit_moveforward.cpp).
//  No `extern "C"` here: that would give this declaration a different
//  (unmangled) linker symbol than the real, plain-C++ definition in that
//  file, which is exactly the "declaration disagrees with its definition
//  in another TU" defect link_check.py exists to catch.
int __fastcall DefaultUnitMoveFilter(const CFloat*, void*);

//  vtable[0xDC]'s tail-forward target for MoveTo, when `forward != 0`, is
//  CUnit::MoveForward's own filter/context slots (a6/a7) - see that file.
//  `PathFilterFn` itself already comes from pathfind.h (pulled in
//  transitively through widget.h), so it is used as-is rather than
//  re-typedef'd here.

//  0x6F283A50/0x6F2ADA60/0x6F3334C0/0x6F28E170 - already thunked,
//  Misc/citemclosure_batchL_thunks.cpp.  Declared there as free
//  `__stdcall(void)` functions reached with `this` in ecx (a thiscall with
//  zero stack arguments looks identical to a stdcall with zero arguments
//  at the call site, and that file's own header explains why plain
//  `__stdcall` was chosen over inventing a `this`-taking declaration
//  nothing else needed yet).
void __stdcall RefreshPositionNotifyState(void);
void __stdcall ReleaseAndRefreshFootprintOrientation(void);
void __stdcall RefreshUnitTypeSplatIfPresent(void);
void __stdcall RefreshUberSplatAttachment(void);

//  0x6F4E8360 - Misc/citemclosure_batchL_thunks.cpp's own
//  ResetFloatTargetSlotForRecord: declared there as a zero-argument
//  placeholder because nothing called it by name yet (its own tail is a
//  vtable-slot `jmp`, so its real cleanup count cannot be read off this
//  dump either).  This call site is this repo's first real caller;
//  reused as-is rather than re-declared, since a second declaration of
//  the same address under a different signature is exactly the
//  "two symbols, one address" defect that file's own
//  ApplyControllerSlotVec3/ResetFloatTargetSlotForRecord note warns
//  about (it would let canonlib's SYM resolution silently re-point some
//  *other*, already-matching call site at this file's new symbol
//  instead).  The real call here reads `this=[esi+28h]`, `edx=4`, one
//  stack argument (`&this->m_something294`) - not reproduced byte-exact
//  by the zero-arg declaration, and flagged DIFFERS for exactly that gap.
void __stdcall ResetFloatTargetSlotForRecord(void);

//  0x6F2781F0 - reached with a real `this` in ecx and two stack arguments
//  here (an address, then the literal `1`).  GameUI/cameracandidatescan.cpp
//  and GameUI/cameraupdate.cpp each already carry their own file-local
//  redeclaration of this same shipped address for their own call shape -
//  this file's own copy, named for what this call site passes it.
namespace
{
    struct SPositionDeltaRepublishTarget
    {
        void RefreshPositionDelta(void* outAddr, int mode);
    };
    __declspec(naked) void SPositionDeltaRepublishTarget::RefreshPositionDelta(void*, int)
    {
        __asm { mov eax, 06F2781F0h }
        __asm { jmp eax }
    }
}

//  0x6F0787D0 - SUnitAbilityHost::FindAbility, already reconstructed
//  (Unit/unit_findability.cpp); redeclared the same way every other call
//  site of it does.
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

//  0x6F110BC0 - already reconstructed, Misc/cunit_agent8_abilityfactories.cpp.
unsigned int GetFourCC_6F110BC0();

//  0x6F472890 / 0x6F0C6840 - SAttachmentOwnerQuery::QueryOwner /
//  SPositionRepublishTarget::RepublishOwnerPosition, both already
//  reconstructed (Widget/attachmentownerquery.cpp,
//  Widget/attachmentpositionrepublish.cpp); redeclared locally the same
//  way that file's own header explains every call site of it does.
struct SPositionRepublishTarget
{
    void RepublishOwnerPosition();
};

//  0x6F12D520 - reached only when FindAbility just found a *new* grant for
//  the ability GetFourCC_6F110BC0() names.  A real, non-trivial function
//  (SEH frame, five of its own callees - InitializeFromTemplateRecord/
//  sub_6F12C490, already a THUNK in tools/funcmap.py for the identical
//  "CUnit::Reposition's own callee tree" reason; SAttachmentOwnerQuery::
//  QueryOwner and CPathRef::SmartPtrToWorldVec3, both already real;
//  PublishFacingVectorState, already thunked) - past this pass's own
//  depth-2/3 budget on its own.  `this` = the ability record FindAbility
//  returned, *not* this CUnit; no stack arguments (bare `retn`).
struct SAbilityGrantRecord
{
    void HandleGrantedDuringReposition();
};
__declspec(naked) void SAbilityGrantRecord::HandleGrantedDuringReposition()
{
    __asm
    {
        mov     eax, 06F12D520h
        jmp     eax
    }
}

int CUnit::Reposition(CFloat* posX, CFloat* posY, int forward, int a4,
                      int a5, int a6, int a7, int noDelta, int a10, int a11)
{
    int visible = QueryVisibleImpl(1);

    if (visible != 0 || (m_int248 & 0x200) != 0)
    {
        RemoveFootprint(0, 1);
        DispatchPositionNotifyState(0);

        g_unitMoveFilterFootprint = m_footprintType;
        g_unitMoveFilterOwner = GetOwningPlayerIndex();

        if (a5 == -1)
        {
            int gx = CFloatToInt(WorldToGrid(*posX - g_pGameData->m_originX));
            int gy = CFloatToInt(WorldToGrid(*posY - g_pGameData->m_originY));
            g_unitMoveFilterTerrain = RowMaxWithTerrainType(gx, gy, 0);
        }
        else
        {
            g_unitMoveFilterTerrain = a5;
        }
    }

    //  MoveTo's own trailing "a6 = filter, a7 = context" slots: the
    //  caller's value if it supplied one, else - only when this unit is
    //  currently visible - CUnit's own defaults, else null.  The same
    //  "arg ? arg : (visible ? default : 0)" idiom CUnit::MoveForward
    //  computes for its own filter/context; the shipped code computes
    //  both branchlessly (`neg`/`sbb`/`and`), which is /O2's own
    //  codegen for this exact ternary shape at this optimisation level,
    //  not a hand-rolled bit trick the source needs to spell out (see
    //  Widget/widget.cpp's own MoveForward comment on the identical
    //  pattern).
    //  Direct (non-virtual) call to the base class body, matching the
    //  shipped `call sub_6F2AC220` - see Widget/widget.cpp's own MoveTo.
    //  `facing` is always null here; a10/a11 pass straight through.
    //  `context` (a7) is computed before `filter` (a6), matching the
    //  shipped `mov ecx,[arg_18]` before `mov eax,[arg_14]` order.
    CWidget::MoveTo(posX, posY, forward, a4, a5,
                    (int)(a6 ? (PathFilterFn)a6
                             : (visible ? DefaultUnitMoveFilter : (PathFilterFn)0)),
                    (int)(a7 ? (void*)a7
                             : (visible ? (void*)&g_unitMoveFilterFootprint : 0)),
                    noDelta, 0, a10, a11);

    RefreshAttachmentPlacement();

    {
        float outXY[2];
        ((SPositionDeltaRepublishTarget*)this)
            ->RefreshPositionDelta(&outXY[0], 1);
    }

    RefreshAttachmentPlacement();
    RefreshPositionNotifyState();
    ReleaseAndRefreshFootprintOrientation();

    if (m_pSprite != 0)
        RefreshUnitTypeSplatIfPresent();

    //  `field1EC` - Unit/unit_abilityshortcuts.cpp's own reserved-block
    //  neighbour of `m_atk1E8` (both sit inside unit.h's own
    //  `m_reserved1E4[0x18]` gap); reached the same "reinterpret this
    //  call tree's own two dwords out of the shared reserved block"
    //  way that file's own SAbilityAgentCollector-shaped local struct
    //  does, rather than carving a new named field out of a header every
    //  other TU also reads.
    int field1ECIsZero = (*(int*)((char*)this + 0x1EC) == 0);

    if (visible != 0 || (m_int248 & 0x200) != 0 || field1ECIsZero != 0)
    {
        AddFootprint(1, 1, 1);
        DispatchPositionNotifyState(1);

        if (visible != 0)
            RefreshUberSplatAttachment();

        if (m_pSprite != 0 && field1ECIsZero != 0)
        {
            //  GetPosition's own 4 stack arguments (out, wantGround, a3,
            //  a4) are the *top* 4 of 5 dwords the shipped code pushes
            //  here - the bottom-most (`mask`, pushed first) survives
            //  GetPosition's own `retn 10h` untouched and becomes
            //  MergeControllerSlotVec3's own second stack argument
            //  below, a real MSVC scheduling shape (pushing a later
            //  call's argument ahead of an earlier, unrelated call) not
            //  a guess.
            FVec3 pos;
            GetPosition(&pos, 1, -1, 0);
            MergeControllerSlotVec3((void*)m_pSprite, 1,
                                    &pos.m_x, 0);
            ResetFloatTargetSlotForRecord();
        }
    }

    //  `m_atk1E8` - Unit/unit_findability.cpp's own name for this field
    //  (the ability-agent handle host cached at +0x1E8, 'Aatk'); reached
    //  the same reinterpret-in-place way that file's own local struct
    //  does rather than a header-level named field.
    void* atk1E8 = *(void**)((char*)this + 0x1E8);
    if (atk1E8 != 0)
        ((SPositionRepublishTarget*)atk1E8)->RepublishOwnerPosition();

    unsigned int rawcode = GetFourCC_6F110BC0();
    void* ability =
        ((SUnitAbilityHost*)this)->FindAbility((int)rawcode, 0, 0, 1, 1);
    if (ability != 0)
        ((SAbilityGrantRecord*)ability)->HandleGrantedDuringReposition();

    return (int)ability;
}

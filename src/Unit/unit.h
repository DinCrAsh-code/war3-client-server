//============================================================================
//  CUnit - the class at 0x6F931934 (`??_7CUnit@@6B@`, 118 slots).
//
//  This is the vtable-sweep target: every real address in
//  user_knowledge.json's CUnit table pulled as its own `calltree_asm` dump
//  (`tools/ida_query.py calltree_asm <addr> --depth 2`), read together, and
//  reconstructed slot by slot.  See docs/targets/CUnit__vtable.md.
//
//  **Derives from CSelectable.**  The vtables say so and nothing else has
//  to be argued: CSelectable has 107 slots, CUnit has 118, and CUnit's
//  first 107 are CSelectable's 107 with 75 of them overridden and *not one
//  reordered* - the single-inheritance override pattern.  The 32 it does
//  not override are inherited verbatim, including CWidget's own
//  0x6F266C10 at slot 94 and CAgent's 0x6F001F70 at slot 0, which a class
//  that did not descend from them could not carry.
//
//  Two independent confirmations:
//
//    * slot 22 (0x6F26C1A0) returns the literal string "CUnit", one class
//      name per class exactly as CWidget/CSelectable/CItem do;
//    * slot 7 (0x6F26C1F0) returns '+w3u' = 0x2B773375, 0x30 bytes past
//      the '+w3u' constant misc_rawcode_getters.cpp already had - the same
//      GetAgileTypeId shape every class in this family uses.
//
//  ## The +0x164 sub-object
//
//  Fifteen of CUnit's overrides are one or two instructions long and all
//  have the same shape: `add ecx, 164h` then `jmp <somewhere>`.  That is
//  not a multiple-inheritance adjustor thunk - there is no second base -
//  it is /O2's tail call for `return m_movement.Something(args);` on a
//  member sub-object that starts at +0x164 and carries a handle pair at
//  its own +0x08/+0x0C.  widget_vtable_leaves.cpp's note on slots 48-51
//  had already read that shape off four of them before CUnit itself was
//  modelled; this header is where the sub-object finally gets a name.
//============================================================================
#ifndef UNIT_H
#define UNIT_H

#include "selectable.h"
#include "textsink.h"
#include "agenttimer.h"
#include "unitownedref.h"
#include "unittrackedref.h"

class CDataStore;
class CDataStoreScratch;
class FVec3;
class CAgentPtr;
template <class T> class TSGrowableArray;
struct SCellBoxF;
struct SHandleWithType;
struct SAgentMessage;
struct SWidgetOffsetSource;

//----------------------------------------------------------------------------
//  CUnit's movement/handle sub-object at +0x164.  Only the two fields the
//  slot bodies actually reach are named: LookupHandle (0x6F03FA30,
//  handle.cpp) is called with +0x0C in edx and +0x08 in ecx, which is this
//  repo's handle-pair calling shape everywhere else it appears.
//----------------------------------------------------------------------------
struct SUnitMovement
{
    //  The methods CUnit's own slots tail-call into.  Each is reached as
    //  `add ecx, 164h` / `jmp <method>` - /O2's tail call for
    //  `m_movement.Method(args)`, not a multiple-inheritance adjustor
    //  thunk.  All of them open by resolving m_handle/m_handleType through
    //  LookupHandle, exactly as SUnitTrackedRef's do.
    //
    //  All six are thunks in unreconstructed_thunks.cpp, and they have to
    //  be: IDA has their bodies attached to the *caller* as tail chunks
    //  rather than defined as functions of their own, so a `calltree_asm`
    //  dump carries no `### name @ 0x...` heading for them - which means
    //  verify.py folds them into CUnit's two-instruction slot and nothing
    //  in this pipeline can score them separately.  See
    //  docs/targets/CUnit__vtable.md.
    void ApplyClamped(int a);       // 0x6F474B30 (`retn 4`) - slot 48
    void Apply(int a);              // 0x6F474BA0 (`retn 4`) - slot 49
    void SetLowNibble(int a);       // 0x6F474BD0 (`retn 4`) - slot 50
    void SetHighNibble(int a);      // 0x6F474BF0 (`retn 4`) - slot 51
    int  GetPlacementMask();        // 0x6F476340 (`retn 0`) - slot 86
    void SetPlacementBits(unsigned int bits, int a2);
                                    // 0x6F477050 (`retn 8`) - slot 87

    //  0x6F474460 (`retn 4`) - the collision-grid registration's own box
    //  midpoint {avgX, avgY} (each axis' bound sum halved, rounding toward
    //  zero), or - when either half comes out negative, i.e. nothing is
    //  registered yet - the point the trace has reached by now instead
    //  (GetTravelDistance/PointAtDistance, converted through CFloatToInt).
    //  Reached only from CUnit::RefreshWidgetPlacement's slow path.  Own
    //  translation unit, unit_widgetplacement.cpp.
    void GetCellMidpoint(int* out);

    //  Three more, reached only from CUnit's per-template initialiser
    //  (0x6F2A0E30, still Method_0x1AC / slot 107) rather than through any
    //  vtable tail-call.  See unit_movementflags.cpp.
    void SetPathFlagDefault();                    // 0x6F474A00 (`retn 0`)
    void SetPathFlagLowNibble(unsigned char v);    // 0x6F4749C0 (`retn 4`)
    void SetLaneReleaseFlag(int on);               // 0x6F476200 (`retn 4`)

    //  Two more of the same cluster (cunit_vtable_closure_worklist.json,
    //  slot 107 closure batch 24).  See unit_movementflags.cpp.
    int  GetFootprintSizeClass();                 // 0x6F474A60 (`retn 0`)
    void SetPathClientFlagBit23(int on);           // 0x6F474C60 (`retn 4`)
    //  Same shape as SetPathClientFlagBit23 immediately above, plus one
    //  extra step: ensures the CPathClient exists first
    //  (PrepareUnitPathClient) rather than assuming a caller already
    //  built it.  Reached from CUnit's own slot 107 closure too.
    void EnsurePathClientAndSetFlagBit23(int on);  // 0x6F474C10 (`retn 4`)
    //  0x6F474CF0 - out of this agent's own worklist slice (see
    //  unit_applymovementclassificationparams.cpp); declared here only so
    //  that TU's own CUnit::ApplyMovementClassificationParams compiles.
    void ApplyClassificationParams_6F474CF0(int noWorkerFlags,
        int secondaryField, int colorNibble, int field230);

    void*        m_vtable;      // +0x00 - ~CUnit stamps ??_7TRefCnt@@6B@ here
    int          m_refcount;    // +0x04
    unsigned int m_handle;      // +0x08
    unsigned int m_handleType;  // +0x0C
};

//  The five TRefCnt-derived handle holders CUnit embeds at +0x98, +0xB8,
//  +0xFC, +0x118 and +0x214 are all SUnitTrackedRef (unittrackedref.h).
//  ~CUnit is what says they are TRefCnt-derived: it stamps
//  ??_7TRefCnt@@6B@ over each one's vtable word on the way out, which is
//  the inlined base-destructor shape and not something a non-derived
//  member would produce.

class CUnit : public CSelectable
{
public:
    //====================================================================
    //  CUnit's vtable contribution.  Declaration order *is* vtable order
    //  (C++'s own rule), so this block is sorted by slot index and has to
    //  stay that way - inserting one line in the wrong place renames every
    //  slot after it.  Slots 1-117; see docs/targets/CUnit__vtable.md and
    //  tools/check_vtables.py, which is what actually holds it honest.
    //
    //  Every name here is the base class's name for the same slot, because
    //  an override is the same function under a different implementation;
    //  only the eleven slots CUnit *adds* (107-117) get new names.
    //====================================================================
    //  0x6F28AC90 - unit_ctor.cpp.  Not virtual, for the same reason
    //  ~CUnit() below isn't: a derived class calls its base's constructor
    //  directly at compile time, and CSelectable::CSelectable() (the
    //  implicit base-class init) runs automatically ahead of this body -
    //  no explicit call is written for it, the same way CItem::CItem()
    //  (item.cpp) does not write one for CWar3Image's.
    CUnit();

    //  0x6F28B340 - unit_dtorbody.cpp.  Not virtual: the same reasoning
    //  refcnt.h gives for TRefCnt's - a derived class calls its base's
    //  destructor directly at compile time, and a virtual one here would
    //  add a slot and shift all 118.
    ~CUnit();

    //  slot 1 / +0x04 (0x6F28B670) - unit_dtor.cpp
    virtual CUnit* DeleteSelf(int flags);
    //  slot 3 / +0x0C (0x6F2A7E60) - unit_dispatch.cpp
    virtual int Method_0x0C(const struct SAgentMessage* msg);
    //  slot 7 / +0x1C (0x6F26C1F0) - unit_typeid.cpp
    virtual unsigned int GetAgileTypeId() const;
    //  slot 8 / +0x20 (0x6F26C440) - unit_dumpstate.cpp
    virtual int Method_0x20(int context);
    //  slot 9 / +0x24 (0x6F275E20)
    virtual int Method_0x24(int context);
    //  slot 13 / +0x34 (0x6F282920)
    virtual void Deactivate();
    //  slot 14 / +0x38 (0x6F278660)
    virtual void Save(class CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F290850) - unit_load.cpp.
    virtual void Load(class CDataStore* store);
    //  slot 16 / +0x40 (0x6F29E930)
    virtual void RefreshOwningPlayerColor();
    //  slot 17 / +0x44 (0x6F26C550) - unit_dumpstate.cpp
    virtual void DumpState(void* sink);
    //  slot 22 / +0x58 (0x6F26C1A0) - unit_typeid.cpp
    virtual const char* GetClassName();
    //  slot 23 / +0x5C (0x6F28B070) - unit_thunks.cpp
    virtual void Method_0x5C();
    //  slot 25 / +0x64 (0x6F28B1E0) - unit_thunks.cpp
    virtual unsigned int Method_0x64();
    //  slot 32 / +0x80 (0x6F284B40)
    virtual void ClearPendingAndNotify();
    //  0x6F26ECD0 - non-virtual, called from ClearPendingAndNotify above.
    //  unit_attachment274visibility.cpp
    void SetAttachment274Visible(int visible);
    //  0x6F28B740 / 0x6F28CAA0 - ctor helper leaves.  unit_ctorfootprintinit.cpp
    void NotifyExistingUnitsOfType_6F28B740();
    void CtorHelper_6F277370();
    void CtorHelper_6F282FC0();
    void CtorHelper_6F283F60();
    void InitFootprintAndFlags_6F28CAA0();
    //  0x6F28CAF0 - cunit_agent5_worklist.md: set CAgentWar3::m_flags bit
    //  0x40, then run the same undumped ctor helper CtorHelper_6F282FC0's
    //  caller above already tail-calls.  unit_ctorfootprintinit.cpp.
    void SetFlagBit40_6F28CAF0();
    //  0x6F26FAE0 - CUnit::CUnit()'s own tail-end field-init leaf; kept a
    //  real out-of-line call rather than folded into unit_ctor.cpp, the
    //  way the shipped build keeps it.  unit_ctornotifyfields.cpp.
    void CtorHelper_6F26FAE0();
    //  slot 33 / +0x84 (0x6F291790)
    virtual void SetPending();
    //  slot 34 / +0x88 (0x6F29B700)
    virtual void SetModelByHandle(unsigned int modelHandle, int notify);
    //  slot 35 / +0x8C (0x6F27B3E0)
    virtual float PlayAnimation(unsigned int name, int flags, int notify);
    //  slot 36 / +0x90 (0x6F27B420)
    virtual float PlayAnimationLooping(unsigned int name, int flags, int notify);
    //  slot 40 / +0xA0 (0x6F285BC0)
    virtual void SetFlagBit2(int on);
    //  slot 42 / +0xA8 (0x6F07B4A0) - unit_flagbit8.cpp.  Overrides
    //  CWidget::SetFlagBit8 (agentwar3_flagbits.cpp): does not touch
    //  m_flags at all, toggles a hidden ability ('Avul' rawcode) instead.
    virtual void SetFlagBit8(int on);
    //  slot 43 / +0xAC (0x6F28B300) - unit_thunks.cpp
    virtual int GetDefaultCellCentres();
    //  slot 44 / +0xB0 (0x6F28B2D0) - unit_thunks.cpp
    virtual int Method_0xB0();
    //  slot 45 / +0xB4 (0x6F29D880) - unit_ownershiphighlight.cpp
    //  (CUnit::RefreshOwnershipHighlight's real body).  The *name* stays
    //  Method_0xB4 here: C++ binds a virtual override by name (plus
    //  signature), not by vtable position, and CWidget's own body at this
    //  slot is `int CWidget::Method_0xB4()` (widget_vtable_leaves.cpp) - a
    //  differently-named CUnit method would not override it at all, and
    //  would silently append a 119th slot instead (caught by
    //  tools/check_vtables.py, not by anything that scores).
    virtual int Method_0xB4();
    //  slot 46 / +0xB8 (0x6F28B0E0) - unit_thunks.cpp
    virtual SHandleWithType* GetHandleObject();
    //  slot 47 / +0xBC (0x6F28B180) - unit_thunks.cpp
    virtual float Method_0xBC();
    //  slot 48 / +0xC0 (0x6F28B0F0) - unit_movementthunks.cpp
    virtual void Method_0xC0(int a);
    //  slot 49 / +0xC4 (0x6F28B100) - unit_movementthunks.cpp
    virtual void Method_0xC4(int a);
    //  slot 50 / +0xC8 (0x6F28B110) - unit_movementthunks.cpp
    virtual void Method_0xC8(int a);
    //  slot 51 / +0xCC (0x6F28B120) - unit_movementthunks.cpp
    virtual void Method_0xCC(int a);
    //  slot 53 / +0xD4 (0x6F283470).  `int`, not `void`: the fast path
    //  (m_int1FC != 0x10) just tail-calls the base and returns whatever it
    //  left in eax, but the slow path computes a real bucket index and
    //  leaves *that* in eax with no store anywhere - the same "a slot's
    //  return type is a property of the slot, not of one body" pattern
    //  already used for slots 63/67.  unit_widgetplacement.cpp.
    virtual int RefreshWidgetPlacement();
    //  slot 55 / +0xDC (0x6F2A5FB0)
    virtual int MoveForward(CFloat* posX, CFloat* posY, int useBoundedSearch,
                            int terrainType, int lockFlag, PathFilterFn filter,
                            void* context, const SCellBoxF* bounds,
                            int cellCentresOverride, int unitType, int player);
    //  slot 56 / +0xE0 (0x6F26F320)
    virtual FVec3* GetPosition(FVec3* out, int wantGround, int a3, int a4);
    //  slot 57 / +0xE4 (0x6F281F50)
    virtual float Method_0xE4(FVec3* pos, int a1, int a2, int a3);
    //  slot 58 / +0xE8 (0x6F29E850)
    virtual float Method_0xE8();
    //  slot 59 / +0xEC (0x6F28B1D0) - unit_thunks.cpp
    virtual int GetOwningPlayerIndex();
    //  slot 60 / +0xF0 (0x6F26E340)
    virtual int QueryVisible(int which);
    //  slot 61 / +0xF4 (0x6F26E350)
    virtual void RefreshSpriteVisibility();
    //  slot 62 / +0xF8 (0x6F28C510)
    virtual int IsVisibilityMaskLive(unsigned int a, unsigned int b);
    //  slot 63 / +0xFC (0x6F285080)
    virtual int SubmitToPlayerTable(unsigned int playerIndex, int flags, int extra);
    //  slot 64 / +0x100 (0x6F285110) - unit_publishposition.cpp.  Same
    //  0x6F285xxx module as slot 63; submits the unit through
    //  SPlayerTable, picking between two submit entry points on whether
    //  the acting slot's handle-ref field is set.
    virtual void PublishPosition(int a1, int a2);
    //  slot 65 / +0x104 (0x6F29D990)
    virtual void RefreshTerrainTint();
    //  0x6F29D690 - non-virtual, called only from RefreshTerrainTint's own
    //  body (unit_terraintint.cpp/unit_footprinttint.cpp).  `this` in ecx,
    //  one stack argument (the out pointer) - not __fastcall.
    unsigned int* SampleFootprintTerrainTint(unsigned int* out);
    //  0x6F2E52F0 - non-virtual, unit_abilityhostnotify.cpp.  `this` in
    //  ecx, no stack arguments - not __fastcall.
    void NotifyAbilityHostChanged();
    //  slot 66 / +0x108 (0x6F26FA70)
    virtual float Method_0x108();
    //  slot 67 / +0x10C (0x6F28B330) - unit_thunks.cpp
    virtual int* Method_0x10C(int* out);
    //  slot 69 / +0x114 (0x6F257730)
    virtual int Method_0x114();
    //  slot 70 / +0x118 (0x6F29E570)
    virtual CFloat* Method_0x118(CFloat* out);
    //  slot 71 / +0x11C (0x6F28B150) - unit_thunks.cpp
    virtual int Method_0x11C();
    //  slot 72 / +0x120 (0x6F2A40D0)
    virtual void* NegateAndForwardOffset(void* target, struct SWidgetOffsetSource* src,
                                         int a3, int a4);
    //  slot 73 / +0x124 (0x6F28B1F0) - unit_movementthunks.cpp
    virtual void SetLife(const CFloat* value);
    //  slot 74 / +0x128 (0x6F28B230) - unit_life.cpp
    virtual CFloat AddLife(const CFloat* delta);
    //  slot 75 / +0x12C (0x6F28B2B0) - unit_movementthunks.cpp
    virtual CFloat* GetLife(CFloat* out);
    //  slot 76 / +0x130 (0x6F28B080) - unit_thunks.cpp
    virtual CFloat* GetMaxLife(CFloat* out);
    //  slot 77 / +0x134 (0x6F28B0A0) - unit_thunks.cpp
    virtual CFloat* GetMinLife(CFloat* out);
    //  slot 78 / +0x138 (0x6F29F320)
    virtual CFloat* Method_0x138(CFloat* out);
    //  slot 79 / +0x13C (0x6F28B2F0) - unit_thunks.cpp
    virtual int Method_0x13C();
    //  slot 80 / +0x140 (0x6F28B0C0) - unit_movementthunks.cpp
    virtual void SetMinLife(const CFloat* value);
    //  slot 81 / +0x144 (0x6F28B0D0) - unit_movementthunks.cpp
    virtual void SetMaxLife(const CFloat* value);
    //  slot 86 / +0x158 (0x6F28B130) - unit_thunks.cpp
    virtual int GetPlacementMask();
    //  slot 87 / +0x15C (0x6F28B140) - unit_thunks.cpp
    virtual void Method_0x15C(unsigned int bits, int a2);
    //  slot 88 / +0x160 (0x6F29F380)
    virtual int Method_0x160();
    //  slot 89 / +0x164 (0x6F29F390)
    virtual int Method_0x164();
    //  slot 90 / +0x168 (0x6F29F3A0)
    virtual CFloat* GetPlacementRadius(CFloat* out);
    //  slot 91 / +0x16C (0x6F28D4F0)
    virtual void RefreshPlacementRadius();
    //  slot 92 / +0x170 (0x6F28B160) - unit_thunks.cpp
    virtual int GetPositionNotifyHandle();
    //  slot 93 / +0x174 (0x6F28B170) - unit_thunks.cpp
    virtual void SetPositionNotifyHandle(int value);
    //  slot 95 / +0x17C (0x6F2769B0)
    virtual unsigned int Method_0x17C();
    //  slot 96 / +0x180 (0x6F2A5D50)
    virtual int Reposition(CFloat* posX, CFloat* posY, int forward, int a4, int a5,
                           int a6, int a7, int noDelta, int a10, int a11);
    //  slot 97 / +0x184 (0x6F285A40)
    virtual void ReleaseSelectionVisual();
    //  slot 98 / +0x188 (0x6F285630)
    virtual int Method_0x188();
    //  slot 99 / +0x18C (0x6F285650)
    virtual int Method_0x18C();
    //  slot 100 / +0x190 (0x6F285770)
    virtual void RefreshSelectionHighlight(int wantHighlight, int visualArg);
    //  slot 101 / +0x194 (0x6F28DCF0)
    virtual void EnsureSelectionCircle(int wantUi, int runSlot103, int wantCircle,
                                       int unused3, int unused4);
    //  slot 103 / +0x19C (0x6F27A730)
    virtual int Method_0x19C();
    //  slot 104 / +0x1A0 (0x6F278D40) - unit_attachmentplacement.cpp.
    //  Overrides CSelectable::RefreshAttachmentPlacement
    //  (selectable_attachrefresh.cpp): same job, but adds a third
    //  attachment slot (m_handle278) and a trailing SelCircleOnWater block.
    virtual void RefreshAttachmentPlacement();
    //  slot 105 / +0x1A4 (0x6F26F4B0)
    virtual void SetAttachmentScale(float scale);
    //  slot 106 / +0x1A8 (0x6F26F430)
    virtual void ShowAttachments();

    //====================================================================
    //  The eleven slots CUnit adds on top of CSelectable's 107.  Names are
    //  positional (Method_0x1AC..Method_0x1D4) except where the body
    //  establishes one - see docs/targets/CUnit__vtable.md.
    //====================================================================
    //  slot 107 / +0x1AC (0x6F2A0E30, `retn 10h`) - four stack arguments.
    virtual int Method_0x1AC(int a, int b, int c, int d);
    //  slot 108 / +0x1B0 (0x6F25C7C0) - unit_flagbits.cpp.
    virtual int Method_0x1B0();
    //  slot 109 / +0x1B4 (0x6F2783C0) - a float in st0, not an int:
    //  both exits are an x87 load.  unit_boundvalue.cpp.
    virtual float Method_0x1B4();
    //  slot 110 / +0x1B8 (0x6F282160, `retn 8`) - two stack arguments.
    virtual int Method_0x1B8(int a, int b);
    //  slot 111 / +0x1BC (0x6F28B2E0).  No arguments and an int result:
    //  slot 62 (unit_visibility.cpp) tests it as a plain flag.
    virtual int Method_0x1BC();
    //  slot 112 / +0x1C0 (0x6F29F3C0) - slot 90's twin, one CFloat out
    //  buffer in and the same buffer back.  unit_placement.cpp.
    virtual CFloat* Method_0x1C0(CFloat* out);
    //  slot 113 / +0x1C4 (0x6F27A620).  One stack argument and a float
    //  result: slot 66 (unit_footprintscale.cpp) is nothing but a virtual
    //  call into it with -1, and returns what it returns in st0.
    virtual float Method_0x1C4(int a);
    //  slot 114 / +0x1C8 (0x6F2856C0) - no arguments.
    virtual int Method_0x1C8();
    //  slot 115 / +0x1CC (0x6F285BF0, `retn 8`) - two stack arguments.
    virtual int Method_0x1CC(int a, int b);
    //  slot 116 / +0x1D0 (0x6F2979E0, `retn 8`) - two stack arguments.
    virtual int Method_0x1D0(int a, int b);
    //  slot 117 / +0x1D4 (0x6F297B30, `retn 8`) - two stack arguments.
    virtual int Method_0x1D4(int a, int b);

public:
    //====================================================================
    //  Not vtable slots: the out-of-line helpers CUnit's own slots call
    //  directly (`call <addr>` with `this` unchanged).  Bodies are in
    //  unreconstructed_thunks.cpp until each one is reconstructed for
    //  real; the argument counts are read off each callee's own `retn`.
    //====================================================================
    //  0x6F2833E0 (`retn 0`) / 0x6F26FE60 / 0x6F26FE70 (`retn 4`) - the
    //  eligibility gate the unit factory tests before its two creation
    //  notifies, and the two notifies themselves.  unit_creationgates.cpp.
    int  IsEligibleForCreationNotify();
    void NotifyCreatedA(int flag);
    void NotifyCreatedB(int flag);
    //  0x6F28DBE0 (`retn 8`) - slot 33 re-runs it with (1, 0).
    void RefreshPendingState(int a, int b);
    //  0x6F296C40 (`retn 4`) - slot 34 tells it -1 after a model change.
    void NotifyModelChanged(int index);
    //  0x6F27A5A0 (`retn 0`) - slot 13 drops the unit's motion state with
    //  it: release the +0x2C4 reference, zero the three floats at
    //  +0x2B8/+0x2BC/+0x2C0, and stop the sprite if there is one.
    void ClearMotionState();
    //  0x6F2E5A70 (`retn 0`) - unit_notifyref304.cpp, reconstructed.
    void NotifyRef304Deactivated();
    //  0x6F28B6A0 (`retn 4`) - unit_assignref304.cpp.  Release the notify
    //  on the current m_ref304 target, then chain-resolve a new value into
    //  it from `source`.
    void AssignRef304(const void* source);
    //  0x6F26DE50 (`retn 4`) - the visibility probe slot 60 is a bare `jmp`
    //  into and slots 43/44 call directly rather than dispatching back
    //  through the vtable to reach it.
    int QueryVisibleImpl(int which);
    //  0x6F285DD0 (`retn 0`) - visible (QueryVisibleImpl(1)) and bit 25 of
    //  +0x5C set.  EnsureSelectionCircle's own closure (slot 101).
    //  unit_visibleflagchecks.cpp.
    int IsVisibleWithFlagBit25();
    //  0x6F285DF0 (`retn 0`) - same shape, bit 26.  unit_visibleflagchecks.cpp.
    int IsVisibleWithFlagBit26();
    //  0x6F28D490 (`retn 0`) - bit 14 of +0x5C gating IsVisibleWithFlagBit25:
    //  short-circuits to 0 without even probing visibility if the bit is
    //  clear.  Reached from slot 107's own closure.  unit_visibleflagchecks.cpp.
    int IsFlagBit14AndNotVisibleWithFlagBit25();
    //  0x6F28D4B0 (`retn 0`) - same shape, bit 15.  unit_visibleflagchecks.cpp.
    int IsFlagBit15AndNotVisibleWithFlagBit25();
    //  0x6F285B00 (`retn 4`) - is this unit *not* in the relation its own
    //  owner (+0x58) holds toward `widget`'s owner?  Reached from both
    //  slot 3's and slot 72's own closures.  unit_hostiletoownerof.cpp.
    int IsNotRelatedToOwnerOf(class CWidget* widget);
    //  0x6F25D480 (`retn 0Ch`) - which player slots a unit-enumeration's
    //  owner-relationship filter *excludes*, as the low sixteen bits of the
    //  enumeration mask.  `this` is the unit the relationships are measured
    //  against and **may be null**: the shipped body tests it and falls back
    //  on the explicit player id the same filter carries, so it is a member
    //  for the calling convention rather than because the body always has a
    //  receiver.  See unitrelationplayermask.cpp.
    unsigned int RelationPlayerMask(unsigned int relations, int player,
                                    int invert);
    //  0x6F2859F0 (`retn 0`) - slot 97 runs it first.
    void TearDownSelectionVisual();
    //  0x6F2917B0 (`retn 4`) - NotifyModelChanged's own tail call, after
    //  every attachment scale it owns has been republished.  Still a
    //  thunk - see unit_selectionvisual_notify.cpp / citemclosure_batchL_thunks.cpp.
    void RebuildAnimationIndexTable(int index);
    //  0x6F333310 (`retn 0`) - slot 40 runs it after clearing the bit.
    void OnSelectionBitCleared();
    //  0x6F26D6F0 (`retn 0`) - unit_activitytimestamps.cpp.  Reset the two
    //  +0x90/+0x94 activity timestamps to "two minutes ago"; run once at
    //  the very end of Load.
    void ResetActivityTimestamps();
    //  0x6F29E8F0 (`retn 0`) - cunit_agent7 batch (CUnit closure BFS
    //  depth 1-2).  See unit_colornibbles.cpp.
    int GetColorPlayerOrGreyNibble();
    //  0x6F27A4E0 (`retn 4`) - reached from slot 3 (0x6F2A7E60).
    //  unit_republishpanmodifier.cpp.
    void RepublishPanFloatModifier(float value);
    //  0x6F2458F0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F2458F0();
    //  0x6F2384E0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F2384E0();
    //  0x6F238C30 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F238C30();
    //  0x6F239380 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F239380();
    //  0x6F239AD0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F239AD0();
    //  0x6F23A150 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23A150();
    //  0x6F23A7D0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23A7D0();
    //  0x6F23B0C0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23B0C0();
    //  0x6F23B740 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23B740();
    //  0x6F23BE90 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23BE90();
    //  0x6F23E730 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23E730();
    //  0x6F241240 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F241240();
    //  0x6F242B70 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F242B70();
    //  0x6F2431F0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F2431F0();
    //  0x6F243870 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F243870();
    //  0x6F243EF0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F243EF0();
    //  0x6F244570 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F244570();
    //  0x6F244BF0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F244BF0();
    //  0x6F245270 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F245270();
    //  0x6F246040 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F246040();
    //  0x6F2466C0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F2466C0();
    //  0x6F246D40 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F246D40();
    //  0x6F247630 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F247630();
    //  0x6F247CB0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F247CB0();
    //  0x6F248330 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F248330();
    //  0x6F2489B0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F2489B0();
    //  0x6F249030 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F249030();
    //  0x6F2496B0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F2496B0();
    //  0x6F249D30 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F249D30();
    //  0x6F24A3B0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F24A3B0();
    //  0x6F24AB00 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F24AB00();
    //  0x6F24B180 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F24B180();
    //  0x6F24B800 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F24B800();
    //  0x6F24BE80 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F24BE80();
    //  0x6F24C500 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F24C500();
    //  0x6F24CC50 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F24CC50();
    //  0x6F29AC70 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F29AC70();
    //  0x6F238680 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F238680();
    //  0x6F23AA40 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23AA40();
    //  0x6F23C030 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23C030();
    //  0x6F23C780 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23C780();
    //  0x6F23CE00 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23CE00();
    //  0x6F23D480 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23D480();
    //  0x6F23DBD0 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23DBD0();
    //  0x6F23E250 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23E250();
    //  0x6F23EE80 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23EE80();
    //  0x6F23F500 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23F500();
    //  0x6F23FB80 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F23FB80();
    //  0x6F240200 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F240200();
    //  0x6F240A20 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F240A20();
    //  0x6F241310 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F241310();
    //  0x6F241A60 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F241A60();
    //  0x6F2421B0 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F2421B0();
    //  0x6F242DE0 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F242DE0();
    //  0x6F245C30 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F245C30();
    //  0x6F247F20 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F247F20();
    //  0x6F24AD70 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F24AD70();
    //  0x6F29AEE0 - cunit_agent7 batch (CUnit closure BFS depth 1-2), slice B.
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F29AEE0();
    //  0x6F29B2F0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
    //  unit_getability_agent7_batch.cpp.
    CAgent* GetAbility_6F29B2F0();
    //  Twenty more of the same "GetXxxAbility" family, all reached from
    //  slot107's own closure (CUnit's per-template initialiser,
    //  0x6F2A0E30) rather than from the cunit_agent7 batch above -
    //  unit_getability_batch_slot107.cpp.
    CAgent* GetAbility_6F2380D0();
    CAgent* GetAbility_6F23A3C0();
    CAgent* GetAbility_6F23BF60();
    CAgent* GetAbility_6F23C6B0();
    CAgent* GetAbility_6F23CD30();
    CAgent* GetAbility_6F23D3B0();
    CAgent* GetAbility_6F23DB00();
    CAgent* GetAbility_6F23E180();
    CAgent* GetAbility_6F23EDB0();
    CAgent* GetAbility_6F23F430();
    CAgent* GetAbility_6F23FAB0();
    CAgent* GetAbility_6F240130();
    CAgent* GetAbility_6F240950();
    CAgent* GetAbility_6F241170();
    CAgent* GetAbility_6F241990();
    CAgent* GetAbility_6F2420E0();
    CAgent* GetAbility_6F242760();
    CAgent* GetAbility_6F245A90();
    CAgent* GetAbility_6F2478A0();
    CAgent* GetAbility_6F24A6F0();
    //  0x6F07B3E0 (`retn 8`) - a real slot-42 (SetFlagBit8) sibling for the
    //  'Arsc' ability, reached from slot107's own closure rather than from
    //  CUnit's vtable directly - see Unit/unit_flagbit8_sibling.cpp.  `desc`
    //  is really a `struct SFlagBitDescriptor*` (private to that TU); typed
    //  `void*` here so this declaration does not need that struct's
    //  definition in scope.
    void SetAbilityFlagBit_Arsc(void* desc, int on);
    //  0x6F41CE90 (`retn 4`) - reached from slot107's own closure: behind a
    //  feature gate, resolve/make the '+pusd' agent and wire two of its
    //  embedded handle-ref sub-objects to this unit and to `arg0` - see
    //  Unit/unit_optionalpusd.cpp.
    void EnsureOptionalPusdAgent(int arg0);
    //  0x6F291920 (`retn 0Ch`) - reached from slot107's own closure: fill
    //  the +0x2D8 anim-prop set, optionally replay the idle/attack
    //  animation pair, optionally refresh the uber-splat attachment, then
    //  refresh +0x2D4 (m_int2D4) from the footprint registry and rebuild
    //  the animation index table.  See Unit/unit_notifymodelchanged.cpp.
    void PopulateAnimPropsAndModelColor(unsigned int typeId, int replayAnim, int refreshSplat);
    //  0x6F41F9A0 (`retn 0`) - the raw handle half of the order queue's
    //  own tail entry.  Reached from slot 115's closure.
    //  unit_slot115closureleaves.cpp.
    unsigned int GetOrderQueueTailHandleRaw();
    //  0x6F421E30/0x6F421EF0 (`retn 4` each) - tail-call two fixed offsets
    //  of an unnamed sub-object into SAbilityAgentCollector::CollectAgents
    //  (0x6F421A80, unitsubgrouprefresh.cpp).  Reached from slots 116/117's
    //  own closures.  unit_slot115closureleaves.cpp.
    void CollectAbilityAgentsAt1E8(TSGrowableArray<CAgentPtr>* out);
    void CollectAbilityAgentsAt1C4(TSGrowableArray<CAgentPtr>* out);

    //====================================================================
    //  The order path - see unitorder.h for the order object itself and
    //  for MakeOrderAgent.  Two of these are reconstructed and the rest
    //  are redirects in unitorderthunks.cpp; every argument count below
    //  came off the callee's own `retn <n>` in the shipped image, and all
    //  of them are `this`-in-ecx members (the two call sites with no
    //  `mov ecx,esi` in front of them are MSVC dropping a copy it can see
    //  is already there, not free functions - see unitordersubmit.cpp).
    //====================================================================
    //  0x6F2A4B80 (`retn 4`) - unitstoporder.cpp.
    void IssueStopOrder(int replaceCurrent);
    //  0x6F2A3C90 (`retn 0`) - unit_issueorder0xd0006.cpp.  Build order id
    //  0xD0006 with no target reference and start it immediately.
    void IssueOrder_0xD0006();
    //  0x6F2A4BB0 (`retn 4`) - unit_submitorder0xd0006.cpp.  IssueStopOrder's
    //  own shape (unitstoporder.cpp) with order id 0xD0006 instead of
    //  0xD0004.
    void SubmitOrder_0xD0006(int replaceCurrent);
    //  0x6F2A4AB0 (`retn 0Ch`) - unitordersubmit.cpp.
    void SubmitOrder(void* order, int replaceCurrent, int flags);
    //  0x6F2831A0 (`retn 0`) - drop whatever the unit is doing now.
    void CancelCurrentOrder();
    //  0x6F2A0510 (`retn 4`) - make this order the current one.
    void StartOrderNow(void* order);
    //  0x6F2A0650 (`retn 0Ch`) - put it on the queue instead.
    void AppendOrder(void* order, int afterCurrent, int flags);
    //  0x6F2832E0 (`retn 4`) - is the unit already carrying this order
    //  out?  SubmitOrder drops the new one when it is.
    int  IsOrderAlreadyActive(void* order);
    //  0x6F2A4A10 (`retn 0`) - run before StartOrderNow when nothing was
    //  queued.
    void PrepareForNewOrder();
    //  0x6F2964E0 (`retn 0`) - run before AppendOrder on that same empty
    //  queue, and the only thing between it and the append is the order
    //  target notification below.
    void FinishCurrentOrder();
    //  0x6F283340 (`retn 0`) - tell whatever +0x19C names that this unit
    //  is taking a new order, when that object asks to be told (bit 3 of
    //  its own +0x20).
    void NotifyOrderTarget();

    //====================================================================
    //  The rest of the order path, added with JASS_IssueTargetOrderById
    //  (0x6F3C89D0).  Same rule as above: `this` in ecx, and every
    //  argument count came off the callee's own `retn <n>`.
    //====================================================================
    //  0x6F279A90 (`retn 4`) - which of this unit's abilities should carry
    //  `order` out?  Walks the ability list at +0x1DC and returns the best
    //  candidate, or null.  See unitorderability.cpp.
    void* FindAbilityForOrder(void* order);
    //  0x6F279560 (`retn 14h`) - can any of them take this order at a
    //  point?  0 means yes; otherwise the *lowest* refusal code any
    //  candidate gave, starting from 0xDD.  unitcheckpointorder.cpp.
    int CheckPointOrder(int orderId, int player, void* abilitySource,
                        const void* x, const void* y);
    //  0x6F279630 (`retn 10h`) - ... and at a widget target.
    int CheckTargetOrder(int orderId, int player, void* abilitySource,
                         void* target);
    //  0x6F29DFF0 (`retn 8`) - hand the order to whichever of the three
    //  per-class starters matches it and announce that it has begun.
    //  A redirect (unitorderpaththunks.cpp).
    void BeginOrder(void* order, int reschedule);
    //  0x6F2A49C0 (`retn 0`) - stop the unit moving: re-publish the
    //  movement object at +0x164 and clear bits 20 and 21 of +0x5C.
    //  A redirect (unitorderpaththunks.cpp).
    void StopMovement();
    //  0x6F26F900 (`retn 0`) - cancel the +0x180 timer and, if the pending
    //  bit at +0x280 is up, put the controller ramp back and clear it.
    //  unitrampflush.cpp.
    void FlushPendingRamp();
    //  0x6F26DD30 (`retn 0`) - re-arm the controller slot the ramp drives.
    //  A redirect (unitorderpaththunks.cpp); see its note there.
    void RestartPropertyRamp();
    //  0x6F2793E0 (`retn 8`) - re-arm the +0x180 timer with a caller-given
    //  delay, and if both the delay and a caller-given rate are nonzero,
    //  store rate/delay into controller slot 7 of the +0x28 object and set
    //  the pending-ramp bit.  unit_armpropertyramp.cpp.
    void ArmPropertyRampWithRate(const CFloat* delay, float compareRate);
    //  0x6F28BEE0 (`retn 0`) - clear +0x5C bit 0, then
    //  ArmPropertyRampWithRate(&g_CFloatZero, 0.0f) (always a no-op past
    //  the re-arm, at this call site).  unit_armpropertyramp.cpp.
    void CancelPropertyRampProperty0x5C();
    //  0x6F06F2D0 / 0x6F06F9D0 - CUnit batch-19, slot 107's own depth<=2
    //  closure.  Two ability-toggle bodies keyed off a footprint-type
    //  FourCC.  See unit_abilitybyfootprinttype.cpp.
    void AddAbilityFromRecordUnconditional_6F06F2D0();
    void ToggleAbilityByFootprintType_6F06F9D0();
    //  0x6F0708B0 - CUnit batch-19, slot 107's own depth<=2 closure (same
    //  worklist slice as the pair immediately above).  Classifies this
    //  unit by its own footprint-type FourCC into one of four buckets and
    //  installs the matching innate-ability CAgent.  See
    //  unit_installabilitybyfootprintkind.cpp.
    void InstallAbilityAgentByFootprintKind();
    //  0x6F2A0830 - same worklist slice again.  See
    //  unit_applymovementclassificationparams.cpp.
    void ApplyMovementClassificationParams();
    //  0x6F27B460 (`retn 4`) / 0x6F27B4F0 (`retn 4`) - a small
    //  fixed-capacity (6) unique-value set living at this unit's own
    //  +0x2D8 (count) / +0x2E0..+0x2F4 (six dwords) - read through raw
    //  offsets rather than m_animMode2D8/m_animName2E0 below, which are a
    //  different investigation's names for the same bytes (see
    //  unit_field2d8set.cpp).
    int AddToField2D8Set(unsigned int value);
    int RemoveFromField2D8Set(unsigned int value);
    //  0x6F285910 - walk a data source's "animProps" property array
    //  backwards and AddToField2D8Set every element.
    //  unit_populatefield2d8fromanimprops.cpp.
    void PopulateField2D8FromAnimProps(void* dataSource);
    //  0x6F282F40 (`retn 0`) - if the +0x1F8 inventory ability is set, OR
    //  bit 0x100 into its own +0x20 flags word.  unit_inventoryflag20.cpp.
    void SetInventoryAbilityFlag0x20Bit0x100();
    //  0x6F275D90 (`retn 0`) - resolve m_ref304, or null if unset.
    //  unit_resolveref304.cpp.
    void* ResolveRef304();
    //  0x6F2855E0 (`retn 4`) - conditionally clears the motion reference
    //  through a QueryHitAllowed-style gate. unit_clearmotionrefongate.cpp.
    void ClearMotionRefOnHitGate(void* arg);
    //  0x6F277770 (`retn 4`) - one of CUnit::Method_0x0C's (slot 3,
    //  0x6F2A7E60) many message-case handlers, copying a field out of the
    //  dispatched message into m_int194.  The message type is one of the
    //  slot's own ~140 jump-table cases (jpt_6F2A7F33) this session did not
    //  resolve to a name; the message object's own layout is unknown past
    //  the one pointer-then-field chain this handler reads.
    void StoreMessageField0x34ToInt194(void* message);
    //  0x6F27BA70 (`retn 8`) - broadcast (a, b) to every entry of the
    //  +0x1DC ability chain FindAbilityForOrder also walks, through each
    //  entry's own vtable slot 0x100.  unit_notifyabilitieschain.cpp.
    void NotifyAbilitiesOfMessage(int a, int b);
    //  0x6F28E2C0 (`retn 4`) - walk the same +0x1DC ability chain, but
    //  toggle each entry's own +0x20 bit 1 and call through vtable slot
    //  0xC8 (`activate`) or 0xC4 (not) instead of dispatching a message.
    //  unit_abilitychainlistentoggle.cpp.
    void NotifyAbilityChainListenerToggle(int activate);
    //  0x6F079CA0 (`retn 4`) - tell the ability host and the game UI about
    //  a newly-granted/changed ability manager.  Reached from dozens of
    //  per-ability leaves.  Unit/unit_gameuiabilitylistenernotify.cpp.
    void NotifyAbilityManagerAndGameUI(void* manager);
    //  0x6F277430 (`retn 0`) - one of Method_0x0C's message-case handlers
    //  (case 852314): drop every ability, then tail-call slot 0x5C.
    //  unit_dispatch314.cpp.
    void Method_0x0C_DropAllAbilities();
    //  0x6F277D20 (`retn 0`) - if the pending order-target ref (+0x19C) is
    //  set, resolve it and clear bit 2 of the resolved object's own +0x20
    //  flags word.  unit_clearordertargetflag.cpp.
    void ClearOrderTargetExecutingFlag();
    //  0x6F2840A0 (`retn 0`) - pop one node off the +0x174 pending-task
    //  chain FlushPendingRamp's own caller (FinishCurrentOrder) tears down,
    //  and tail-call FlushPendingRamp once it is gone.  unit_taskpop.cpp.
    void PopPendingTask();
    //  0x6F29DA00 (`retn 0`) - cancel four pending events by id (0xD0148,
    //  0xD014A, 0xD0178, kEventOrderFinished) through CancelEventById, then
    //  a fifth call with argument 0 through Method_0x2963F0.
    //  unit_canceleventgroup.cpp.  CUnit batch-13, slot 3 closure.
    void CancelPendingEventGroup();

    //  CUnit vtable slot-finalize pass: the rest of slot 3's own message
    //  dispatcher (Method_0x0C, 0x6F2A7E60) - naked leaves with no dump
    //  of their own reached only from that one switch, `this` in ecx and
    //  nothing else pushed unless the comment says otherwise. Named by
    //  address, since nothing in this dump resolves what a given message
    //  id actually means.  unit_dispatch.cpp.
    void MsgHandler_6F2A4990();
    void MsgHandler_6F29DB50(void* message);   //  `retn 4` - takes the dispatched message.
    void MsgHandler_6F2A0330();
    void MsgHandler_6F29DA40(void* message);   //  `retn 4` - takes the dispatched message.
    void MsgHandler_6F2DCC70();
    void MsgHandler_6F2E5C20();
    void MsgHandler_6F2A4950();
    void MsgHandler_6F28BAA0();
    void MsgHandler_6F2A0140();
    void MsgHandler_6F2828C0();
    void MsgHandler_6F29D570();
    void MsgHandler_6F28BF00();
    void MsgHandler_6F294920();
    void MsgHandler_6F28C030();
    void MsgHandler_6F28BFF0();
    void MsgHandler_6F2A01C0();
    void MsgHandler_6F2A4810();
    void MsgHandler_6F29ED60();
    void MsgHandler_6F2A7D80();
    void MsgHandler_6F29DBC0(void* message);   //  `retn 4` - takes the dispatched message.
    void MsgHandler_6F284950();

    //  CUnit vtable slot-finalize pass: slot 32's own four still-unnamed
    //  leaves (ClearPendingAndNotify, unit_clearpendingnotify.cpp) - see
    //  that file's own header for what little is known of each.
    void OnAbilityGrantedForNotify_6F2E5030();
    void DispatchPositionNotifyState(int arg);
    //  0x6F295AC0 (`retn 4`) - SEH-blocked (own __except_handler4 frame,
    //  security cookie, scope table - the confirmed ceiling documented in
    //  docs/targets/CUnit__vtable.md).  Called with an event id
    //  (kEventOrderFinished and three sibling 0xD01xx constants seen so
    //  far).  Thunked only - never a reconstruction target.
    //  unit_canceleventgroup.cpp.
    void CancelEventById(unsigned int eventId);
    //  0x6F2963F0 (`retn 4`) - SEH-blocked, same ceiling as
    //  CancelEventById.  Called with a small int (0, 8 seen so far);
    //  purpose not established past its ABI.  Thunked only.
    //  unit_canceleventgroup.cpp.
    void Method_0x2963F0(int arg);

    //  0x6F26F970 (`retn 0`) - announce message id 0xD01DE through slot 4
    //  (+0x10, PostAgentEventFn), the same CAgent-level notify
    //  CUnit::FinishCurrentOrder uses for its own order events, except the
    //  event's own `m_pOrder` word is stamped with `this` rather than an
    //  order object.  Real name of the 0xD01DE message not established;
    //  reached from MsgHandler_6F284950 (unit_dispatch.cpp) via
    //  sub_6F284950, itself off slot 3 (0x6F2A7E60).
    //  unit_msgevent_26f970.cpp.
    void Method_0x26F970();

    //====================================================================
    //  CUnit's inventory gate - see unitinventory.h, which is where the
    //  ability object at +0x1F8 and the flag words these read live.  They
    //  arrived with JASS_UnitAddItemById (0x6F3C74F0) on their own minimal
    //  `class CUnit : public CSelectable`, declared before this one
    //  existed; the two are one class now, which is what the vtable sweep
    //  made possible - and necessary, since only one of them can emit
    //  ??_7CUnit@@6B@.
    //====================================================================
    //  0x6F282C60 - can this unit hold items at all right now?
    int CanUseInventory();
    //  0x6F26EC20 - whatever is parked in inventory slot `index`, or null.
    //  Six instructions ending in a tail jump into the ability object's
    //  own reader - see unititeminslot.cpp.  `retn 4`.
    CAgent* GetItemInSlot(int index);
    //  0x6F282CA0 - why can this unit not be given an item?  0 when it can.
    int CheckItemAddable(int skipOwnerCheck, int isPowerup, int allowFullBag);
    //  0x6F282D30 - re-check for the item's own powerup bit, then hand it
    //  to the inventory ability.
    int AddItem(void* item, int skipOwnerCheck);
    //  0x6F277A80 - not reconstructed; `retn 0`, so no stack arguments.
    int IsOwnedByAPlayer();
    //  0x6F282A10 - unit_ownerworldmask.cpp.  `retn 0`.  Is the owning
    //  player's bit set in the world-record table's own +0x2E0 word?
    int IsOwningPlayerInWorldMask2E0();
    //  0x6F2AB460 (`retn 0`) - CWidget's; slot 73 runs it after publishing.
    //  Declared on CWidget rather than here - see widget.h.

    //  0x6F27AE90 / 0x6F27AF20 - the two JASS `unitstate` dispatchers, in
    //  unit_state.cpp.  Not vtable slots: GetUnitState and SetUnitState
    //  call them directly on the resolved CUnit.  They are what proves
    //  slots 73/75/76 are the *life* pool and m_mana the mana one - see
    //  that file's header.  `retn 8` both.
    CFloat* GetState(CFloat* out, int state);
    void    SetState(int state, const CFloat* value);

    //  0x6F26EC60 - unit_inventorysize.cpp.  `retn 0`.  The inventory
    //  ability's slot count, or 0 when there is no such ability.
    int GetInventorySize();
    //  0x6F26EC00 - unit_inventoryvalidcount.cpp.  `retn 0`.  How many of
    //  the inventory ability's own slots still resolve to a live handle -
    //  what CAbilityInventory::DetachAndNotify's own "does the owner have
    //  anything left" gate asks after detaching an item.
    unsigned int CountValidInventoryHandles();
    //  0x6F285740 - unit_subgrouprefreshgate.cpp.  `retn 0`.  Gates
    //  CAbilityInventory::DetachAndNotify's own "tell the local player
    //  their subgroup selection changed" notification: true outright for
    //  a unit whose footprint-type tag starts with an uppercase letter and
    //  whose +0x5C bit 30 is clear, otherwise true only while this unit
    //  still has at least one valid inventory handle.
    int WantsSubgroupRefreshNotify();
    //  0x6F3329B0 - unit_subgrouprefreshnotify.cpp.  `retn 0`.  Hand
    //  `this` to the game UI singleton's own subgroup-notify member, if
    //  the UI has been created yet.
    void NotifySubgroupChanged();
    //  0x6F27A320 - unit_querydetection.cpp.  `retn 8`.  Is this unit
    //  detected by `playerIndex`, for either of the two kinds of detection
    //  `kinds` selects (bit 0 and bit 1)?  JASS's IsUnitDetected asks for
    //  both at once with -1.
    int QueryDetection(unsigned int playerIndex, int kinds);

    //  The four leaves SPlayerTableSubmitParts::RefreshUnitFog
    //  (Widget/playertableunitfogrefresh.cpp, 0x6F40A650) reaches straight
    //  off its own `CUnit*` stack argument.  All four are still naked
    //  redirects (Unit/unit_fogrefresh_thunks.cpp) - `0x6F27A460`'s own
    //  callees (`sub_6F15BA20`/`sub_6F165AB0`) and `0x6F29F150`'s
    //  (`sub_6F29F040`/`sub_6F406AB0`) have no dump, and `0x6F284DA0`'s own
    //  12-player relation-mask sweep is worth a session of its own.  All
    //  four confirmed `retn 0` (bare `retn`, no operand) off their own
    //  dumped bodies in
    //  `asm/sub_6F40A650_0x6F40A650_calltree_asm.md`, so the thiscall,
    //  no-stack-argument declarations below are ABI-safe even undone.
    //
    //  0x6F27A460 - a channel-mask/special-ability query; also reached
    //  from CWorldFrameWar3::AddSelectedUnitIfNotAlready
    //  (0x6F39A1E0) as the "already effectively selected" gate.
    int QuerySpecialSelectionMask();
    //  0x6F284E80 - a second, narrower vision-flag/ability query,
    //  returning a 16-bit word ('lga+'-tagged widget +0xCC).
    unsigned short QueryLegacyVisionWord();
    //  0x6F29F150 - refreshes `+0x10C` from the player table's own +0x34
    //  sub-object; side-effecting, return value unused by its one caller.
    void RefreshField10C();
    //  0x6F284DA0 - the twelve-player relation-mask sweep into `+0x148`/
    //  `+0x14C`, calling QuerySpecialSelectionMask() per matching player
    //  bit; side-effecting, return value unused by its one caller.
    void RefreshRelationMaskFields();

    //====================================================================
    //  Fields.
    //
    //  CUnit's own storage starts at +0x54, where CSelectable's ends
    //  (selectable.h).  Two dumps fix almost all of it and neither needs a
    //  guess: slot 17 (DumpState, 0x6F26C550, unit_dumpstate.cpp) walks
    //  every field the shipped build thought worth printing and its choice
    //  of printer gives each one's *type* - SIntMiniValue for a plain
    //  dword, SFltMiniValue for a CFloat, SRefMiniValue for a handle pair,
    //  FloatMiniB for a {val, min, max} triple - and ~CUnit (0x6F28B340,
    //  unit_dtorbody.cpp) gives the offsets of everything with a
    //  destructor, in reverse declaration order, out of its own unwind
    //  funclet table.
    //
    //  The names are positional (`m_int58`, `m_ref130`) rather than
    //  invented.  A debug dump says what a field *is*, not what it means,
    //  and this repo's rule is that a name should read like the original's
    //  - so where nothing establishes a meaning, the offset is the
    //  honest name.  Fields that a slot body does give a meaning to are
    //  named for it.
    //====================================================================
    //  +0x54 .. +0x64 - five plain dwords, all five printed as [IntMini].
    unsigned int   m_int54;              // +0x54
    //  +0x58 - the owning player index.  Slot 25 hands it back raw and so
    //  does slot 59, and slot 59 is GetOwningPlayerIndex - two slots at
    //  the same field is what names it.
    unsigned int   m_owningPlayer58;     // +0x58
    unsigned int   m_flags5C;            // +0x5C - CUnit's own flags word
    //  0x6F03FDC0 (`retn 0`) - unit_footprinttypeorflagbit30.cpp.  True
    //  only if m_footprintType's own first (most-significant) byte is an
    //  uppercase ASCII letter AND flag bit 30 of m_flags5C is clear.
    int TestFootprintTypeRangeAndFlagBit30Clear();
    unsigned int   m_int60;              // +0x60
    unsigned int   m_int64;              // +0x64
    //  +0x68 / +0x7C - two embedded CAgentTimer objects (agenttimer.h).
    //  0x6F477E20 in ~CUnit's own unwind funclets is ~CAgentTimer and
    //  0x6F477D30 in slot 13 is CAgentTimer::Cancel, which is what types
    //  them; 0x14 apart is CAgentTimer's own size.
    CAgentTimer    m_timer68;            // +0x68
    CAgentTimer    m_timer7C;            // +0x7C
    //  +0x90 - a CFloat, read by SUnitMovement::GetFootprintSizeClass
    //  (0x6F474A60, unit_movementflags.cpp) and handed straight to
    //  ClassifySlopeAxis - the same "radius" role CPathClient::m_radius
    //  plays for its own owner.  Nothing in this call tree writes it.
    CFloat         m_radius90;           // +0x90
    char           m_reserved94[0x04];   // +0x94
    //  +0x98 - a TRefCnt-derived sub-object: ~CUnit stamps
    //  ??_7TRefCnt@@6B@ over its vtable word before releasing it, which is
    //  the inlined-base-destructor shape, and slot 8 dispatches through
    //  its own vtable+0x18.
    SUnitTrackedRef m_life;             // +0x98 - slots 73-81 all reach this one
    void*          m_pRefA8;             // +0xA8 - refcounted, released by ~CUnit
    char           m_fltAC[0x08];        // +0xAC - a FloatMini ([FltMini])
    void*          m_pRefB4;             // +0xB4 - refcounted
    SUnitTrackedRef m_mana;             // +0xB8
    char           m_fltC8[0x08];        // +0xC8 - FloatMini
    char           m_fltD0[0x08];        // +0xD0 - FloatMini
    void*          m_pRefD8;             // +0xD8 - refcounted
    char           m_fltDC[0x08];        // +0xDC - FloatMini
    unsigned int   m_intE4;              // +0xE4
    unsigned int   m_intE8;              // +0xE8
    unsigned int   m_intEC;              // +0xEC
    char           m_reservedF0[0x0C];   // +0xF0 - nothing reaches it yet
    SUnitTrackedRef m_refFC;             // +0xFC
    unsigned int   m_int10C;             // +0x10C
    void*          m_pRef110;            // +0x110 - refcounted
    unsigned int   m_int114;             // +0x114
    SUnitTrackedRef m_ref118;            // +0x118
    SOwnedAgentRef m_ref128;             // +0x128
    SOwnedQueueRef m_ref12C;             // +0x12C
    SRefMiniValue  m_ref130;             // +0x130 - handle pair; slot 13
                                         //   resolves and releases it
    unsigned int   m_int138;             // +0x138
    SRefMiniValue  m_ref13C;             // +0x13C - handle pair, slot 13's second
    char           m_reserved144[0x0C];  // +0x144
    unsigned int   m_int150;             // +0x150
    unsigned int   m_int154;             // +0x154
    unsigned int   m_int158;             // +0x158
    unsigned int   m_int15C;             // +0x15C
    unsigned int   m_int160;             // +0x160
    SUnitMovement  m_movement;           // +0x164 - TRefCnt-derived, and the
                                         //   target of every `add ecx, 164h`
    SRefMiniValue  m_ref174;             // +0x174 - handle pair
    unsigned int   m_int17C;             // +0x17C
    CAgentTimer    m_timer180;           // +0x180 - a third CAgentTimer
    unsigned int   m_int194;             // +0x194
    unsigned int   m_int198;             // +0x198
    SRefMiniValue  m_ref19C;             // +0x19C - handle pair
    unsigned int   m_int1A4;             // +0x1A4
    SRefMiniValue  m_ref1A8;             // +0x1A8 - handle pair
    unsigned int   m_int1B0;             // +0x1B0
    //  +0x1B4 - how long the order queue is.  StartOrderNow and AppendOrder
    //  each add one and refuse past kOrderQueueLimit (500),
    //  NotifyOrderTarget takes one off as it pops the head, and both
    //  CancelCurrentOrder and PrepareForNewOrder reset it to zero.  Not the
    //  same counter as m_int198, which is what SubmitOrder branches on.
    int            m_orderQueueLength;   // +0x1B4
    unsigned int   m_int1B8;             // +0x1B8
    unsigned int   m_int1BC;             // +0x1BC
    unsigned int   m_int1C0;             // +0x1C0
    unsigned int   m_int1C4;             // +0x1C4
    unsigned int   m_int1C8;             // +0x1C8
    unsigned int   m_int1CC;             // +0x1CC
    unsigned int   m_int1D0;             // +0x1D0
    unsigned int   m_int1D4;             // +0x1D4
    unsigned int   m_int1D8;             // +0x1D8
    SRefMiniValue  m_ref1DC;             // +0x1DC - handle pair
    char           m_reserved1E4[0x18];  // +0x1E4
    unsigned int   m_int1FC;             // +0x1FC
    unsigned int   m_int200;             // +0x200
    char           m_fltB204[0x0C];      // +0x204 - a FloatMiniB ([FltMiniB]),
                                         //   declared as its first three words
                                         //   so that the fourth can be named
                                         //   separately - see m_bound210
    //  +0x210 IS the FloatMiniB's own m_max.  It is named here as well
    //  because slot 9 (unit_fieldgroups.cpp) reads it back through `this`
    //  (`mov eax, [edi+210h]`, addressed off the object, not off the
    //  sub-object's own `lea ecx, [edi+204h]`) and republishes it, and that
    //  is the only spelling under which this compiler keeps the store:
    //  read through the sub-object and MSVC proves `m_max = m_max` and
    //  drops it.  Same address either way.
    CFloat         m_bound210;           // +0x210 - the FloatMiniB's m_max
    SUnitTrackedRef m_ref214;            // +0x214
    SOwnedAgentRef m_ref224;             // +0x224
    char           m_reserved228[0x18];  // +0x228
    unsigned int   m_int240;             // +0x240
    unsigned int   m_int244;             // +0x244
    unsigned int   m_int248;             // +0x248
    unsigned int   m_int24C;             // +0x24C
    SRefMiniValue  m_ref250;             // +0x250 - handle pair
    unsigned int   m_int258;             // +0x258
    unsigned int   m_notifyHandle25C;    // +0x25C - slots 92/93 read and write it
    unsigned int   m_handle260;          // +0x260 - slot 106 releases it
    char           m_reserved264[0x8];   // +0x264
    unsigned int   m_notifyGate26C;      // +0x26C - gates the NotifyStormSingleton
                                         //   call in SetAttachment274Visible below
    unsigned int   m_pendingVisible270;  // +0x270 - the flag SetAttachment274Visible
                                         //   caches and republishes
    unsigned int   m_handle274;          // +0x274 - slot 106 releases it
    unsigned int   m_handle278;          // +0x278 - slot 97 releases it and puts
                                         //   the g_unk6F932954 "none" sentinel back
    unsigned int   m_int27C;             // +0x27C
    unsigned int   m_flags280;           // +0x280 - slot 56 hands bit 1 out
    //  +0x284..+0x28C - the unit's own position, three hardware floats.
    //  Slot 56 copies all three into the caller's FVec3 in one run.
    float          m_posX284;            // +0x284
    float          m_posY288;            // +0x288
    float          m_posZ28C;            // +0x28C
    char           m_reserved290[0x28];  // +0x290
    //  +0x2B8..+0x2C0 - three hardware floats ClearMotionState (0x6F27A5A0)
    //  zeroes as one x87 store triple.
    float          m_motion2B8;          // +0x2B8
    float          m_motion2BC;          // +0x2BC
    float          m_motion2C0;          // +0x2C0
    void*          m_pRef2C4;            // +0x2C4 - refcounted, the first thing
                                         //   ~CUnit releases
    unsigned int   m_int2C8;             // +0x2C8 - slot 97 clears it
    float          m_float2CC;           // +0x2CC - slot 47 returns it in st0
    unsigned int   m_int2D0;             // +0x2D0
    unsigned int   m_int2D4;             // +0x2D4 - slot 67 hands it out
    unsigned int   m_animMode2D8;        // +0x2D8 - slots 35/36 pass it to
                                         //   PlaySpriteAnimation as the mode
    unsigned int   m_int2DC;             // +0x2DC
    unsigned int   m_animName2E0;        // +0x2E0 - slots 35/36 store the
                                         //   requested animation id here and
                                         //   pass its *address* on
    char           m_reserved2E4[0x20];  // +0x2E4 - +0x300 (one dword within
                                         //   it) is 1.0f out of CUnit::CUnit()
    SRefMiniValue  m_ref304;             // +0x304 - handle pair
    //  +0x30C - one dword past where DumpState's field walk and ~CUnit's
    //  own unwind funclet table both stop (docs/targets/CUnit__vtable.md,
    //  "Save... one dword at +0x30C... real evidence of one more field
    //  neither of those two sources happened to reach").  CUnit::CUnit()
    //  is the first thing in this call tree to actually write it (zero);
    //  true object size is 0x310, not 0x30C.
    unsigned int   m_int30C;             // +0x30C
};

#endif

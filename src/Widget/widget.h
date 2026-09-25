//============================================================================
//  The vtable[54] method shared by CWidget/CSelectable/CDestructable/CUnit/
//  CItem/CCaptainAI (0x6F2AC220) - see asm/sub_6F2AC220_.../## User notes.
//  Only the vtable slots and fields this call tree actually reaches are
//  named; the rest of the six-class hierarchy is not recovered here.
//============================================================================
#ifndef WIDGET_H
#define WIDGET_H

#include "game.h"
#include "footprinttype.h"
#include "pathfind.h"
#include "agent.h"
#include "agentwar3.h"
#include "war3image.h"
#include "dataallocator.h"
#include "footprinttype.h"
#include "sprite.h"
#include "cmatrix3.h"
#include "fvec3.h"

//  Only two vtable slots (0xB8 = index 0x2E, "get the handle-bearing
//  sub-object"; 0xDC = index 0x37, "forward a full move") and one data
//  member (+0x30, the footprint type id) are read by this call tree; they
//  are reached through raw offsets in widget.cpp rather than a formal class
//  with 0x2D invented virtuals in front of them.
//
//  CWidget derives from CAgent (agent.h), established by the
//  CWidget__Constructor dump (0x6F2ACAE0): its root function places a
//  freshly allocated block's `this` through CAgent::CAgent() (a real,
//  out-of-line call to sub_6F001730) before overwriting the vtable pointer
//  with CWidget's own - the base-then-derived constructor shape - and
//  CAgent's own size (0x20 bytes: TRefCnt+CObserver+CAgent's own three
//  fields, see agent.h) is exactly what the dump zeroes starting at +0x20,
//  so CWidget's own fields pick up right where CAgent's leave off with no
//  gap.  The vtable stored there is CWidget's own real one
//  (`??_7CWidget@@6B@`, 0x6F92E7BC in user_knowledge.json), confirming
//  this call tree constructs a full CWidget, not merely a CAgent.
//
//  A later target (CItem__Constructor, 0x6F2B7520) showed this is one
//  layer too shallow: there is a trivial `CAgentWar3` between CAgent and
//  CWidget (agentwar3.h) that CWidget__Constructor's own dump could never
//  reveal, because nothing in that call tree could throw between
//  CAgent::CAgent() returning and CWidget's own vtable store, so /O2 fully
//  inlined CAgentWar3's empty default constructor away with no trace.
//  CWidget now derives from CAgentWar3 instead of CAgent directly - a
//  layout-neutral, purely-documentation-level correction (CAgentWar3 adds
//  no fields) that costs this target's own already-matched instructions
//  nothing; verify.py confirms the report is unchanged.

//  The object CWidget::NegateAndForwardOffset (slot 0x120) reads its float
//  out of.  Only the one field it touches is named.
struct SWidgetOffsetSource
{
    char  m_reserved00[0x10];
    float m_value;      // +0x10
};

//----------------------------------------------------------------------------
//  The block of creation parameters CWidget::InitSprite (0x6F2ACDE0) is
//  handed - a unit/doodad "art descriptor", read but never written.
//
//  Its first four CFloats are read two different ways by two different
//  consumers, which is only sound because CFloat's storage already is an
//  IEEE single: CWidget::MoveTo takes the position and facing as CFloat*,
//  while CWidget::CreateSpriteFromDescriptor `fld`s the same bytes as
//  hardware floats to build the sprite's world transform.
//
//  Offsets past +0x2C are named by what InitSprite does with them; the ones
//  it only forwards through a virtual setter keep a positional name, since
//  the setters that would say what they mean are all `nullsub` on CWidget
//  and CItem and only CUnit implements them (user_knowledge.json's vtable
//  dumps, slots 48-51, 73, 80, 81).
//----------------------------------------------------------------------------
struct SWidgetArtDescriptor
{
    CFloat        m_x;              // +0x00
    CFloat        m_y;              // +0x04
    CFloat        m_z;              // +0x08
    CFloat        m_facing;         // +0x0C
    float         m_animationScale; // +0x10
    int           m_teamColor;      // +0x14  -1 = leave the sprite's own
    int           m_wantsFullDetail;// +0x18  the flag GetModelDetailFlags reads
    float         m_timeScale;      // +0x1C  skipped when equal to (float)g_animationSpeedSentinel
    int           m_detailLevel;    // +0x20  picks CSpriteUber over CSpriteMini
    unsigned int  m_widgetFlags;    // +0x24  copied wholesale into CWidget::m_flags
    unsigned int  m_owningPlayer;   // +0x28
    char          m_modelHandle[0x0C]; // +0x2C  an SHandleHolder (handleobject.cpp)
    int           m_setting38;      // +0x38  -> vtable[0x124]
    int           m_setting3C;      // +0x3C  -> vtable[0x144]
    int           m_setting40;      // +0x40  -> vtable[0xC0]
    int           m_setting44;      // +0x44  -> vtable[0xC4]
    unsigned char m_setting48;      // +0x48  -> vtable[0xC8], by value and byte-wide
    char          m_reserved49[3];  // +0x49
    int           m_setting4C;      // +0x4C  -> vtable[0xCC], by value
    unsigned int  m_field50;        // +0x50  -> CWidget::m_reserved40
    int           m_flag54;         // +0x54  -> CWidget::m_flags bit 0x10
};

//----------------------------------------------------------------------------
//  0x6F2AC220 - move/reposition the object: clamp the requested position to
//  the map, forward it through the handle-bearing sub-object's own move
//  method, look up the object's footprint (if any) and fold its facing
//  quadrant into the move, then re-clamp and forward the final position to
//  the time-sync bookkeeping.  `this` in ecx; 11 stack arguments as recorded
//  in the dump's ## User notes ground truth.
//----------------------------------------------------------------------------
//  Forward declarations for the slot signatures below - the vtable
//  block declares slots whose parameter types are defined further
//  down the include graph, and a pointer only needs the name.
class CDataStore;
class CDataStoreScratch;
class FVec3;
struct SCellBoxF;
struct SHandleWithType;

class CWidget : public CWar3Image
{
public:
    //  0x6F2AB460 - not a vtable slot: CUnit::SetLife
    //  (unit_setvalue.cpp) calls it directly after publishing a new value,
    //  and it is in CWidget's own 0x6F2ABxxx neighbourhood.  `retn 0`.
    void NotifyLifeChanged();
    //  0x6F2AB1E0 - not a vtable slot: CUnit::Deactivate
    //  (unit_deactivate.cpp) calls it directly, and it lives in CWidget's
    //  own 0x6F2ABxxx neighbourhood.  See widget_postagentevent.cpp.
    void PostAgentEvent(void* target, int direct);
    //  0x6F26EF10 - same shape as PostAgentEvent above, event id 0xD01A2
    //  rather than 0xD01A1.  cunit_agent2_worklist.md.
    //  See widget_postagentevent2.cpp.
    void PostAgentEvent2(void* target, int direct);

    //====================================================================
    //  CWidget's vtable contribution.  Declaration order *is* vtable order
    //  (C++'s own rule), so this block is sorted by slot index and has
    //  to stay that way - inserting one line in the wrong place renames
    //  every slot after it.  Slots 1-95; see
    //  docs/targets/CWidget_CSelectable_CItem__vtables.md.
    //====================================================================
    //  agentwar3_dtor.cpp, which is where it has to live to get
    //  ~CAgentWar3 inlined into it the way the shipped code does.
    //  slot 1 / +0x04 (0x6F267180)
    virtual CWidget* DeleteSelf(int flags);
    //  one *is* virtual (agent.h declares it).
    //  slot 7 / +0x1C (0x6F2AB0C0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 8 / +0x20 (0x6F2AB240)
    virtual int Method_0x20(int context);
    //  slot 9 / +0x24 (0x6F2AC330)
    virtual int Method_0x24(int context);
    //  destroying it: drop its pathing footprint, release whatever handle
    //  +0x38 names, and let go of the sprite.  CSelectable and CItem both
    //  extend it (0x6F2C7410 / 0x6F2B5C30).  See widget_deactivate.cpp.
    //  slot 13 / +0x34 (0x6F2ABEA0)
    virtual void Deactivate();
    //  save stream.  See widget_save.cpp.
    //  slot 14 / +0x38 (0x6F2AC790)
    virtual void Save(class CDataStoreScratch* store);
    //  save stream, the exact counterpart of Save above.  See
    //  widget_load.cpp.
    //  slot 15 / +0x3C (0x6F2AC810)
    virtual void Load(class CDataStore* store);
    //  slot 16 / +0x40 (0x6F2AB980)
    virtual void RefreshOwningPlayerColor();
    //  words (m_flags at +0x20 and m_footprintType at +0x30) to a text
    //  sink.  See widget_dump.cpp.
    //  slot 17 / +0x44 (0x6F2AC770)
    virtual void DumpState(void* sink);
    //  slot 22 / +0x58 (0x6F2AB070)
    virtual const char* GetClassName();
    //  bit; with a null first argument it also pokes slot 61.  See
    //  widget_spriteflag.cpp.
    //  slot 37 / +0x94 (0x6F2AB6D0)
    virtual void SetSpriteVisible(int force, int visible);
    //  slot 38 / +0x98 (0x6F2AD7E0)
    virtual void NotifyWidgetVisibilityChanged();
    //  0x6F2ACDE0 - vtable[0x27] (index 39), shared unoverridden by
    //  CWidget, CSelectable, CDestructable, CUnit, CItem and CCaptainAI.
    //  Place the widget, give it a sprite, and push the descriptor's
    //  settings through this object's own virtual setters.  See
    //  widgetinitsprite.cpp.
    //  slot 39 / +0x9C (0x6F2ACDE0)
    virtual void InitSprite(unsigned int footprintType, const SWidgetArtDescriptor* desc, int forward, int a5);
    //  slot 40 / +0xA0 (0x6F267120)
    virtual void SetFlagBit2(int on);
    //  slot 41 / +0xA4 (0x6F267140)
    virtual void SetFlagBit4(int on);
    //  slot 42 / +0xA8 (0x6F077E20)
    virtual void SetFlagBit8(int on);
    //  the default MoveForward (widget.cpp) falls back on when its caller
    //  supplies no override.  Always 1 here.
    //  slot 43 / +0xAC (0x6F2669C0)
    virtual int GetDefaultCellCentres();
    //  three trees.
    //  slot 44 / +0xB0 (0x6F2669D0)
    virtual int Method_0xB0();
    //  own slot 65 (+0x104).
    //  slot 45 / +0xB4 (0x6F2AB780)
    virtual int Method_0xB4();
    //  mover in this family reaches through (widget.cpp's
    //  GetHandleObjectFn).  A plain CWidget has none; CItem returns
    //  &m_position (item.cpp).
    //  slot 46 / +0xB8 (0x6F2669E0)
    virtual SHandleWithType* GetHandleObject();
    //  g_flt6F931ED0, returned in st0.
    //  slot 47 / +0xBC (0x6F2AB9A0)
    virtual float Method_0xBC();
    //  Slots 48-51: four consecutive one-argument slots, empty in CWidget
    //  and in every class down to CItem, but `retn 4` in all four shipped
    //  bodies - one stack dword each, not none.  CUnit is the one class in
    //  the image that overrides them (0x6F28B0F0/0x6F28B100/0x6F28B110/
    //  0x6F28B120), and all four of its overrides forward their single
    //  argument to a handle-resolved sub-object at CUnit+0x164, which is
    //  what fixes the argument count independently of the `retn`.
    //  slot 48 / +0xC0 (0x6F2669F0, nullsub_1192)
    virtual void Method_0xC0(int a);
    //  slot 49 / +0xC4 (0x6F266A00, nullsub_1193)
    virtual void Method_0xC4(int a);
    //  slot 50 / +0xC8 (0x6F266A10, nullsub_1194)
    virtual void Method_0xC8(int a);
    //  slot 51 / +0xCC (0x6F266A20, nullsub_1195)
    virtual void Method_0xCC(int a);
    //  0x6F2AE850 - widgetpathref.cpp.  A thiscall member: `add` is pushed
    //  and cleaned (`retn 4`).  Spelled __fastcall it cleans nothing, which
    //  tools/abi_audit.py reports as FATAL - a `ret` where the shipped code
    //  has `ret 4` unbalances the caller's stack on the very first call.
    //  slot 52 / +0xD0 (0x6F2AE850)
    virtual void AdjustWidgetPathRefs(int add);
    //  slot 53 / +0xD4 (0x6F2ACF90).  `int`, not `void`: CUnit's own
    //  override of this same slot (unit_widgetplacement.cpp) computes a
    //  real bucket index on one of its two paths and returns it, which
    //  C++ cannot express if the base declares the slot void - the same
    //  constraint slot 63's own note above already documents.  This body
    //  itself never sets a return value (it just falls off the end after
    //  the ProbeWidgetTerrainCell call), so the declared type costs it
    //  nothing.
    virtual int RefreshWidgetPlacement();
    //  slot 54 / +0xD8 (0x6F2AC220)
    virtual void MoveTo(CFloat* posX, CFloat* posY, int forward, int a4, int a5, int a6, int a7, int noDelta, const CFloat* facing, int a10, int a11);
    //  0x6F2ABFB0 - CWidget__MoveForward: the vtable[0xDC] "forward a full
    //  move" implementation this dump was named after and pinned to that
    //  CSelectable, CDestructable, CItem and CCaptainAI (none of the five
    //  override it).  MoveTo() above calls *through* this slot when its own
    //  `forward` flag is set; this function does not call MoveTo back - the
    //  two are siblings, not caller/callee, despite the coincidental "one
    //  TU, close addresses, same 0x2C stack-arg total" shape.  See
    //  widget.cpp for the five extra vtable slots (+0xAC/+0xB8/+0xD0/+0x158/
    //  +0x168) this one reaches that MoveTo does not.
    //
    //  Clamp the point, optionally bracket the whole call in a vtable[0xD0]
    //  lock/unlock pair, look up (or take) a terrain type, build a search
    //  rectangle, run either the bounded ring search or the unbounded
    //  jitter search depending on `useBoundedSearch`, then on success fold
    //  the footprint's facing quadrant into the final position exactly like
    //  MoveTo does.  Returns the search's own success flag (0/1).
    //  slot 55 / +0xDC (0x6F2ABFB0)
    virtual int MoveForward(CFloat* posX, CFloat* posY, int useBoundedSearch, int terrainType, int lockFlag, PathFilterFn filter, void* context, const SCellBoxF* bounds, int cellCentresOverride, int unitType, int player);
    //  position, with the ground height filled in when asked.  See
    //  widget_slot56.cpp.
    //  slot 56 / +0xE0 (0x6F267F00)
    virtual FVec3* GetPosition(FVec3* out, int wantGround, int a3, int a4);
    //  0x6F278190 - widgetoffsetposition.cpp.  Not a slot: a plain member
    //  that dispatches slot 56 above through `this` and then folds the
    //  module's fixed world offset into the answer when m_flags bit 0x1000
    //  is set.  CGameUI's screen projection calls it on a unit where it
    //  calls the slot directly on anything else.
    FVec3* GetOffsetPosition(FVec3* out, int wantGround, int a3, int a4);

    //  slot 57 / +0xE4 (0x6F2AB680)
    virtual float Method_0xE4(FVec3* pos, int a1, int a2, int a3);
    //  for a plain widget.  CItem::Method_0xE4 (item.cpp) adds this slot's
    //  answer to the terrain height it queries.
    //  slot 58 / +0xE8 (0x6F266A30)
    virtual float Method_0xE8();
    //  0x6F266A40 - widgetowningplayer.cpp.  Slot 0xEC.  Always -1 on a
    //  widget or an item; only CUnit overrides it.
    //  slot 59 / +0xEC (0x6F266A40)
    virtual int GetOwningPlayerIndex();
    //  IsVisibilityMaskLive (widgetvisibilitygate.cpp) calls with a literal
    //  1.  Always 0 on a plain widget.
    //  slot 60 / +0xF0 (0x6F266A50)
    virtual int QueryVisible(int which);
    //  sprite should be showing and push the answer at it.  See
    //  widget_slot61.cpp.
    //  slot 61 / +0xF4 (0x6F2AD770)
    virtual void RefreshSpriteVisibility();
    //  0x6F2AC6F0 - widgetvisibilitygate.cpp.  Slot 0xF8.  Two stack
    //  arguments, neither read (`retn 8`).
    //  slot 62 / +0xF8 (0x6F2AC6F0)
    virtual int IsVisibilityMaskLive(unsigned int, unsigned int);
    //  0x6F2AD680 - widgetregisterquery.cpp.  Slot 0xFC.
    //  slot 63 / +0xFC (0x6F2AD680)
    //  `int`, not `void`: CWidget's own body leaves whatever
    //  SPlayerTable::SubmitWidget returned in eax, and CUnit's override
    //  (unit_playertable.cpp) computes a real 0/1 answer and returns it -
    //  which C++ cannot express if the base declares the slot void.
    virtual int SubmitToPlayerTable(unsigned int playerIndex, int flags, int extra);
    //  the world-object singleton instead of to a caller's buffer.  See
    //  widget_slot64.cpp.
    //  slot 64 / +0x100 (0x6F2AD710)
    virtual void PublishPosition(int a1, int a2);
    //  under this widget.  Slot 45 (+0xB4) and CWidget::Method_0x40 both
    //  tail-jump into it through the vtable, and both are spelled as
    //  returning int because the shipped code forwards whatever is in eax;
    //  the body itself sets no return value.  See widget_slot65.cpp.
    //  slot 65 / +0x104 (0x6F2AD860)
    virtual void RefreshTerrainTint();
    //  the footprint type's own "scaleFactor" property times the
    //  SelectionCircle/ScaleFactor config value, doubled.  CItem overrides
    //  it with the same thing undoubled (0x6F2B6100, item_vtable.cpp).
    //  slot 66 / +0x108 (0x6F2AC920)
    virtual float Method_0x108();
    //  slot 67 / +0x10C (0x6F267160)
    //  `int*`, not void: both bodies leave the caller's own out pointer in
    //  eax and CUnit's (unit_thunks.cpp) only reproduces the shipped
    //  register assignment if the return type says so.  CWidget's own body
    //  already has it there, so saying it costs nothing.
    virtual int* Method_0x10C(int* out);
    //  a freshly made widget-side agent, clear `mask` from m_playerMaskA
    //  and notify the UI.  Two stack arguments (`retn 8`); the second is
    //  never read.  See widget_slot68.cpp.
    //  slot 68 / +0x110 (0x6F2AF6A0)
    virtual void ClearPlayerMaskBits(unsigned int mask, int unused);
    //  slot 69 / +0x114 (0x6F266A60)
    virtual int Method_0x114();
    //  0x6F266B60 / 0x6F266B70 / 0x6F266B80 / 0x6F266B90 / 0x6F266BA0) -
    //  five CFloat getters that all hand back encoded zero through a
    //  caller-supplied out-buffer and return that buffer.  Slot 74 takes
    //  one more (unread) stack argument than the other four.
    //  slot 70 / +0x118 (0x6F266B60)
    virtual CFloat* Method_0x118(CFloat* out);
    //  slot 71 / +0x11C (0x6F266A70)
    virtual int Method_0x11C();
    //  0x6F2AE8D0 - widgetnegateforward.cpp.  Slot 0x120.
    //  slot 72 / +0x120 (0x6F2AE8D0)
    virtual void* NegateAndForwardOffset(void* target, struct SWidgetOffsetSource* src, int, int);
    //  slot 73 / +0x124 (0x6F266A80, nullsub_1196)
    virtual void SetLife(const CFloat* value);
    //  slot 74 / +0x128 (0x6F266B70)
    virtual CFloat AddLife(const CFloat* delta);
    //  slot 75 / +0x12C (0x6F266B80)
    virtual CFloat* GetLife(CFloat* out);
    //  slot 76 / +0x130 (0x6F266B90)
    virtual CFloat* GetMaxLife(CFloat* out);
    //  slot 77 / +0x134 (0x6F266BA0)
    virtual CFloat* GetMinLife(CFloat* out);
    //  (+0x130) and hand the same buffer back.
    //  slot 78 / +0x138 (0x6F266A90)
    virtual CFloat* Method_0x138(CFloat* out);
    //  and report whether it is strictly greater than zero.
    //  slot 79 / +0x13C (0x6F266BB0)
    virtual int Method_0x13C();
    //  slot 80 / +0x140 (0x6F266AB0, nullsub_1197)
    virtual void SetMinLife(const CFloat* value);
    //  slot 81 / +0x144 (0x6F266AC0, nullsub_1198)
    virtual void SetMaxLife(const CFloat* value);
    //  0x6F2AD0C0 - slot 0x148 (index 82), RemoveFootprint's counterpart:
    //  build this widget's pathing-grid registration and put its footprint
    //  on the grid.  See widget_addfootprint.cpp.
    //  slot 82 / +0x148 (0x6F2AD0C0)
    virtual void AddFootprint(int blocking, int visible, int useAlternate);
    //  0x6F2AD300 - widgetfootprintremove.cpp.  Slot 0x14C, the counterpart
    //  of slot 0x148: free the pathing registration this widget holds.
    //  slot 83 / +0x14C (0x6F2AD300)
    virtual void RemoveFootprint(int blocking, int useAlternate);
    //  slot 84 / +0x150 (0x6F2AD400)
    virtual void ApplyWidgetFootprint();
    //  slot 85 / +0x154 (0x6F2AE690)
    virtual void UpdateWidgetFootprintRegion();
    //  (widget.cpp's GetPlacementMaskFn).  Zero on a plain widget.
    //  slot 86 / +0x158 (0x6F266AD0)
    virtual int GetPlacementMask();
    //  sub-object (slot 46) and fold the argument into the agile registry
    //  record's own low-24-bit word (SHandleWithType::SetLow24Bits,
    //  widget_handlebits.cpp).  The second argument is never read.
    //  slot 87 / +0x15C (0x6F266AE0)
    virtual void Method_0x15C(unsigned int bits, int);
    //  small integers, 10 and 2.
    //  slot 88 / +0x160 (0x6F266B00)
    virtual int Method_0x160();
    //  slot 89 / +0x164 (0x6F266B10)
    virtual int Method_0x164();
    //  (widget.cpp's GetPlacementRadiusFn): encoded 1.0 through the
    //  caller's out-buffer.
    //  slot 90 / +0x168 (0x6F266BF0)
    virtual CFloat* GetPlacementRadius(CFloat* out);
    //  slot 91 / +0x16C (0x6F266B20, nullsub_1199)
    virtual void RefreshPlacementRadius();
    //  slot 92 / +0x170 (0x6F266B30)
    virtual int GetPositionNotifyHandle();
    //  slot 93 / +0x174 (0x6F266B40, nullsub_1200)
    virtual void SetPositionNotifyHandle(int value);
    //  through the out-pointer, and hand the pointer itself back in eax -
    //  which is what pins the return type down: written `void`, MSVC picks
    //  the other register for the out-pointer and the whole three-store
    //  body comes out permuted.  The only reader of m_reserved40 anywhere,
    //  and the reason CSelectable's old "+0x40 is a real 4-byte gap" note
    //  had to go.
    //  slot 94 / +0x178 (0x6F266C10)
    virtual unsigned int* Method_0x178(unsigned int* out);
    //  slot 95 / +0x17C (0x6F266B50)
    virtual unsigned int Method_0x17C();

    //  0x6F2ACAE0's inlined tail: everything after the `call sub_6F001730`
    //  (CAgent::CAgent(), already out-of-line by construction since it
    //  lives in a different translation unit - agent_ctor.cpp) is CWidget's
    //  own field init, inlined directly into the root function rather than
    //  reached through a separate call - there is no second address in the
    //  dump for it.  Writing it as a genuine constructor and relying on
    //  /O2 to inline a single-call-site, same-TU constructor (see
    //  CWidget__Construct, widget.cpp) reproduces that shape without
    //  hand-inlining the source itself.
    //  Defined here rather than in widget.cpp because it now has *two*
    //  inlining call sites in two different translation units:
    //  CDataAllocator::ConstructCWidget (widget.cpp, the 0x6F2ACAE0 root)
    //  and CSelectable::CSelectable (selectable.cpp, 0x6F266C30), both of
    //  which have these stores inlined in the shipped code rather than a
    //  call.  Out of line in widget.cpp it could only ever be inlined into
    //  the first of the two.
    CWidget()
        : m_playerMaskA(0),
          m_playerMaskB(0),
          m_footprintType(0),
          m_pathGridRegistration(0),
          m_reserved38((unsigned int)-1),
          m_reserved3C(0)
    {
        //  m_flags (+0x20) is CAgentWar3's and +0x24/+0x28 are
        //  CWar3Image's (war3image.h), so all three zeroes come out of the
        //  base constructors, which run before this list and put the
        //  stores exactly where both shipped constructors have them -
        //  immediately after `call sub_6F001730` and before the derived
        //  vtable stamp.  m_reserved40 (+0x40) is deliberately absent:
        //  neither shipped constructor touches it.
    }










    //  0x6F2AC340 - widgetfootprintshape.cpp.  A thiscall member: the flag
    //  is pushed and cleaned (`retn 4`), never passed in edx.
    CFootprintShape* GetOwnFootprintShape(int useAlternate);





    //  The vptr lives at +0 (now CAgent's, per the base class above) and
    //  the two interesting slots MoveTo/MoveForward reach are still picked
    //  up by raw offset rather than a formal virtual, so nothing new is
    //  declared virtual here either.
    //
    //====================================================================
    //  The rest of CWidget's own vtable slots, from this session's full
    //  sweep of all three vtables - see docs/targets/
    //  CWidget_CSelectable_CItem__vtables.md.  Bodies in
    //  widget_vtable_leaves.cpp unless a comment says otherwise.
    //
    //  A name that says what the slot does is used wherever the body or an
    //  existing call site in this repo establishes it; the rest keep a
    //  positional `Method_0xNN` (the byte offset, i.e. index*4), which is
    //  the convention item.h already uses.  None of them is declared
    //  `virtual` - see the note above the field block below.
    //====================================================================




    //  0x6F2AB8C0 / 0x6F26B7D0 - the two out-of-line halves Deactivate's
    //  tail is made of, each its own address in the shipped code.
    void ReleaseHandle38();














    //  0x6F2AB310 - not a vtable slot: build a CEventTargetLost record and
    //  push it through slot 4.  See widget_targetlost.cpp.
    void NotifyTargetLost(int a, int b);



    //  0x6F2AD5D0 - "does the local player get to see this widget".  See
    //  widget_visibleflags.cpp.
    int IsVisibleToLocalPlayer();


    //  0x6F00C9F0 - the shared body that slot does its work through, and
    //  the only place in these three vtables that reads a widget's world
    //  position back out of its handle-bearing sub-object.
    //  widget_worldpos.cpp.
    FVec3* GetWorldPositionWithGround(FVec3* out, int wantGround, int a3,
                                       int a4);
























    //  +0x00-+0x1F: CAgent, +0x20: CAgentWar3::m_flags (base classes
    //  above).  m_flags used to be declared here, and CSelectable declared
    //  its own m_reserved20 at the same offset; the two turned out to be
    //  one inherited field - see agentwar3.h for the vtable slots that
    //  prove it.  Every offset below is unchanged, and so is every
    //  instruction the constructor emits for it.

    //  +0x24 and +0x28 are CWar3Image's (war3image.h) - the constructor
    //  zeroes them before the derived vtable stamp, which is where base
    //  constructors put their stores.
    //  Two 16-bit player bitmasks, one bit per player, zeroed by the
    //  constructor and first read back by NotifyWidgetVisibilityChanged
    //  (0x6F2AD7E0, widgetvisibility.cpp), which ANDs them against the
    //  player-slot table's own "which players exist" mask.  Which is "can
    //  see" and which is "has seen" that call tree does not say - it only
    //  reads them - so they keep positional names.
    unsigned short m_playerMaskA;    // +0x2C
    unsigned short m_playerMaskB;    // +0x2E
    unsigned int  m_footprintType;   // +0x30  a bare FourCC, zeroed by the constructor, overwritten by InitSprite
    //  +0x34 - zeroed by the constructor.  The widget's own pathing-grid
    //  registration: AdjustWidgetPathRefs walks it as an array of pinned
    //  path data (widgetpathref.cpp), and all three footprint slots gate on
    //  it being non-null and hand it to the per-cell callback as context.
    void*         m_pathGridRegistration;   // +0x34
    unsigned int  m_reserved38;      // +0x38 - set to -1 by the constructor (an "empty" sentinel, matching the -1 pattern CAgent's own +0x0C/+0x10 already use), and back to -1 by InitSprite
    unsigned int  m_reserved3C;      // +0x3C - zeroed by the constructor, and again by InitSprite
    unsigned int  m_reserved40;      // +0x40 - past what the constructor touches; InitSprite copies the descriptor's own +0x50 into it
};

//  0x6F2ACAE0 (root) - see dataallocator.h's CDataAllocator::ConstructCWidget
//  (defined in widget.cpp) and widget.cpp for the body.

#endif

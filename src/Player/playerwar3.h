//============================================================================
//  CPlayerWar3 : public CAgentWar3 - the 33-slot vtable at 0x6F94E9D4
//  (RTTI `??_7CPlayerWar3@@6B@`).  CAgentWar3 contributes no fields of its
//  own beyond m_flags at +0x20 (agentwar3.h) and ends at +0x24, so every
//  CPlayerWar3-owned field starts there.
//
//  THIS FILE IS A PARTIAL RECONSTRUCTION.  See docs/targets/CPlayerWar3.md
//  for the full slot table and an honest account of what has and has not
//  landed yet.  Only the overrides actually implemented below are declared
//  `virtual` here - a slot this session did not reach is simply not
//  mentioned, rather than declared and left without a definition (which
//  would either not link or force the class abstract).  Do **not** treat
//  the absence of a slot here as "inherited unchanged" without checking
//  docs/targets/vtables/CPlayerWar3.txt and the write-up first - several of
//  the missing ones are real NEW overrides nobody has transcribed yet.
//
//  No constructor or destructor is declared here yet either: neither has
//  been reconstructed this session (see the write-up), so nothing stamps
//  `??_7CPlayerWar3@@6B@` and this class is never actually instantiated by
//  this reconstruction - every function below is reachable and scorable on
//  its own as a plain member-function definition, per CLAUDE.md's own
//  "leaves first" workflow.
//============================================================================
#ifndef PLAYERWAR3_H
#define PLAYERWAR3_H

#include "agentwar3.h"
#include "textsink.h"
#include "floatmini.h"
#include "rcstring.h"
#include "agenttypedslots.h"
#include "agenttimer.h"
#include "storm.h"
#include "playerheroeval.h"

//  0x6F4221B0's own class - never modelled concretely (see
//  playerwar3_ctorsubobjects.cpp's header comment); kept opaque here purely
//  so CPlayerWar3's own +0x34 field can be typed as a pointer to it and
//  CPlayerWar3::Save (playerwar3_save.cpp) can call its own ::Save
//  (0x6F423FC0, selectionwar3_save.cpp) as a real member call.
class CSelectionWar3
{
public:
    void Save(class CDataStoreScratch* store);

    //  0x6F4207D0 - selectionwar3_save.cpp.  The one-line adapter
    //  CSelectionWar3::Save's own first call needs (its own list lives at
    //  `this+0xC` directly).
    class CDataStoreScratch* SaveMainList(class CDataStoreScratch* store);

    //  0x6F424130 - playerwar3_selectionwar3loadthunk.cpp.  A redirect, not
    //  a reconstruction - see that file's own header comment for why.
    void Load(class CDataStore* store);
};

//  0x6F401390's own class - same reasoning, +0x2D0.  ::Save is 0x6F3F08C0
//  (techtreesave.cpp).
class CTechTree
{
public:
    void Save(class CDataStoreScratch* store);

    //  0x6F403A30 - playerwar3_techtreeload.cpp.
    void Load(class CDataStore* store);
};

class CPlayerWar3 : public CAgentWar3
{
public:
    //  0x6F418A60 - see playerwar3_dtor.cpp.  Not virtual - same reasoning
    //  as CAgent::~CAgent()/CAgentWar3::~CAgentWar3() (agent.h/agentwar3.h):
    //  the vtable-dispatched teardown entry point is slot 1 (DeleteSelf,
    //  playerwar3_deleteself.cpp), which calls this explicitly.
    ~CPlayerWar3();

    //  0x6F4193E0 - see playerwar3_ctor.cpp.  Vtable-completion gate: only
    //  10 of 33 slots are real as of this commit, so ctor_vtable_audit.py
    //  lists this constructor RISKY (the same accepted-gap state
    //  CCameraWar3/CEscMenu/CSimpleTop's own constructors are already in) -
    //  see that file's own header comment for why no source-level
    //  workaround exists once a class has real virtuals declared.
    CPlayerWar3();

    //====================================================================
    //  CPlayerWar3's own vtable contribution, slots reconstructed so far.
    //====================================================================

    //  slot 1 / +0x04 (0x6F4196F0).  See playerwar3_deleteself.cpp.
    //  Covariant return, matching CAgent::DeleteSelf/TRefCnt::DeleteSelf's
    //  own chain (agent.h/refcnt.h).
    virtual CPlayerWar3* DeleteSelf(int flags);

    //  slot 3 / +0x0C (0x6F41F410).  See playerwar3_method0x0c.cpp.  An
    //  8-way switch on `msg->m_id` (0..7, not the usual wire-format id
    //  space - reused here as a small internal selector), six of whose
    //  arms build a `CPlayerStateChanged` (layout-compatible with
    //  `SAgentMessage`, the same non-inheriting overlay netcommand.h's own
    //  `CNetCommandBase` already establishes) and publish it through the
    //  base class's own inherited slot 4 (`Method_0x10`, observer.h - not
    //  overridden here).  Two arms (6/7) skip the publish and write
    //  through the ability-entry array's own handle-owner fields instead;
    //  see playerwar3_method0x0c.cpp for the full field map.
    virtual int Method_0x0C(const SAgentMessage* msg);
    //====================================================================
    //  Declaration order matters (it is vtable order) but this header does
    //  not yet carry every NEW slot - see the file header above.
    //====================================================================

    //  slot 7 / +0x1C (0x6F40AFD0).  '+','p','l','y' low byte first, same
    //  FourCC-reversal scheme CAgentWar3::GetAgileTypeId (agentwar3_slots.cpp)
    //  and its siblings use.
    virtual unsigned int GetAgileTypeId() const;

    //  slot 9 / +0x24 (0x6F40F400).  Seeds a counter from the base class's
    //  own default (CAgent::Method_0x24), then walks the 26-entry array at
    //  +0x40 calling each element's own vtable slot 3, incrementing the
    //  counter once per element.  See CPlayerWar3AbilityEntry below.
    virtual int Method_0x24(int context);

    //  slot 8 / +0x20 (0x6F40F320).  Seeds a counter from the base class's
    //  own default (CAgent::Method_0x20, always 0), then walks the first 25
    //  of the 26 AbilityEntry array elements, submitting an
    //  SPositionRecordBlock through each (as an SPositionRecordSink,
    //  positionrecordbuild.cpp/positionrecordblock.h) only while `context`
    //  is non-zero, then submits one more, differently-tagged block through
    //  the 26th element (which - at array stride 0x10 from +0x40 - lands at
    //  exactly +0x1D0) unconditionally past the loop.  Returns the final
    //  count (26) plus one.
    virtual int Method_0x20(int context);

    //  slot 13 / +0x34 (0x6F415BB0).  Five calls to a small shared release
    //  helper (playerwar3_releasehelper.cpp) over the object's own +0x27C,
    //  +0x280, +0x284, +0x290, +0x294 fields.
    virtual void Deactivate();

    //  slot 14 / +0x38 (0x6F415BF0).  See playerwar3_save.cpp; own depth-3
    //  closure in selectionwar3_save.cpp and techtreesave.cpp.
    virtual void Save(class CDataStoreScratch* store);

    //  slot 15 / +0x3C (0x6F419720).  See playerwar3_load.cpp - mirrors
    //  Save field for field, in the same order.  Own depth-3 closure in
    //  playerwar3_loadhelpers.cpp, playerwar3_techtreeload.cpp and
    //  playerwar3_selectionwar3loadthunk.cpp (the one genuine exception -
    //  see that file's own header comment).
    virtual void Load(class CDataStore* store);

    //  slot 17 / +0x44 (0x6F40F440).  A long, mechanical list of
    //  ((SIntMiniValue*/FloatMini*/SRefMiniValue*)&field)->DumpTo(sink)
    //  calls, exactly the shape CAgentWar3::DumpState (agentwar3_slots.cpp)
    //  established - just with ~27 fields instead of one, plus one real
    //  loop over the +0x2C0/+0x2C4 int array.  See playerwar3_dumpstate.cpp.
    virtual void DumpState(void* sink);

    //  slot 22 / +0x58 (0x6F40AF80).
    virtual const char* GetClassName();

    //  Slots 23-29 are not reconstructed yet - declaration order below
    //  this point is real vtable order (C++ appends new virtuals by
    //  declaration order), so slots 30-32 have to be declared in THAT
    //  order even though 23-29 are still missing; a session filling in
    //  23-29 must insert them *above* this point, never between these
    //  three, and must audit this ordering with check_vtables.py before
    //  any constructor may stamp ??_7CPlayerWar3@@6B@ (verified this
    //  session: check_vtables.py catches a wrong order immediately as a
    //  slot MISMATCH once the vtable is actually emitted, which happens
    //  the moment any real constructor or destructor exists - see
    //  playerwar3_dtor.cpp).

    //  slot 30 / +0x78 (0x6F415AA0).  See playerwar3_slot30.cpp.
    virtual void Method_0x78Slot30();

    //  slot 31 / +0x7C (0x6F40B4A0).  See
    //  playerwar3_appendsummaryline.cpp.
    virtual void AppendPlayerSummaryLine(char* dest, unsigned int maxLen);

    //  slot 32 / +0x80 (0x6F40FF20).  Debug-dumps the "dead hero" list -
    //  see playerwar3_dumpdeadherolist.cpp.
    virtual void DumpDeadHeroList(void* sink);

    //  0x6F40FAF0 / 0x6F02D800 - see
    //  playerwar3_buildintegerlistenerlevels.cpp.  Not virtual - internal
    //  helpers slot 30's own body reaches by a real `call`, not through
    //  any vtable.
    void** BuildIntegerListenerLevel1(void** outListener, int five, int six,
                                       void* owner, int zero);
    void** BuildIntegerListenerLevel0(void** outListener, int six,
                                       void* owner, int zero);

    //====================================================================
    //  Fields touched so far.  Offsets below +0x24 belong to CAgent/
    //  CAgentWar3 (agent.h/agentwar3.h); CPlayerWar3's own layout starts at
    //  +0x24.  Only members an implemented slot actually reads or writes
    //  get a name - everything else is reserved padding until a later
    //  session's dump gives it a name.  See docs/targets/CPlayerWar3.md for
    //  the destructor's own much longer field inventory, none of which is
    //  modelled here yet.
    //====================================================================
    //  +0x24..+0x30: an RCString (rcstring.h) - CPlayerWar3::Save reads its
    //  own GetCStr (0x6F4C4630, GetHandleOrZero's own second name - see
    //  rcstring.h) with an empty-string ("ValueName") fallback when it has
    //  none, the same shape GetPlayerDisplayName already uses.  Revealed by
    //  the Save dump (playerwar3_save.cpp).
    //
    //  Kept as raw bytes rather than a real `RCString` member: the
    //  destructor (playerwar3_dtor.cpp) already destructs this field by
    //  hand through a raw `((RCString*)(this+0x24))->~RCString()` cast (the
    //  shipped dtor's own explicit teardown order, matched and scored
    //  before this session) - typing it for real here would make the
    //  compiler *also* append an implicit `~RCString()` call at the end of
    //  `~CPlayerWar3`, a second, unshipped destruction on top of the first
    //  (confirmed against `build/playerwar3_dtor.asm`: a first draft that
    //  typed it moved that function's own score, evidence the compiler was
    //  really emitting the extra call rather than folding it away).  Same
    //  reasoning as the +0x2AC/+0x2FC/+0x310 fields below.
    char m_valueNameStorage[0x0C];   // +0x24, RCString-shaped, ends at +0x30

    //  +0x30: a byte flag Save writes raw (`movzx eax, byte ptr[edi+30h]`);
    //  no dump this session reaches sets it, so nothing pins a meaning.
    unsigned char m_flag30;   // +0x30
    char m_reserved31[0x34 - 0x31];   // +0x31, alignment padding to +0x34

    //  +0x34: a fresh CSelectionWar3 the constructor allocates and
    //  placement-constructs (playerwar3_ctor.cpp/
    //  playerwar3_ctorsubobjects.cpp) - see the class's own declaration
    //  above for why it stays opaque.  CPlayerWar3::Save calls its own
    //  ::Save (0x6F423FC0) through this pointer.
    CSelectionWar3* m_selectionWar3;   // +0x34

    //  +0x38: an SCheckedAllySlot (agenttypedslots.h) - the constructor
    //  builds it through the same "Assign only" converting-constructor
    //  family every sibling slot has (`new(this+0x38) SCheckedAllySlot(0)`,
    //  playerwar3_ctor.cpp), the destructor releases it by hand
    //  (playerwar3_dtor.cpp's own "+0x38 touched twice" note) and Save
    //  writes its own {handle,typeTag} pair (WriteAgentHandlePairDupA,
    //  playerwar3_savewritehandlepair.cpp).  Kept as raw bytes rather than
    //  a real `SCheckedAllySlot` member for the same reason
    //  m_valueNameStorage above is - SCheckedAllySlot has no default
    //  constructor, so a real member here would force the implicit
    //  member-initialiser the constructor's own placement-new immediately
    //  overwrites to fail to compile at all (confirmed: a first draft that
    //  typed it does not compile, C2512).
    char m_allySlotStorage[0x04];   // +0x38, SCheckedAllySlot-shaped, ends at +0x3C

    //  +0x3C: the AbilityEntry array's own element count, always
    //  constructed to 0x1A (26) - the same literal the `eh vector
    //  constructor/destructor iterator` calls at +0x40 use.  Revealed by
    //  the constructor dump (`mov dword ptr [esi+3Ch], 1Ah` immediately
    //  before the array's own placement-new loop) - CPlayerWar3::Method_0x24
    //  and ::Method_0x20 (already EXACT) both hard-code the 26-iteration
    //  loop bound instead of reading this field, so naming it here changes
    //  nothing about either of their compiled bodies.
    unsigned int m_abilityEntryCount;   // +0x3C

    //  +0x40: 26 (0x1A) 16-byte polymorphic entries, walked by Method_0x24/
    //  Method_0x20 above and, per the constructor and destructor dumps
    //  (playerwar3_dtor.cpp), placement-constructed/torn down with an
    //  `eh vector constructor/destructor iterator` calling
    //  sub_6F208450/sub_6F208470 per element.  Slot 9's own override is the
    //  only evidence of this array's element shape found so far: each
    //  element has its own vtable with (at least) a slot 3 taking two ints,
    //  and (per the slot-30 dump, not yet reconstructed) a slot 5 an
    //  IntegerListener gets built into.  The class that vtable belongs to
    //  could not be pinned down within this session's scope - no dump
    //  names a real constructor for it beyond the placement-new call, and
    //  its own destructor (sub_6F208470, playerwar3_abilityentrydtor.cpp)
    //  turned out to be nothing but TRefCnt's own trivial vtable-restamp -
    //  so the element is modelled as the minimum shape needed to make the
    //  calls this session's slots make: a vtable pointer followed by 12
    //  reserved bytes.  See the write-up.
    struct AbilityEntry
    {
        void** m_vtbl;
        char   m_reserved04[0x0C];

        //  0x6F208450 - no dump body anywhere in this session's scope (the
        //  constructor dump, asm/sub_6F4193E0_0x6F4193E0_calltree_asm.md,
        //  reaches it only as a function-pointer argument to the compiler's
        //  own `eh vector constructor iterator`, which IDA's call-tree walk
        //  does not follow into).  Redirected rather than guessed: `retn 0`
        //  per the `void (__thiscall *)(void *)` function-pointer type that
        //  same iterator call declares it with - no stack argument is
        //  possible under that type, so this is read off the interface, not
        //  invented.  Spelled `Construct` rather than as a real C++
        //  constructor (the same reason SCheckedDestructableSlot::Construct,
        //  agenttypedslots.h, is spelled that way): a naked body cannot
        //  reliably stand in for a constructor's own implicit "return
        //  `this`" contract, and every call site here is a placement `new`
        //  whose null test the compiler already emits regardless.  See
        //  playerwar3_abilityentryctor.cpp.
        void Construct();

        //  0x6F208470 - see playerwar3_abilityentrydtor.cpp.  Declared
        //  here (not virtual - nothing in this session's scope calls it
        //  through the vtable, only via the array's own `eh vector
        //  destructor iterator`) so the destructor's own dump can point at
        //  a real, named member instead of a bare address.
        void Destruct();

        //  0x6F473180 - see playerwar3_ability5integerlistener.cpp.
        void BuildIntegerListener(void** outListener, int minusOne, int six,
                                   void* owner, int zero);
    };
    AbilityEntry m_abilityEntries[0x1A];   // +0x40, ends at +0x1E0 (26 * 0x10 = 0x1A0)

    //  +0x1E0 / +0x214: the leading capacity/count word of each of the two
    //  12-entry SIntMiniValue arrays below - both always constructed to
    //  0xC (12), the same literal DumpState's own loop bound uses.
    //  Revealed by the constructor dump the same way m_abilityEntryCount
    //  was; DumpState (already EXACT) hard-codes the loop bound rather
    //  than reading either of these, so naming them changes nothing about
    //  its compiled body.
    unsigned int m_dumpArrayACount;   // +0x1E0

    //  +0x1E4..+0x213 and +0x218..+0x247: two 12-entry SIntMiniValue arrays,
    //  dumped interleaved by DumpState (playerwar3_dumpstate.cpp) - one
    //  entry of the first array, then the corresponding entry of the
    //  second, twelve times.
    SIntMiniValue m_dumpArrayA[0xC];   // +0x1E4
    unsigned int m_dumpArrayBCount;    // +0x214
    SIntMiniValue m_dumpArrayB[0xC];   // +0x218, ends at +0x248

    //  +0x248..+0x278: nine individual SIntMiniValue fields, dumped in
    //  address order.
    SIntMiniValue m_reserved248;   // +0x248
    SIntMiniValue m_reserved24C;   // +0x24C
    SIntMiniValue m_reserved250;   // +0x250
    SIntMiniValue m_reserved254;   // +0x254
    SIntMiniValue m_reserved258;   // +0x258
    SIntMiniValue m_reserved25C;   // +0x25C
    SIntMiniValue m_reserved260;   // +0x260
    SIntMiniValue m_reserved264;   // +0x264
    SIntMiniValue m_reserved268;   // +0x268

    //  +0x26C: dumped *twice* by DumpState - once here in address order,
    //  once again later (see playerwar3_dumpstate.cpp) after `ebp` is left
    //  pointing at it and never reloaded.  One field, two DumpTo() calls;
    //  reproduced exactly rather than "simplified" to one.
    SIntMiniValue m_reserved26C;   // +0x26C

    SIntMiniValue m_reserved270;   // +0x270
    SIntMiniValue m_reserved274;   // +0x274
    SIntMiniValue m_reserved278;   // +0x278, ends at +0x27C

    //  +0x27C, +0x280, +0x284: released by Deactivate above (via the shared
    //  helper) but not yet reached by anything that names their type.
    void* m_reserved27C;   // +0x27C
    void* m_reserved280;   // +0x280
    void* m_reserved284;   // +0x284

    char m_reserved288[0x290 - 0x288];

    //  +0x290, +0x294: released by Deactivate above.
    void* m_reserved290;   // +0x290
    void* m_reserved294;   // +0x294

    //  (no gap here: +0x294 + 4 == +0x298)

    //  +0x298, +0x2A0: two embedded FloatMini objects (floatmini.h), dumped
    //  via the non-virtual DumpTo the same way CItem's own embedded
    //  FloatMini/FloatMiniB fields are (position.h precedent).  8 bytes
    //  each (a vtable pointer plus one CFloat), which is exactly the gap
    //  between the two offsets.
    FloatMini m_reserved298;   // +0x298
    FloatMini m_reserved2A0;   // +0x2A0, ends at +0x2A8

    SIntMiniValue m_reserved2A8;   // +0x2A8

    //  +0x2AC: the "dead hero" array, per the destructor's own field
    //  inventory - `sub_6F286590`'s target, a `TSGrowableArray<CAgentPtr>`
    //  (`.?AV?$TSGrowableArray@VCAgentPtr@@` RTTI tag).  DumpState does not
    //  touch it, but Save does (playerwar3_save.cpp): WriteDword(m_count)
    //  then one WriteAgentHandlePairDupC per element.  Field order/size
    //  read directly off Save's own `[edi+2B0h]`(count)/`[edi+2B4h]`(data)
    //  offsets, which line up exactly with TSGrowableArray<T>'s own
    //  m_alloc/m_count/m_data/m_chunk layout (storm.h).
    //
    //  Kept as raw bytes for the same reason m_valueNameStorage above is:
    //  the destructor already destructs it by hand
    //  (`((TSGrowableArray<CAgentPtr>*)(this+0x2AC))->~TSGrowableArray()`,
    //  playerwar3_dtor.cpp) and typing it here would double that call.
    //  playerwar3_save.cpp reads it through the same raw cast.
    char m_deadHeroArrayStorage[0x10];   // +0x2AC, TSGrowableArray<CAgentPtr>-shaped, ends at +0x2BC

    //  +0x2BC: the IntegerMini array's own leading "alloc" field - the
    //  three-field (no m_chunk) variant storm.h calls `TSFixedArray<T>`,
    //  matching PlayerWar3IntMiniArrayReserve's own `arrv`/`alloc+1`/
    //  `alloc+2` reads (playerwar3_ctorsubobjects.cpp) against this exact
    //  address (playerwar3_ctor.cpp: `PlayerWar3IntMiniArrayReserve((char*)
    //  this + 0x2BC)`).  m_intMiniCount/m_intMiniArray below are its own
    //  +0x4/+0x8 fields, kept under their own pre-existing names (DumpState/
    //  DumpDeadHeroList already read them) rather than folded into a
    //  `TSFixedArray<SIntMiniValue>` that would rename them.
    unsigned int m_intMiniAlloc;  // +0x2BC

    //  +0x2C0 / +0x2C4: a count-and-pointer pair DumpState walks as a real
    //  loop, dumping `count` SIntMiniValue entries out of the array at
    //  +0x2C4.  Per the destructor's own notes this same +0x2C4 buffer is
    //  SMemFree'd and RTTI-tagged `.?AVIntegerMini@@` - "IntegerMini" is
    //  this array's own element type name, consistent with the "[IntMini]"
    //  tag SIntMiniValue::DumpTo prints.
    unsigned int m_intMiniCount;  // +0x2C0
    SIntMiniValue* m_intMiniArray; // +0x2C4

    char m_reserved2C8[0x2CC - 0x2C8];

    SIntMiniValue m_reserved2CC;   // +0x2CC

    //  +0x2D0: a fresh CTechTree the constructor allocates and
    //  placement-constructs (playerwar3_ctor.cpp/
    //  playerwar3_ctorsubobjects.cpp - see the class's own declaration
    //  above).  CPlayerWar3::Save calls its own ::Save (0x6F3F08C0)
    //  through this pointer (techtreesave.cpp).
    CTechTree* m_techTree;   // +0x2D0

    //  +0x2D4: an SRefMiniValue pair (t, b), dumped between +0x2CC and
    //  +0x2E0 - out of address order relative to those two, matching the
    //  dump's own call sequence exactly.
    SRefMiniValue m_reserved2D4;   // +0x2D4, ends at +0x2DC

    char m_reserved2DC[0x2E0 - 0x2DC];

    SIntMiniValue m_reserved2E0;   // +0x2E0

    //  +0x2E4: the BestHeroData array's own leading "alloc" field - the
    //  same three-field `TSFixedArray<T>` shape as the IntegerMini array
    //  above, count at +0x2E8, data at +0x2EC (PlayerWar3WriteBestHeroData-
    //  Array, playerwar3_save.cpp: `ebp=&this+0x2E4; count=[ebp+4];
    //  data=[ebp+8]`, element stride 0xC per `add ebx,0Ch`).  Matches the
    //  destructor's own field inventory naming "the SMemFree'd BestHeroData
    //  array at +0x2EC" (its own data pointer).
    //
    //  **Real type, not a private duplicate**: this session's own Load
    //  (playerwar3_load.cpp) reads it through SetCount at 0x6F418CE0, and
    //  that function's own body calls SetAlloc at 0x6F413790 -
    //  Player/bestherodataarray.cpp's own address for
    //  `TSFixedArray<BestHeroData>::SetAlloc` (`BestHeroData` from
    //  playerheroeval.h - "the snapshot the player record caches at
    //  +0x2E4", that header's own words, written before this field was
    //  ever traced back to it).  So this field really is
    //  `TSFixedArray<BestHeroData>`, and Save's own three raw-dword writes
    //  are exactly `m_typeId`/`m_level`/`m_experience` in address order -
    //  playerwar3_save.cpp is untouched (it reads through `void* arr`
    //  raw pointer arithmetic, not this member's type) but is kept
    //  in sync in spirit; only the type declaration here changes.
    TSFixedArray<BestHeroData> m_bestHeroData;   // +0x2E4, ends at +0x2F0

    //  +0x2F0, +0x2F4, +0x2F8: three more raw dwords Save writes directly
    //  (WriteDword/WriteDword/WriteDwordAlt2) between the BestHeroData
    //  array header and the CAgentTimer below - not modelled beyond that;
    //  DumpState does not touch them.
    unsigned int m_reserved2F0;   // +0x2F0
    unsigned int m_reserved2F4;   // +0x2F4
    unsigned int m_reserved2F8;   // +0x2F8

    //  +0x2FC: a CAgentTimer (agenttimer.h), per the destructor's own field
    //  inventory.  Save reaches its own vtable slot 8 (+0x20) with `this` =
    //  &this field and one argument (store) - a real virtual call this
    //  session cannot give a C++ name to without a fuller CAgentTimer
    //  vtable (slots 2-7 are still unknown), so it stays a raw slot
    //  dispatch (playerwar3_save.cpp) per CLAUDE.md's own
    //  vtable-dispatch-audit idiom rather than an invented `virtual`.
    //  Kept as raw bytes for the same double-destruction reason as
    //  m_valueNameStorage above - the destructor already destructs it by
    //  hand (`((CAgentTimer*)(this+0x2FC))->~CAgentTimer()`).
    char m_bestHeroTimerStorage[0x14];   // +0x2FC, CAgentTimer-shaped, ends at +0x310

    //  +0x310: a second RCString (rcstring.h), per the destructor's own
    //  field inventory.  Save reads its own GetCStr the same way it does
    //  for +0x24 (the same "ValueName" empty-string fallback).  Same
    //  double-destruction reason, kept raw.
    char m_reserved310Storage[0x0C];   // +0x310, RCString-shaped, ends at +0x31C

    //  +0x31C..+0x32B: not modelled.
    char m_reserved31C[0x32C - 0x31C];

    //  +0x32C, +0x330, +0x334: three more individual SIntMiniValue fields,
    //  the last three DumpState touches.
    SIntMiniValue m_reserved32C;   // +0x32C
    SIntMiniValue m_reserved330;   // +0x330
    SIntMiniValue m_reserved334;   // +0x334

    //  Rest of the object (destructor touches fields out past +0x338 - see
    //  the write-up) is not modelled here at all yet.
};

#endif

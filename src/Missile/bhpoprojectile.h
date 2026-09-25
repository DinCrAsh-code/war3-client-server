//============================================================================
//  CBhPoProjectile - the "projectile tick" object ProjectilePositionTrack's
//  own slot 3 override (AttachFromTable, 0x6F47AAA0,
//  Pathfinding/projectilepositiontrackattach.cpp) resolves out of its handle
//  table and arms tick relays for.  Shipped RTTI type descriptor:
//  `.?AVCBhPoProjectile@NIpse@@` (confirmed by 0x6F47AA10's own allocation
//  tag string, agent_worktrees names.json 0x6FA83E3C: "AVCBhPoProjectile@
//  NIpse@@") i.e. `NIpse::CBhPoProjectile` - the `NIpse::` namespace prefix
//  is dropped here the same way `NIpse::CPrRelation`/`NIpse::CSiRequest<>`
//  already are elsewhere in this repo (Sync/syncrequest.h), since this
//  codebase's own flat C++ namespace does not reproduce the shipped ones.
//
//  This dump's own roots never construct or fully enumerate this class's
//  vtable - agent_worktrees/classes/ has no RTTI vtable entry for it at all,
//  so unlike PositionTrack/CAgentTickRelay there is no committed ground
//  truth to check a real vtable declaration against.  Two of its virtual
//  slots ARE confirmed by real indirect calls in this dump (offset 0x0C from
//  0x6F47AA70, offset 0x10 from 0x6F4AB280), but nothing establishes what
//  sits at 0x00/0x04/0x08 first, so this header does not declare C++
//  `virtual` members at all - both known call sites are spelled with the
//  explicit vtable-slot-dispatch idiom CLAUDE.md's own house rules use for
//  exactly this situation (a real indirect call whose declared signature is
//  a typed guess from the call site alone, not a modelled virtual).
//
//  Layout: only the members this dump's own functions touch are named;
//  everything else is an explicit reserved gap.  Confirmed by the
//  constructor (0x6F479F90, bhpoprojectile_ctor.cpp) zero/-1/g_CFloatZero
//  -initialising every one of them up to +0x78, and by the slot-array
//  functions (bhpoprojectile_slots.cpp) reading +0x94..+0xC3.
//============================================================================
#ifndef BHPOPROJECTILE_H
#define BHPOPROJECTILE_H

#include "cellbuffer.h"   // CMemBlock
#include "CFloat.h"

class CGridVec2;

class CAgentTickRelay;    // Agent/agenttickrelay.h - what the slot array holds
class CDataStoreScratch;  // Net/cdatastorescratch.h - Save's own writer
class CDataStore;         // Net/cdatastore.h - Load's own reader
struct SHandleRegistrationRequest;  // Agent/prbehavior.h - AttachHandle/Apply's own request

//----------------------------------------------------------------------------
//  The growable array of CAgentTickRelay* slots at CBhPoProjectile's own
//  +0x9C: identical header shape to Widget/positionrecordarray.h's own
//  SPositionRecordArray (a CMemBlock, then m_growBy/m_capacity/m_count), but
//  holding raw 4-byte pointers rather than 0x2C-byte records - confirmed by
//  0x6F4AB050's own `(capacity+grow)*4` sizing (bhpoprojectile_slots.cpp).
//----------------------------------------------------------------------------
struct SBhPoTickRelaySlotArray
{
    //  0x6F4AB050 - grow (if `count` more slots do not fit past the current
    //  m_count) and advance m_count by `count`, optionally broadcasting the
    //  single pointer at `*fillValue` into every newly added slot.
    int Grow(void* fillValue, unsigned int count);

    //  0x6F4AAC80 - the broadcast-fill Grow() above calls: write the one
    //  pointer *fillValue into every slot [from, from+count), clamped to
    //  m_count.
    void FillNewSlots(unsigned int from, void* const* fillValue,
                       unsigned int count);

    //  0x6F4AB0D0 - remove `count` slots starting at `start` (clamped to
    //  what actually fits before m_count), sliding every slot past the
    //  removed range down over the hole via `memmove`, then shrinking
    //  m_count by however many were actually removed. Returns 0 with no
    //  effect at all when `start >= m_count`, 1 otherwise (even when the
    //  clamped count came out to 0, i.e. `start == m_count`).  Not called
    //  by anything in `CBhPoProjectile`'s own dump - reached instead from
    //  `NIpse::CPoPosBh::TeardownRelatedAgents` (Agent/poposbh_relatedagents.cpp),
    //  which embeds this exact struct shape at its own +0x9C - the generic
    //  array machinery is shared source, not per-class duplicated code, the
    //  same way `FillNewSlots` above already is documented as reachable
    //  from `SPositionRecordArray::FillRange`'s own header comment
    //  (Widget/positionrecordarray.h).  `this` in ecx, `retn 8`.
    int RemoveRange(unsigned int start, unsigned int count);

    //  0x6F4AAC40 - shrink the backing `CMemBlock` down to exactly
    //  `m_count` pointers (`m_capacity` follows it down to match), via the
    //  same `CMemBlock::SetSize` `Grow()` already calls to grow the other
    //  way.  Return value (the byte delta `SetSize` freed) is computed but
    //  unread by this struct's own one caller
    //  (`CPoPosBh::TeardownRelatedAgents`), kept anyway since the shipped
    //  stream computes it unconditionally.  `this` in ecx, no stack
    //  arguments, `retn`.  See `RemoveRange` above for why this lives here
    //  rather than in `bhpoprojectile_slots.cpp`.
    int ShrinkToFit();

    CMemBlock    m_storage;   // +0x00 (m_data, +0x0C, is this array's own base)
    unsigned int m_growBy;    // +0x14
    unsigned int m_capacity;  // +0x18
    unsigned int m_count;     // +0x1C
};

//  0x6F47AA70 - bhpoprojectile_attach.cpp.  `this`/`edx` receiver shape no
//  MSVC calling-convention keyword but `__fastcall` reaches (ecx=outObj,
//  edx=record) - see that file's own header comment.  Declared here so
//  Pathfinding/projectilepositiontrackattach.cpp can call it without a
//  second, disagreeing declaration.
void __fastcall LookupAndArmSlot(void* outObj, void* record, int dispatch);

class CBhPoProjectile
{
public:
    //  0x6F479F90.
    CBhPoProjectile();

    //  0x6F47A090 - vtable slot 0. Every class in this vtable family
    //  exposes its own shipped-source class name this way (see
    //  Agent/presence.h and friends for the identical shape); "BhPoProjectile"
    //  is the untruncated string per agent_worktrees names.json.
    const char* GetTypeName();

    //  0x6F47A430 - vtable slot 1, the real (non-scalar) destructor. Unlinks
    //  `this` from whatever intrusive doubly-linked list it is threaded on
    //  (a null-checked `next->prev = prev` / `prev->next = next` splice,
    //  fields at +0x04/+0x08 - the same two fields
    //  Pathfinding/projectilepositiontrackattach.cpp's own slot-3 override
    //  bit-tests, i.e. "this projectile's own attach-list linkage"), then
    //  returns `this` to CBhPoProjectilePool's own fast free list
    //  (Missile/bhpoprojectile_attach.cpp) exactly the way
    //  CBhPoProjectilePool::AllocateFromPool pops it back off - `raw = this
    //  - 4` (the allocator's own header dword sits immediately before the
    //  object, per AllocateFromPool's own `obj = raw + 4`), `raw->next =
    //  pool->m_fastFreeHead`, `pool->m_fastFreeHead = raw`,
    //  `pool->m_reserved18--`. No stack argument, `this` in ecx (`retn`).
    ~CBhPoProjectile();

    //  0x6F47A470 - vtable slot 2, ScalarDeletingDestructor. Named
    //  `ScalarDeletingDestructorTrivial` for the same reason
    //  Agent/presence.h's own does: restamps straight to
    //  `??_7CPresence@NTempest@@6B@` (0x6F951CA0) with no base-class
    //  teardown call at all - byte-identical in shape to
    //  `NTempest::CPresence::ScalarDeletingDestructorTrivial`
    //  (Agent/presence_dtor.cpp) and to `CBhPoBehavior`'s own
    //  (Missile/bhpobehavior.h), differing only in address. See
    //  presence.h's own file header for the open question of why this
    //  triplet-of-classes shape skips real teardown.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F47A0A0 - vtable slot 15 (+0x3C), ComputeChecksum. Calls the base
    //  `NTempest::CPresenceTagged::FoldLinkFieldsIntoHash` (0x6F4AEE60,
    //  Agent/agentbaseabscomputechecksum.cpp) first, then folds this
    //  class's own 8 fields (+0x40, +0x44, +0x50, +0x54, +0x58, +0x5C,
    //  +0x60, +0x64) into the same accumulator with the identical
    //  byte-fold-and-add idiom that file's own `AgentBaseAbsFoldDword`
    //  already names (duplicated locally here - no shared translation unit
    //  between Agent/ and Missile/, matching that file's own precedent for
    //  why it isn't shared further either). `this` in ecx, the
    //  `unsigned int*` accumulator a single stack arg, `retn 4`.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F47A300 - vtable slot 16, DumpDebugState. Formats this
    //  projectile's own tag pair and start/velocity CFloat fields through
    //  the shared `sub_6F4B73F0` sink-formatter
    //  (Agent/presencetagged_dumpdebugstate.cpp's own precedent for the
    //  identical helper, same "[Tag] label[fields]" shape). `this` in ecx,
    //  the sink in a single stack arg, `retn 4`.
    void DumpDebugState(void* sink);

    //  0x6F496A40 - vtable slot 3, this class's own override of the base
    //  `NIpse::CPrBehavior::AttachHandle` (0x6F4AA340, Agent/prbehavior.h -
    //  CBhPoBehavior's own slot 3 is that same address, inherited
    //  unmodified one level up, see bhpobehavior.h's own file header).
    //  Same "call base slot-3, then reset a block of this class's own
    //  fields" shape every `Apply` in this NIpse family has
    //  (Agent/poposbh.h, Agent/poposcl.h): forwards `request` to the base
    //  unchanged (a plain non-virtual call on `this` reinterpreted as the
    //  base type - this repo has no real `class ... : public CPrBehavior`
    //  anywhere in this chain, see prbehavior.h's own file header for why),
    //  then re-zeroes the start/velocity block (+0x40..+0x54), reloads
    //  g_CFloatZero twice (once for the +0x58/+0x5C/+0x4C/+0x60 group, a
    //  second time for +0x74/+0x78 - two separate loads, not one kept
    //  live, the same "two real copy-initialisations" tell
    //  Agent/impactcontext_ctor.cpp's own header already names), resets
    //  +0x68/+0x6C to -1 and sets flag bit 0x40000 on +0x30.
    void Apply(SHandleRegistrationRequest* request);

    //  0x6F496AA0 - vtable slot 9, Save. Base `NIpse::CPrBehavior::Save`
    //  first (0x6F4AA450, forwarded unchanged), then this class's own
    //  +0x38/+0x3C pair, its start/velocity block (+0x40..+0x54), its own
    //  velMag/velOri/reserved4C/velTime quartet (+0x58/+0x5C/+0x4C/+0x60,
    //  in THAT order - not offset order, the shipped code threads the
    //  writer's own return value through this one group, unlike the
    //  others, which simply reload `store` fresh each call), the cached
    //  velPeriod (+0x64, read early, written late - `WriteDword`), the
    //  +0x68/+0x6C handle-ref pair (`WriteDword` again) and +0x70
    //  (`WriteDwordAlt2` - the duplicate-body idiom prbehavior.h's own
    //  header already names, reused here at a different address), then
    //  +0x74/+0x78.
    void Save(CDataStoreScratch* store);

    //  0x6F496B90 - vtable slot 10, Load. Save's mirror, `ReadCFloat`/
    //  `ReadDword`/`ReadDwordAlt3` in place of the Write- forms, with one
    //  real asymmetry Save does not have: the +0x68/+0x6C/+0x70 group is
    //  gated behind a version check - a real indirect call through
    //  `store`'s own vtable slot 5 (`store->GetVersion() >= 0x1771`, this
    //  session's own reading of an otherwise-unnamed accessor; nothing
    //  else in this dump characterizes that slot further) - so an older
    //  save simply leaves +0x68/+0x6C/+0x70 at whatever `Apply` already
    //  reset them to.
    void Load(CDataStore* store);

    //  0x6F497340 - vtable slot 17, this class's own concrete override of
    //  the base's pure virtual (`NIpse::CPrBehavior`'s own vtable slot 17,
    //  `_purecall` at that level - CBhPoBehavior does not override it
    //  either, see bhpobehavior.h's own file header - so this is the
    //  first concrete body anywhere in the chain). Reached only through
    //  the inherited-unmodified `CPrBehavior::DispatchIfEnabled`
    //  (0x6F4AA370, this class's own slot 8), which hands its own `int
    //  arg` straight through - really a pointer to a small event record
    //  this session did not otherwise identify (only its own +0x08 dword,
    //  an opaque tag constant, and +0x10, a second object pointer, are
    //  read here).
    //
    //  Early-outs unless `arg`'s own +0x08 tag is exactly 0x60627065 (not
    //  a legible FourCC either way round). On a match: resolve arg's own
    //  +0x10 into `target` and write it `target->+0x08 = m_velOri -
    //  target->+0x00`, `target->+0x0C = m_velMag - target->+0x04` (via
    //  the already-real `CFloat::operator-`, Math/CFloat.h), OR its own
    //  +0x14 flag word with bit 1.
    //
    //  Then resolve this object's own +0x68/+0x6C handle pair
    //  (`SHandleRefAt0x68Owner::ResolveHandle`, already real,
    //  Misc/misc_handle_resolvers.cpp - confirms +0x68/+0x6C really is a
    //  {handle,typeTag} pair, not a bare sentinel, refining this header's
    //  own earlier naming) and, only if this object's own +0x1C is
    //  non-zero, the resolve succeeded, and the resolved object's own
    //  +0x10 matches a second opaque tag (0x60706375): read the resolved
    //  object's own +0xC0 CFloat and, unless it compares greater than
    //  zero (`test ah,5`/`jp` - docs/msvc-vc8-idioms.md's own `a > b`
    //  row), treat the resolved object as a `CPathTrace*` (RTTI-style tag
    //  match, not a static type), fetch its current point
    //  (`CPathTrace::GetCurrentPosition`, 0x6F38B140, Pathfinding/CPathTrace2.cpp)
    //  and hand it to `ReconcilePathTraceNotify` (0x6F496EB0, real now -
    //  Missile/bhpoprojectile_notifycore.cpp - a follow-up session's own
    //  work; a ~90-instruction routine resolving a position, subtracting
    //  the incoming point, and recomputing this object's own
    //  m_velMag/m_velOri, +0x58/+0x5C, through several still-unnamed
    //  leaves) - then OR `target`'s own +0x14 with bit 0.
    void NotifyVelocityDelta(int arg);

    //  0x6F496EB0 - see this method's own file
    //  (Missile/bhpoprojectile_notifycore.cpp) for the full trace. `point`
    //  in the one stack argument, `retn 4`.
    void __thiscall ReconcilePathTraceNotify(CGridVec2* point);

    void*        m_vtable;         // +0x00 - ??_7CBhPoProjectile@NIpse@@6B@
    unsigned int m_reserved04;     // +0x04 = 0
    unsigned int m_reserved08;     // +0x08 = 0
    unsigned int m_reserved0C;     // +0x0C = 0
    unsigned int m_reserved10;     // +0x10 = 0

    //  +0x14/+0x18 - a {handle, typeTag} pair copied verbatim into a
    //  CAgentTickRelay's own m_agentHandle/m_agentTypeTag
    //  (bhpoprojectile_slots.cpp's own Arm-a-slot function) - "the agent
    //  this projectile is bound to".  The same two fields also read as
    //  CHandleObject's own m_flags/m_typeTag shape (game.h):
    //  CTimeSync::GetChannel(this) tests bit 31 of +0x14 for "remote", and
    //  this class's own slot-3 override
    //  (Pathfinding/projectilepositiontrackattach.cpp) makes the identical
    //  bit-31 test on this same field to pick between two sentinel values -
    //  i.e. the handle's own high bit is this engine's "remotely owned"
    //  convention, not a coincidence, so both readings agree.
    unsigned int m_agentHandle;    // +0x14 = -1
    unsigned int m_agentTypeTag;   // +0x18 = -1

    unsigned int m_reserved1C;     // +0x1C = 0
    unsigned int m_reserved20;     // +0x20 = 0
    int          m_sentinel24;     // +0x24 = -1
    int          m_sentinel28;     // +0x28 = -1
    unsigned int m_reserved2C;     // +0x2C = 0
    unsigned int m_reserved30;     // +0x30 = 0
    int          m_sentinel34;     // +0x34 = -1
    unsigned int m_reserved38;     // +0x38 = 0
    unsigned int m_reserved3C;     // +0x3C = 0
    //  +0x40/+0x44/+0x50/+0x54/+0x58/+0x5C/+0x60 - CFloat-shaped fields
    //  (bhpoprojectile_dumpdebugstate.cpp's own DumpDebugState override,
    //  0x6F47A300, formats every one of them with `FormatCFloat`, and
    //  ComputeChecksum, 0x6F47A0A0, folds all but +0x48/+0x4C into the
    //  checksum too) - "start"/"velocity" per DumpDebugState's own
    //  "\tstart x[%s] y[%s]" / "\tvel x[%s] y[%s] mag[%s] ori[%s] tim[%s]
    //  prd[%u]" format strings, naming order confirmed by that function's
    //  own push order (see its header comment). Kept as raw `unsigned int`
    //  rather than `CFloat` for the same naked-constructor reason as
    //  +0x74/+0x78 below - a non-trivial-default-ctor member anywhere in
    //  this class would force a hidden sub-object-construction prologue
    //  onto this class's own `__declspec(naked)` constructor
    //  (bhpoprojectile_ctor.cpp) - the byte layout is identical either way.
    unsigned int m_startX;         // +0x40 = 0  (CFloat 0.0)
    unsigned int m_startY;         // +0x44 = 0  (CFloat 0.0)
    unsigned int m_reserved48;     // +0x48 = 0
    char         m_reserved4C[4];  // +0x4C - untouched by the ctor
    unsigned int m_velX;           // +0x50 = 0  (CFloat 0.0)
    unsigned int m_velY;           // +0x54 = 0  (CFloat 0.0)
    unsigned int m_velMag;         // +0x58 = g_CFloatZero
    unsigned int m_velOri;         // +0x5C = g_CFloatZero
    unsigned int m_velTime;        // +0x60 = g_CFloatZero
    //  +0x64 - the one field of this group DumpDebugState prints with a
    //  plain `%u`, not `FormatCFloat` - "prd" in that format string, a
    //  genuine integer (period/tick count?) rather than a CFloat.
    unsigned int m_velPeriod;      // +0x64 = 0

    //  +0x68/+0x6C - a {handle, typeTag} pair, confirmed by
    //  `NotifyVelocityDelta`'s own `SHandleRefAt0x68Owner::ResolveHandle`
    //  call below (Misc/misc_handle_resolvers.cpp) - -1/-1 (`Apply`'s own
    //  reset) is this handle convention's own "unset" value, not a bare
    //  sentinel pair as this header used to have it.
    unsigned int m_boundHandle68;  // +0x68 = -1
    int          m_boundTypeTag6C; // +0x6C = -1
    unsigned int m_reserved70;     // +0x70 = 0
    unsigned int m_reserved74;     // +0x74 = g_CFloatZero
    unsigned int m_reserved78;     // +0x78 = g_CFloatZero

    //  +0x7C..+0x93 - untouched by the constructor; not read by anything
    //  else in this dump either, so left as an explicit gap.
    char         m_reserved7C[0x94 - 0x7C];

    //  +0x94/+0x98 - two pointers the slot-3 override (0x6F47AAA0,
    //  Pathfinding/projectilepositiontrackattach.cpp) reads and
    //  bit-twiddles a `+0x34` field on (clearing/setting flag bits on what
    //  looks like a separate owner object's own state word).  Not otherwise
    //  identified this session - named generically rather than guessed at.
    void*        m_reserved94;     // +0x94
    void*        m_reserved98;     // +0x98

    SBhPoTickRelaySlotArray m_slots;   // +0x9C

    //  +0xBC - the cached CSyncChannel registration this class re-samples
    //  through (bhpoprojectile_slots.cpp's own scheduling function);
    //  +0xC0 - the CFloat sample it registers with, by value.  Kept as a
    //  raw `unsigned int` bit pattern rather than `CFloat` for the same
    //  naked-constructor reason as +0x58 etc above - this member is never
    //  touched by the constructor itself, but a non-trivial-default-ctor
    //  member anywhere in the class is enough to trigger MSVC's hidden
    //  sub-object-construction wrapper around it.
    void*        m_pendingRegistration;  // +0xBC
    unsigned int m_pendingSample;        // +0xC0

    //  0x6F4AB280 - release slot `index`: if it holds a live
    //  CAgentTickRelay, retire both its registration references, clear its
    //  "armed" flag bit, and - if `notify` is non-zero - call the relay's
    //  own vtable slot 0x10 with a literal 0.  Either way, null the slot
    //  and re-sample this projectile's own sync channel.
    void ReleaseTickRelaySlot(unsigned int index, int notify);

    //  0x6F4AB1A0 - re-sample this projectile's own CSyncChannel
    //  registration with `*value` (every call site in this dump passes
    //  `&m_pendingSample`): if the currently cached registration is still
    //  live and its sample would differ from the fresh one, retire it
    //  first; either way, register a fresh sample and cache the result.
    void ResampleSyncChannel(const CFloat* value);

    //  0x6F4AB5D0 - install `relay` at slot `index` (releasing whatever was
    //  there first), growing the slot array if `index` does not fit yet,
    //  then stamp the relay's own "armed" bit and {agentHandle,
    //  agentTypeTag} pair from this projectile's own, and re-sample the
    //  sync channel.  The third argument is read by no path in this dump.
    void ArmTickRelaySlot(CAgentTickRelay* relay, unsigned int index,
                          int unused, int notify);
};

#endif

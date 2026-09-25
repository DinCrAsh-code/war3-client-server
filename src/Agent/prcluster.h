//============================================================================
//  NIpse::CPrCluster (`??_7CPrCluster@NIpse@@6B@`, 0x6F95230C),
//  NIpse::CPrClusterGen (`??_7CPrClusterGen@NIpse@@6B@`, 0x6F95315C) and
//  NIpse::CPrClusterGroup (`??_7CPrClusterGroup@NIpse@@6B@`, 0x6F951EC4) -
//  three 17-slot "clustering" classes in the same NTempest/NIpse
//  positioning family the base chain (docs/targets/NTempestNIpse_BaseChain.md)
//  belongs to, plus the two `NTempest::CLinkedList<T>` instantiations over
//  the first two of them (`CLinkedList<CPrClusterGroup>`, 0x6F952274, and
//  `CLinkedList<CPrCluster>`, 0x6F952624), which each override only vtable
//  slot 2 (their own scalar deleting destructor) and inherit every other
//  slot's address unchanged from the element type's own vtable.
//
//  The first session (NTempestNIpse cluster D, 2026-09-08) closed slots 0-2
//  of the three element-type classes plus both `CLinkedList<T>` overrides -
//  see docs/targets/NTempestNIpse_ClusterD.md for the full write-up.  A
//  follow-up session (2026-09-12) closed slots 9/10 (Save/Load) for
//  `CPrCluster`/`CPrClusterGen`, plus slots 3/4 (Apply/Slot4) for
//  `CPrClusterGen` alone - `CPrCluster`'s own slot 3/4 pair and all three
//  classes' slot 15/16 (ComputeChecksum/DumpDebugState) stay `TODO`,
//  documented in that same file's own "what is still open" section: slot
//  3/4's own callee tree for `CPrCluster` (a per-listener notification
//  pool this session did not chase) is a materially larger investigation
//  than `CPrClusterGen`'s own pair turned out to be, and 15/16 are the
//  300+-instruction checksum/dump bodies already flagged as this cluster's
//  lowest priority.
//
//  **Base class, established from the shipped instruction stream, not
//  RTTI**: none of these three classes' own scalar deleting destructors
//  restamp their own vtable - CPrClusterGroup's and CPrClusterGen's own
//  (slot 2) restamp straight to `??_7CPresence@NTempest@@6B@` (0x6F951CA0)
//  with no base-destructor call at all (the same "trivial" no-teardown
//  shape Agent/presence_dtor.cpp's own `CPresence::
//  ScalarDeletingDestructorTrivial` already documents for CPresence/
//  CPresenceTagged/CPrRelation), and CPrCluster's own calls a real
//  `~CPrCluster()` (0x6F4842E0, prcluster_dtor.cpp) whose own body releases
//  one `CMemBlock` sub-object and then ALSO restamps straight to
//  `??_7CPresence@NTempest@@6B@`, skipping any restamp of its own -
//  meaning all three classes sit directly on `NTempest::CPresence` in the
//  real inheritance chain, not on the deeper `CPresenceTagged` ->
//  `CPrRelation` -> `CRlAgent` -> `CAgentBaseAbs` chain the base-chain
//  session's classes use.  As with every other class in this family, kept
//  as a flat, offset-addressed layout rather than a real C++ base
//  (Pathfinding/crlagent.h's own file header explains why) - only the
//  fields this session's own call tree actually touches are named.
//
//  All three share CPresence's own +0x04/+0x08 intrusive doubly-linked-list
//  prev/next pair (the same fields Agent/presence.h's own
//  `CPresence::ReturnToStaticPool` unlinks), each with its own static
//  free-list pool off `g_pTimeSync`
//  (Agent/agentbasechain_staticpools.h - CPrCluster_GetStaticPool/
//  CPrClusterGen_GetStaticPool/CPrClusterGroup_GetStaticPool, added by this
//  session).
//============================================================================
#ifndef PRCLUSTER_H
#define PRCLUSTER_H

#include "CFloat.h"
#include "cellbuffer.h"

struct SHandleRegistrationRequest;
class CDataStoreScratch;
class CDataStore;
class CHandleObject;

//  0x6FAB73F4 - a default CFloat bit pattern this session's own Apply
//  bodies stamp into several fields; same "unmapped global in the shared
//  named-constant run" shape floatmini.h's own g_unk6FAAE624/g_unk6FAAE628
//  already document.  funcmap.py's DATA table.
extern const unsigned int g_unk6FAB73F4;

//----------------------------------------------------------------------------
//  `CPrCluster`'s own +0x1C member (Agent/prclusterrecordtable.cpp's own
//  SaveRecordTable/LoadRecordTable, 0x6F4914B0/0x6F491520): a `CMemBlock`
//  (Pathfinding/cellbuffer.h) growable byte block holding an array of
//  `SPrClusterRecord`s, plus three more fields appended right after the
//  block's own five - `m_field14`/`m_field18` (untouched meaning; Load's
//  own `CMemBlock::Init` call derives its `header` argument as
//  `m_field18 - m_size`) and `m_count`, the element count both
//  Save/LoadRecordTable's own loop and CPrCluster's own dtor-continuation
//  (0x6F493130, out of this session's scope) gate the record-array release
//  on.  Same offset CPrCluster's own destructor (prcluster_dtor.cpp)
//  already releases as a flat `CMemBlock`-only byte range - extending it to
//  a bigger struct here changes nothing about the bytes at +0x1C..+0x30
//  that call already touches.
//----------------------------------------------------------------------------
struct SPrClusterRecordTable
{
    CMemBlock    m_block;      // +0x00..+0x13
    unsigned int m_field14;    // +0x14
    unsigned int m_field18;    // +0x18
    unsigned int m_count;      // +0x1C
};

//----------------------------------------------------------------------------
//  One 0x2C-byte element of `SPrClusterRecordTable::m_block`'s own array -
//  see Agent/prclusterrecordio.cpp's own WriteClusterRecord/ReadClusterRecord
//  (0x6F48E9E0/0x6F48EA60).  `m_reserved14` is never written by
//  WriteClusterRecord (the +0x14 slot the field pair above and below it
//  straddle is genuinely skipped) but IS zeroed by ReadClusterRecord after
//  loading everything else - the same "Load re-derives what Save does not
//  stream" shape this whole family already carries elsewhere.
//----------------------------------------------------------------------------
struct SPrClusterRecord
{
    unsigned int m_field00;      // +0x00
    unsigned int m_field04;      // +0x04
    unsigned int m_field08;      // +0x08 (Dword"Alt2"/"Alt3" variant)
    CFloat       m_pair0C[2];    // +0x0C, +0x10 - a real two-element array,
                                  //   not two scalars: the dump caches the
                                  //   +0x0C address once and reaches +0x10
                                  //   via a +4 offset off it rather than
                                  //   re-deriving from `rec` each time.
    char         m_reserved14[4];// +0x14 - see this struct's own header comment
    CFloat       m_pair18[2];    // +0x18, +0x1C - same array shape as m_pair0C
    CFloat       m_field20;      // +0x20 - genuinely two independent scalars:
    CFloat       m_field24;      // +0x24   the dump re-derives each address
                                  //   from `rec` instead of caching a base.
    unsigned int m_field28;      // +0x28 (Dword"Alt2"/"Alt3" variant)
};

//  0x6F48E9E0 - write one SPrClusterRecord.  Agent/prclusterrecordio.cpp.
CDataStoreScratch* __fastcall WriteClusterRecord(CDataStoreScratch* store,
                                                  const SPrClusterRecord* rec);
//  0x6F48EA60 - read one SPrClusterRecord back.  Same file.
CDataStore* __fastcall ReadClusterRecord(CDataStore* store, SPrClusterRecord* rec);

//  0x6F4914B0/0x6F491520 - save/load the whole SPrClusterRecordTable, header
//  fields then every element through WriteClusterRecord/ReadClusterRecord
//  above.  Agent/prclusterrecordtable.cpp.  Own translation unit from both
//  their one caller (CPrCluster::Save/Load, prcluster_saveload.cpp) and
//  from WriteClusterRecord/ReadClusterRecord themselves (prclusterrecordio.cpp) -
//  three different real `call`s in the shipped dump, so none of the three
//  files may see another's definition.
CDataStoreScratch* __fastcall SaveRecordTable(CDataStoreScratch* store,
                                               SPrClusterRecordTable* table);
CDataStore* __fastcall LoadRecordTable(CDataStore* store,
                                       SPrClusterRecordTable* table);

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F486B80/0x6F486BD0/0x6F486C10(idx15)/0x6F4871C0(idx16)/0x6F487360/
//  0x6F492580(idx9)/0x6F492680(idx10).  Idx3/4 (Apply/Slot4, 0x6F4924D0/
//  0x6F493130) and idx15/16 stay `TODO` - see this file's own header
//  comment.
//----------------------------------------------------------------------------
class CPrCluster
{
public:
    //  0x6F4842E0 - the real (non-scalar) destructor.  Own translation unit
    //  (prcluster_dtor.cpp), deliberately: this build has no SEH frames
    //  (`/GS- /EHs-c-`) so with the definition visible here
    //  ScalarDeletingDestructor's own `call` would inline and lose both the
    //  call and its `this != 0` guard (docs/msvc-vc8-idioms.md's own
    //  "An `__except_handler4`-shaped frame..." section - the same
    //  placement rule Agent/agent_deleteself.cpp already follows).
    ~CPrCluster();

    //  0x6F486B80 - vtable slot 0.  Trivial class-name accessor, same shape
    //  as every other class in this family (Agent/presence_gettypename.cpp).
    const char* GetTypeName();

    //  0x6F486BD0 - vtable slot 1.  Same unlink-then-return-to-pool shape as
    //  NTempest::CPresence::ReturnToStaticPool (Agent/presence_returntopool.cpp),
    //  against this class's own static pool.
    void ReturnToStaticPool();

    //  0x6F487360 - vtable slot 2, scalar deleting destructor.  Calls the
    //  real ~CPrCluster() above first and carries the `this != 0` guard on
    //  the free - the same shape NTempest::CMemBlock::ScalarDeletingDestructor
    //  (Agent/cmemblock.h) and NIpse::CRlAgent::ScalarDeletingDestructor
    //  (Pathfinding/crlagent.h) use whenever a real destructor precedes the
    //  free.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F492580 - vtable slot 9, Save.  SaveRecordTable(store,&m_records)
    //  first (its own return re-chained through every following write, the
    //  same "one store threading through the whole function" shape every
    //  other Save in this family already shows), then eleven scalar/CFloat
    //  fields at +0x40..+0x80 in strict offset order, then the three
    //  handle-tag-pair references at +0x78 (validated - see below)/+0x7C/
    //  +0x3C through the already-reconstructed WritePathRefEntry
    //  (Pathfinding/pathref_write.cpp), which really just writes two dwords
    //  read off the referenced object's own +0x14/+0x18 (this whole
    //  family's own handle-table tag pair, Pathfinding/handletable.h) or
    //  {-1,-1} when the pointer is null.
    //
    //  +0x78's own reference is validated first: null if the pointer is
    //  null OR if `*(ptr+0x38) == -1` - a generation/slot sentinel on
    //  WHATEVER +0x78 points at (not on `this`), the same "the target
    //  considers itself detached" shape idx3/4's own unreconstructed
    //  0x6F491430 call tree (this file's own header, "what is still open")
    //  applies to +0x1C's own record table - kept as a raw offset rather
    //  than a named field on an invented type, since nothing else in this
    //  call tree needs that type's own layout.
    void Save(CDataStoreScratch* store);

    //  0x6F492680 - vtable slot 10, Load.  Save's exact mirror through
    //  LoadRecordTable/the Read* family, with two real asymmetries: the
    //  +0x78/+0x7C/+0x3C trio is read back with no validation (three plain
    //  SPathRefReader::ReadEntry() calls, Pathfinding/pathref_load.cpp -
    //  already an out-of-scope naked redirect, unreconstructed past its own
    //  declaration) rather than WritePathRefEntry's own validate-then-write,
    //  and +0x84 is zeroed at the very end with no corresponding Save
    //  write at all - a pure re-derive-on-load field, the same shape this
    //  family's own CPrClusterGroup::Load (Save/Load pair) already
    //  documents for its own live-list membership.
    void Load(CDataStore* store);

    //  0x6F486C10 - vtable slot 15, ComputeChecksum.  This class's own base
    //  contribution (NTempest::CPresenceTagged::FoldLinkFieldsIntoHash,
    //  agentbaseabsFold-shaped) first, then every one of the +0x1C..+0x3C
    //  record table's own elements - only 5 of each 0x2C-byte
    //  SPrClusterRecord's own 9 fields (m_field00/m_field04/m_pair0C[0]/
    //  m_pair0C[1]/m_field28 - confirmed off the dump, not a guess: the
    //  other four, m_pair18 and m_field20/24, are genuinely skipped), then
    //  the fourteen scalar/CFloat fields at +0x40..+0x80 in strict offset
    //  order (the same four gaps Save/Load's own doc comment already
    //  names: +0x48/+0x78/+0x7C/+0x84).
    void ComputeChecksum(unsigned int* hash);

    //  0x6F4871C0 - vtable slot 16, DumpDebugState.  Same "[PrTg] t[%u]
    //  b[%u]" header line as every sibling in this family
    //  (CPrClusterGroup::DumpDebugState), then "[PrCl]"/"\t[members]" and
    //  one "\t\t%u -> t[%u] b[%u] ofs x[%s] y[%s] o..." line per record-
    //  table element (own tag pair at +0x00/+0x04, formatted CFloats at
    //  +0x10/+0x0C - note the swapped x/y argument order off the dump),
    //  then the same +0x40..+0x80 field span ComputeChecksum reads,
    //  grouped into four LogFormatted calls matching the shipped body's
    //  own line breaks.
    void DumpDebugState(void* sink);

    void*        m_vtable;          // +0x00
    void*        m_prevLink;        // +0x04 - CPresence's own intrusive
    void*        m_nextLink;        // +0x08 -   free-list prev/next
    char         m_field0C[8];       // +0x0C..+0x14 - untouched by this call tree
    unsigned int m_tagT;             // +0x14 - CPresenceTagged's own tag pair
    unsigned int m_tagB;             // +0x18
    SPrClusterRecordTable m_records; // +0x1C..+0x3C - see this file's own
                                      //   header comment on SPrClusterRecordTable
    void*        m_field3C;          // +0x3C - a handle-tag-pair reference
                                      //   (see Save's own doc comment above);
                                      //   idx3/4's own release of this same
                                      //   field is out of this session's scope
    unsigned int m_field40;          // +0x40
    unsigned int m_field44;          // +0x44
    unsigned int m_field48;          // +0x48 (Dword"Alt2" variant)
    CFloat       m_field4C;          // +0x4C
    CFloat       m_field50;          // +0x50
    CFloat       m_field54;          // +0x54
    CFloat       m_field58;          // +0x58
    unsigned int m_field5C;          // +0x5C
    unsigned int m_field60;          // +0x60
    unsigned int m_field64;          // +0x64
    unsigned int m_field68;          // +0x68
    unsigned int m_field6C;          // +0x6C
    CFloat       m_field70;          // +0x70
    CFloat       m_field74;          // +0x74
    void*        m_field78;          // +0x78 - validated on Save (see above)
    void*        m_field7C;          // +0x7C
    unsigned int m_field80;          // +0x80 (Dword"Alt2" variant)
    unsigned int m_field84;          // +0x84 - Load-only, never saved
};

//----------------------------------------------------------------------------
//  0x6F491780/0x6F491D20/0x6F491D60.
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  One 0xC-byte element of `CPrClusterGen::m_refSlots` (below): a
//  handle/type-tag pair (Pathfinding/game.h's own `LookupHandle(unsigned
//  int handle, int typeTag)`) plus one more untouched dword.  Confirmed by
//  CPrClusterGen::Load's own body (prclustergen_saveload.cpp): it re-derives
//  `m_resolved[i]` from `LookupHandle(m_refSlots[i].m_handle,
//  m_refSlots[i].m_typeTag)` rather than streaming a pointer.
//----------------------------------------------------------------------------
struct SPrClusterGenRefSlot
{
    unsigned int m_handle;    // +0x00
    unsigned int m_typeTag;   // +0x04
    unsigned int m_field08;   // +0x08 - saved/loaded verbatim; meaning unclear
};

//----------------------------------------------------------------------------
//  0x6F491780/0x6F491D20/0x6F491D60/0x6F48F0D0(idx3)/0x6F48F130(idx4)/
//  0x6F48F140(idx9)/0x6F491FF0(idx10) - idx3/4/9/10 closed in the cluster-D
//  follow-up session (2026-09-12); idx15/16 stay `TODO` (this file's own
//  header comment).
//----------------------------------------------------------------------------
class CPrClusterGen
{
public:
    //  0x6F491780 - vtable slot 0.
    const char* GetTypeName();

    //  0x6F491D20 - vtable slot 1.  Same shape as CPrCluster::ReturnToStaticPool
    //  above (the raw dump keeps `this` in ecx across the pool-getter call
    //  rather than caching it into esi first - a register-allocation
    //  difference only, not a different unlink shape).
    void ReturnToStaticPool();

    //  0x6F491D60 - vtable slot 2, scalar deleting destructor.  The
    //  "trivial" no-teardown shape (see this file's own header comment):
    //  restamps straight to `??_7CPresence@NTempest@@6B@`, no base
    //  destructor call, no `this != 0` guard on the free.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F48F0D0 - vtable slot 3, Apply.  Registers (or re-registers) this
    //  object's own handle-table entry via `SHandleTableEntry::RegisterOrReuse`
    //  (Pathfinding/handletable.h, `this` cast straight to `SHandleTableEntry*`
    //  the same way this whole family's own Apply already does - meaning
    //  CPrClusterGen's own m_slotHandle/m_slotIndex sit at +0x14/+0x18,
    //  inside `m_reserved0C` below, matching every sibling class), then
    //  resets the twelve `m_refSlots`/`m_resolved` pairs' own handle/typeTag
    //  fields to -1 (own translation unit, Agent/prclustergen_helpers.cpp -
    //  0x6F48F0A0 - which does NOT touch `m_refSlots[i].m_field08`, a real
    //  asymmetry confirmed off the dump), then the eleven scalar/CFloat
    //  fields at +0xDC..+0x100 to their own defaults (`-1`/`g_unk6FAB73F4`/
    //  `0`/`g_unk6FAAE624`/`0` in strict offset order).
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F48F130 - vtable slot 4.  Just two tail calls: clear m_field0xF8
    //  through the refcounted-pointer-assign helper (own translation unit,
    //  Agent/prclustergen_helpers.cpp - 0x6F48EE90, decrements the OLD
    //  pointer's own +0x1C refcount field before storing the new one), then
    //  tail-jump straight into `SHandleTableEntry::UnregisterAndNotify`
    //  (Pathfinding/handletable.h) with the incoming stack argument passed
    //  through unread by this function itself, matching that callee's own
    //  documented "arg is dead" note.
    void Slot4(int arg);

    //  0x6F48F140 - vtable slot 9, Save.  Eleven scalar/CFloat fields at
    //  +0xDC..+0x100 (four writers: Dword/DwordAlt2/WriteCFloat, in offset
    //  order), then the two handle-tag-pair references at +0xF4 (validated
    //  the same way CPrCluster::Save's own +0x78 is - null if
    //  `*(ptr+0x38)==-1`)/+0xF8 through WritePathRefEntry
    //  (Pathfinding/pathref_write.cpp), then the fixed 12-element
    //  `m_refSlots` array: each slot's own three dwords (Dword/Dword/
    //  DwordAlt2), followed by ONE more write per slot - not the slot's own
    //  field, but a boolean derived from the PARALLEL `m_resolved[i]`
    //  pointer (`1` if `m_resolved[i] == (CHandleObject*)-1`, else `0`) -
    //  see Load's own doc comment for why streaming a boolean is enough to
    //  reconstruct `m_resolved[i]` on the way back in.
    void Save(CDataStoreScratch* store);

    //  0x6F491FF0 - vtable slot 10, Load.  Save's mirror through the Read*
    //  family, with the same "+0xF4/+0xF8 read back unvalidated" and
    //  "+0xF0 zeroed, never saved" asymmetries CPrCluster::Load's own doc
    //  comment already establishes for this family, plus one asymmetry of
    //  its own: the fixed loop reads each `m_refSlots[i]` triple back
    //  faithfully, then reads one more scratch dword (never stored into the
    //  struct) and branches on it - nonzero means "was empty", storing the
    //  `-1` sentinel straight into `m_resolved[i]`; zero means
    //  `LookupHandle(m_refSlots[i].m_handle, m_refSlots[i].m_typeTag)`
    //  (Game/game.h) resolves the live pointer instead.  So Save's own
    //  boolean (see its own doc comment) really means "was `m_resolved[i]`
    //  ever non-null", and Load re-derives the pointer from the handle
    //  pair rather than streaming it - the same "Load re-derives what Save
    //  does not stream" shape this family's own CPrClusterGroup::Load
    //  already documents for its own live-list membership.
    void Load(CDataStore* store);

    //  0x6F491790 - vtable slot 15, ComputeChecksum.  See
    //  Agent/prclustergen_checksum_dump.cpp's own doc comment.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F491BC0 - vtable slot 16, DumpDebugState.  Same file.
    void DumpDebugState(void* sink);

    //  0x6F48F0A0 - callee of Apply above, own translation unit
    //  (Agent/prclustergen_helpers.cpp) so that real `call` stays real.
    //  See Apply's own doc comment.
    void ResetRefSlots();

    //  0x6F48EE90 - callee of Slot4 above, same own-translation-unit reason.
    //  See Slot4's own doc comment.
    void AssignField0F8(void* value);

    void*        m_vtable;          // +0x00
    void*        m_prevLink;        // +0x04
    void*        m_nextLink;        // +0x08
    char         m_reserved0C[8];    // +0x0C..+0x14 - untouched by this
                                      //   call tree
    unsigned int m_tagT;             // +0x14 - CPresenceTagged's own tag
                                      //   pair; ALSO this class's own
                                      //   handle-table slot/index pair (see
                                      //   Apply's own doc comment) - the
                                      //   same field playing both roles the
                                      //   way CPrCluster's own m_tagT does
    unsigned int m_tagB;             // +0x18
    SPrClusterGenRefSlot m_refSlots[12]; // +0x1C..+0xAC
    CHandleObject*       m_resolved[12]; // +0xAC..+0xDC - see Load's own
                                          //   doc comment; `-1` is a real
                                          //   sentinel value here, not a
                                          //   null-pointer stand-in
    unsigned int m_field0DC;   // +0xDC
    unsigned int m_field0E0;   // +0xE0
    unsigned int m_field0E4;   // +0xE4 (Dword"Alt2"/"Alt3" variant)
    CFloat       m_field0E8;   // +0xE8
    CFloat       m_field0EC;   // +0xEC
    unsigned int m_field0F0;   // +0xF0 - Apply-only, never saved/loaded
    void*        m_field0F4;   // +0xF4 - validated on Save (see Save's own
                                //   doc comment); plain assignment, no
                                //   refcounting
    void*        m_field0F8;   // +0xF8 - refcounted (see Slot4's own doc
                                //   comment)
    CFloat       m_field0FC;   // +0xFC
    unsigned int m_field100;   // +0x100 (Dword"Alt2"/"Alt3" variant)
};

//----------------------------------------------------------------------------
//  0x6F47AE70/0x6F47B0A0/0x6F47B0E0/0x6F48ED30(idx3)/0x6F48ED60(idx4)/
//  0x6F48EDA0(idx9)/0x6F48EDE0(idx10)/0x6F47AE80(idx15)/0x6F47AFC0(idx16) -
//  slots 3/4/9/10/15/16 closed in the cluster-D follow-up session
//  (NTempestNIpse cluster D, 2026-09-12), completing this class per
//  docs/targets/NTempestNIpse_ClusterD.md's own "start with
//  CPrClusterGroup's own slots 3/4/9/10" recommendation, plus 15/16 -
//  see this file's own header comment and the follow-up doc for the full
//  per-slot writeup.
//
//  Slot 15's own ComputeChecksum calls `((NTempest::CPresenceTagged*)this)
//  ->FoldLinkFieldsIntoHash(hash)` and slot 16's own DumpDebugState prints
//  "[PrTg] t[%u] b[%u]" off this object's own +0x14/+0x18 exactly the way
//  Agent/presencetagged_dumpdebugstate.cpp's own CPresenceTagged and
//  Missile/bhpoprojectile_checksum_dump.cpp's own CBhPoProjectile both do -
//  the SAME flat, address-only field reuse those two already establish,
//  NOT a contradiction of this file's own "these three classes sit
//  directly on CPresence, not CPresenceTagged" destructor-restamp finding
//  above (CBhPoProjectile does exactly both at once: restamps to
//  `??_7CPresence@NTempest@@6B@` on destruction while still reusing
//  CPresenceTagged's own tag-fold/tag-print bodies by flat reinterpret-cast
//  for its own +0x14/+0x18 pair - a class need not really derive from
//  CPresenceTagged in C++ to share its field layout at those two offsets).
//  +0x14/+0x18 also double as `SHandleTableEntry`'s own m_slotHandle/
//  m_slotIndex (Pathfinding/handletable.h) - confirmed by slot 3 below,
//  which casts `this` straight to `SHandleTableEntry*` to call
//  `RegisterOrReuse` against the SAME offsets.
//----------------------------------------------------------------------------
class CPrClusterGroup
{
public:
    //  0x6F47AE70 - vtable slot 0.
    const char* GetTypeName();

    //  0x6F47B0A0 - vtable slot 1.  Same shape as
    //  NTempest::CPresence::ReturnToStaticPool.
    void ReturnToStaticPool();

    //  0x6F47B0E0 - vtable slot 2, scalar deleting destructor.  Same
    //  "trivial" no-teardown shape as CPrClusterGen's own above.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F48ED30 - vtable slot 3, "Apply" (same slot/name/signature as
    //  every other class in this whole family - Agent/popos.h's own
    //  Apply, Pathfinding/crlagent.h's own).  Registers (or re-registers)
    //  this object's own +0x14/+0x18 handle-table entry via
    //  `SHandleTableEntry::RegisterOrReuse` (Pathfinding/handletable.h,
    //  already real), then publishes `this` onto the live-instance list
    //  rooted at `g_pTimeSync + 0x384` (own translation unit's own
    //  SAgentLiveInstanceListHead::InsertAtHead, 0x6F48E610 - the FIRST
    //  reconstructed writer of this shape; every ReturnToStaticPool in
    //  this whole family only ever UNLINKS from a list like this one, and
    //  nothing until this slot ever INSERTS - see that type's own header
    //  comment).  Likely reached from this class's own (unreconstructed -
    //  see this file's own top header comment) constructor, though nothing
    //  in this call tree proves that directly.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F48ED60 - vtable slot 4, "Slot4" (same generic fallback name
    //  Agent/presence.h's own CPresence::Slot4 and Agent/popos.h's own
    //  CPoPos::Slot4 already use for this exact role one level up this
    //  family). Unlinks `this` from whichever doubly-linked list it
    //  currently sits in via its own +0x04/+0x08 prev/next pair (the
    //  SAME generic unlink idiom ReturnToStaticPool above already uses,
    //  NOT the +0x14/+0x18 handle-table pair Apply above touches),
    //  resets `m_vpr`/`m_vcr` to the shared non-zero default constant
    //  (`g_unk6FAAE624`, Math/floatmini.h) and `m_lir` to `g_CFloatZero`
    //  (Math/CFloat.h) - leaving `m_mbr` untouched - then tail-calls
    //  `SHandleTableEntry::UnregisterAndNotify` (0x6F4AEF00,
    //  Pathfinding/handletable.h) against this object's own +0x14/+0x18
    //  handle-table entry.
    void Slot4(int arg);

    //  0x6F48EDA0 - vtable slot 9, Save.  Writes `m_mbr` via
    //  `CDataStoreScratch::WriteDword` (Net/cdatastorescratch.h) then
    //  `m_vpr`/`m_vcr`/`m_lir`, in that offset order, via the already-
    //  reconstructed `WriteCFloat` (Net/cdatastore_writecfloat.cpp) -
    //  same fluent `store->WriteX(...)->WriteY(...)` chain-of-`this`-
    //  returns shape Agent/poposbh.h's own Save already establishes for
    //  a sibling class.  NOT preceded by any base-class Save call (unlike
    //  CPoPos's own siblings, which call `((CRlProperty*)this)->Save(...)`
    //  first) - nothing in this class's own dump calls into a base Save,
    //  matching this file's own "sits directly on CPresence" finding
    //  (CPresence itself has no Save/Load vtable slot of its own to call).
    void Save(CDataStoreScratch* store);

    //  0x6F48EDE0 - vtable slot 10, Load.  Save's exact mirror
    //  (`CDataStore::ReadDword`/the already-reconstructed `ReadCFloat`,
    //  Net/cdatastore_readcfloat.cpp) EXCEPT for one real asymmetry at the
    //  end: after reading all four fields back, Load also re-publishes
    //  `this` onto the same live-instance list Apply's own tail does
    //  (`g_pTimeSync + 0x384`, `SAgentLiveInstanceListHead::InsertAtHead`)
    //  - loading an instance back in re-registers it as live the same way
    //  constructing one presumably does, matching the same
    //  "Load re-derives what Save does not stream" shape Agent/popos.h's
    //  own Load doc comment already flags for a sibling class (there, the
    //  sync channel; here, live-list membership).
    void Load(CDataStore* store);

    //  0x6F47AE80 - vtable slot 15, ComputeChecksum.  Folds the base
    //  `((NTempest::CPresenceTagged*)this)->FoldLinkFieldsIntoHash(hash)`
    //  (0x6F4AEE60, Agent/presence.h) first, then this class's own four
    //  added fields - `m_mbr`, `m_vpr`, `m_vcr`, `m_lir`, in that offset
    //  order - through the same per-byte `(b*0x7A2D)^b`-then-add fold
    //  idiom Agent/agentbaseabscomputechecksum.cpp's own
    //  `AgentBaseAbsFoldDword` and Missile/bhpoprojectile_checksum_dump.cpp's
    //  own `BhPoFoldDword` already duplicate locally for their own,
    //  unrelated callers (own local copy here for the same reason: no
    //  shared translation unit across Agent/ and Missile/).
    void ComputeChecksum(unsigned int* hash);

    //  0x6F47AFC0 - vtable slot 16, DumpDebugState.  Same three-`LogFormatted`-
    //  call shape as every sibling dump in this family
    //  (Storm/textsink.h, 0x6F4B73F0): an empty line, then
    //  "[PrTg] t[%u] b[%u]" off this object's own +0x14/+0x18 (SAME format
    //  string and SAME field pair CPresenceTagged's own DumpDebugState
    //  prints - see this class's own header comment on why that is not a
    //  contradiction), then "[PrClGroup]" and finally
    //  "\tmbr[%u] vpr[%s] vcr[%s] lir[%s]" - `m_mbr` printed raw, the three
    //  CFloats through `FormatCFloat` (0x6F4B76E0, base 10/precision -1,
    //  same call shape as every other `FormatCFloat` user in this repo).
    //  Argument order confirmed by the dump's own push order (deepest
    //  push = last variadic argument, cdecl): `lir` is pushed first
    //  (evaluated last in source order) and `mbr` last (evaluated first),
    //  matching left-to-right format-string order once cdecl's own
    //  right-to-left push is accounted for.
    void DumpDebugState(void* sink);

    void*        m_vtable;          // +0x00
    void*        m_prevLink;        // +0x04
    void*        m_nextLink;        // +0x08
    char         m_reserved0C[8];   // +0x0C-+0x13 - untouched by this
                                     //   session's own call tree
    unsigned int m_tagT;             // +0x14 - also SHandleTableEntry's own
                                      //   m_slotHandle (see this class's own
                                      //   header comment)
    unsigned int m_tagB;             // +0x18 - also m_slotIndex
    unsigned int m_mbr;               // +0x1C - "mbr[%u]" (DumpDebugState)
    CFloat       m_vpr;                // +0x20 - "vpr[%s]"
    CFloat       m_vcr;                // +0x24 - "vcr[%s]"
    CFloat       m_lir;                // +0x28 - "lir[%s]"
};

}  // namespace NIpse

namespace NTempest {

//----------------------------------------------------------------------------
//  0x6F952624 - NTempest::CLinkedList<NIpse::CPrCluster>.  This image was
//  linked without /OPT:ICF, so an unmodified inherited virtual still gets
//  its own, separately-compiled copy at the derived class's own vtable
//  slot - the same "duplicate compiled body, different address" shape
//  Misc/ntempest_cpmquery_scalardtors.cpp's own CPmQuery_CPoQu/CPmQuery_CSeQu
//  already document.  Every slot but 2 is inherited unchanged from
//  CPrCluster's own vtable (this session's own class-vtable diff); only
//  slot 2 is a real override, reached through this template instantiation's
//  own distinct address, so only that one method needs its own tiny struct
//  here.
//----------------------------------------------------------------------------
struct CLinkedList_CPrCluster
{
    //  0x6F488620 - vtable slot 2 of NTempest::CLinkedList<CPrCluster>
    //  (0x6F952624).  Byte-for-byte the same body as
    //  CPrCluster::ScalarDeletingDestructor above (calls the same real
    //  ~CPrCluster(), same `this != 0` guard) - a second, separately
    //  addressed compiled copy of the identical source, not a real
    //  override.
    void* ScalarDeletingDestructor(unsigned int flags);
};

//----------------------------------------------------------------------------
//  0x6F952274 - NTempest::CLinkedList<NIpse::CPrClusterGroup>.  Same
//  "duplicate compiled body" shape as CLinkedList_CPrCluster above; slot 2
//  is CPrClusterGroup's own "trivial" no-teardown shape, recompiled at its
//  own address.
//----------------------------------------------------------------------------
struct CLinkedList_CPrClusterGroup
{
    //  0x6F486510 - vtable slot 2 of NTempest::CLinkedList<CPrClusterGroup>
    //  (0x6F952274).
    void* ScalarDeletingDestructorTrivial(unsigned int flags);
};

}  // namespace NTempest

#endif  // PRCLUSTER_H

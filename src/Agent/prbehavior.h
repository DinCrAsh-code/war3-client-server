//============================================================================
//  NIpse::CPrBehavior (`??_7CPrBehavior@NIpse@@6B@`, 0x6F951D44, 19 slots) -
//  docs/targets/NTempestNIpse_CPrBehavior.md (NTempestNIpse cluster B,
//  2026-09-08).
//
//  Ground truth for the whole base chain this branches off of lives in
//  Pathfinding/crlagent.h; the piece that matters here is the note this
//  session's own task list already flagged: unlike CPrRelation/CRlAgent/
//  CAgentBaseAbs (which all branch off *below* CPresenceTagged, through
//  NIpse::CPrRelation), CPrBehavior's own vtable slots 15/16 are inherited
//  UNCHANGED from CPresenceTagged directly (0x6F4AEE60/0x6F479C30 -
//  FoldLinkFieldsIntoHash/DumpDebugState, Agent/presence.h) - CPrRelation's
//  own overrides of those same two slots (0x6F4A60A0/CAgentRelation's own
//  DumpDebugState-equivalent) never appear here. So CPrBehavior derives
//  from NTempest::CPresenceTagged directly, a different branch point in
//  this family than every other class this repo has reconstructed off this
//  chain so far.
//
//  A REAL, CONFIRMING CROSS-CHECK: CPresenceTagged's own +0x14/+0x18 pair
//  (presence.h's `m_tagT`/`m_tagB`, printed by its own DumpDebugState as
//  "[PrTg] t[%u] b[%u]") turns out to be exactly SHandleTableEntry's own
//  m_slotHandle/m_slotIndex (Pathfinding/handletable.h) - AttachHandle below
//  calls SHandleTableEntry::RegisterOrReuse with `this` passed completely
//  unadjusted (no `lea` before the call), which only writes the correct
//  fields if CPrBehavior's own +0x14/+0x18 really are that base's handle
//  slot, inherited unchanged. That reads as the origin of the class NAME
//  "PresenceTagged" in the first place - a presence "tagged" with a live
//  handle-table registration - rather than two arbitrary debug numbers,
//  though nothing in this call tree renames presence.h's own fields (they
//  stay `m_tagT`/`m_tagB` there, this file just does not re-declare new
//  ones at the same offsets).
//
//  CPrBehavior itself is abstract: its own vtable slot 17 (+0x44, idx17) is
//  the shipped image's shared `_purecall` trampoline (0x6F7E10C2 - see
//  purecallstub.cpp), never a real body at this level. DispatchIfEnabled
//  below dispatches through that exact slot on every live object, so a real
//  behaviour only exists once a concrete derived class (this task's own
//  note names NIpse::CBhPoBehavior as the likely candidate, sharing slots
//  15/16 the same way) overrides it.
//
//  Flat, offset-addressed placeholder like every other unmodeled base in
//  this chain (Pathfinding/crlagent.h's own file header explains why) -
//  no real C++ inheritance from CPresenceTagged modelled here, so the
//  layout below restates only the offsets this call tree actually reads or
//  writes, from +0x00 (base-chain territory, reserved) on up.
//============================================================================
#ifndef PRBEHAVIOR_H
#define PRBEHAVIOR_H

#include "game.h"

//  SHandleRegistrationRequest's own +0x0C is not itself named by
//  Pathfinding/handletable.h (only +0x24/+0x28 are) - AttachHandle below
//  reads it directly by raw offset rather than adding a name to that shared
//  struct for a field only this one call site touches (CLAUDE.md's own
//  "do not invent fields the call tree does not read" - this field IS read,
//  just not characterized beyond "an id copied into the caller's own state
//  when a request is supplied").
struct SHandleRegistrationRequest;

namespace NIpse {

class CPrBehavior
{
public:
    //  0x6F479D70 - vtable slot 0. `retn` with zero stack bytes, no `this`
    //  read - the literal string "PrBehavior". Same shape as every other
    //  GetTypeName in this chain (Agent/presence.h, Agent/agentrelation.h,
    //  Pathfinding/crlagent.h).
    const char* GetTypeName();

    //  0x6F479E00 - vtable slot 1. Same unlink-then-return-to-pool shape as
    //  every other ReturnToStaticPool in this chain
    //  (Agent/presence_returntopool.cpp and siblings), against this class's
    //  own static pool (CPrBehavior_GetStaticPool,
    //  agentbasechain_staticpools.h - g_pTimeSync + 0x618).
    void ReturnToStaticPool();

    //  0x6F479D90 - vtable slot 2, scalar deleting destructor. BYTE-IDENTICAL
    //  (down to raw_bytes, apart from the call displacement) to the other
    //  three degenerate scalar-deleting-destructors this chain already has
    //  at slot 2 (Agent/presence.h's own file header has the full oddity):
    //  no base teardown, no `this != 0` guard, restamps straight to
    //  `??_7CPresence@NTempest@@6B@` regardless of which class's vtable this
    //  was actually reached through. Named `ScalarDeletingDestructorTrivial`
    //  for the same reason presence.h's own copy is.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4AA340 - vtable slot 3. (Re)binds this object's handle-table
    //  registration (SHandleTableEntry::RegisterOrReuse, handletable.h -
    //  `this` reinterpreted unadjusted, see this file's own header comment
    //  on why +0x14/+0x18 really are that base's slot pair here) and then
    //  remembers an id out of `request` for later use: -1 if `request` is
    //  null, otherwise `request`'s own +0x0C dword. Called directly (not
    //  through the vtable) by this class's own constructor with a fixed
    //  request, and reachable live through this slot too - name and field
    //  meaning are a best-effort reading of the shape alone; nothing in
    //  this call tree characterizes the +0x0C field beyond "copied here as
    //  m_field34".
    void AttachHandle(SHandleRegistrationRequest* request);

    //  0x6F4AA550 - vtable slot 4. A bare tail sequence, no register saved
    //  across it: release both pending sync registrations
    //  (Agent/agenttickrelay.h's own CAgentTickRelay::ReleasePendingA/B,
    //  `this` reinterpreted the same way AttachHandle reinterprets it for
    //  RegisterOrReuse - same generic "retire the registration and clear
    //  the pointer" shape against this class's own m_registration/
    //  m_channelReg at the same +0x1C/+0x20 offset CAgentRelation's own
    //  pair sits at, Agent/agentrelation.h), then tail-jump straight into
    //  SHandleTableEntry::UnregisterAndNotify(arg) - same shape as
    //  CAgentRelation::Teardown (Agent/agentrelationteardown.cpp), just
    //  compiled lean enough here that MSVC never needs to spill `this` to a
    //  callee-saved register, so the whole body degenerates to two calls
    //  and one tail jmp with no prologue/epilogue at all.
    void Teardown(int arg);

    //  0x6F4AA370 - vtable slot 8. Flag-guarded dispatch through this
    //  object's own vtable slot 17 (+0x44 - DispatchIfEnabled below is the
    //  ONLY caller of that slot in this call tree): skip the call only when
    //  m_flags bit 2 is set and bit 8 is clear; otherwise set bit 4 for the
    //  duration of the call (a reentrancy guard) and clear it again after.
    //  Slot 17 itself is `_purecall` at this level (see this file's own
    //  header comment), so calling it through a CPrBehavior object with no
    //  concrete override crashes live - this only ever does something once
    //  a real derived class exists.
    void DispatchIfEnabled(int arg);

    //  0x6F4AA450 - vtable slot 9. Serialize five own fields (m_field24/
    //  m_field28/m_field2C/m_flags/m_field34, in that order - the two
    //  "Alt2"-suffixed writer calls are the already-established byte-
    //  identical duplicate of CDataStoreScratch::WriteDword,
    //  Agent/*_returntopool.cpp's own sibling precedent for this shipped
    //  binary's duplicate-address idiom, not a semantic distinction), then
    //  both sync registrations (m_registration, m_channelReg) via
    //  WriteSyncRegistrationRecordForPrBehavior - the exact same body as
    //  Agent/agentrelationwritesyncrecord.cpp's own WriteSyncRegistrationRecord,
    //  compiled a second time at a different address (same duplicate-body
    //  idiom, see prbehaviorsyncrecord.cpp). Unlike CAgentRelation::Save
    //  (Agent/agentrelationsyncsave.cpp), this body never threads the
    //  writer's own return value through - it always re-loads `writer`
    //  itself for the next call, which produces the identical result only
    //  because every one of these writer methods returns `this` unchanged.
    void Save(void* writer);

    //  0x6F4AA7C0 - vtable slot 10, the Load counterpart to Save above:
    //  read the same five fields back with CDataStore::ReadDword/
    //  ReadDwordAlt3 (Pathfinding/handletableassignid.cpp's own sibling
    //  precedent), then reconstruct both sync registrations through two
    //  calls to sub_6F4AA720 - a deep, genuinely new "allocate and load one
    //  SSyncRegistration-shaped CSIRequest object" leaf this session left a
    //  naked redirect for (prbehaviorloadhelpers.cpp) rather than
    //  reconstructing: reaching depth 5+ into an entirely new allocator
    //  pattern this cluster's own ten addresses never asked for. The
    //  shipped body still runs correctly through the redirect.
    void Load(void* reader);

    unsigned char m_reserved00[0x1C];    // +0x00-0x1B, base-chain territory
                                          //   (CPresence/CPresenceTagged's
                                          //   own fields, incl. the
                                          //   +0x14/+0x18 handle-table pair
                                          //   this file's header discusses -
                                          //   left unnamed here since every
                                          //   access to it goes through the
                                          //   SHandleTableEntry/CAgentTickRelay
                                          //   reinterpret-casts above, never
                                          //   directly)
    SSyncRegistration* m_registration;   // +0x1C
    SSyncRegistration* m_channelReg;     // +0x20
    unsigned int        m_field24;       // +0x24
    unsigned int        m_field28;       // +0x28
    unsigned int        m_field2C;       // +0x2C
    unsigned int        m_flags;         // +0x30
    unsigned int        m_field34;       // +0x34
};

}  // namespace NIpse

#endif  // PRBEHAVIOR_H

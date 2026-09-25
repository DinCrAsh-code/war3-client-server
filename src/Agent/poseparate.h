//============================================================================
//  NIpse::CPoSeparate (`??_7CPoSeparate@NIpse@@6B@`, 0x6F9521B4, 13 slots)
//  and NTempest::CLinkedList<NIpse::CPoSeparate> (`??_7?$CLinkedList@V
//  CPoSeparate@NIpse@@@NTempest@@6B@`, 0x6F9522BC, same 13 slots except its
//  own override of slot 2) - the smallest class in the deepest,
//  most-derived part of the whole NTempest/NIpse "positioning/relation"
//  family (docs/targets/NTempestNIpse_BaseChain.md,
//  docs/targets/NTempestNIpse_SmallClasses.md).
//
//  **Naming warning, unrelated to this repo's own already-reconstructed
//  Position family**: this class and its siblings (CPoPos, CPoPosCl,
//  CPoPosBh - a future session's own scope) are `NIpse`-namespaced
//  positioning classes with textually similar names to, but NOTHING TO DO
//  WITH, the already-reconstructed `Position`/`PositionTrack`/
//  `SmartPosition`/`ProjectilePosition` family (docs/targets/
//  Position-family.md, Pathfinding/positiontrack.h). That family's real
//  RTTI vtable is the plain, unnamespaced `??_7Position@@6B@` at a
//  completely different address (0x6F877DDC) from anything in this file -
//  the same kind of same-sounding-but-unrelated collision
//  Position-family.md's own writeup already flags against
//  Widget/position.h's unrelated `Position` record. Nothing below derives
//  from, wraps, or is used by that family; the shared "Po"/"Position"-ish
//  spelling is a coincidence of this binary's own naming, not a
//  relationship.
//
//  ---------------------------------------------------------------------
//  What CPoSeparate is, from its own vtable slots:
//  ---------------------------------------------------------------------
//  Slots 0-2 and 5-8 are the exact same shape every class in this family's
//  base chain uses (Agent/presence.h, Pathfinding/crlagent.h): slot 0 is a
//  bare `GetTypeName` returning a literal ("PoSeparate"), slot 1 unlinks
//  `this` from its own +0x04/+0x08 doubly-linked list of live instances and
//  pushes it onto a dedicated static free-list pool
//  (`CPoSeparate_GetStaticPool`, Agent/agentbasechain_staticpools.h - the
//  fifth of these one-instruction leaves, `g_pTimeSync + 0x7F8`), slot 2 is
//  the same byte-identical-across-the-whole-chain scalar deleting
//  destructor that restamps straight to `??_7CPresence@NTempest@@6B@` with
//  no base teardown (presence.h's own header comment has the open
//  question), and slots 5-8 are the four shared no-ops
//  (`nullsub_1473..1476`) every class in this chain inherits unchanged.
//
//  CPoSeparate's own vtable is genuinely only 13 slots long - there is no
//  shared 17/18-slot tail the way the base-chain classes have, so this is
//  NOT modelled as inheriting from `NIpse::CRlAgent`/`CAgentBaseAbs`; it is
//  a much shallower, independent branch off the same `CPresence` root
//  (confirmed by slot 2's own restamp target).  Own fields, from the
//  genuinely new slots (3, 4, 9-12):
//
//    +0x14  m_field14   - a plain dword, zeroed by slot 4 (Slot4 below);
//                          meaning not otherwise established in this
//                          cluster (not read or written anywhere else this
//                          session reached).
//    +0x18  m_sepvX      - CFloat, the "sepv x" DumpDebugState (slot 10)
//                          prints; also what ComputeChecksum (slot 9)
//                          folds and what Save/Load (slots 11/12) stream.
//    +0x1C  m_sepvY      - CFloat, "sepv y", same three call sites.
//    +0x20  m_opt        - a plain dword, "opt[0x%08X]" in the same debug
//                          dump, folded by ComputeChecksum and streamed by
//                          Save/Load as a fourth (non-CFloat) field.
//
//  (+0x00-+0x13 is base-chain territory this cluster does not touch -
//  vtable pointer, the +0x04/+0x08 link pair slot 1 already accounts for,
//  and 8 reserved bytes.)
//
//  Slot 3 ("Apply", matching this whole family's own per-column naming
//  convention - Pathfinding/crlagent.h's own `Apply`/`Slot4`/`Slot17`
//  precedent names a slot for its *column*, not what any one class's own
//  override happens to do) resets the separation state: both CFloats back
//  to `g_CFloatZero`'s own encoded bits and `m_opt` to 0. Slot 4 ("Slot4",
//  same convention) is CPoSeparate's own real override of the column
//  `CPresence::Slot4` (presence.h) leaves a bare tail-call: it unlinks
//  `this` from the +0x04/+0x08 list and zeroes `m_field14` FIRST, then
//  still tail-calls through `this`'s own vtable slot 1
//  (`ReturnToStaticPool`) exactly the way `CPresence::Slot4` does - so a
//  live call ends up unlinking the (already-emptied) +0x04/+0x08 pair a
//  second time, harmlessly, before the pool push. Transcribed exactly as
//  the shipped double-unlink, not "cleaned up" into one - see
//  Agent/poseparate_slot4.cpp.
//
//  Slots 9-12 stream `m_sepvX`/`m_sepvY`/`m_opt` through the same generic
//  helpers this family's own base chain uses (Agent/
//  agentbaseabscomputechecksum.cpp's byte-fold idiom for slot 9;
//  Storm/textsink.h's `LogFormatted`/`FormatCFloat` for slot 10;
//  Net/cdatastorescratch.h's `WriteCFloat`/`WriteDwordAlt2` and
//  Net/cdatastore.h's `ReadCFloat`/`ReadDwordAlt3` for slots 11/12 - see
//  each function's own .cpp for the exact evaluation-order reasoning that
//  ties the disassembly's own stack-reuse trick back to a real C++ call
//  shape).
//
//  ---------------------------------------------------------------------
//  NTempest::CLinkedList<NIpse::CPoSeparate>:
//  ---------------------------------------------------------------------
//  Shares every one of CPoSeparate's own 13 slot targets EXCEPT slot 2,
//  which it overrides with its own, separately-compiled address
//  (0x6F486580) - byte-identical to CPoSeparate's own slot 2 body (same
//  restamp-to-CPresence-vftable, same Storm free, this image's own
//  /OPT:ICF-less linking again), so it earns its own tiny struct
//  (`CLinkedList_CPoSeparate`) the same way the small-classes session's own
//  `CDynTable_CPoPosClPtr`/`CMemBlockT_CPoPosClPtr` do
//  (Misc/ntempest_dyntable_scalardtors.cpp) rather than being folded into
//  `CPoSeparate` itself - modelled as the real base per this cluster's own
//  scope note (the plain class is the base, `CLinkedList<T>` inherits it
//  unchanged except where it overrides, matching `CRlAgent`/
//  `CLinkedList<CRlAgent>`'s own already-established precedent), with only
//  the one slot that actually differs getting its own address.
//============================================================================
#ifndef POSEPARATE_H
#define POSEPARATE_H

#include "CFloat.h"

namespace NIpse {

class CPoSeparate
{
public:
    //  0x6F4843B0 - vtable slot 0. Literal "PoSeparate", no `this` read.
    const char* GetTypeName();

    //  0x6F484540 - vtable slot 1. Same shape as
    //  NTempest::CPresence::ReturnToStaticPool (Agent/presence_returntopool.cpp):
    //  unlink `this` from its own +0x04/+0x08 live-instance list, then push
    //  it onto CPoSeparate's own static free-list pool
    //  (CPoSeparate_GetStaticPool).
    void ReturnToStaticPool();

    //  0x6F484580 - vtable slot 2, scalar deleting destructor. Byte-identical
    //  to the whole chain's own no-base-teardown restamp-to-CPresence
    //  triplet (Agent/presence.h's own header comment) - not a coincidence,
    //  the same open finding, just a fourth (and, with
    //  CLinkedList<CPoSeparate>'s own slot 2 below, fifth) member of it.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F495010 - vtable slot 3, "Apply" (column name, see this file's
    //  own header comment). Resets the separation state: both CFloats to
    //  g_CFloatZero's own bits, m_opt to 0. `flags` is read nowhere in the
    //  body - the same "declared but unused" shape this whole family's own
    //  slot-1 overrides already share (presence.h's own CPresence::Slot4
    //  comment).
    void Apply(unsigned int flags);

    //  0x6F495030 - vtable slot 4, "Slot4" (column name). See this file's
    //  own header comment for the real double-unlink this override adds on
    //  top of CPresence::Slot4's bare tail call.
    void* Slot4(unsigned int flags);

    //  0x6F4843C0 - vtable slot 9, ComputeChecksum. Folds m_sepvX, m_sepvY
    //  and m_opt (in that offset order) into *hash via the same per-byte
    //  `(b*0x7A2D)^b` fold, summed by addition, that
    //  Agent/agentbaseabscomputechecksum.cpp already establishes for this
    //  family - own copy of the helper (own translation unit, no shared
    //  header for it yet).
    void ComputeChecksum(unsigned int* hash);

    //  0x6F4844B0 - vtable slot 10, DumpDebugState. Prints "\t[PoSe]" then
    //  "\t\tsepv x[%s] y[%s] opt[0x%08X]" with m_sepvX/m_sepvY formatted
    //  through FormatCFloat and m_opt as the raw dword - see
    //  Agent/poseparate_dumpdebugstate.cpp for the argument-evaluation-order
    //  reasoning that ties the shipped stack-reuse trick back to this
    //  exact call shape.
    void DumpDebugState(void* sink);

    //  0x6F495070 - vtable slot 11, Save. Streams m_sepvX, m_sepvY (each via
    //  WriteCFloat) then m_opt (via CDataStoreScratch::WriteDwordAlt2) into
    //  the caller's CDataStoreScratch.
    void Save(void* store);

    //  0x6F4950A0 - vtable slot 12, Load. Save's exact counterpart: reads
    //  m_sepvX, m_sepvY (each via ReadCFloat) then m_opt (via
    //  CDataStore::ReadDwordAlt3) from the caller's CDataStore.
    void Load(void* store);

    //  0x6F494FB0/0x6F494FD0/0x6F494FF0 - three tiny nibble-packing
    //  setters into m_opt (+0x20), reached from Agent/poposcl.h's own
    //  CPoPosCl::Slot4/::Load helper (sub_6F495ED0,
    //  Agent/poposcl_relatedagentslot4.cpp) right after allocating a fresh
    //  CPoSeparate - each transcribed exactly per its own shipped mask
    //  (which does not line up with the shifted field's own natural
    //  width; kept as-shipped rather than "corrected" to a wider mask).
    //  Named generically (`SetOptNibbleAtBitN`) since nothing in this call
    //  tree establishes what each nibble means beyond its own bit
    //  position.
    void SetOptNibbleAtBit16(unsigned char value);   // 0x6F494FB0
    void SetOptNibbleAtBit20(unsigned short value);  // 0x6F494FD0
    void SetOptNibbleAtBit28(unsigned char value);   // 0x6F494FF0

    //  +0x00-+0x13 - base-chain territory (vtable pointer, the +0x04/+0x08
    //  live-instance link pair slot 1 owns, and 8 reserved bytes) not
    //  modelled here; see Pathfinding/crlagent.h's own file header for why
    //  this whole chain stays flat rather than a real C++ inheritance tree.
    char            m_reserved00_14[0x14];
    unsigned int    m_field14;   // +0x14 - zeroed by Slot4; meaning open.
    CFloat          m_sepvX;     // +0x18
    CFloat          m_sepvY;     // +0x1C
    unsigned int    m_opt;       // +0x20
};

//  The static-pool ALLOCATE side (0x6F4958D0/0x6F495AD0/0x6F495C70) is
//  declared in poseparate_pool.h, not here - it needs CPoSeparate's own
//  definition (above) to be complete first.

//  NTempest::CLinkedList<NIpse::CPoSeparate>'s own single override - see
//  this file's own header comment.
struct CLinkedList_CPoSeparate
{
    //  0x6F486580 - vtable slot 2 of NTempest::CLinkedList<NIpse::CPoSeparate>
    //  (0x6F9522BC). Byte-identical to CPoSeparate::ScalarDeletingDestructor
    //  above.
    void* ScalarDeletingDestructor(unsigned int flags);
};

}  // namespace NIpse

#endif  // POSEPARATE_H

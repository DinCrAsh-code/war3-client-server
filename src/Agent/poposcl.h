//============================================================================
//  NIpse::CPoPosCl (`??_7CPoPosCl@NIpse@@6B@`, 0x6F952554, 24 slots) and
//  NTempest::CLinkedList<NIpse::CPoPosCl> (`??_7?$CLinkedList@VCPoPosCl@
//  NIpse@@@NTempest@@6B@`, 0x6F95266C, same 23 slots except its own
//  override of slot 2) - CPoPos's (popos.h) other sibling extension,
//  part of the same "PoPos*" cluster-H follow-up as CPoPosBh (poposbh.h).
//
//  Slots 5-8, 11-14 and 17 are the shared no-ops (see popos.h's own header
//  comment). Slot 18 is the shared `CAgentRelation::OnRegistrationRetired`
//  - CPoPosCl never overrides it (unlike CPoPosBh).
//
//  FLAT, offset-addressed, exactly like popos.h's own CPoPos and
//  poposbh.h's own CPoPosBh (NOT `: public CPoPos` - see popos.h's header
//  comment for why). CPoPosCl's own added fields start at the shipped
//  +0x9C - immediately after CPoPos's own last modelled field at +0x98,
//  unlike CPoPosBh's own +0xC0 (the two are independent siblings, not one
//  derived from the other).
//
//  CPoPosCl's own added fields, from ComputeChecksum's own fold order and
//  DumpDebugState's own now-FULLY-traced four dump lines ("\tclus t[%u]
//  b[%u]", "\tglr[%s] prw[%s] acc[%s] ang[%s]", "\tlmg[%s] ldl[%s] lor[%s]
//  lod[%s] pcl x[%u] y[%u] opt[0x%08X]" - the earlier session's own dump
//  fragment cut this last string off mid-word; this follow-up session
//  re-read it whole via `worktree_store.py resolve aLmgSLdlSLorSLo`):
//
//    +0x9C  m_clusT   (plain dword, "clus t[%u]")
//    +0xA0  m_clusB   (plain dword, "b[%u]")
//    +0xA8  m_relatedAgent  (a SINGLE related-agent pointer, NOT an array -
//           corrected this follow-up session: `CPoPosCl::Slot4`'s own real
//           body (0x6F495F50, poposcl_relatedagentslot4.cpp) reads it,
//           calls its own vtable slot 4 with a literal 0, and clears it -
//           no count field, no loop, nothing array-shaped at all. The
//           earlier session's own guess - "SAME shape CRlAgent's own +0x58
//           CDynTable and CPoPosBh's own +0xA8 array, count at +0xB8" -
//           does not hold for THIS class; CPoPosBh really does have that
//           array shape at its own +0x9C (see poposbh.h), but CPoPosCl
//           does not carry the array/count pair at all)
//    +0xAC  m_poSeparate  (`NIpse::CPoSeparate*` - an embedded, separately
//           pool-allocated CPoSeparate this class creates/destroys through
//           its own vtable slot 4 (Agent/poseparate.h), NOT a coincidence:
//           `CPoPosCl` = "Positioning + Separation, Clustered")
//    +0xB0  m_glr   +0xB4  m_acc   +0xB8  m_ang   +0xBC  m_prw
//    +0xC0  m_lmg   +0xC4  m_ldl   +0xC8  m_lor   +0xCC  m_lod
//    +0xD0  m_pclX  +0xD4  m_pclY  +0xD8  m_opt (options bitfield - an
//           `0x%08X` in DumpDebugState, not a CFloat; zeroed by `Apply`,
//           which is consistent with an all-off bitfield default)
//
//    **Correction (further follow-up session):** the previous version of
//    this comment had the +0xB0..+0xD8 run one slot off from +0xC0 onward
//    (`m_prw` at +0xB4 instead of +0xBC, `m_lmg` at +0xC4 instead of
//    +0xC0, and so on) and treated +0xB8 as a "related-agent count" that
//    does not exist on this class - both settled by tracing
//    `DumpDebugState`'s OWN real argument-push order back from
//    `Save`'s own already-committed field-by-field write order (which
//    already wrote seven CFloats B0/B4/B8/BC/C0/C4/C8 in exactly that
//    sequence - `poposcl_saveload.cpp` was right all along; only this
//    header's own names were wrong). `Save`/`ComputeChecksum` never
//    referenced these member names directly (they use raw `base + 0xNN`
//    offsets throughout), so nothing else needed to change.
//
//  Apply (slot 3, 0x6F494650) is simple: base `CPoPos::Apply` first, then
//  a block of fixed-constant stamps over +0x9C..+0xD8 - no related-agent
//  or CPoSeparate work of its own. Slot4 (slot 4, 0x6F495F50) is where the
//  CPoSeparate embedding actually shows: it releases the +0xA8 related
//  agent (its own vtable slot 4, literal 0), tears down and conditionally
//  re-creates the +0xAC CPoSeparate through a small pool allocator
//  (Agent/poseparate.h's own static pool), stamps three bitfields into the
//  new instance's own +0x20 `m_opt` (three tiny setter leaves, own
//  addresses), inserts it into a global list, then tail-calls the base
//  `CPoPos::Slot4`. STILL `TODO` this follow-up session: the allocator
//  chain itself (0x6F494DC0/0x6F495AD0) is genuinely new and this
//  session's own trace of the OUTER Slot4/`sub_6F495ED0` shape did not
//  reach a confident signature for it in the time available - see
//  `docs/targets/NTempestNIpse_clusterH_remainder.md`'s own "Update
//  (further follow-up)" section.
//============================================================================
#ifndef POPOSCL_H
#define POPOSCL_H

#include "popos.h"
#include "poseparate.h"

class CGridVec2;   // Game/game.h - Slot21's own position argument

namespace NIpse {

class CPoPosCl
{
public:
    //  0x6F487B50 - vtable slot 0. Literal "PoPosCl". SHARED with
    //  NTempest::CLinkedList<NIpse::CPoPosCl> (same address, idx0).
    const char* GetTypeName();

    //  0x6F487BA0 - vtable slot 1. SHARED with CLinkedList<CPoPosCl>,
    //  against this class's own static pool (CPoPosCl_GetStaticPool).
    void ReturnToStaticPool();

    //  0x6F487BE0 - vtable slot 2, scalar deleting destructor. CPoPosCl's
    //  OWN (CLinkedList<CPoPosCl> overrides this one slot with its own
    //  separate address, 0x6F488670 - byte-identical body, see
    //  CLinkedList_CPoPosCl below). Same trivial no-base-teardown restamp
    //  triplet as CPoPos's own.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F494650 - vtable slot 3, "Apply". SHARED with CLinkedList<CPoPosCl>.
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F495F50 - vtable slot 4, "Slot4". SHARED with CLinkedList<CPoPosCl>.
    //  Real now (Agent/poposcl_relatedagentslot4.cpp) - `void`, not
    //  `void*`: the shipped body's own final `call` into the base
    //  `CPoPos::Slot4` (itself `void`) is a genuine call+epilogue, not a
    //  sibling-call `jmp`, and nothing downstream reads a return value out
    //  of this slot anywhere in this call tree.
    void Slot4(unsigned int flags);

    //  0x6F494C00 - vtable slot 9, Save. SHARED with CLinkedList<CPoPosCl>.
    void Save(CDataStoreScratch* store);

    //  0x6F495FC0 - vtable slot 10, Load. SHARED with CLinkedList<CPoPosCl>.
    void Load(CDataStore* store);

    //  0x6F4946D0 - vtable slot 15, ComputeChecksum. SHARED with
    //  CLinkedList<CPoPosCl>. Folds the base `CPoPos::ComputeChecksum`.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F494AD0 - vtable slot 16, DumpDebugState. SHARED with
    //  CLinkedList<CPoPosCl>. Dumps the base `CPoPos::DumpDebugState`.
    void DumpDebugState(void* sink);

    //  0x6F4945F0 - vtable slot 19, own override. Tail-calls the base
    //  `CPoPos::Slot19(arg)` (THUNK, popos_slots1920_thunk.cpp) first, then
    //  - only if this object is not already linked into the +0x438 global
    //  list (`m_prev`/`m_next` at +4/+8 both null) - inserts it via
    //  `SPathOrderLink::InsertAfter` (0x6F493810, already real), the SAME
    //  list-insert call `poposcl_saveload.cpp`'s own `Save` documents.
    void Slot19(unsigned int arg);

    //  0x6F4A74A0 - vtable slot 20, own override. Same shape as Slot19
    //  above but with no argument, tail-calling the base `CPoPos::Slot20()`
    //  (THUNK) first.
    void Slot20();

    //  0x6F495140 - vtable slot 21, own override. Real now
    //  (Agent/poposcl_relatedagentslot21.cpp, that file's own header
    //  comment has the full shape): given a new world position, does
    //  nothing at all if the floored cell coordinate hasn't moved from
    //  the cached +0xD0/+0xD4 (`m_pclX`/`m_pclY`); otherwise rebuilds the
    //  set of other agents related through the spatial grid's own OLD and
    //  NEW cells (`CCellGrid::CollectRelated`, Pathfinding/spatialgrid.h),
    //  notifies the survivors of each through their own vtable slot 8,
    //  and stamps the new cell coordinate in. `this` in ecx, `retn 4` -
    //  one stack argument, the new position (`CGridVec2`).
    void Slot21(const CGridVec2* pos);

    //  0x6F4954F0 - vtable slot 22, own override. Copies the base
    //  `m_fieldE` (CPoPos's own +0x8C) into this class's own +0xC8
    //  (`m_lor`), stamps +0xCC (`m_lod`) with the shared
    //  `dword_6FAAE470` CFloat-zero-shaped constant, then unlinks `this`
    //  from whatever doubly-linked list it is threaded on (the same
    //  null-checked splice `Slot19`/`Slot4` above use). `this` in ecx, no
    //  stack arguments, `retn`.
    void Slot22();

    //  0x6F495ED0 - the shared Slot4/Load helper: tear down the +0xAC
    //  m_poSeparate (its own vtable slot 4, arg 0) if live, then - only
    //  when `create` is set - allocate a fresh one (via
    //  Agent/poseparate_pool_alloc.cpp), stamp its own +0x14 owner
    //  pointer back to `this`, run the three nibble setters into its own
    //  m_opt, and splice it onto the global list rooted at
    //  `g_pTimeSync+0x514`. `this` in ecx; `retn 0x10` - four stack
    //  arguments (create, opt16, opt20, opt28). Not a vtable slot of its
    //  own.
    void TeardownAndOptionallyRecreatePoSeparate(int create, unsigned char opt16,
                                                  unsigned short opt20, unsigned char opt28);

    //  0x6F506EA0 - vtable slot 23. NOT this class's - confirmed a
    //  misattributed `CGeoset`-loading function unrelated to this family
    //  entirely (reads it directly: stamps `??_7CGeoset@@6B@`, streams
    //  "HGEOSET"/"HGEOSETDATA" handle-table pairs). Left untouched, same
    //  finding as docs/targets/NTempestNIpse_clusterG.md's own
    //  0x6F5067E0/0x6F506EA0 note for a different class pair.

    char         m_base00_9C[0x9C];  // +0x00-+0x9B, CPoPos's own territory.
    unsigned int m_clusT;        // +0x9C
    unsigned int m_clusB;        // +0xA0
    unsigned int m_reservedA4;   // +0xA4
    void*        m_relatedAgent; // +0xA8 - a SINGLE related-agent pointer,
                                  //         not an array - see this file's
                                  //         own header comment
    CPoSeparate* m_poSeparate;   // +0xAC - embedded, pool-allocated
    CFloat       m_glr;          // +0xB0
    CFloat       m_acc;          // +0xB4
    CFloat       m_ang;          // +0xB8
    CFloat       m_prw;          // +0xBC
    CFloat       m_lmg;          // +0xC0
    CFloat       m_ldl;          // +0xC4
    CFloat       m_lor;          // +0xC8
    CFloat       m_lod;          // +0xCC
    unsigned int m_pclX;         // +0xD0
    unsigned int m_pclY;         // +0xD4
    unsigned int m_opt;          // +0xD8 - options bitfield, "opt[0x%08X]"
};

//  0x6F493830 - splice `node` in immediately after the list anchored at
//  `g_pTimeSync + 0x514`. Byte-identical to `SPathOrderLink::InsertAfter`
//  (0x6F493810, Pathfinding/pathorderlink.cpp) - a separate, /OPT:ICF-less
//  compiled twin over the SAME `+0x04`/`+0x08` prev/next pair shape this
//  whole chain shares (CPoSeparate's own live-instance link, reused here
//  as the list this global anchor threads through). `this` (ecx) is the
//  anchor; `retn 4` - one stack argument, the node to insert. Own
//  translation unit (Agent/poposcl_relatedagent_insertafter.cpp) so it
//  stays a real `call` out of
//  CPoPosCl::TeardownAndOptionallyRecreatePoSeparate rather than being
//  inlined into it.
struct SPoSeparateGlobalListAnchor
{
    void InsertAfter(CPoSeparate* node);

    char          m_reserved00[4];
    CPoSeparate*  m_prev;   // +0x04
    CPoSeparate*  m_next;   // +0x08
};

//  NTempest::CLinkedList<NIpse::CPoPosCl>'s own single override, matching
//  Agent/poseparate.h's own CLinkedList_CPoSeparate precedent.
struct CLinkedList_CPoPosCl
{
    //  0x6F488670 - vtable slot 2 of NTempest::CLinkedList<NIpse::CPoPosCl>
    //  (0x6F95266C). Byte-identical to CPoPosCl::ScalarDeletingDestructorTrivial
    //  above.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);
};

}  // namespace NIpse

#endif  // POPOSCL_H

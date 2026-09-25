//============================================================================
//  NIpse::CPmRegion (`??_7CPmRegion@NIpse@@6B@`, 0x6F9529C4, 17 slots) -
//  branches off NTempest::CPresenceTagged directly (this cluster's own
//  vtable slots 15/16 are its OWN overrides here, unlike the other four
//  classes in this cluster, which all inherit CPresenceTagged's own
//  FoldLinkFieldsIntoHash/DumpDebugState unchanged - see basemap.h's own
//  file header). No shared address with either NIpse::CBaseMap<> template,
//  so no inheritance ambiguity to resolve for this class.
//
//  Flat, offset-addressed placeholder, same convention as basemap.h. An
//  earlier session (claude-nipse-batch2) reconstructed slots 0-4; this
//  session (claude-pmregion-syncdata) reconstructs the four that were left
//  documented-but-not-attempted - slots 9 (Save), 10 (Load), 15
//  (ComputeChecksum) and 16 (DumpDebugState) - which is what makes the
//  field layout below possible: Save/Load/ComputeChecksum/DumpDebugState
//  between them touch every one of +0x1C..+0x40, so the offsets are no
//  longer guesses, they are cross-checked against several independent call
//  sites each (see pmregion_saveload.cpp/pmregion_checksum.cpp/
//  pmregion_dumpdebugstate.cpp for the per-field reasoning).
//
//  Why this class matters more than a typical one this batch: CPoPos::Apply
//  and SPathRefAllocator::Alloc both allocate objects from THIS class's own
//  static pool (PmRegion_GetStaticPool, Agent/agentbasechain_staticpools.h)
//  through Pathfinding/pmregionpoolalloc.cpp's own
//  AllocatePooledRegistration/SPmRegionPoolAllocator::AllocElement, and the
//  latter's own callee sub_6F48C000 (still a THUNK - see that file's own
//  header) stamps THIS class's own `??_7CPmRegion@NIpse@@6B@` vtable on a
//  freshly grown element. Reconstructing this class's own fields is a
//  prerequisite for ever un-thunking that pool-growth step, not this
//  session's task by itself - see pmregionpoolalloc.cpp for whether that
//  bonus panned out this session.
//============================================================================
#ifndef PMREGION_H
#define PMREGION_H

#include "handletable.h"   // SHandleRegistrationRequest, SHandleTableEntry

class CDataStoreScratch;
class CDataStore;

namespace NIpse {

class CPmRegion
{
public:
    //  0x6F48AD40 - vtable slot 0.
    const char* GetTypeName();

    //  0x6F48AE20 - vtable slot 1. Own real static pool (Agent/
    //  agentbasechain_staticpools.h's own PmRegion_GetStaticPool).
    void ReturnToStaticPool();

    //  0x6F48AE60 - vtable slot 2, scalar deleting destructor. Restamps
    //  straight to `NTempest::CPresence`'s own vftable with NO base
    //  teardown call at all - the same open finding docs/targets/
    //  NTempestNIpse_BaseChain.md already flags for CPresence/
    //  CPresenceTagged/CPrRelation's own scalar deleting destructors (and
    //  now also CBaseMap<CPaCell>/CBaseMap<CPmCell>'s real destructors,
    //  basemap.h) - one more data point for that same still-open question,
    //  not a new one. DIFFERS, not EXACT: unlike every other class in this
    //  cluster, this one genuinely has no destructor call to make in the
    //  first place (no forward-declared out-of-scope callee stands in for
    //  anything here), so the whole body is faithfully reconstructed.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F49E7C0 - vtable slot 9 (+0x24), Save. See pmregion_saveload.cpp.
    //  Two path-ref-shaped link pointers (+0x2C/+0x30) via the already-real
    //  WritePathRefEntry, then the plain-dword fields in the same order
    //  Load below reads them back in.
    void Save(CDataStoreScratch* store);

    //  0x6F4A0100 - vtable slot 10 (+0x28), Load. Save's near-mirror (see
    //  pmregion_saveload.cpp for the one real asymmetry: the final
    //  conditional call into sub_6F49F930, left a THUNK - a genuine,
    //  substantial grid-write routine this session did not confidently
    //  re-derive, not a boring CRT leaf).
    void Load(CDataStore* store);

    //  0x6F49E640 - vtable slot 15 (+0x3C), ComputeChecksum. This class's
    //  OWN override (NOT the shared NTempest::CPresenceTagged::
    //  FoldLinkFieldsIntoHash the other four classes in this cluster
    //  inherit unchanged) - calls that real base fold first, then this
    //  class's own five added fields. See pmregion_checksum.cpp.
    void ComputeChecksum(unsigned int* hash);

    //  0x6F48AD50 - vtable slot 16 (+0x40), DumpDebugState. This class's
    //  OWN override (same "not the shared CPresenceTagged::DumpDebugState"
    //  story as slot 15 above) - genuinely DUPLICATES
    //  CPresenceTagged::DumpDebugState's own two lines inline rather than
    //  calling it, then adds its own "[CPmRg]" tag and two field lines. See
    //  pmregion_dumpdebugstate.cpp for why this reads as duplicated source,
    //  not a codegen artifact.
    //  `retn 4`, one stack argument only - unlike NTempest::CSyncData's own
    //  DumpDebugState (csyncdata.h), which genuinely does take a second,
    //  unread padding dword.
    void DumpDebugState(void* sink);

    //  +0x00-+0x1B - CPmRegion's own vfptr plus the inherited
    //  NTempest::CPresence/CPresenceTagged territory (CPresenceTagged's
    //  own m_tagT/m_tagB already live at +0x14/+0x18, Agent/presence.h) -
    //  unmodeled here, flat/opaque, same convention as every sibling in
    //  this cluster (poposbh.h's own m_base00_C0, basemap.h's own, etc).
    char m_base00_1C[0x1C];

    //  CPmRegion's own added fields. All confirmed by Save/Load's own
    //  identical (mirrored) treatment; the first four also by
    //  DumpDebugState's own "\tt[%d] l[%d] b[%d] r[%d]" line printing them
    //  in this exact offset order - a natural reading for a class named
    //  "Region": a bounding rectangle.
    unsigned int m_top;       // +0x1C - "t" in "\tt[%d] l[%d] b[%d] r[%d]"
    unsigned int m_left;      // +0x20 - "l"
    unsigned int m_bottom;    // +0x24 - "b"
    unsigned int m_right;     // +0x28 - "r"

    //  +0x2C/+0x30 - a linked pair, written/read through the same
    //  WritePathRefEntry/SPathRefReader::ReadEntry handle-serialization
    //  pair CPoPos's own +0x94/+0x98 CGridRegistration* handles use
    //  (popos.h) - here most plausibly this class's own
    //  NTempest::CLinkedNode<NTempest::CPresence> prev/next pair
    //  (hierarchy_raw names that base explicitly), reasoned rather than
    //  independently confirmed the way that file's own header flags for
    //  its comparable fields. Untyped `void*` rather than guessing a
    //  pointee type neither Save/Load nor DumpDebugState ever dereferences.
    void* m_linkPrev;    // +0x2C
    void* m_linkNext;    // +0x30

    //  +0x34 - "\tmsk[0x%X]" in DumpDebugState; written/read via plain
    //  WriteDword/ReadDword (like +0x38/+0x3C/+0x40), NOT the WriteDwordAlt2
    //  /ReadDwordAlt3 pair +0x1C-+0x28 use - a bitmask, per the format
    //  string's own "0x%X" spelling.
    unsigned int m_mask;      // +0x34

    //  +0x38/+0x3C - plain dwords, Save/Load only (WriteDword/ReadDword
    //  mirror pair) - never read by ComputeChecksum or DumpDebugState in
    //  this dump, so nothing beyond "exists, is a dword" is known.
    unsigned int m_field38;   // +0x38
    unsigned int m_field3C;   // +0x3C

    //  +0x40 - a flags dword: Load's own tail tests `this->m_flags &
    //  0x10000000` to decide whether to call the (THUNK'd) sub_6F49F930 at
    //  all - see pmregion_saveload.cpp.
    unsigned int m_flags;     // +0x40
};

}  // namespace NIpse

//  0x6F49E0C0 - vtable slot 3 (+0x0C). A bare `jmp sub_6F4AEFF0` in the
//  dump - not this class's own override at all, just a zero-adjustment
//  forwarding stub into NTempest::CPresenceTagged's own slot-3 body
//  (SHandleTableEntry::RegisterOrReuse, Pathfinding/handletable.h, already
//  reconstructed) - see docs/msvc-vc8-idioms.md's own "An MSVC-generated
//  adjustor thunk into an already-reconstructed function is not a
//  `__declspec(naked)` shipped-code thunk" section for exactly this shape
//  and why the safe way to write it is a naked `jmp` to the
//  already-reconstructed function's own qualified name, not a raw address.
//  Written as a free function, not a `CPmRegion` member - this compiler
//  rejects `__declspec(naked)` on a member function (`C2488`, and even on a
//  free function's own separate prototype - only the definition itself may
//  carry it), the same restriction docs/msvc-vc8-idioms.md's own
//  "`__declspec(naked)` is rejected on a constructor" section already
//  documents for constructors specifically; the established fix for a
//  naked adjustor thunk in this codebase
//  (Net/netclientgame0_deleteselfadjustor.cpp) is a plain `__fastcall` free
//  function taking the object pointer as its own first (and, here, only
//  meaningful) argument instead, defined once with no separate prototype
//  (see pmregion_slots34.cpp - nothing else in this cluster calls either
//  by name). We do not know *why* the compiler needed a distinct entry
//  point instead of just reusing RegisterOrReuse's own address directly in
//  this class's own vtable (no multiple-inheritance evidence found for
//  this class from this session's own reading) - left open rather than
//  guessed.
//
//  0x6F49E0D0 - vtable slot 4 (+0x10). Same shape as slot 3 above, into
//  SHandleTableEntry::UnregisterAndNotify.

#endif  // PMREGION_H

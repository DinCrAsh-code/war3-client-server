//============================================================================
//  CCustomData / CAbilityCustomData - the lazily-created singleton parked
//  at CGameDataCache::m_field40 (array slot 0x10), the checksum-provider
//  registry's other shared lazy-singleton slot alongside CAbilityDatabase
//  (docs/notes/checksum-provider-registry.md, batch D).
//
//  Identified by walking LazyInitGameDataCacheField40's own construct call
//  (0x6F25AC40 -> 0x6F256520) down to its vtable stamp
//  (`??_7CAbilityCustomData@@6B@`, confirmed with
//  `ida_query vtable 0x6F92CC04`) and its base class
//  (0x6F2518A0 stamps `??_7CCustomData@@6B@` first, confirmed with
//  `ida_query vtable 0x6F92C68C` - CAbilityCustomData overrides only slots
//  2 and 3, sharing 0 and 1 with the base).
//
//  **Both constructors stay redirects, not reconstructions - deliberately,
//  same reasoning as Containers/hashtablectors.cpp's own TSHashTable
//  redirects.**  CCustomData::CCustomData(int) (0x6F2518A0) switches on its
//  one argument (0-6) and calls one of six sibling lazy-singleton-pool
//  helpers (0x6F71C6E0/70/800/890/920/9B0 - each its own SEH-carrying
//  "get or create a global pool" function, its own module entirely,
//  unrelated to the checksum-provider registry this batch is about) to fill
//  +0x0C; CAbilityCustomData::CAbilityCustomData() calls that with 4, then
//  stamps its own vtable over the base's.  Reconstructing either
//  constructor for real means reconstructing that whole six-function pool
//  cluster too, which is out of scope for this batch (see
//  LazyInitGameDataCacheField40's own file for the __try/__finally this
//  batch DOES reconstruct around the redirected call).  `Construct()`
//  rather than a real C++ constructor for the same reason
//  hashtablectors.cpp gives: MSVC will not take __declspec(naked) on an
//  actual constructor.
//
//  **checksum-batch-B correction (2026-09-12):** `agent_worktrees/classes/
//  0x6F92C68C.json` carries real Class Informer RTTI now
//  (`hierarchy_raw: "CCustomData: CSynchronousData;"`), so `CCustomData`
//  really does derive from `CSynchronousData` (`Game/synchronousdata.h`) -
//  per CLAUDE.md's own "fix it immediately, everywhere it's referenced"
//  rule for a class whose hierarchy was not yet reflected here.  With real
//  inheritance, slot 0 is `CSynchronousData::ComputeChecksum()` *overridden*
//  (now real - see abilitycustomdata_checksum.cpp) and slot 1 is
//  `CSynchronousData::DebugDumpTo()` *inherited unchanged* - the vtable
//  dump's own slot 1 target (`nullsub_53`, `0x6F002000`) is the exact same
//  address `CSynchronousData`'s own base body already compiles to
//  (Game/synchronousdata.h), confirming CCustomData adds no override of its
//  own rather than needing a second stub.  This also fixes the previous
//  `Method0()`/`Method1()` placeholders below, which predated the RTTI fix
//  and did not know slot 1 was inherited, not overridden.
//============================================================================
#ifndef ABILITYCUSTOMDATA_H
#define ABILITYCUSTOMDATA_H

#include "synchronousdata.h"

//  0x6F92C68C, 4 slots: sub_6F256190 (0, own override), nullsub_53
//  (1, inherited from CSynchronousData unchanged), sub_6F25B130 (2, own),
//  nullsub_1153 (3, own).  CAbilityCustomData overrides only 2 and 3 below.
//  Layout past the vtable pointer is a type tag (the ctor's own int
//  argument, stored verbatim), a resource this class's own destructor
//  releases through its own vtable slot 0 (m_ownedRef, below - always null
//  through CCustomData::Construct's own switch, see abilitycustomdata_ctor.cpp:
//  that ctor only ever writes +0x0C, never +0x08, so this release path is
//  live for some other, not-yet-found construction path and dead for every
//  object this repo's own Construct() redirect can build) and the pool
//  pointer one of the six sub_6F71Cxxx helpers fills.
//
//  Batch (2026-09-04) reconstructed slot 2 for both classes (the scalar
//  deleting destructor, DeleteSelf below) plus the two real, non-virtual
//  helpers underneath it - ~CCustomData() (0x6F256180) and
//  ReleaseOwnedRefAndFinalize() (0x6F251950).
//
//  checksum-batch-B (2026-09-12): slot 0 (sub_6F256190,
//  CCustomData::ComputeChecksum, shared by both classes) is real now too -
//  see abilitycustomdata_checksum.cpp and
//  Misc/customdatachecksumcache.cpp for the two sub-object walkers it
//  reaches through m_ownedRef/m_pPool.  CAbilityCustomData's own slot 3
//  override (sub_6F251BB0) is still open - see this header's own note
//  below and docs/notes/checksum-provider-registry.md.
class CCustomData : public CSynchronousData
{
public:
    //  0x6F2518A0 - `retn 4`.  Still a redirect; see this header's own note.
    CCustomData* Construct(int type);

    //  0x6F256190 - slot 0, overrides CSynchronousData::ComputeChecksum().
    //  Folds this object's own m_pPool/m_ownedRef sub-objects (each its own
    //  dirty-flag-guarded checksum cache) - see abilitycustomdata_checksum.cpp
    //  and Misc/customdatachecksumcache.cpp for the real body,
    //  reconstructed checksum-batch-B (2026-09-12).
    virtual unsigned int ComputeChecksum();

    //  0x6F25B130 - slot 2, `retn 4`.  The scalar deleting destructor -
    //  see abilitycustomdata_deleteself.cpp.  CCustomData's own added
    //  virtual (CSynchronousData declares only ComputeChecksum/DebugDumpTo),
    //  which is why it lands at slot 2 rather than slot 1.
    virtual CCustomData* DeleteSelf(unsigned int flags);

    //  nullsub_1153 for CCustomData itself - CAbilityCustomData overrides
    //  it (sub_6F251BB0) with something real, still out of scope for this
    //  batch (see the override's own note below).  Called as a tail chain
    //  out of ReleaseOwnedRefAndFinalize() below, so it has to stay a real
    //  virtual dispatch even at this base level, not an inline no-op.
    virtual void Method3() {}

    //  0x6F256180 - `~CCustomData()`.  Declared, not defined, here: the
    //  scalar deleting destructor's own dump makes a real `call` to this
    //  address rather than inlining it, so the definition (abilitycustomdata_dtor.cpp)
    //  has to stay out of every TU that calls it - the same split
    //  Agent/agent_deleteself.cpp documents for CAgent.
    ~CCustomData();

private:
    //  0x6F251950 - the destructor's own tail call.  Declared, not defined,
    //  here for the same inlining reason as ~CCustomData() itself.
    void ReleaseOwnedRefAndFinalize();

    unsigned int m_type;       // +0x04
    void*        m_ownedRef;   // +0x08 - see this class's own header note
    void*        m_pPool;      // +0x0C - one of the six sub_6F71Cxxx pools
};

//  0x6F92CC04, 4 slots: sub_6F256190 (0, shared with CCustomData), nullsub_53
//  (1, shared), sub_6F25B570 (2, overridden), sub_6F251BB0 (3, overridden).
//  CAbilityCustomData adds no fields of its own.
class CAbilityCustomData : public CCustomData
{
public:
    //  0x6F256520 - `retn 0`.  Still a redirect; calls CCustomData::Construct
    //  with type 4, then stamps this class's own vtable over the base's.
    CAbilityCustomData* Construct();

    //  0x6F25B570 - slot 2, `retn 4`.  Byte-identical to CCustomData's own
    //  DeleteSelf: CAbilityCustomData declares no destructor of its own (no
    //  extra fields to release), so its compiler-generated `~CAbilityCustomData()`
    //  reduces to nothing but a call into the base - the dump confirms it by
    //  calling `sub_6F256180` (CCustomData::~CCustomData) directly, the very
    //  same address CCustomData's own DeleteSelf calls.  See
    //  abilitycustomdata_deleteself.cpp.
    virtual CAbilityCustomData* DeleteSelf(unsigned int flags);

    //  0x6F251BB0 - slot 3.  Confirmed the same self-redirect-thunk shape as
    //  the six siblings' own slot 3 overrides (Item/customdatasiblings.h/
    //  customdatasiblings_method3.cpp): lazily fetches two further
    //  SEH-carrying singletons of its own (CAbilityDB via sub_6F71C410,
    //  CAbilityMetaDB via sub_6F71C890, each its own __except_handler4-shaped
    //  frame this toolchain cannot reproduce - the same class of frame
    //  docs/msvc-vc8-idioms.md already documents for CAgentPtr/
    //  TSGrowableArray), threads the first through the shared
    //  sub_6F6F4B50 one-line store, then tail-jumps into the shared
    //  sub_6F7057A0 (clears two internal dynamic arrays at this+0x1CC/
    //  this+0x1F4).  Nothing here is reproducible from source, so - same as
    //  every sibling - it is filed as a naked self-redirect to its own real
    //  address (abilitycustomdata_method3.cpp) rather than an empty stub.
    //  This was the class's last open slot.
    virtual void Method3();
};

#endif

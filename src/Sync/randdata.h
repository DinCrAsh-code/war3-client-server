//============================================================================
//  CRandData - the "rand" checksum provider (CGameDataContext's array
//  slot 3, gamecontext.h).  RTTI-confirmed to derive from
//  CSynchronousData (Game/synchronousdata.h) at mdisp 0.
//
//  Registered through the generic per-index lazy-init/install helper
//  AcquireRandDataProvider (randdataprovider.cpp), called with the literal
//  index 3 by the one-line wrapper AcquireRandChecksumProvider - the same
//  shape GameUI/acquiregamesingleton.cpp already established for CGameWar3
//  at slot 6 (docs/notes/checksum-provider-registry.md).
//
//  45 eight-byte history records (`SResidueKey`, Misc/residuekey.h) follow
//  the vtable pointer - 4 + 45*8 = 0x16C bytes, exactly the allocation
//  size AcquireRandDataProvider asks Storm for.  Reusing SResidueKey
//  rather than a second, near-identical struct is not a style choice:
//  0x6F4AB660, the function that seeds each record, is the *same address*
//  residuekey.cpp already reconstructed under that name for CMinimap's own
//  unrelated call - this really is one shared primitive, not two.
//  ComputeChecksum folds all 45 records through the already-reconstructed
//  CTimeSync_HashInto (Sync/ctimesynchashinto.cpp) - the same
//  per-8-byte-pair scramble CTimeSync's own per-tick hash uses, just
//  walked here over a different object's own history buffer instead of
//  CTimeSync's two leading dwords.
//============================================================================
#ifndef RANDDATA_H
#define RANDDATA_H

#include "residuekey.h"
#include "synchronousdata.h"

class CRandData : public CSynchronousData
{
public:
    //  0x6F280B10 - a genuinely dead constructor: zero real (unhooked)
    //  callers (`ida_query xrefs` came back empty). Seeds all 45 records,
    //  the same count the live construction path (inlined directly into
    //  AcquireRandDataProvider's own body, not a call to this function)
    //  uses - the two are not an off-by-one pair, just two different
    //  ways of writing the same 45-record loop that the shipped binary
    //  happens to carry at two separate addresses, only one of which is
    //  ever actually reached.
    CRandData();

    //  0x6F280C80 - the "rand" checksum: fold all 45 history records
    //  through CTimeSync_HashInto, front to back.
    virtual unsigned int ComputeChecksum();

    //  0x6F280CB0 - "Random val: %u" per record into the caller's buffer,
    //  via Storm_548 (see cheatdata.cpp for that ordinal's own signature).
    virtual void DebugDumpTo(void* buffer);

    SResidueKey m_history[45];   // +0x04
};

#endif

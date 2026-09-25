//============================================================================
//  CCheatData - the "cheat" checksum provider (CGameDataContext's array
//  slot 0, gamecontext.h).  RTTI-confirmed to derive from CSynchronousData
//  (Game/synchronousdata.h) at mdisp 0 - see that header for the base and
//  docs/notes/checksum-provider-registry.md for the whole mechanism.
//
//  Registered directly, not through a generic per-index helper the way
//  CRandData is (Sync/randdata.h): the cheat subsystem's own init function
//  (sub_6F387180) allocates 8 bytes, stamps the vtable, zeroes m_flags and
//  stores the result at context->m_pCache[0] before going on to register
//  ~20 individual cheat commands this session did not reconstruct (no
//  dumps, and well out of this session's scope - see
//  checksum-provider-registry.md).  That registration function therefore
//  has no funcmap entry of its own; only the two vtable methods below do.
//============================================================================
#ifndef CHEATDATA_H
#define CHEATDATA_H

#include "synchronousdata.h"

class CCheatData : public CSynchronousData
{
public:
    //  0x6F3834D0 - the "cheat" checksum: the flags word with bit 12
    //  masked out.  Not established this session which cheat that bit
    //  tracks; masking it out of the sync value means whichever one it is
    //  is deliberately allowed to differ between machines without
    //  desyncing (a purely local/UI cheat flag, most likely).
    virtual unsigned int ComputeChecksum();

    //  0x6F3834E0 - "Cheat code flags %d\n" into the caller's buffer via
    //  Storm_548 (SStrPrintf-shaped, see the .cpp).
    virtual void DebugDumpTo(void* buffer);

    unsigned int m_flags;   // +0x04
};

#endif

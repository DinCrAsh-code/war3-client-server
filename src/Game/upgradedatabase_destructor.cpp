//============================================================================
//  0x6F252CF0 - CUpgradeDatabase::DestructInternal, the real
//  ~CUpgradeDatabase().  Found but not claimed by checksum-batch-A (see
//  its own "still_open" list); reconstructed here (checksum-round2-F).
//
//  Carries the same `push -1 / push offset SEH_.../ mov eax, fs:0`
//  __CxxFrameHandler3-personality prologue this whole family's own
//  larger destructors already document as toolchain-unreproducible at
//  this repo's fixed `/GS- /EHs-c-` (docs/msvc-vc8-idioms.md) - filed
//  DIFFERS for that reason rather than forced to match, per this batch's
//  own seed instructions.
//
//  Body: stamp the derived vtable, then - only if `m_recordSet` is
//  non-null - release the 15 simple field pairs' own `m_wedbResult`
//  pointers (ReleaseForward_6F4C8880, Agent/refcnt.h) and, for each of
//  the 4 indexed groups (effect/base/mod/code), four more releases at a
//  flat 4-byte stride starting at the group's own base.  Tail-calls the
//  real base ~CWar3SlkDatabase() (0x6F005D40, Item/war3slkdatabase.cpp)
//  explicitly rather than hand-stamping a flat base view - this class now
//  really derives from CWar3SlkDatabase (2026-09-13), so that base dtor's
//  own compiler-emitted vtable restore is exactly the shipped tail.
//
//  **That last loop does NOT walk `m_values[4]`** (Game/upgradedatabase.h's
//  own field for the 4 registered indices) - it walks the 16 bytes
//  starting at the group's own `m_lastWedbResult` slot instead
//  (`[base+0]`, `[base+4]`, `[base+8]`, `[base+0xCh]` - the first three of
//  which upgradedatabase_setrecordset.cpp's own header note already
//  documents as unwritten "reserved" padding, since
//  `RegisterUpgradeIndexedGroup` only ever stores the *last* of its own 4
//  iterations' WEDB-result pointers into that one scratch slot).  Written
//  here as a raw stride-4 pointer walk, matching the shipped instructions
//  exactly, rather than reusing the named `SUpgradeIndexedGroup` fields -
//  a genuine latent shipped-binary quirk (releasing whatever bytes sit in
//  the three "reserved" slots as if they were pointers), not a
//  reconstruction bug, and out of scope to resolve further here.
//============================================================================
#include "upgradedatabase.h"
#include "refcnt.h"

#define UPGRADE_DATABASE_VTABLE ((void*)0x6F92C70C)

//  Release the 4 raw dwords starting at `groupBase`, stride 4 - see this
//  file's own header note on why this does not walk `m_values[4]`.
static void ReleaseIndexedGroupSlots(void* groupBase)
{
    char* p = (char*)groupBase;
    for (int i = 0; i < 4; ++i)
    {
        ReleaseForward_6F4C8880((TRefCnt*)*(void**)p);
        p += 4;
    }
}

void __thiscall CUpgradeDatabase::DestructInternal()
{
    *(void**)this = UPGRADE_DATABASE_VTABLE;   // +0x00, the implicit CWar3SlkDatabase vfptr

    if (m_recordSet != 0)
    {
        ReleaseForward_6F4C8880((TRefCnt*)m_upgradeid.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_race.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_class.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_flag.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_maxlevel.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_used.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_inherit.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_goldbase.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_goldmod.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_lumberbase.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_lumbermod.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_timebase.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_timemod.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_version.m_wedbResult);
        ReleaseForward_6F4C8880((TRefCnt*)m_global.m_wedbResult);

        ReleaseIndexedGroupSlots(&m_effect);
        ReleaseIndexedGroupSlots(&m_base);
        ReleaseIndexedGroupSlots(&m_mod);
        ReleaseIndexedGroupSlots(&m_code);
    }

    this->CWar3SlkDatabase::~CWar3SlkDatabase();
}

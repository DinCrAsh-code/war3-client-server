//============================================================================
//  0x6F267E90 - AcquireDestructableDatabase: get-or-create slot `index` of
//  the per-thread game-data cache as a fresh CDestructableDatabase.  Same
//  generic "AcquireXProvider(index)" registration shape
//  Game/acquireupgradedatabase.cpp documents for CUpgradeDatabase - found
//  the same way, `tools/worktree_store.py xrefs` on the class's own
//  vtable-stamp constructor.  No SEH frame, same reason as
//  AcquireUpgradeDatabase's own header note: CDestructableDatabase's
//  construction here is the trivial inline zero-then-stamp, not a real
//  placement-new constructor call.
//
//  **`Construct()` is inlined here by hand, not called** - folding it
//  directly into this function's own body (rather than a real cross-TU
//  `obj->Construct()` call this no-LTO toolchain cannot inline back down)
//  raised the score from 0.421 (12/32) to 0.786 (22/32), the same fix
//  Game/acquireupgradedatabase.cpp's own header note documents for its
//  CUpgradeDatabase sibling - checksum-round2-F,
//  docs/notes/checksum-provider-registry.md.  `Construct()` itself stays
//  declared/defined in destructabledatabase_ctor.cpp as the readable,
//  named statement of the same logic.
//============================================================================
#include "gamecontext.h"
#include "destructabledatabase.h"
#include "storm.h"

static const char* const kDestructableDataHeader =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\unit\\../Data.h";

#define DESTRUCTABLE_DATABASE_VTABLE ((void*)0x6F92C6FC)

void* __fastcall AcquireDestructableDatabase(int index, int /*unused*/)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    CDestructableDatabase* obj = (CDestructableDatabase*)SMemAlloc(
        sizeof(CDestructableDatabase), kDestructableDataHeader, 0x74, 0);

    //  Inlined directly rather than a separate Construct() call - see
    //  destructabledatabase.h's own header note on why (checksum-round2-F).
    if (obj != 0)
    {
        obj->m_recordSet = 0;
        obj->m_haveChecksum = 0;
        obj->m_checksum = 0;
        *(void**)obj = DESTRUCTABLE_DATABASE_VTABLE;
    }

    ((void**)context->m_pCache)[index] = obj;

    return ((void**)context->m_pCache)[index];
}

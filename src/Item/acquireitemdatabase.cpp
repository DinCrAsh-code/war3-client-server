//============================================================================
//  0x6F2B6860 - AcquireItemDatabase: get-or-create slot `index` of the
//  per-thread game-data cache as a fresh CItemDatabase.  The same generic
//  "AcquireXProvider(index)" registration shape
//  Game/acquireupgradedatabase.cpp already documents for CUpgradeDatabase
//  - found the same way, `tools/worktree_store.py xrefs 0x6F257370 --to`
//  (CItemDatabase::Construct's own one real caller).
//
//  **Does have a real `__try`/`__finally` SEH frame, unlike
//  AcquireUpgradeDatabase** - this one calls CItemDatabase::Construct()
//  through a real `call` rather than inlining it, so MSVC wraps it the
//  same way Item/itemcustomdata_lazyinit.cpp's own
//  LazyInitGameDataCacheField34 wraps its own `Construct()` call: the
//  toolchain-unreproducible __CxxFrameHandler3 frame
//  docs/msvc-vc8-idioms.md documents, reconstructed past that frame the
//  same way.
//============================================================================
#include "gamecontext.h"
#include "itemdatabase.h"
#include "storm.h"

static const char* const kItemDataHeader =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\unit\\../Data.h";

void* __fastcall AcquireItemDatabase(int index, int /*unused*/)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    CItemDatabase* raw = (CItemDatabase*)SMemAlloc(
        sizeof(CItemDatabase), kItemDataHeader, 0x74, 0);
    CItemDatabase* obj;

    if (raw != 0)
    {
        __try
        {
            raw->Construct();
            obj = raw;
        }
        __finally
        {
        }
    }
    else
    {
        obj = 0;
    }

    ((void**)context->m_pCache)[index] = obj;
    return ((void**)context->m_pCache)[index];
}

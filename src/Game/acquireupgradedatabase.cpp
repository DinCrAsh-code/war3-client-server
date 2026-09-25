//============================================================================
//  0x6F25F0B0 - AcquireUpgradeDatabase: get-or-create slot `index` of the
//  per-thread game-data cache as a fresh CUpgradeDatabase, the same
//  generic "AcquireXProvider(index)" registration shape
//  Unit/unitdatabase_checksum.cpp's own header note already establishes
//  for CUnitDatabase's own sub_6F280650 (not reconstructed there) and
//  GameUI/acquiregamesingleton.cpp already reconstructs for CGameWar3 -
//  this one for CUpgradeDatabase instead, checksum-provider registry
//  batch (docs/notes/checksum-provider-registry.md).
//
//  **No SEH frame here**, unlike AcquireGameSingleton: CUpgradeDatabase's
//  own construction is the trivial inline zero-then-stamp
//  Game/upgradedatabase_ctor.cpp's own Construct() reconstructs (nothing
//  that can throw), not a real placement-new constructor call - the same
//  reason Item/abilitydatabase_lazyinit.cpp's own LazyInitGameDataCacheField14
//  has no SEH frame either while its CAbilityCustomData/CItemCustomData
//  siblings (which DO call through a real constructor) do.
//
//  **`Construct()` is inlined here by hand, not called.**  The dump's own
//  body has no `call` at all between the allocation and the vtable
//  stamp - four flat stores (`mov [eax+4],0` / `+8` / `+0Ch` /
//  `mov [eax], offset ??_7CUpgradeDatabase@@6B@`), exactly
//  upgradedatabase_ctor.cpp's own `Construct()` logic, but the shipped
//  binary's whole-program optimisation inlined it across its own module
//  boundary the same way war3slkdatabase.h's own header note documents
//  for `CWar3SlkDatabase()` itself.  This build compiles one `cl.exe /c`
//  per TU with no whole-program optimisation, so a real `call
//  CUpgradeDatabase::Construct` (defined in a different .cpp) cannot be
//  inlined away here - `tools/quick.py` measured the call-based spelling
//  at 0.421 (12/32) and this hand-inlined spelling matches every
//  instruction after the allocation.  `Construct()` itself stays declared
//  and defined in upgradedatabase_ctor.cpp as the readable, named
//  statement of the same logic (nothing calls it - it carries no address
//  of its own to score, the same "no dedicated address" fate
//  war3slkdatabase.h's own ctor documents).
//============================================================================
#include "gamecontext.h"
#include "upgradedatabase.h"
#include "storm.h"

//  Same allocation tag every lazy-init helper in this family pushes.
static const char* const kUpgradeDataHeader =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\unit\\../Data.h";

//  0x6F92C70C - `??_7CUpgradeDatabase@@6B@`, this class's own real vtable.
//  Same literal-stamp choice upgradedatabase_ctor.cpp's own header note
//  explains (no real C++ constructor call site exists to retarget at our
//  own compiled vtable instead).
#define UPGRADE_DATABASE_VTABLE ((void*)0x6F92C70C)

void* __fastcall AcquireUpgradeDatabase(int index, int /*unused*/)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    CUpgradeDatabase* obj = (CUpgradeDatabase*)SMemAlloc(
        sizeof(CUpgradeDatabase), kUpgradeDataHeader, 0x74, 0);

    //  Inlined directly rather than a separate Construct() call - see
    //  upgradedatabase.h's own header note on why (checksum-round2-F).
    if (obj != 0)
    {
        obj->m_recordSet = 0;
        obj->m_haveChecksum = 0;
        obj->m_checksum = 0;
        *(void**)obj = UPGRADE_DATABASE_VTABLE;
    }

    ((void**)context->m_pCache)[index] = obj;

    return ((void**)context->m_pCache)[index];
}

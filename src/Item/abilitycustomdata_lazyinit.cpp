//============================================================================
//  0x6F25AC40 - LazyInitGameDataCacheField40: allocate and construct the
//  CAbilityCustomData parked at the game-data cache's own +0x40 slot
//  (array index 0x10), then store and re-fetch it.  See gamecontext.h's
//  own declaration comment and abilitycustomdata.h for the class this
//  identifies and why its own constructor stays a redirect.
//
//  Own translation unit, same "one translation unit per original module"
//  reasoning as abilitydatabase_lazyinit.cpp - its own comment covers why
//  0x6F25EFE0 and this function (0x6F25AC40) are not one module either,
//  despite Game/gamecontext.h declaring both.
//
//  **The SEH frame is the toolchain-unreproducible shape.**  Unlike
//  LazyInitGameDataCacheField14 (whose CAbilityDatabase constructor is
//  trivial and gets inlined flat with no unwind story at all), this
//  function's own call to CAbilityCustomData::Construct() is a real
//  virtual-dispatch-carrying constructor MSVC cannot prove won't throw, so
//  it wraps the call in a `__try`/`__finally` - the same shape
//  Agent/agentregistry.cpp's own AgentSlotArrayAppend already documents at
//  length (docs/msvc-vc8-idioms.md, "An __except_handler4-shaped frame
//  this toolchain cannot reproduce"): under this repo's fixed
//  `/GS- /EHs-c-`, the bundled `vs8/` `cl.exe` compiles a `__try`/
//  `__finally` into a plain, cookie-less `__except_handler3` frame with an
//  EBP base, never the cookie-XORed, frame-pointer-omitted,
//  `__CxxFrameHandler3`-routed per-function trampoline this dump's own
//  `SEH_6F25AC40` prologue and appended scope-table block
//  (0x6F811FC0-0x6F811FE7, ending `call sub_6F7E1059` /
//  `jmp __CxxFrameHandler3`) show.  Past that frame the body matches the
//  dump instruction for instruction and in the same order - the raw
//  allocation, the if/else around the construct call exactly mirroring
//  AgentSlotArrayAppend's own `if (slot) {...} else { slot = 0; }`, the
//  empty `__finally` (a tail-call to NoOpFinallyStub/nullsub_45, same as
//  AgentSlotArrayAppend's), and the final store-then-refetch through
//  `context->m_pCache`.
//
//  **Not safe to arm in mix_config.json.**  CAbilityCustomData's own
//  constructor is a redirect (abilitycustomdata.h), so this is fine as far
//  as it goes, but the class's vtable is not "finished" in
//  vtable_classes.json (2 of 4 slots have no dump) - anything that calls
//  through this object's own vtable would be at the same risk
//  abilitydatabase_lazyinit.cpp documents for CAbilityDatabase.  Left
//  `false`.
//============================================================================
#include "abilitycustomdata.h"
#include "gamecontext.h"

//  Storm.dll ordinal 401 - see abilitydatabase_lazyinit.cpp's own note.
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename,
                          int logline, unsigned int flags);

//  Same allocation tag as CAbilityDatabase's own lazy-init helper - both
//  push the identical string literal (0x6F8782C0).
static const char* const kAbilityDataHeader =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\unit\\../Data.h";

void* __fastcall LazyInitGameDataCacheField40(unsigned int index, unsigned int)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    CAbilityCustomData* raw = (CAbilityCustomData*)SMemAlloc(
        sizeof(CAbilityCustomData), kAbilityDataHeader, 0x74, 0);
    CAbilityCustomData* obj;

    if (raw != 0)
    {
        __try
        {
            obj = raw->Construct();
        }
        __finally
        {
        }
    }
    else
    {
        obj = 0;
    }

    //  `context->m_pCache` read as a flat `void**` and indexed by the
    //  caller's own runtime `index` (0x10 for this slot) - same reasoning
    //  as LazyInitGameDataCacheField14's own note (abilitydatabase_lazyinit.cpp).
    ((void**)context->m_pCache)[index] = obj;
    return ((void**)context->m_pCache)[index];
}

//============================================================================
//  0x6F25AB40 - LazyInitGameDataCacheField34: allocate and construct the
//  CItemCustomData parked at the game-data cache's own +0x34 slot (array
//  index 0x0D), then store and re-fetch it.  See gamecontext.h's own
//  declaration comment and customdatasiblings.h for the class this identifies
//  and why its own constructor stays a redirect.
//
//  Byte-identical shape to LazyInitGameDataCacheField40
//  (Item/abilitycustomdata_lazyinit.cpp) - same __try/__finally around a
//  real, virtual-dispatch-carrying `Construct()` call, the same
//  toolchain-unreproducible __CxxFrameHandler3 frame
//  docs/msvc-vc8-idioms.md documents (this dump's own
//  `SEH_6F25AB40`/scope-table tail confirms it, ending `call
//  sub_6F7E1059` / `jmp __CxxFrameHandler3`).  Reconstructed past that
//  frame the same way that file's own function is.
//
//  Allocation size is 0x10 bytes (`push 10h`) - smaller than
//  CAbilityCustomData's own allocation (Item/abilitycustomdata_lazyinit.cpp
//  uses `sizeof(CAbilityCustomData)`) but CItemCustomData adds no fields
//  of its own over CCustomData (Item/abilitycustomdata.h), whose own
//  three fields (vtable pointer, m_type, m_ownedRef, m_pPool) already sum
//  to 0x10 - `sizeof(CItemCustomData)` is used here for the same reason.
//
//  **Not safe to arm in mix_config.json** - same reasoning
//  abilitycustomdata_lazyinit.cpp gives: CItemCustomData's own vtable slots
//  have not been traced by this batch, so anything that calls through an
//  object built here is at risk.  Left `false`.
//============================================================================
#include "customdatasiblings.h"
#include "gamecontext.h"

//  Storm.dll ordinal 401 - see abilitydatabase_lazyinit.cpp's own note.
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename,
                          int logline, unsigned int flags);

//  Same allocation tag every lazy-init helper in this family pushes
//  (0x6F8782C0) - see abilitycustomdata_lazyinit.cpp's own note.
static const char* const kAbilityDataHeader =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\unit\\../Data.h";

void* __fastcall LazyInitGameDataCacheField34(unsigned int index, unsigned int)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    CItemCustomData* raw = (CItemCustomData*)SMemAlloc(
        sizeof(CItemCustomData), kAbilityDataHeader, 0x74, 0);
    CItemCustomData* obj;

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

    ((void**)context->m_pCache)[index] = obj;
    return ((void**)context->m_pCache)[index];
}

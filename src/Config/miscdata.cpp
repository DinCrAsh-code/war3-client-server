//============================================================================
//  0x6F002790 - the map's configuration overrides, created on first use.
//
//  Its own translation unit rather than part of src/CMiscCustom.cpp because it
//  constructs a CMiscCustom: with the constructor's definition visible MSVC
//  would inline it, and the shipped code calls it.
//============================================================================
#include <new>

#include "CMiscCustom.h"
#include "gamecontext.h"

//  Storm.dll ordinal 401.  Same declaration as src/storm.h's, repeated rather
//  than pulling in that header's containers - see docs/msvc-vc8-idioms.md,
//  "Declaring imports so the diff can see them", for why it is not extern "C".
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename,
                          int logline, unsigned int flags);

//----------------------------------------------------------------------------
//  The allocation is tagged with the header the type was declared in, which is
//  what Blizzard's allocation macros pass where Storm expects a __FILE__.
//----------------------------------------------------------------------------
static const char* const kDataHeader =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\Data.h";

CMiscCustom* GetMiscCustom()
{
    CGameDataCache* cache =
        ((CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData))->m_pCache;
    if (cache->m_pMiscCustom != 0)
        return cache->m_pMiscCustom;

    //  Fetched again rather than reused: the shipped code calls the slot fetch
    //  a second time on this path, and it has to, because the first result is
    //  not kept alive across the test.
    CGameDataContext* context =
        (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    //  Placement new over Storm's allocator: the null test the shipped code
    //  has in front of the constructor call, and the null store on the failing
    //  path, are both MSVC's own code for `new (p) T` - see
    //  docs/msvc-vc8-idioms.md, "Placement new".
    context->m_pCache->m_pMiscCustom =
        new (SMemAlloc(sizeof(CMiscCustom), kDataHeader, 0x74, 0)) CMiscCustom;
    return context->m_pCache->m_pMiscCustom;
}

//----------------------------------------------------------------------------
//  0x6F005E20 - see the declaration comment in gamecontext.h.  The cache
//  pointer is read once as a flat `void**` (m_pCache's own struct view is
//  irrelevant here - every provider lives at m_pCache[i] for i in
//  [0, m_providerCount)) and walked back to front, XOR-rotate-folding each
//  live entry's vtable-slot-0 return value into the running hash.  The
//  reverse walk and the pre-decrement (`while (i-- != 0)` rather than a
//  forward `for`) are both exactly what the dump's own
//  `sub esi,1` / `cmp [eax+esi*4],0` / trailing `test esi,esi ; jnz` loop
//  shows, not a stylistic choice.
//----------------------------------------------------------------------------
unsigned int __fastcall GameDataContextChecksumProviderHash(CGameDataContext* context)
{
    unsigned int i = context->m_providerCount;
    unsigned int hash = 0;

    if (i != 0)
    {
        do
        {
            //  m_pCache is re-read off `context` every iteration - not
            //  hoisted - because a provider's own vtable-slot-0 call below
            //  could reach back in and change it, exactly why the shipped
            //  loop body re-loads `[edi+10h]` on every pass instead of
            //  keeping it in a register across the call.
            void** providers = (void**)context->m_pCache;
            --i;
            void* provider = providers[i];
            if (provider != 0)
            {
                typedef unsigned int (__thiscall *ProviderChecksumFn)(void*);
                ProviderChecksumFn fn = (ProviderChecksumFn)(*(void***)provider)[0];
                unsigned int result = fn(provider);
                unsigned int mixed = hash ^ result;
                hash = (mixed << 3) | (mixed >> 29);
            }
        } while (i != 0);
    }

    return hash;
}

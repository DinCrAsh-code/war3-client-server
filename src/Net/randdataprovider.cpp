//============================================================================
//  AcquireRandDataProvider (0x6F28ABE0) - the generic "get-or-create slot N
//  of the checksum-provider array, construct a CRandData there" helper -
//  and AcquireRandChecksumProvider (0x6F28DCE0), the one caller this
//  session found, a one-line wrapper that hard-codes index 3 (the "rand"
//  slot - docs/notes/checksum-provider-registry.md).  Same shape
//  GameUI/acquiregamesingleton.cpp already established for CGameWar3 at
//  slot 6: a real __CxxFrameHandler3-shaped SEH frame wraps the body (the
//  CRandData construction below can, in principle, run arbitrary code),
//  which is this build's unreproducible ceiling (docs/msvc-vc8-idioms.md),
//  documented rather than modelled.
//
//  The construction is written out here rather than a call to
//  CRandData::CRandData (Sync/randdata.cpp) - the dump shows the vtable
//  stamp and the 45-record seed loop inlined directly at this call site,
//  not a `call CRandData::CRandData` (that out-of-line constructor is
//  dead code - see randdata.h's own note - not a different, shorter
//  variant of this one; both seed all 45 records the same way).  Written
//  out here anyway, with the real shipped `??_7CRandData@@6B@` as a
//  literal rather than through `new (block) CRandData` - the same "stamp
//  the real vtable without the class's own ctor" idiom
//  Item/abilitydatabase_lazyinit.cpp already uses, for a different reason:
//  there the class's vtable is incomplete, here it is simply that the
//  shipped stream never routes this call site through a real
//  `call CRandData::CRandData` at all, so writing one here would add an
//  instruction the dump does not have.
//============================================================================
#include "gamecontext.h"
#include "randdata.h"
#include "storm.h"

//  The real, complete, shipped ??_7CRandData@@6B@ - see the file header.
#define RANDDATA_VTABLE ((void*)0x6F92FE18)

void* __fastcall AcquireRandDataProvider(int index, int /*unused*/)
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(kThreadLocalGameData);

    void* block = SMemAlloc(sizeof(CRandData),
                             "e:\\drive1\\temp\\buildwar3x\\war3\\source\\Data.h",
                             0x74, 0);

    CRandData* obj = 0;
    if (block)
    {
        obj = (CRandData*)block;
        *(void**)obj = RANDDATA_VTABLE;

        SResidueKey* record = obj->m_history;
        for (int count = 44; count >= 0; --count)
        {
            record->Set(0);
            ++record;
        }
    }

    ((void**)context->m_pCache)[index] = obj;
    return ((void**)context->m_pCache)[index];
}

void* __fastcall AcquireRandChecksumProvider(int, int)
{
    return AcquireRandDataProvider(3, 0);
}

#undef RANDDATA_VTABLE

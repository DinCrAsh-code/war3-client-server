//============================================================================
//  0x6F549B80 - AllocEventSlot.  See netdataeventqueue.h for the file-split
//  rationale.  The allocation-side counterpart to ReleaseNetEvent
//  (netdataeventrelease.cpp): reuse-or-grow one 0x18-byte chunk out of the
//  TInstanceRecycler<CNetEvent> pool at CNetData+0x2290 through that pool's
//  own already-real CDataRecycler::TryPopChunk/Realloc (vtable slot 3,
//  dispatched indirectly - the exact same shape AcquireChunk
//  (netclient_chunkpool.cpp) already documents for the same pool class,
//  just a different target size and a different post-acquire stamp), then
//  zero the five fields DestroyNetEvent/UnlinkAndCount read
//  (+0x00.. +0x10) and mark the node "no event yet": a type byte of 0xFF
//  at +0x14 (the same sentinel UnlinkAndCount's own type-byte read treats
//  as its counter table's own top slot) and a zeroed flag byte at +0x15.
//  LookupOrEnqueueEvent_6F54C490 (netevent_replaystream_records.cpp) then
//  overwrites +0x08/+0x0C/+0x10/+0x14/+0x15 for real on its own success
//  path - `id`, `dest`, `size`, `size2`, `zero` in turn.
//
//  Called with `ecx` pointed at the CDataRecycler sub-object directly
//  (`lea ecx,[esi+2290h]` at the one call site), not through CNetData
//  itself, so it is modelled as a free function over a `CDataRecycler*`
//  the same way AcquireChunk already is, for the same reason (kept out of
//  cdatarecycler.h, which only carries the class's own committed 5-slot
//  vtable and what dispatches through it).
//============================================================================
#include "netdataeventqueue.h"
#include "cdatarecycler.h"

typedef void* (__thiscall* CDataRecyclerReallocSlot)(void*, void*, unsigned int, unsigned int*, const char*, int);

void* __fastcall AllocEventSlot_6F549B80(void* pool)
{
    CDataRecycler* recycler = (CDataRecycler*)pool;

    void* ptr = 0;
    unsigned int size = 0;
    recycler->TryPopChunk(&ptr, &size, 0, 0);

    void* node;
    if (size >= 0x18)
    {
        node = ptr;
    }
    else
    {
        CDataRecyclerReallocSlot reallocSlot = (CDataRecyclerReallocSlot)(*(void***)recycler)[3];
        node = reallocSlot(recycler, ptr, 0x18, &size, 0, 0);
    }

    if (node)
    {
        *(unsigned int*)((char*)node + 0x00) = 0;
        *(unsigned int*)((char*)node + 0x04) = 0;
        *(unsigned int*)((char*)node + 0x08) = 0;
        *(unsigned int*)((char*)node + 0x0C) = 0;
        *(unsigned int*)((char*)node + 0x10) = 0;
        *(unsigned char*)((char*)node + 0x14) = 0xFF;
        *(unsigned char*)((char*)node + 0x15) = 0;
    }
    return node;
}

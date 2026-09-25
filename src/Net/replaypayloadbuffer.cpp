//============================================================================
//  0x6F650D40 - PreparePayloadBuffer.  Borrow a chunk from the per-thread
//  size-class-14 pool (`GetThreadLocalSlot(14)`; the pool object itself is
//  not chased further - out of scope for this session), reporting its
//  recorded capacity through *outCapacity and growing it with
//  Storm_405/SMemReAlloc when `requestedSize` does not already fit.  If
//  the pool has not been set up on this thread at all, falls back to a
//  fresh Storm_401/SMemAlloc'd block sized exactly to `requestedSize` (or
//  a null, zero-capacity result for a zero request - the shape every one
//  of netevent_replaystream.cpp's own ~14 record-echo handlers actually
//  exercises, always requesting 0).  `this` = requestedSize, `edx` =
//  outCapacity, retn bare.
//
//  Its own module neighbourhood (0x6F650D40) sits apart from every one of
//  its callers here (0x6F54Dxxx/0x6F54Exxx/0x6F54Cxxx,
//  netevent_replaystream.cpp) - kept in its own translation unit rather
//  than folded in beside them so `/Ob2` cannot inline it into a caller in
//  a different original module the way the shipped build's own separate
//  compilation never would (CLAUDE.md, "one translation unit per
//  original module" - the shipped dump calls this function for real
//  every time, never inlines it).
//============================================================================
#include "gamecontext.h"             // GetThreadLocalSlot (0x6F4C34D0)
#include "storm.h"                   // SMemAlloc/SMemReAlloc
#include "cdatarecycler.h"           // CDataRecycler::TryPopChunk

//  sub_6F657010 - the pool's own "reuse an already-checked-in chunk, if
//  one exists" fast path: `this`+0x664 is a `CDataRecycler`-shaped
//  sub-object (not named further - the outer per-thread pool-set object
//  it lives in is out of scope here), and this is a thin wrapper handing
//  the two out-params straight to that sub-object's own `TryPopChunk`
//  (cdatarecycler.h, already reconstructed).  retn 8 (2 stack args).
//  netremainder2 session (batch-G closure pass): TryPopChunk was still a
//  naked thunk when this comment was written but is a real reconstruction
//  now, so this wrapper is one too.
void __fastcall AcquireChunkPool14_6F657010(
    void* pool, void* /*unused_edx*/, void** outChunk, unsigned int* outCapacity)
{
    CDataRecycler* recycler = (CDataRecycler*)((char*)pool + 0x664);
    recycler->TryPopChunk(outChunk, outCapacity, 0, 0);
}

//  The exact fallback source file every SMemAlloc/SMemReAlloc/SMemFree call
//  in this module tags itself with when it does not go through
//  CDataRecycler's own null-fname substitution - the same string
//  cdatastorecache.cpp/cdatastorescratch.cpp already use for this file's
//  own SMemFree calls (0x108).
static const char kNetGameCpp[] = "e:\\drive1\\temp\\buildwar3x\\engine\\source\\netgame.cpp";

void* __fastcall PreparePayloadBuffer_6F650D40(unsigned int requestedSize, unsigned int* outCapacity)
{
    void* pool = GetThreadLocalSlot(14);
    if (pool != 0)
    {
        void* chunk;
        unsigned int capacity;
        AcquireChunkPool14_6F657010(pool, 0, &chunk, &capacity);
        if (capacity >= requestedSize)
        {
            *outCapacity = capacity;
            return chunk;
        }
        chunk = SMemReAlloc(chunk, requestedSize, kNetGameCpp, 0x119, 0);
        *outCapacity = requestedSize;
        return chunk;
    }

    if (requestedSize == 0)
    {
        *outCapacity = 0;
        return 0;
    }

    void* mem = SMemAlloc(requestedSize, kNetGameCpp, 0xF7, 0);
    *outCapacity = requestedSize;
    return mem;
}

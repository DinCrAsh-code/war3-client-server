//============================================================================
//  A CSyncChannel's pending-request machinery: the block a request record
//  lives in, the deadline heap the channel keeps them in, and an overlay
//  naming the three sub-objects the channel carries ahead of its own
//  sample.
//
//  The record itself is game.h's SSyncRegistration - what
//  CSyncChannel::Register hands back and what CAgentRelation parks in its
//  +0x1C/+0x20.  Its shipped name is `NIpse::CSiRequest<NIpse::CPrRelation,
//  long>`, read straight out of the RTTI descriptor 0x6F4A6650 hands the
//  pool allocator (`ida_query bytes 0x6FA845E8`); the same string is the
//  only evidence in this repo that CAgentRelation's own shipped name is
//  `NIpse::CPrRelation`.  Neither is renamed - both already have
//  established names here and a rename buys no instruction - but the
//  strings are written down so nobody has to read them out of the image
//  twice.
//
//  The channel's sub-objects are an *overlay* and not members added to
//  CSyncChannel, for the reason widgetagentsubmit.cpp's SAgentMakerHost
//  gives: game.h is included nearly everywhere, and it cannot see CMemBlock
//  or CDataAllocator without dragging two more headers into every one of
//  those translation units.  The offsets are exact and each is established
//  by the code that touches it - the heap by 0x6F46AEE0's `lea ecx,[edi+4]`
//  and its CMemBlock reads at +0x0C/+0x1C, the allocator by 0x6F4A6650's
//  `lea esi,[edi+24h]` and the tag it hands CDataAllocator::Alloc, and the
//  free list and live count by the same instruction's own +0x14/+0x18
//  displacements off that `lea`.
//============================================================================
#ifndef SYNCREQUEST_H
#define SYNCREQUEST_H

#include "game.h"
#include "cellbuffer.h"
#include "dataallocator.h"

//----------------------------------------------------------------------------
//  One pool element.  The record starts four bytes in; the first word is
//  the channel's own free-list link, threaded through the block rather than
//  through the record, which is why the free head points four bytes below
//  every registration the channel ever handed out.
//----------------------------------------------------------------------------
struct SSyncRequestBlock
{
    SSyncRequestBlock* m_nextFree;   // +0x00
    SSyncRegistration  m_request;    // +0x04
};

//----------------------------------------------------------------------------
//  The channel's deadline heap: the same CMemBlock-backed growable array
//  SPathFrontier (pathfind.h) and SAStarHeap (pathastar.h) are, with a
//  4-byte pointer element, plus a real heap insert on top of it.
//
//  One-based, like SAStarHeap: index 0 is never used and the percolation
//  stops at 1, which is what makes `idx >> 1` the parent.
//----------------------------------------------------------------------------
class SSyncRequestHeap : public CMemBlock
{
public:
    //  0x6F46A0F0 - make room for `count` more entries, filling the new
    //  ones from `*fill` when it is not null.  Identical in shape to
    //  SPathFrontier::Grow (pathfrontierarray.cpp).
    int Grow(const void* fill, unsigned int count);

    //  0x6F468570 - fill [start, min(start + count, m_count)) from one
    //  repeated entry.  Its one caller passes the *new* count as `count`
    //  and lets the clamp do the work, exactly as SPathFrontier::Grow
    //  does.
    void FillFrom(unsigned int start, const void* value, unsigned int count);

    //  0x6F46AEE0 - append one request and percolate it up.
    void Push(SSyncRegistration* entry);

    SSyncRegistration** Entries() { return (SSyncRegistration**)m_data; }

    unsigned int m_growChunk;    // +0x14
    unsigned int m_capacity;     // +0x18
    unsigned int m_count;        // +0x1C
};

//----------------------------------------------------------------------------
//  Where a channel's request records come from: the shared block allocator,
//  the channel's own free list of records it has finished with, and how
//  many are out.
//
//  The three are one sub-object and not three fields of the channel, and
//  the shipped code says so rather than the grouping being a convenience:
//  0x6F4A6650 forms `lea esi,[edi+24h]` - the allocator's address - once,
//  ahead of the branch that may not even use it, and then addresses both
//  the free-list store and the counter off *that* register at +0x14 and
//  +0x18.  A channel that kept them as three separate members would have
//  no reason to.
//----------------------------------------------------------------------------
struct SSyncRequestPool
{
    CDataAllocator     m_blocks;       // +0x00  (channel +0x24)
    SSyncRequestBlock* m_freeList;     // +0x14  (channel +0x38)
    unsigned int       m_liveCount;    // +0x18  (channel +0x3C)
};

//----------------------------------------------------------------------------
//  CSyncChannel's first 0x40 bytes, named.  Only MakeRequest and the heap
//  reach any of it.
//----------------------------------------------------------------------------
struct SSyncChannelStore
{
    char               m_reserved00[0x04];
    SSyncRequestHeap   m_pending;      // +0x04
    SSyncRequestPool   m_pool;         // +0x24
};

#endif

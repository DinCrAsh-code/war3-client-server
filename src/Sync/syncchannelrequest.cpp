//============================================================================
//  0x6F4A6650 - CSyncChannel::MakeRequest: take a request record, fill it
//  in and queue it on this channel's deadline heap.
//
//  This is the body CSyncChannel::Register (0x6F4A66E0) hands on to once it
//  has clamped the caller's sample up to g_CFloatEpsilon and added it to
//  the channel's own current value - so `deadline` is that sum and `value`
//  is still the caller's own unclamped pointer, kept by address rather than
//  copied.
//
//  The record comes off the channel's own free list first and only falls
//  back on the pool allocator, which is why the free-list pop and the
//  CDataAllocator::Alloc call are two arms of one `if` and not a loop:
//  0x6F4A6650 never grows the free list itself, the allocator does.  The
//  tag it hands the allocator is the RTTI descriptor
//  `.?AV?$CSiRequest@VCPrRelation@NIpse@@J@NIpse@@` - the shipped names for
//  this record and for CAgentRelation (syncrequest.h says what is done with
//  that).
//
//  Two fields are written twice and both pairs are in the shipped stream:
//  the deadline is set to g_CFloatZero and then to `*deadline`, and the
//  flags to 0 and then to 0x20000.  That is the shape a constructor
//  followed by two assignments leaves, and it is reproduced literally
//  rather than folded, because folding either one loses a store the
//  original has.
//
//  Own translation unit: it calls the heap's Push() for real.
//============================================================================
#include "syncrequest.h"

static const char kSiRequestTypeName[] =
    ".?AV?$CSiRequest@VCPrRelation@NIpse@@J@NIpse@@";

SSyncRegistration* CSyncChannel::MakeRequest(void* owner, int* time,
                                             CFloat value,
                                             const CFloat& deadline, int seq)
{
    SSyncChannelStore* store = (SSyncChannelStore*)this;
    SSyncRequestPool* pool = &store->m_pool;

    SSyncRequestBlock* block = store->m_pool.m_freeList;
    if (block == 0)
        block = (SSyncRequestBlock*)pool->m_blocks.Alloc(
                    0, kSiRequestTypeName, -2);
    else
        pool->m_freeList = block->m_nextFree;

    pool->m_liveCount++;

    SSyncRegistration* request = &block->m_request;

    request->m_next     = 0;
    request->m_deadline = g_CFloatZero;
    request->m_value    = value;
    request->m_channel  = this;
    request->m_flags    = 0;
    request->m_sequence = seq;
    request->m_owner    = owner;
    request->m_time     = *time;

    request->m_deadline = deadline;
    request->m_flags    = 0x20000;

    store->m_pending.Push(request);

    return request;
}

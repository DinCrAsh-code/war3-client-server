//============================================================================
//  0x6F46AEE0 - SSyncRequestHeap::Push: append one request record and
//  percolate it up to its place.
//
//  A one-based binary heap, like SAStarHeap (pathastar.h): index 0 is never
//  occupied, the parent of `i` is `i >> 1`, and the walk stops at 1.  The
//  first thing it does is Grow(0, 1) - one more slot, filled from nothing -
//  and the index it then percolates from is the slot that grow just added.
//
//  **The ordering is by deadline, with the channel's own sample sequence as
//  the tie-break.**  The two deadlines are compared on the x87 as ordinary
//  hardware floats, not through CFloat's integer comparisons: each record's
//  encoded word is copied into a stack slot and `fld`ed straight back out,
//  which is the same raw-word round trip JASS_GetLocationZ does and what
//  the two `mov [esp+..], reg` / `fld [esp+..]` pairs in the shipped stream
//  are.  Equal deadlines fall through to `entry->m_sequence <
//  above->m_sequence`, so a record registered earlier stays ahead of one
//  registered later at the same instant - which is what makes the channel
//  deterministic, and is presumably why the sequence counter exists at all.
//
//  The comparison is a static helper and not written out twice: the shipped
//  stream has it inlined at its one site (there is no call), and MSVC
//  inlines it here for the same reason - it is used once.
//
//  Own translation unit: it calls Grow() for real, and CSyncChannel::
//  MakeRequest calls it for real.
//============================================================================
#include "syncrequest.h"

//----------------------------------------------------------------------------
//  Does `entry` belong above `above`?
//
//  Both encoded words go through a named `int` before being reinterpreted,
//  because that is what the shipped code does: it loads each record's +0x04
//  into a register, stores it into a frame slot and `fld`s the slot.
//  Reading `*(const float*)&record->m_deadline` directly would `fld` the
//  record itself and lose both stores.
//----------------------------------------------------------------------------
static bool RequestPrecedes(const SSyncRegistration* entry,
                            const SSyncRegistration* above)
{
    int entryBits = entry->m_deadline.m_bits;
    int aboveBits = above->m_deadline.m_bits;

    float x = *(const float*)&entryBits;
    float y = *(const float*)&aboveBits;

    if (y != x)
        return y > x;

    return entry->m_sequence < above->m_sequence;
}

void SSyncRequestHeap::Push(SSyncRegistration* entry)
{
    Grow(0, 1);

    unsigned int idx = m_count - 1;
    while (idx > 1)
    {
        unsigned int parent = idx >> 1;

        if (!RequestPrecedes(entry, Entries()[parent]))
            break;

        //  Read a second time rather than kept from the comparison: the
        //  shipped code reloads m_data and re-indexes it here.
        Entries()[idx] = Entries()[parent];
        idx = parent;
    }

    Entries()[idx] = entry;
}

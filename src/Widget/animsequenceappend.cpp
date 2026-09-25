//============================================================================
//  0x6F4D5070 - SAnimKeyArray::Append (animsequence.h): grow the candidate
//  list by `count` and copy `keys` in at the end.  SetCount's own grow
//  (ComputeChunk/SetAlloc, animsequencefilter.cpp/animkeyarray.cpp) with the
//  copy folded in rather than left to the caller, and the per-slot `!= 0`
//  guard the same address-computed-before-tested idiom this repo already
//  has for SetAlloc's own copy loops.
//
//  Own translation unit: two real calls out of it (ComputeChunk, SetAlloc),
//  both already reconstructed elsewhere for this same class.
//============================================================================
#include "animsequence.h"

//  Returns the array's own count *before* the append - the shipped stream
//  computes `m_count - count` after the store (new count minus how many
//  were just added) and leaves it in eax for the `retn`, even though every
//  call site in this closure ignores it.
unsigned int SAnimKeyArray::Append(unsigned int count, const unsigned int* keys)
{
    unsigned int newCount = m_count + count;

    if (newCount > m_capacity)
    {
        unsigned int alloc = newCount;
        if (newCount > 2)
        {
            unsigned int chunk = ComputeChunk(newCount);
            unsigned int over = newCount % chunk;
            if (over != 0)
                alloc = newCount + (chunk - over);
        }
        SetAlloc(alloc);
    }

    for (unsigned int i = 0; i < count; i++)
    {
        unsigned int* slot = &m_keys[m_count + i];
        if (slot != 0)
            *slot = keys[i];
    }

    m_count += count;
    return m_count - count;
}

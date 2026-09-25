//============================================================================
//  0x6F2F79B0 - SPointerArray::Append: put `count` pointers on the end of a
//  growable array and hand back the index the first of them landed at.
//
//  TSGrowableArray's own grow (storm.h's SetCount / Add carry the same
//  chunk-rounding arithmetic) followed by a copy loop.  Four bytes per
//  element - `lea ecx,[edi+ecx*4]` - which is what makes this a *pointer*
//  array and not the 0x14-byte one its neighbour at 0x6F2F7830 destroys.
//
//  The null test inside the copy loop is on the *computed element address*,
//  not on the value being stored, and it is in the shipped code: a null
//  m_data makes the address null and the store is skipped rather than
//  faulting.  Left as it is.
//
//  Own translation unit: it calls the array's own chunk and resize helpers
//  for real.
//============================================================================
#include "storm.h"

struct SPointerArray
{
    unsigned int Append(unsigned int count, void* const* values);

    //  0x6F2F56B0 / 0x6F2F5710 - this instantiation's own ComputeChunk and
    //  SetAlloc, each `retn 4`.  Below this slice; thunked.
    unsigned int ComputeChunk(unsigned int count);
    void SetAlloc(unsigned int alloc);

    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    void**       m_data;    // +0x08
    unsigned int m_chunk;   // +0x0C
};

unsigned int SPointerArray::Append(unsigned int count, void* const* values)
{
    unsigned int needed = m_count + count;
    if (needed > m_alloc)
    {
        unsigned int chunk = m_chunk;
        if (chunk == 0)
            chunk = ComputeChunk(needed);

        unsigned int over = needed % chunk;
        if (over != 0)
            needed = chunk - over + needed;

        SetAlloc(needed);
    }

    unsigned int i = 0;
    if (count > 0)
    {
        do
        {
            void** slot = &m_data[m_count + i];
            if (slot != 0)
                *slot = values[i];
            i++;
        }
        while (i < count);
    }

    m_count += count;
    return m_count - count;
}

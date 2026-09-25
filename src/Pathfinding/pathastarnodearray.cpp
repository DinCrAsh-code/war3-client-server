//============================================================================
//  0x6F4984E0 / 0x6F498710 / 0x6F497C90 - the A* node pool's own storage.
//
//  Third instantiation of pathfrontierarray.cpp's growable array, this time
//  for the 36-byte SAStarNode.  MSVC's canonical *36 is `lea reg,[i+i*8]`
//  followed by two `add reg,reg`, which is why every index computation in
//  this module reads as a *9 and two doublings.
//============================================================================
#include "pathastar.h"

extern "C" void* __cdecl memmove(void* dst, const void* src, unsigned int count);


//----------------------------------------------------------------------------
//  0x6F498010 - the repeated-value fill Grow runs over the slots it just added.
//----------------------------------------------------------------------------
void SAStarNodeArray::FillFrom(unsigned int start, const SAStarNode* value,
                          unsigned int count)
{
    //  Clamped the other way round from SAStarHeap::FillFrom's own copy of
    //  this loop: that one starts from `start + count` and pulls it down to
    //  m_count, this one starts from m_count and pulls it down.  The shipped
    //  pair really do differ here - m_count is loaded first and the sum is
    //  compared against it, not the other way about.
    unsigned int end = m_count;
    unsigned int want = start + count;
    if (want < end)
        end = want;

    if (start < end)
        for (unsigned int i = start; i < end; i++)
            Nodes()[i] = *value;
}

int SAStarNodeArray::Grow(const SAStarNode* fill, unsigned int count)
{
    if (count == 0)
        return count + 1;

    unsigned int needed = m_count + count;
    if (needed > m_capacity)
    {
        unsigned int chunk = m_growChunk;
        if (chunk == 0)
            return 0;

        unsigned int shortfall = m_count - m_capacity + count;
        unsigned int grow = (chunk > shortfall) ? chunk : shortfall;

        if (!SetSize((m_capacity + grow) * 36, 1))
            return 0;

        m_capacity += grow;
    }

    unsigned int start = m_count;
    m_count = start + count;
    if (fill != 0)
        FillFrom(start, fill, m_count);
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F498710 - shift-erase [start, start+count) out of the node pool.
//----------------------------------------------------------------------------
int SAStarNodeArray::EraseRange(unsigned int start, unsigned int count)
{
    unsigned int total = m_count;
    if (start >= total)
        return 0;

    if (start + count > total)
        count = total - start;

    unsigned int trailing = total - start - count;
    if (trailing != 0)
        memmove(Nodes() + start, Nodes() + start + count, trailing * 36);

    m_count -= count;
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F497C90 - shrink to exactly the live nodes, returning the bytes freed.
//----------------------------------------------------------------------------
int SAStarNodeArray::ShrinkToFit()
{
    unsigned int before = m_size;
    SetSize(m_count * 36, 1);
    m_capacity = m_count;
    return before - m_size;
}

//============================================================================
//  0x6F35C2D0 - resize the wide-char scratch buffer's own
//  TSGrowableArray<unsigned short> (Os/widecharscratch.cpp's own
//  instantiation, dword_6FAD13E4's neighbour - 0x6F4DBED0 ComputeChunk,
//  0x6F34C1E0 SetAlloc, both already reconstructed there) to exactly
//  `newSize` elements.
//
//  This is *not* TSGrowableArray<T>::SetCount (Containers/tsarray.inl):
//  that generic body chunk-rounds every grow through ComputeChunk and
//  placement-constructs/destroys the delta elements one at a time. This
//  one does neither - a shrink to zero frees the backing store outright
//  and clears all three header words, any other change hands the exact
//  requested size straight to SetAlloc with no rounding and stores it as
//  the new count. That is consistent with `unsigned short` never needing a
//  constructor or destructor call and this particular buffer never wanting
//  to over-allocate, but it is a real, different body - named for what it
//  does rather than assumed to be the generic member.
//
//  Modelled as its own opaque receiver rather than a new
//  TSGrowableArray<unsigned short> member: the two share layout exactly,
//  but adding a member to that shared template would apply to every other
//  instantiation in the codebase for a body only this one address has -
//  the same STargetFlagIndexArray-style idiom
//  src/Item/abilityfieldflagclosure_thunks.cpp already uses for the same
//  reason (this toolchain also rejects __thiscall on a free function).
//============================================================================
#include "storm.h"

class SAgent4WordBuffer_6F35C2D0
{
public:
    void SetExactSize(unsigned int newSize);

    unsigned int    m_alloc;    // 0x00
    unsigned int    m_count;    // 0x04
    unsigned short* m_data;     // 0x08
    unsigned int    m_chunk;    // 0x0C
};

void SAgent4WordBuffer_6F35C2D0::SetExactSize(unsigned int newSize)
{
    if (newSize == m_count)
        return;

    if (newSize == 0)
    {
        if (m_data)
            SMemFree(m_data, "aG_4", -2, 0);
        m_alloc = 0;
        m_count = 0;
        m_data = 0;
        return;
    }

    //  0x6F34C1E0 - TSGrowableArray<unsigned short>::SetAlloc, already
    //  reconstructed (Os/widecharscratch.cpp) and the same layout as this
    //  opaque type, so the real symbol resolves straight through the cast.
    ((TSGrowableArray<unsigned short>*)this)->SetAlloc(newSize);
    m_count = newSize;
}

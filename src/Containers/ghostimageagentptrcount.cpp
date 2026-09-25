//============================================================================
//  0x6F2C7150 - TSGrowableArray<SCheckedGhostImageSlot>::SetCount.
//
//  The third of three instances of one body in this target - the same Storm
//  SetCount (Containers/tsarray.inl) that 0x6F334410 and 0x6F37CBD0 are,
//  over CAgentPtr<CGhostImage> instead of CAgentPtr<CUnit>.  The
//  element type is the shipped tag its SetAlloc (0x6F2ACCA0) hands Storm,
//  ".?AV?$CAgentPtr@VCGhostImage@@@@", which this repo spells
//  SCheckedGhostImageSlot.
//
//  An explicit specialisation for the same reason the other two are: the
//  generic body constructs each new element with `new (&m_data[i]) T` and
//  this element type has no constructor in the image at all - only the one
//  store of zero that construction compiles to, which is written out here
//  behind the null test placement new puts there.
//
//  Its one call site is the world frame's constructor asking for 0x7D0
//  elements of the array at +0x650 (0x6F39AE61).
//
//  Own translation unit: ComputeChunk and SetAlloc are both real calls into
//  another shipped module.
//============================================================================
#include "ghostimageslot.h"
#include "tsarray.inl"

//  Both are redirects, in Containers/agentptrarray_thunks.cpp.  Declared
//  here so this translation unit does not implicitly instantiate the generic
//  bodies instead - see that file.
template <>
unsigned int TSGrowableArray<SCheckedGhostImageSlot>::ComputeChunk(unsigned int);
template <>
void TSGrowableArray<SCheckedGhostImageSlot>::SetAlloc(unsigned int);

template <>
void TSGrowableArray<SCheckedGhostImageSlot>::SetCount(unsigned int count)
{
    if (count > m_count)
    {
        if (count > m_alloc)
        {
            unsigned int chunk = m_chunk;
            if (chunk == 0)
                chunk = ComputeChunk(count);

            unsigned int alloc = count;
            unsigned int over = count % chunk;
            if (over != 0)
                alloc = count + (chunk - over);

            SetAlloc(alloc);
        }

        for (unsigned int i = m_count; i < count; i++)
        {
            SCheckedGhostImageSlot* element = &m_data[i];
            if (element != 0)
                element->m_value = 0;
        }
    }
    else if (count < m_count)
    {
        for (unsigned int i = count; i < m_count; i++)
        {
            CAgent* held = m_data[i].m_value;
            if (held)
            {
                if (--held->m_refcount == 0)
                    held->ReleaseSelf();
            }
        }
    }

    m_count = count;
}

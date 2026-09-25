//============================================================================
//  0x6F37CBD0 - TSGrowableArray<SCheckedSelectableSlot>::SetCount.
//
//  The second of three instances of one body in this target - the same
//  Storm SetCount (Containers/tsarray.inl) that 0x6F334410 and 0x6F2C7150
//  are, over CAgentPtr<CSelectable> instead of CAgentPtr<CUnit>.  The
//  element type is the shipped tag its SetAlloc (0x6F3796A0) hands Storm,
//  ".?AV?$CAgentPtr@VCSelectable@@@@", which this repo spells
//  SCheckedSelectableSlot.
//
//  An explicit specialisation, for the reason
//  Containers/unitagentptrarraycount.cpp gives: the generic body needs a
//  default constructor on the element and giving the class one would put a
//  second `SCheckedSelectableSlot::SCheckedSelectableSlot` in the listing
//  beside the converting one at 0x6F2C9670, which is a funcmap key.
//
//  The release is written out here rather than going through
//  SCheckedSelectableSlot::Release for one instruction's worth of reason:
//  that member spells the decrement as `m_refcount += -1` followed by a
//  separate `== 0` test, which is what its own callers' inlined scope-exit
//  code does and which reloads the field; this loop's shipped stream is
//  `add [ecx+4], -1` / `jnz` with no reload, i.e. `--m_refcount == 0`.
//
//  Its one call site is the world frame's constructor asking for 0x14
//  elements of the array at +0x630 (0x6F39AE51).
//
//  Own translation unit: ComputeChunk and SetAlloc are both real calls into
//  another shipped module.
//============================================================================
#include "agenttypedslots.h"
#include "tsarray.inl"

//  Both are redirects, in Containers/agentptrarray_thunks.cpp.  Declared
//  here so this translation unit does not implicitly instantiate the generic
//  bodies instead - see that file.
template <>
unsigned int TSGrowableArray<SCheckedSelectableSlot>::ComputeChunk(unsigned int);
template <>
void TSGrowableArray<SCheckedSelectableSlot>::SetAlloc(unsigned int);

template <>
void TSGrowableArray<SCheckedSelectableSlot>::SetCount(unsigned int count)
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
            SCheckedSelectableSlot* element = &m_data[i];
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

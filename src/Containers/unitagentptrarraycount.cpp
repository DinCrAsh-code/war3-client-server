//============================================================================
//  0x6F334410 - TSGrowableArray<SCheckedUnitSlot>::SetCount.
//
//  Storm's own SetCount (Containers/tsarray.inl) over the binary's
//  CAgentPtr<CUnit>, which this repo calls SCheckedUnitSlot: the same chunk
//  rounding, the same grow-then-construct, the same shrink-side destroy and
//  the same single store of the new count on both exits.  The element type is
//  not a guess - this body's own two callees are 0x6F0860C0 and 0x6F09F890,
//  which funcmap already maps to that instantiation's ComputeChunk and
//  SetAlloc, and Containers/unitagentptrarray.cpp read the shipped tag
//  ".?AV?$CAgentPtr@VCUnit@@@@" off the latter.
//
//  Written as an **explicit specialisation** rather than as a plain
//  instantiation, for the reason unitagentptrarray.cpp gives for its own
//  SetAlloc: the generic body constructs each new element with
//  `new (&m_data[i]) T`, and giving SCheckedUnitSlot a default constructor
//  to make that legal would put a second `SCheckedUnitSlot::SCheckedUnitSlot`
//  in the listing beside the converting one at 0x6F038450 - which is a
//  funcmap key, so its row would silently become the mangled-parameter-list
//  form and the hook key with it.  The element's construction is written out
//  instead: one store of zero, behind the null test placement new puts there
//  and which the shipped stream carries.
//
//  Its two call sites are the world frame's constructor asking for 0x7D0 and
//  0xC8 elements of the arrays at +0x600 and +0x620 (0x6F39AE34 / 0x6F39AE44).
//
//  Own translation unit: ComputeChunk and SetAlloc are both real calls and
//  both live in another shipped module.
//
//  Score capped by the __except_handler4 frame MSVC builds round a loop that
//  constructs objects; this repo's fixed /GS- /EHs-c- emits none of it.
//============================================================================
#include "agenttypedslots.h"
#include "tsarray.inl"

//  Containers/unitagentptrarray.cpp defines SetAlloc for this instantiation
//  as an explicit specialisation.  Declaring it here as well is not
//  decoration: without the declaration this translation unit would
//  *implicitly instantiate* the generic body instead, which copies each
//  moved element with the compiler's own memberwise copy - no addref - and
//  the linker would then pick one of the two COMDATs arbitrarily.  That is
//  the shape of defect tools/link_check.py's own notes call a stale body
//  shadowing the real one, and it costs nothing to close.
template <>
void TSGrowableArray<SCheckedUnitSlot>::SetAlloc(unsigned int alloc);

template <>
void TSGrowableArray<SCheckedUnitSlot>::SetCount(unsigned int count)
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
            SCheckedUnitSlot* element = &m_data[i];
            if (element != 0)
                element->m_value = 0;
        }
    }
    else if (count < m_count)
    {
        for (unsigned int i = count; i < m_count; i++)
            m_data[i].Release();
    }

    m_count = count;
}

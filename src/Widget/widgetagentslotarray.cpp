//============================================================================
//  TSGrowableArray<SWidgetAgentSlot> - the per-item array of widget-agent
//  slots reached through the sub-object at CItem +0x650 (itemsinkmaskchanged
//  .cpp's own SItemSink2::m_slotOwner).
//
//  0x6F2AEE80 is New(): grow by one, default-construct the new element with
//  SWidgetAgentSlot(0) (the `new (slot) T(0)` shape tsarray.inl's own New()
//  already documents for SCheckedUnitSlot) and hand its address back.  Its
//  own growth callees are the same generic members: 0x6F2ABB10 ComputeChunk
//  (cap 0x40 = 256/sizeof(SWidgetAgentSlot)) and 0x6F2ACCA0 SetAlloc.
//
//  The Storm allocation tag SetAlloc hands SMemReAlloc/SMemAlloc/SMemFree at
//  0x6F2ACCA0 is ".?AV?$CAgentPtr@VCGhostImage@@@@" - CAgentPtr<CGhostImage>
//  by RTTI name, not SWidgetAgentSlot's own.  That is not a mismatch this
//  reconstruction introduces: it is the literal bytes the shipped function
//  pushes, confirmed by reading the immediate at its own `push offset
//  aAvCagentptrVcg` sites. SWidgetAgentSlot's own constructor (0x6F0955C0)
//  and Assign (0x6F085B50, both widgetagentslot.h) are what genuinely run on
//  the slot this hands back - confirmed by following 0x6F0955C0's own call
//  into 0x6F085B50 - so the element type is SWidgetAgentSlot for real; the
//  debug tag is simply the name the original template instantiation was
//  built under (docs/msvc-vc8-idioms.md's "Storm's __FILE__-slot descriptor
//  names the instantiation, not always the class using it").
//
//  SetAlloc is written out as an explicit specialisation rather than a plain
//  instantiation for the same reason Containers/unitagentptrarray.cpp gives
//  its own SCheckedUnitSlot one: SWidgetAgentSlot's copy constructor cannot
//  be declared without colliding, under MSVC's mangling, with the
//  converting constructor already at 0x6F0955C0.
//============================================================================
#include "widgetagentslot.h"

#define TSARRAY_TYPENAME ".?AV?$CAgentPtr@VCGhostImage@@@@"
#include "tsarray.inl"

template <>
void TSGrowableArray<SWidgetAgentSlot>::SetAlloc(unsigned int alloc)
{
    SWidgetAgentSlot* olddata = m_data;

    if (alloc < m_count)
    {
        for (unsigned int i = alloc; i < m_count; i++)
            olddata[i].Release();
    }

    m_alloc = alloc;
    m_data = (SWidgetAgentSlot*)SMemReAlloc(olddata, alloc * sizeof(SWidgetAgentSlot),
                                            s_typeName, -2, 0x10);
    if (m_data == 0)
    {
        m_data = (SWidgetAgentSlot*)SMemAlloc(alloc * sizeof(SWidgetAgentSlot),
                                              s_typeName, -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = alloc < m_count ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
            {
                CAgent* held = olddata[i].m_value;
                m_data[i].m_value = held;
                if (held)
                    held->m_refcount++;

                olddata[i].Release();
            }
            SMemFree(olddata, s_typeName, -2, 0);
        }
    }
}

template unsigned int TSGrowableArray<SWidgetAgentSlot>::ComputeChunk(unsigned int);
template SWidgetAgentSlot* TSGrowableArray<SWidgetAgentSlot>::New();

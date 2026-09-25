//============================================================================
//  TSGrowableArray<SCheckedWidgetSlot> - a per-item(?) array of type-checked
//  CWidget pointers.  The Storm allocation tag SetAlloc hands
//  SMemReAlloc/SMemAlloc/SMemFree is ".?AV?$CAgentPtr@VCWidget@@@@" (read
//  off the raw `push offset aAvCagentptrVcw` bytes), the same debug tag
//  Agent/agenttypedslots.h's own SCheckedWidgetSlot already carries - see
//  that header for why the tag names the original template instantiation
//  and not always the class actually using it.
//
//  Written as explicit specialisations rather than plain instantiations of
//  the generic Containers/tsarray.inl bodies for the same reason
//  Widget/widgetagentslotarray.cpp gives its own SWidgetAgentSlot ones:
//  SCheckedWidgetSlot's only non-default constructor takes a `CAgent*`, so
//  New()'s own `T(0)` placement-new is fine unspecialised (matches the
//  generic template's own instantiation for SCheckedUnitSlot at 0x6F0A4730
//  exactly), but SetAlloc's move loop needs the real AddRef/Release shape
//  SCheckedWidgetSlot::Release already has - the compiler-generated copy
//  constructor a plain instantiation would use is a bitwise copy with no
//  add-ref at all.
//
//  Own translation unit: the growth (New/ComputeChunk/SetAlloc) and release
//  (the destructor) of one array instantiation.
//============================================================================
#include "agenttypedslots.h"

#define TSARRAY_TYPENAME ".?AV?$CAgentPtr@VCWidget@@@@"
#include "tsarray.inl"

//  0x6F274F10 - the generic template body fits unspecialised: it only
//  touches sizeof(T), never a constructor or destructor.
template unsigned int TSGrowableArray<SCheckedWidgetSlot>::ComputeChunk(unsigned int);

//  0x6F28E580 - likewise the generic New() fits unspecialised: its one
//  placement `new (slot) T(0)` is exactly SCheckedWidgetSlot's own
//  converting constructor.
template SCheckedWidgetSlot* TSGrowableArray<SCheckedWidgetSlot>::New();

//  0x6F280510 - SetAlloc, written out because the move loop needs a real
//  add-ref/release, not the compiler's memberwise copy.
template <>
void TSGrowableArray<SCheckedWidgetSlot>::SetAlloc(unsigned int alloc)
{
    SCheckedWidgetSlot* olddata = m_data;

    if (alloc < m_count)
    {
        for (unsigned int i = alloc; i < m_count; i++)
            olddata[i].Release();
    }

    m_alloc = alloc;
    m_data = (SCheckedWidgetSlot*)SMemReAlloc(olddata,
                                              alloc * sizeof(SCheckedWidgetSlot),
                                              s_typeName, -2, 0x10);
    if (m_data == 0)
    {
        m_data = (SCheckedWidgetSlot*)SMemAlloc(alloc * sizeof(SCheckedWidgetSlot),
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

//  0x6F2868E0 - the destructor, written out for the same reason SetAlloc
//  is: SCheckedWidgetSlot has no destructor of its own (only Release(),
//  called explicitly by whatever owns it), so the generic `m_data[i].~T()`
//  the template's own destructor uses would do nothing here.
template <>
TSGrowableArray<SCheckedWidgetSlot>::~TSGrowableArray()
{
    for (unsigned int i = 0; i < m_count; i++)
        m_data[i].Release();
    if (m_data)
        SMemFree(m_data, s_typeName, -2, 0);
}

//  0x6F28D4D0 - a second, byte-identical copy of the same destructor at a
//  different address (the image was linked without /OPT:ICF - see
//  Agent/agenttypedslots.h's own note on why identical bodies stay
//  distinct symbols).  Not a reconstruction of anything new: a naked
//  redirect into the destructor above, same as this repo's other
//  self-contained COMDAT-style duplicates.
__declspec(naked) void DestructWidgetSlotArrayAlias_6F28D4D0()
{
    __asm { mov eax, 06F2868E0h }
    __asm { jmp eax }
}

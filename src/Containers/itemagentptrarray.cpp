//============================================================================
//  TSGrowableArray<SCheckedItemSlot> - CAgentPtr<CItem>, tagged
//  ".?AV?$CAgentPtr@VCItem@@@@" (aAvCagentptrVci).  SCheckedItemSlot
//  (Item/itemtypedassign.h) already carries this exact Release() shape
//  (`--m_refcount == 0` then a devirtualised call through slot 0), so the
//  destructor (0x6F393660) and SetAlloc (0x6F3939F0) are written out the
//  same way Containers/selectableptrarray.cpp's own pair are for
//  SCheckedSelectableSlot - a declared copy constructor would collide with
//  SCheckedItemSlot's converting one (itemtypedassign.cpp).
//
//  0x6F3953B0 is a second, byte-identical instantiation of the destructor
//  the linker folded to a `jmp` here rather than a second copy - the same
//  pointer-template folding docs/msvc-vc8-idioms.md documents.
//============================================================================
#include "itemtypedassign.h"
#include "tsarray.inl"

template <>
void TSGrowableArray<SCheckedItemSlot>::SetAlloc(unsigned int alloc)
{
    SCheckedItemSlot* olddata = m_data;

    if (alloc < m_count)
    {
        for (unsigned int i = alloc; i < m_count; i++)
            olddata[i].Release();
    }

    m_alloc = alloc;
    m_data = (SCheckedItemSlot*)SMemReAlloc(olddata,
        alloc * sizeof(SCheckedItemSlot), s_typeName, -2, 0x10);
    if (m_data == 0)
    {
        m_data = (SCheckedItemSlot*)SMemAlloc(
            alloc * sizeof(SCheckedItemSlot), s_typeName, -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = alloc < m_count ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
            {
                SCheckedItemSlot* slot = &m_data[i];
                if (slot != 0)
                {
                    CAgent* held = olddata[i].m_value;
                    slot->m_value = held;
                    if (held)
                        held->m_refcount++;
                }

                olddata[i].Release();
            }
            SMemFree(olddata, s_typeName, -2, 0);
        }
    }
}

template <>
TSGrowableArray<SCheckedItemSlot>::~TSGrowableArray()
{
    for (unsigned int i = 0; i < m_count; i++)
    {
        SCheckedItemSlot* element = &m_data[i];
        element->Release();
    }
    if (m_data)
        SMemFree(m_data, s_typeName, -2, 0);
}

//  0x6F3953B0 - the folded second instantiation of the destructor above.
__declspec(naked) void SecondItemAgentPtrArrayDtorAlias()
{
    __asm
    {
        mov     eax, 06F393660h
        jmp     eax
    }
}

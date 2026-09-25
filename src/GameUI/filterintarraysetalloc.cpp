//============================================================================
//  0x6F311880 - SFilterIntArray::SetAlloc (GameUI/cinematicfilter.h).
//
//  CItem vtable-closure batch 2.  The generic TSGrowableArray<T>::SetAlloc
//  shape (Containers/tsarray.inl) specialised for a 4-byte element, tagged
//  ".?AVCImVector@NTempest@@" (agent_worktrees' own string-table resolve of
//  aAvcimvectorNte, 0x6FA76574) - the original instantiation this array was
//  cloned from was over `CImVector` (an NTempest four-byte handle type), not
//  the `int` cinematicfilter.h's own header declares; the Storm tag is what
//  the shipped call sites actually push, cast is cosmetic to the bytes.
//
//  Own translation unit: SFilterSlotArray/SFilterWordArray/SFilterPairArray
//  each keep their own SetAlloc thunked at a different address (a genuinely
//  separate shipped module per array kind), so this one is not folded in
//  with any of them.
//============================================================================
#include "cinematicfilter.h"
#include "storm.h"

static const char s_typeName[] = ".?AVCImVector@NTempest@@";

void SFilterIntArray::SetAlloc(unsigned int alloc)
{
    int* olddata = m_data;

    m_alloc = alloc;
    m_data = (int*)SMemReAlloc(olddata, alloc * sizeof(int), s_typeName,
                                -2, 0x10);
    if (m_data == 0)
    {
        m_data = (int*)SMemAlloc(alloc * sizeof(int), s_typeName, -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = alloc < m_count ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
                m_data[i] = olddata[i];
            SMemFree(olddata, s_typeName, -2, 0);
        }
    }
}

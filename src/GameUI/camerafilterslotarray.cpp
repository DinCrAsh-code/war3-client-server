//============================================================================
//  0x6F303580 - SFilterSlotArray::SetAlloc, and 0x6F3079D0 -
//  SFilterSlotArray::SetCount.  See cinematicfilter.h.
//
//  A plain growable-array reallocation of 12-byte (three-float, i.e.
//  NTempest::C3Vector-shaped - the Storm allocation tag is
//  "class NTempest::C3Vector") elements: try to resize the existing block in
//  place first (Storm flag 0x10), and only when that fails allocate a fresh
//  one and bytewise-copy the surviving elements across - not a per-element
//  copy/destroy pair, because a plain three-float record has neither.  This
//  is TSGrowableArray<T>::SetAlloc's own shape (tsarray.inl) with the
//  per-element construct/destruct dropped, which is exactly what that
//  template collapses to for a trivial T - but this is not that template
//  instantiated: SFilterSlotArray's own `m_data` is untyped (`void*`), and
//  SetCount's own zero-fill loop below stores through the x87 stack
//  (`fst`/`fstp`), not through a placement `new`.
//
//  SetCount is the one call site with a growth check at all - shrinking
//  never reallocates, and every element added is zero-filled by hand: a
//  local `float zero = 0.0f` stored three times per new element rather than
//  a per-element constructor call, which is what the flat `fst` writes and
//  the lack of any placement-new null check both say.
//
//  Both were naked `mov eax,<addr>` / `jmp eax` thunks in gameui_thunks.cpp;
//  replaced now that their real bodies are reconstructed.
//
//  Own translation unit: CWorldFrameWar3::CreateWaypointIndicators's own
//  sibling arrays call these two by address only, not by name, so nothing
//  else in this tree reaches them with a real call; the redirects existed so
//  the tree would link.  CCameraWar3::Load (below this slice) is what
//  actually calls SetCount for real, on the +0xA8 filter-slot array
//  Load/Save mirror each other over.
//============================================================================
#include "cinematicfilter.h"
#include "storm.h"

extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int n);

static const char kElementTag[] = "class NTempest::C3Vector";

//  Never stores `alloc` anywhere - m_alloc is a field this array declares
//  (matching every other growable-array shape in this codebase) but never
//  itself writes or reads; the dump's own SetAlloc body only ever touches
//  m_data.  Left as the dump has it rather than "fixed": a field an
//  instantiation does not use is not this pass's to invent a use for.
void SFilterSlotArray::SetAlloc(unsigned int alloc)
{
    void* olddata = m_data;

    void* newdata = SMemReAlloc(olddata, alloc * 0x0C, kElementTag, -2, 0x10);
    if (newdata != 0)
    {
        m_data = newdata;
        return;
    }

    newdata = SMemAlloc(alloc * 0x0C, kElementTag, -2, 0);
    m_data = newdata;
    if (olddata != 0)
    {
        if (newdata != 0)
        {
            unsigned int moved = (alloc < m_count) ? alloc : m_count;
            memcpy(newdata, olddata, moved * 0x0C);
        }
        SMemFree(olddata, kElementTag, -2, 0);
    }
}

void SFilterSlotArray::SetCount(unsigned int count)
{
    if (count == m_count)
        return;

    if (count == 0)
    {
        if (m_data != 0)
            SMemFree(m_data, kElementTag, -2, 0);
        m_alloc = 0;
        m_count = 0;
        m_data = 0;
        return;
    }

    SetAlloc(count);

    if (m_count < count)
    {
        float* elem = (float*)((char*)m_data + m_count * 0x0C);
        for (unsigned int i = m_count; i < count; i++)
        {
            elem[0] = 0.0f;
            elem[1] = 0.0f;
            elem[2] = 0.0f;
            elem += 3;
        }
    }

    m_count = count;
}

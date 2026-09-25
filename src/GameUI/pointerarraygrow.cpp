//============================================================================
//  0x6F2F56B0 / 0x6F2F5710 - SPointerArray's own ComputeChunk and SetAlloc,
//  a separate shipped module from Append's own 0x6F2F79B0 (a different
//  address neighbourhood - own translation unit, same reasoning
//  pointerarrayappend.cpp already gives, and a second local copy of the
//  struct for the same reason: MSVC mangles only the class *name*, not the
//  translation unit, so a second declaration of the same layout still
//  binds to these two shipped bodies.
//
//  Both are the generic TSGrowableArray<T>::ComputeChunk/SetAlloc shape
//  (Containers/tsarray.inl) specialised for a 4-byte pointer element, tagged
//  ".PAVCInputMode@@" (agent_worktrees' own string-table resolve of
//  aPavcinputmode, 0x6FA75CC4) - the original instantiation this struct's
//  layout was cloned from was over `CInputMode*`, not the `STrackTarget*`
//  gameui.h's own copy uses; the class *name* is still `SPointerArray`
//  either way, which is all the mangled Storm tag call sites need to agree
//  on.
//
//  ComputeChunk rounds down to the nearest power of two below 0x40 (a
//  `count==0` or already-a-power-of-two count returns unchanged, everything
//  else strips its lowest set bit until one remains); at or above 0x40 it
//  latches the cap into `m_chunk` and returns it.  SetAlloc tries an
//  in-place Storm_405 (SMemReAlloc) resize first and falls back to a fresh
//  Storm_401 (SMemAlloc) block with an element-by-element pointer copy (no
//  destructor calls - the element is a bare pointer) and a Storm_403
//  (SMemFree) of the old block, exactly like every other
//  TSGrowableArray<T*>::SetAlloc in this repo.
//============================================================================
#include "storm.h"

struct SPointerArray
{
    unsigned int ComputeChunk(unsigned int count);
    void SetAlloc(unsigned int alloc);

    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    void**       m_data;    // +0x08
    unsigned int m_chunk;   // +0x0C
};

static const char s_typeName[] = ".PAVCInputMode@@";

unsigned int SPointerArray::ComputeChunk(unsigned int count)
{
    if (count < 0x40)
    {
        unsigned int chunk = count;
        while ((chunk & (chunk - 1)) != 0)
            chunk = chunk & (chunk - 1);

        if (chunk < 1)
            return 1;
        return chunk;
    }

    m_chunk = 0x40;
    return 0x40;
}

void SPointerArray::SetAlloc(unsigned int alloc)
{
    void** olddata = m_data;

    m_alloc = alloc;
    m_data = (void**)SMemReAlloc(olddata, alloc * sizeof(void*), s_typeName,
                                  -2, 0x10);
    if (m_data == 0)
    {
        m_data = (void**)SMemAlloc(alloc * sizeof(void*), s_typeName, -2, 0);
        if (olddata != 0)
        {
            unsigned int moved = alloc < m_count ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
                m_data[i] = olddata[i];
            SMemFree(olddata, s_typeName, -2, 0);
        }
    }
}

//============================================================================
//  0x6F476F40 and 0x6F47BAF0 - the two reference-count adjusters
//  AdjustWidgetPathRefs (widgetpathref.cpp) calls.  See the header for why
//  both are thiscall members.
//
//  They share this translation unit because neither calls the other - they
//  are the single-object and whole-array forms of the same +1/-1 on a
//  counter at +0x40.
//============================================================================
#include "widgetpathrefadjust.h"
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F476F40.  Resolve this handle, then pin or unpin the path data behind
//  it.  LookupHandle's result is dereferenced without a null check -
//  faithful to the shipped code, whose one caller only reaches it after the
//  agent-tag test has already established the object exists.
//----------------------------------------------------------------------------
void SPathRefHandle::AdjustPathRefCount(int add)
{
    CHandleObject* obj = LookupHandle(m_handle, m_typeTag);
    SPathRefCounted* path = *(SPathRefCounted**)((char*)obj + 0x98);
    if (add)
        path->m_pinCount += 1;
    else
        path->m_pinCount -= 1;
}

//----------------------------------------------------------------------------
//  0x6F47BAF0.  The same adjustment across the array, skipping empty slots.
//
//  The null test really is written twice in the source: the shipped loop
//  tests the slot in place (`cmp [eax+edx*4], 0`), and then, on the path
//  where it was non-zero, loads it and tests the loaded value again.  One
//  test compiles to one test; this pair only appears from two.
//----------------------------------------------------------------------------
void SPathRefArray::AdjustPathRefArray(int add)
{
    for (unsigned int i = 0; i < m_count; ++i)
    {
        if (m_pItems[i] == 0)
            continue;
        SPathRefCounted* item = m_pItems[i];
        if (item == 0)
            continue;
        if (add)
            item->m_pinCount += 1;
        else
            item->m_pinCount -= 1;
    }
}

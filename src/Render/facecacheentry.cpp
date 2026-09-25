//============================================================================
//  FACEDATA's own constructor, and two small members of the bigger
//  face-cache entry (Render/facedata.h's FACECACHEENTRY) that release its
//  cached-glyph-bitmap block.
//============================================================================
#include "facedata.h"
#include "refcnt.h"

//  0x6F7C85F0 - FACEDATA::FACEDATA.  Statement order is ascending offset;
//  the shipped stream groups the five consecutive zero-stores (+0x0C..
//  +0x1C) and schedules the vtable stamp and the two remaining zero-stores
//  around them - the same kind of independent-store rescheduling
//  Render/facechartable.cpp's own CHARCODEDESC_TABLE ctor already scores
//  0.968 against.  +0x08 is deliberately untouched - the ctor never writes
//  it, and 0x6F7C9210 fills +0x20/+0x24 itself right after this returns.
FACEDATA::FACEDATA()
{
    m_vtable = (void*)kFaceDataVftable;
    m_reserved04 = 0;
    m_reserved0C = 0;
    m_reserved10 = 0;
    m_reserved14 = 0;
    m_reserved18 = 0;
    m_reserved1C = 0;
    m_reserved20 = 0;
    m_reserved24 = 0;
}

//----------------------------------------------------------------------------
//  The cached-glyph-bitmap block a FACECACHEENTRY holds at +0xA0
//  (m_glyphCache) - a `TRefCnt`-derived object with one more reference of
//  its own at +0x28, released unconditionally once this object's own count
//  drops to (at most) one.  Not RTTI-named; only the two offsets
//  sub_6F7C7E70 touches are given fields.
//----------------------------------------------------------------------------
struct GLYPHCACHEBLOCK : public TRefCnt
{
    //  0x6F7C7E70 - drop one reference on this block, and once that leaves
    //  at most one reference outstanding, drop the nested reference at
    //  +0x28 too.  The nested Release() is unconditional in the shipped
    //  body - no null check on m_nested - so this does not add one either.
    void ReleaseWithNested();

    char     m_reserved08[0x20];
    TRefCnt* m_nested;   // +0x28
};

void GLYPHCACHEBLOCK::ReleaseWithNested()
{
    Release();
    if ((unsigned int)m_refcount <= 1)
    {
        TRefCnt* nested = m_nested;
        m_nested = 0;
        nested->Release();
    }
}

//  0x6F7C4F90 - FACECACHEENTRY's own m_glyphCache release-and-clear, then a
//  tail call into the rest of the entry's teardown (0x6F7C4950, thunked -
//  Render/facecacheentry_thunks.cpp).
void FACECACHEENTRY::ReleaseGlyphCache()
{
    if (m_glyphCache)
        ((GLYPHCACHEBLOCK*)m_glyphCache)->ReleaseWithNested();
    m_glyphCache = 0;
    ReleaseDetail();
}

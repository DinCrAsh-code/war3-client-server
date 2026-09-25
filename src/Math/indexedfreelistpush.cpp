//============================================================================
//  0x6F4BC200 - push one index onto one of two indexed free lists a single
//  object carries, chosen by the sign of the index handed in.
//
//  A negative `index` (its own sign bit masked off before use) selects the
//  sub-object at +0x20 with its own head/remaining pair at +0x44/+0x4C; a
//  non-negative one selects `this` itself with its own pair at +0x40/+0x48.
//  The second argument is never read by either function - confirmed dead by
//  0x6F4BC1D0's own dump (indexedfreelist.h), which has no use anywhere for
//  its own `arg_4`.
//
//  See indexedfreelist.h's own correction note: on `g_unk6FAB778C`
//  (Pathfinding/gridregistrationteardown.cpp), one of this method's two
//  known receivers, `this` is now RTTI-confirmed to be
//  `NTempest::CDynTable<CPrRgEntry>` rather than an untyped allocator.
//
//  See indexedfreelist.h for why PushIndexedFreeListEntry itself
//  (0x6F4BC1D0) is a separate translation unit.  STwoIndexedFreeLists is
//  declared in that shared header now (not here) so
//  indexedfreelistallocatedispatch.cpp's own AllocateAt - the mirror-image
//  "hand out a slot" to this "give one back" - can share the one real
//  layout instead of duplicating it.
//============================================================================
#include "indexedfreelist.h"

void __thiscall STwoIndexedFreeLists::Push(int index, int unused)
{
    if (index < 0)
    {
        SIndexedFreeListOwner* obj = &m_negativeOwner;
        PushIndexedFreeListEntry(index & 0x7FFFFFFF, unused, obj,
                                 &m_negativeHead, &m_negativeRemaining);
    }
    else
    {
        SIndexedFreeListOwner* obj = (SIndexedFreeListOwner*)this;
        PushIndexedFreeListEntry(index, unused, obj,
                                 &m_positiveHead, &m_positiveRemaining);
    }
}

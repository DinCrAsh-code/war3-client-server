//============================================================================
//  0x6F47C1A0 - SPathRefArray::ResizeAndFillMasks.
//
//  Reached from SPathRefArrayCtor::Construct (0x6F47C350, below its own
//  depth cut - Widget/widget_load.cpp and widget_addfootprint.cpp both
//  redirect to it), which the shipped code calls after growing/shrinking a
//  widget's path-ref array to bring every live slot's mask field up to
//  date.
//
//  `handleOwner`'s handle/type pair at +0x0C/+0x10 resolves an object which,
//  once checked-cast against the '+agl' agile tag the same way
//  CWidget::AdjustWidgetPathRefs does, becomes the owner every freshly
//  allocated element is registered with; a failed handle or a mismatched
//  tag hands that allocator a null owner instead; not another return exit,
//  this is a pointer the code carries all the way through.
//
//  Shrink first (tear down every element index >= the new count, same
//  per-element TeardownRegistration ReleasePathRegistration uses, but
//  without nulling the slot - the slot is either about to be overwritten by
//  the fill loop below, in a whole-array reload, or never read again),
//  then grow the backing buffer if the new count needs more of it than the
//  array currently has (the exact ChunkSize/remainder/SetAlloc shape
//  SPathRefArray::Load uses, pathref_load.cpp), then walk every index up to
//  the new count: reuse an existing non-null element that was already
//  inside the *old* count, allocate a fresh one for every other slot, and
//  fold `masks[i]`'s low 24 bits into that element's own +0x34 together
//  with the top byte it already had and a set high bit, one field this
//  target does not otherwise name (SPathRefCounted only names +0x40,
//  widgetpathrefadjust.h).
//
//  Own translation unit: LookupHandle, GetField238, ChunkSize, SetAlloc,
//  TeardownRegistration and the allocator are all real calls out of it.
//============================================================================
#include "widget.h"
#include "game.h"
#include "widgetpathrefadjust.h"
#include "spatialgrid.h"   // CGridRegistration::TeardownRegistration - real
                            // body now, gridregistrationteardown.cpp - see
                            // that file's own note on the tempest presence
                            // host's (`g_unk6FAB778C`) now-confirmed
                            // NTempest::CDynTable<CPrRgEntry> identity

//  Same '+agl' tag every handle-registered object carries - see
//  Widget/widgetpathref.cpp for the full note.
static const unsigned int kAgentTagPathable = 0x2B61676Cu;

//  Only the field this function tests.
struct SAgentHandleObject
{
    char         m_reserved00[0x0C];
    unsigned int m_typeTag;      // +0x0C
};

//  0x6F481430 - Misc/misc_trivial_getters.cpp.  g_pTimeSync itself is
//  declared in game.h (already included above).
struct SField238Owner { int __thiscall GetField238(); };

//  0x6F47B8B0 / 0x6F47B900 - the array's own chunk-size query and its
//  reallocation, same as pathref_load.cpp's own declaration.
struct SPathRefArrayGrow
{
    unsigned int ChunkSize(unsigned int wanted);
    void SetAlloc(unsigned int count);
};

//  0x6F4A0440 - now real, Pathfinding/pathrefallocator.cpp - kept in its
//  own TU rather than defined here: making its body visible in this TU let
//  /Ob2 inline it into ResizeAndFillMasks below and reshuffle that
//  already-EXACT function's own register allocation, a real regression
//  (see pathrefallocator.cpp's own header comment).  `this` is the pointer
//  TimeSync's own +0x238 gives (GetField238's return, reused as an
//  allocator rather than an int - the shipped code never dereferences it
//  here, only carries it through in ecx as AllocRegistration's own m_grid,
//  Pathfinding/gridregistrationalloc.cpp), and the two stack arguments
//  (owner, a fixed 0) are what retn 8 cleans.
struct SPathRefAllocator
{
    SPathRefCounted* __thiscall Alloc(void* owner, int zero);
};

void SPathRefArray::ResizeAndFillMasks(CWidget* handleOwner,
                                       unsigned int newCount,
                                       const unsigned int* masks)
{
    void* object = LookupHandle(*(unsigned int*)((char*)handleOwner + 0x0C),
                                *(int*)((char*)handleOwner + 0x10));
    SAgentHandleObject* owner;
    if (object == 0)
        owner = (SAgentHandleObject*)object;
    else
        owner = (((SAgentHandleObject*)object)->m_typeTag ==
                 kAgentTagPathable) ? (SAgentHandleObject*)object : 0;

    int tick = ((SField238Owner*)g_pTimeSync)->GetField238();

    unsigned int oldCount = m_count;
    if (newCount < oldCount)
    {
        for (unsigned int i = newCount; i < oldCount; i++)
        {
            SPathRefCounted* item = m_pItems[i];
            if (item != 0)
                ((CGridRegistration*)item)->TeardownRegistration();
        }
    }

    if (newCount > oldCount)
    {
        if (newCount > *(const unsigned int*)m_reserved00)
        {
            unsigned int alloc = newCount;
            if (newCount > 1)
            {
                unsigned int chunk =
                    ((SPathRefArrayGrow*)this)->ChunkSize(newCount);
                unsigned int rem = newCount % chunk;
                if (rem != 0)
                    alloc = chunk - rem + newCount;
            }
            ((SPathRefArrayGrow*)this)->SetAlloc(alloc);
        }
    }

    m_count = newCount;
    for (unsigned int i = 0; i < newCount; i++)
    {
        SPathRefCounted* item = m_pItems[i];
        if (item == 0 || i >= oldCount)
        {
            item = ((SPathRefAllocator*)tick)->Alloc(owner, 0);
            m_pItems[i] = item;
        }

        unsigned int mask = masks[i];
        unsigned int prior = *(unsigned int*)((char*)item + 0x34);
        mask &= 0xFFFFFF;
        prior &= 0xFF000000;
        mask |= prior;
        mask |= 0x1000000;
        *(unsigned int*)((char*)item + 0x34) = mask;
    }
}

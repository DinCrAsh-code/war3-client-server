//============================================================================
//  0x6F4D76B0 - SAnimRecordArray::SetAlloc: grow or shrink
//  CAnimTrackRing::PushBack's own record storage (Widget/animtrackring.h) to
//  exactly `newAlloc` records.
//
//  Three shapes, on whether newAlloc is above, below or equal to the
//  current live count:
//
//  * shrinking (newAlloc < m_count) frees every dropped record's own
//    key-array heap block (when its capacity is above the two-inline
//    threshold animkeyarray.cpp's rule already gives) and nothing else -
//    the storage itself is never shrunk;
//  * equal is a no-op past re-storing the count;
//  * growing reallocates the block when `newAlloc` exceeds the current
//    allocation - rounding up to a chunk through this array's own
//    ComputeChunk (0x6F4D3A30) exactly the way SetCount's own grow does
//    elsewhere in this repo, except when newAlloc <= 1, which skips the
//    rounding and asks for exactly newAlloc - and default-initialises every
//    newly live record's *key-array header only* (capacity/count/pointer at
//    the record's own +0x08/+0x0C/+0x10), leaving the record's other two
//    words and inline key storage whatever the allocator handed back.
//
//  The reallocation itself is not this array's own SMemReAlloc/SMemAlloc
//  pair: the shipped body calls 0x6F4D5940, Widget/animtrackarray.cpp's
//  SAnimTrackArray::SetAlloc, passing this object's own `this`.  That is a
//  real, ground-truth call in the dump, not an inference - and it is not as
//  strange as it looks: a ring record and a SAnimTrack are the same 28
//  bytes, deep-copy included, and SAnimRecordArray's own three-word header
//  (alloc/count/data) is field-for-field SAnimTrackArray's own capacity/
//  count/tracks, so the shipped code shares the one function that already
//  knows how to grow an array of these records rather than duplicating it.
//  Declared here as a local copy of that class for the same reason every
//  other file in this repo re-declares a shared class: MSVC mangles a
//  member only by the class name.
//
//  Own translation unit: two real calls (ComputeChunk, the shared grow) and
//  the per-dropped-record SMemFree.
//============================================================================
#include "animtrackring.h"
#include "storm.h"

//  A local copy of animtrackarray.cpp's array header, exactly wide enough
//  for the grow call this file makes.  Same class name, same symbol.
struct SAnimTrackArray
{
    void SetAlloc(unsigned int newCount);
};

//  Reused for the same reason animtrackarray.cpp's own asc_6FA4F788-tagged
//  calls are: the operand canonicalises to an anonymous symbol either way,
//  and RTTI is off in this build.
static const char s_recordTag[] = ".?AV?$TSFixedArray@USAnimTrackRecord@@$0?CCCCCCCD@@@";

unsigned int SAnimRecordArray::ComputeChunk(unsigned int count)
{
    const unsigned int maxchunk = 256 / 28;   // 9 - a record is 28 bytes

    if (count < maxchunk)
    {
        unsigned int chunk = count;
        while ((chunk & (chunk - 1)) != 0)
            chunk &= chunk - 1;

        if (chunk < 1)
            return 1;
        return chunk;
    }

    return maxchunk;
}

void SAnimRecordArray::SetAlloc(unsigned int newAlloc)
{
    unsigned int oldCount = m_count;

    if (newAlloc > oldCount)
    {
        if (newAlloc > m_alloc)
        {
            unsigned int alloc = newAlloc;
            if (newAlloc > 1)
            {
                unsigned int chunk = ComputeChunk(newAlloc);
                unsigned int over = newAlloc % chunk;
                if (over != 0)
                    alloc = newAlloc + (chunk - over);
            }
            ((SAnimTrackArray*)this)->SetAlloc(alloc);
        }

        for (unsigned int i = oldCount; i < newAlloc; i++)
        {
            SAnimKeyArrayHeader* keys = (SAnimKeyArrayHeader*)((char*)m_data +
                                        i * 0x1C + 8);
            if (keys != 0)
            {
                keys->m_capacity = 0;
                keys->m_count = 0;
                keys->m_keys = 0;
            }
        }

        m_count = newAlloc;
        return;
    }

    if (newAlloc != oldCount)
    {
        for (unsigned int i = newAlloc; i < oldCount; i++)
        {
            SAnimKeyArrayHeader* keys = (SAnimKeyArrayHeader*)((char*)m_data +
                                        i * 0x1C + 8);
            if (keys->m_capacity > 2)
                SMemFree(keys->m_keys, s_recordTag, -2, 0);
        }
    }

    m_count = newAlloc;
}

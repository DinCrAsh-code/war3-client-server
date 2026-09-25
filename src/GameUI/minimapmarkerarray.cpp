//============================================================================
//  0x6F36C8A0 - SMinimapMarkerArray::SetCount.
//
//  A *fourth* growable-array shape, and the one that is closest to Storm's
//  own: unlike the three SetCounts this session already reconstructed, this
//  one does round the allocation up to a chunk, and unlike Storm's
//  TSGrowableArray::SetCount it has no shrink arm at all - a smaller count
//  is simply stored, elements and allocation untouched.
//
//  Its two call sites are the minimap's own arrays at +0x5D0 and +0x5E4
//  (0x6F36E322 and 0x6F36E333), asked for sixteen and 256 markers.
//
//  **The element constructor writes four of its ten words twice**, and that
//  is the shipped body, not a transcription slip: the sub-object at +0x08
//  runs its own constructor first (three floats and a word, through the
//  x87), and then the record's own body clears all ten words with a plain
//  register zero, the four above included.  Written any other way the
//  stream is four instructions short.  The null test in front of the whole
//  thing is placement new's own.
//
//  The loop walks a byte offset with a 0x28 stride and counts the remaining
//  elements down, which is what the shipped stream does; the element base is
//  re-read from the member every iteration.
//============================================================================
#include "storm.h"

extern "C" void* __cdecl memset(void* dst, int value, unsigned int count);
#pragma intrinsic(memset)

//----------------------------------------------------------------------------
//  One marker.  0x28 bytes - the loop's own stride.  Only the shape the
//  constructor establishes is named: two words, a four-member sub-object
//  with its own constructor, and four more words.  Nothing in this tree
//  reads any of them back.
//----------------------------------------------------------------------------
struct SMinimapMarker
{
    SMinimapMarker();

    int   m_field00;    // +0x00
    int   m_field04;    // +0x04
    float m_f08;        // +0x08  the sub-object at +0x08 .. +0x18
    float m_f0C;        // +0x0C
    float m_f10;        // +0x10
    int   m_field14;    // +0x14
    int   m_field18;    // +0x18
    int   m_field1C;    // +0x1C
    int   m_field20;    // +0x20
    int   m_field24;    // +0x24
};

SMinimapMarker::SMinimapMarker()
{
    //  The sub-object's constructor, inlined - three floats off one `fldz`
    //  and a zeroed word.
    m_f08 = 0.0f;
    m_f0C = 0.0f;
    m_f10 = 0.0f;
    m_field14 = 0;

    //  ... and then the record's own body, which clears every word again.
    memset(this, 0, sizeof(*this));
}

struct SMinimapMarkerArray
{
    //  0x6F33FC80 - pick the growth granularity.  `retn 4`; still a
    //  redirect.
    unsigned int ComputeChunk(unsigned int count);
    //  0x6F35D150 - change the allocated size.  `retn 4`; still a redirect.
    void SetAlloc(unsigned int alloc);
    //  0x6F36C8A0 - this file.
    void SetCount(unsigned int count);

    unsigned int    m_alloc;    // +0x00
    unsigned int    m_count;    // +0x04
    SMinimapMarker* m_data;     // +0x08
    unsigned int    m_chunk;    // +0x0C
};

void SMinimapMarkerArray::SetCount(unsigned int count)
{
    if (count <= m_count)
    {
        m_count = count;
        return;
    }

    if (count > m_alloc)
    {
        unsigned int chunk = m_chunk;
        if (chunk == 0)
            chunk = ComputeChunk(count);

        unsigned int alloc = count;
        unsigned int over = count % chunk;
        if (over != 0)
            alloc = chunk - over + count;

        SetAlloc(alloc);
    }

    //  The byte offset is formed *inside* the guard, not above it: the
    //  shipped stream emits the `lea`/`add` chain that multiplies the old
    //  count by 0x28 after the `cmp`, and hoisting it costs four
    //  instructions their place.
    unsigned int old = m_count;
    if (old < count)
    {
        unsigned int offset = old * 0x28;
        unsigned int remaining = count - old;

        do
        {
            SMinimapMarker* marker =
                (SMinimapMarker*)((char*)m_data + offset);
            if (marker != 0)
                new (marker) SMinimapMarker();

            offset += 0x28;
            remaining--;
        }
        while (remaining != 0);
    }

    m_count = count;
}

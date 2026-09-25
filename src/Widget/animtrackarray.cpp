//============================================================================
//  0x6F4D5940 - SAnimTrackArray::SetAlloc.
//
//  Resize an array of animation tracks, where each track itself owns a
//  small heap array of keys.  Both levels use the same "two or fewer live
//  inline, more than two go to Storm" rule, which is why every capacity
//  test in here compares against 2 and why the release paths only free a
//  block whose capacity exceeded it.
//
//  Four things the shipped code does that the source has to spell out:
//
//  * Tracks being dropped (index >= newCount) have their key blocks freed
//    before anything else happens, using the *old* capacity to decide.
//  * The grow path tries an in-place SMemReAlloc with Storm's 0x10 flag
//    first - "resize where it is or fail" - and returns immediately when
//    that works, so the copy loop below only ever runs on a block that
//    actually moved.
//  * Shrinking to one element or fewer parks the array back on its own
//    inline storage rather than on the heap.
//  * The per-track copy is a deep copy: the destination's key array is
//    allocated fresh (or pointed at the destination's own inline keys) and
//    filled element by element, and the source's key block is then freed -
//    so a track is never left with two owners of one key block.
//
//  The two `!= 0` guards inside the copy - on the destination track and on
//  each destination key slot - are the shipped code's, not this
//  reconstruction's: it computes both addresses and tests them before use,
//  which is what its `jz` after the address arithmetic is.
//============================================================================
#include "storm.h"

//  asc_6FA4F788 / aAuspriteanim - the two Storm log tags this function
//  passes, one per level.  Spelled as static literals for the same reason
//  tsarray.inl's own s_typeName is: the shipped code hands Storm an RTTI
//  descriptor name in the __FILE__ slot, this build has RTTI off so
//  typeid().raw_name() is not available to generate it, and the exact
//  bytes do not affect the generated code - the operand canonicalises to
//  an anonymous symbol on both sides either way.  Deliberately NOT named
//  through funcmap's DATA table: asc_6FA4F788 is *shared* with the
//  TSGrowableArray instantiations (it is the same descriptor string
//  vectorfree_ptr.cpp's destructor passes), so binding it to a name from
//  this file would resolve the target's operand for every one of those
//  too and break the match they already have.
static const char s_animKeyTag[] =
    ".?AV?$TSFixedArray@UANIMKEY@@$0?CCCCCCCD@@@";
static const char s_animTrackTag[] = ".\\AuSpriteAnim.cpp";

struct SAnimTrack
{
    int          m_field0;        // +0x00
    int          m_field4;        // +0x04
    unsigned int m_keyCapacity;   // +0x08
    unsigned int m_keyCount;      // +0x0C
    int*         m_keys;          // +0x10
    int          m_inlineKeys[2]; // +0x14  ends the record at 0x1C
};

struct SAnimTrackArray
{
    unsigned int m_capacity;      // +0x00
    unsigned int m_count;         // +0x04
    SAnimTrack*  m_tracks;        // +0x08
    SAnimTrack   m_inline[1];     // +0x0C

    void SetAlloc(unsigned int newCount);
};

void SAnimTrackArray::SetAlloc(unsigned int newCount)
{
    SAnimTrack* old = m_tracks;

    for (unsigned int dropped = newCount; dropped < m_count; dropped++)
        if (old[dropped].m_keyCapacity > 2)
            SMemFree(old[dropped].m_keys, s_animKeyTag, -2, 0);

    if (newCount > 1)
    {
        if (m_capacity > 1)
        {
            //  Storm's 0x10 flag: resize in place or fail, never move.
            SAnimTrack* grown = (SAnimTrack*)SMemReAlloc(
                old, newCount * sizeof(SAnimTrack), s_animTrackTag, -2, 0x10);
            m_tracks = grown;
            if (grown != 0)
            {
                m_capacity = newCount;
                return;
            }

            m_tracks = (SAnimTrack*)SMemAlloc(
                newCount * sizeof(SAnimTrack), s_animTrackTag, -2, 0);
        }
        else
        {
            m_tracks = (SAnimTrack*)SMemAlloc(
                newCount * sizeof(SAnimTrack), s_animTrackTag, -2, 0);
        }
    }
    else
    {
        if (m_tracks == m_inline)
        {
            m_capacity = newCount;
            return;
        }
        m_tracks = m_inline;
    }

    m_capacity = newCount;
    if (old == 0)
        return;

    unsigned int keep = newCount;
    if (keep >= m_count)
        keep = m_count;

    if (keep > 0)
    {
        for (unsigned int track = 0; track < keep; track++)
        {
            SAnimTrack* src = &old[track];
            SAnimTrack* dst = &m_tracks[track];

            if (dst != 0)
            {
                dst->m_field0 = src->m_field0;
                dst->m_field4 = src->m_field4;
                dst->m_keyCapacity = 0;
                dst->m_keyCount = 0;
                dst->m_keys = 0;

                unsigned int keys = src->m_keyCount;
                const int* srcKeys = src->m_keys;

                if (keys > 2)
                {
                    dst->m_keys = 0;
                    dst->m_keys = (int*)SMemReAlloc(0, keys * 4,
                                                    s_animKeyTag, -2, 0);
                }
                else
                {
                    dst->m_keys = dst->m_inlineKeys;
                }

                dst->m_keyCapacity = keys;

                for (unsigned int key = 0; key < keys; key++)
                {
                    int* slot = dst->m_keys + key;
                    if (slot != 0)
                        *slot = srcKeys[key];
                }

                dst->m_keyCount = keys;
            }

            if (src->m_keyCapacity > 2)
                SMemFree(src->m_keys, s_animKeyTag, -2, 0);
        }
    }

    if (old != m_inline)
        SMemFree(old, s_animTrackTag, -2, 0);
}

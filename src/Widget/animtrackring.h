//============================================================================
//  The sprite layer's ring of animation-track records (0x6F4D3xxx/0x6F4D7xxx,
//  reached from SpriteStartAnimation, 0x6F4D7E50).
//
//  Layout is fixed by the three functions that touch it:
//
//      +0x00  unsigned char   count of live records
//      +0x01  unsigned char   index of the oldest record - the ring's head
//      +0x02  unsigned char   index of the newest record - where a push lands
//      +0x04  the growable storage's own header (0x6F4D76B0 takes its address)
//      +0x08  unsigned int    the ring's modulus, i.e. how many records fit
//      +0x0C  the record array itself
//
//  A record is 28 bytes: every index-to-address computation in the family is
//  `lea r, [i*8]` / `sub r, i` / `lea r, [base + r*4]`, which is i*7*4.
//
//  The modulus is a real `div` (0x6F4D3E90's `div dword ptr [ecx+8]`), not a
//  mask - the ring's size is not required to be a power of two.
//============================================================================
#ifndef ANIMTRACKRING_H
#define ANIMTRACKRING_H

struct SAnimTrackRecord
{
    //  +0x00/+0x04 - written/read as plain dwords by
    //  SaveAnimTrackRecord/LoadAnimTrackRecord (Widget/animtrackringsave.cpp)
    //  - the same 4-byte writer/reader pair every other unadorned dword
    //    field in this repo goes through, not WriteFloat/ReadFloat - so
    //    these are integers, not floats, even though nothing in this
    //    closure says what they count.  +0x08 onward is the record's own
    //    key-array header (SAnimKeyArrayHeader below), addressed by pointer
    //    arithmetic off `this` rather than nested here, matching
    //    animrecordarraysetalloc.cpp's own established idiom.
    unsigned int m_field0;   // +0x00
    unsigned int m_field4;   // +0x04
    char         m_reserved08[28 - 8];
};

//----------------------------------------------------------------------------
//  The ring's storage, seen as the three-word array header it is: the ring's
//  +0x04, +0x08 and +0x0C are one object, and CAnimTrackRing::PushBack hands
//  its address to 0x6F4D76B0.  Declared separately rather than folded into
//  the three fields below, which the two older bodies here already address
//  by name; PushBack casts.
//
//  Not Widget/animtrackarray.cpp's SAnimTrackArray - that is a different
//  class at a different address (0x6F4D5940), for the array of *tracks* a
//  sprite owns rather than for this ring's records.
//----------------------------------------------------------------------------
struct SAnimRecordArray
{
    //  0x6F4D76B0 - grow or shrink to hold exactly `alloc` records.
    //  Widget/animrecordarraysetalloc.cpp.
    void SetAlloc(unsigned int alloc);
    //  0x6F4D3A30 - the growth granularity: largest power of two <= count,
    //  capped at 9 (256 bytes / a 28-byte record).  `retn 4`, and like its
    //  SAnimKeyArray twin (0x6F4D38A0) never reads ecx - still a member
    //  because every call site leaves `this` there regardless.
    unsigned int ComputeChunk(unsigned int count);

    unsigned int      m_alloc;   // +0x00  the ring's +0x04
    unsigned int      m_count;   // +0x04  the ring's +0x08 - its modulus
    SAnimTrackRecord* m_data;    // +0x08  the ring's +0x0C
};

//----------------------------------------------------------------------------
//  A record's own key-array header, at record+0x08 (SAnimTrack's own
//  m_keyCapacity/m_keyCount/m_keys, Widget/animtrackarray.cpp) - the three
//  fields SAnimRecordArray::SetAlloc default-initialises on every newly
//  live record and frees on every dropped one.
//----------------------------------------------------------------------------
struct SAnimKeyArrayHeader
{
    unsigned int m_capacity;   // +0x00 (record +0x08)
    unsigned int m_count;      // +0x04 (record +0x0C)
    int*         m_keys;       // +0x08 (record +0x10)
};

class CAnimTrackRing
{
public:
    //  0x6F4D3E40 - the oldest record, or null on an empty ring.
    SAnimTrackRecord* Front();

    //  0x6F4D78E0 - make room for one more record and hand back where it
    //  goes.  `retn 0`.  Widget/animtrackringpush.cpp.
    SAnimTrackRecord* PushBack();

    //  0x6F4D3E90 - drop the oldest record.  Emptying the ring resets both
    //  indices to zero rather than only advancing the head, which is what
    //  keeps a drained ring starting from index 0 again.
    void PopFront();

    unsigned char   m_count;      // +0x00
    unsigned char   m_head;       // +0x01
    unsigned char   m_write;      // +0x02
    char            m_reserved03[1];
    void*           m_storage;    // +0x04
    unsigned int    m_modulus;    // +0x08
    SAnimTrackRecord* m_records;  // +0x0C
};

#endif

//============================================================================
//  0x6F4D5D80 - SAnimTrack::Assign: copy one animation track over another.
//
//  Two plain words and then the track's embedded key array, which is a deep
//  copy - SAnimKeyArray::Assign (0x6F4D50E0) resizes the destination and
//  fills it element by element rather than sharing the source's block.
//
//  The self-assignment guard is on the *key arrays* and not on the tracks:
//  the shipped code forms `this + 8` and `other + 8` and compares those,
//  which is what a guard written inside the key array's own copy helper
//  produces once it is inlined.  Written on the outer track instead it
//  compares the track addresses and skips the two word copies as well,
//  which the shipped code does not do.
//
//  The layout is Widget/animtrackarray.cpp's, one level of naming deeper:
//  what that file calls m_keyCapacity/m_keyCount/m_keys/m_inlineKeys is the
//  SAnimKeyArray at +0x08 that Widget/animkeyarray.cpp already describes.
//
//  Own translation unit: the key-array copy is a real call.
//============================================================================

//----------------------------------------------------------------------------
//  The key array as animkeyarray.cpp declares it, plus the one member this
//  file needs.  A local copy of the class mangles identically, because MSVC
//  puts only the class *name* in a member's mangled name.
//----------------------------------------------------------------------------
struct SAnimKeyArray
{
    //  0x6F4D50E0 - resize to `count` and copy `values` in.  `retn 8`.
    //  Widget/animkeyarrayassign.cpp.
    void Assign(unsigned int count, const int* values);

    //  Copy another array's live keys into this one, unless it *is* this
    //  one.  Inline: the shipped SAnimTrack::Assign carries the guard and
    //  the two loads with no call between them.
    __forceinline void CopyFrom(const SAnimKeyArray& other)
    {
        if (this != &other)
            Assign(other.m_count, other.m_keys);
    }

    unsigned int m_capacity;    // +0x00
    unsigned int m_count;       // +0x04
    int*         m_keys;        // +0x08
    int          m_inline[2];   // +0x0C
};

struct SAnimTrack
{
    SAnimTrack* Assign(const SAnimTrack* other);

    int           m_field0;     // +0x00
    int           m_field4;     // +0x04
    SAnimKeyArray m_keyArray;   // +0x08 .. +0x1C
};

SAnimTrack* SAnimTrack::Assign(const SAnimTrack* other)
{
    m_field0 = other->m_field0;
    m_field4 = other->m_field4;
    m_keyArray.CopyFrom(other->m_keyArray);
    return this;
}

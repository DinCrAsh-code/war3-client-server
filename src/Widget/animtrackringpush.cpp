//============================================================================
//  0x6F4D78E0 - CAnimTrackRing::PushBack: make room for one more animation
//  track record and hand back the slot it goes in.
//
//  Three things happen, and only the first is conditional on the ring being
//  full:
//
//  * a ring whose live count has reached its modulus grows its storage by
//    one.  If the live records wrap the end of the array - which is exactly
//    `m_head > m_write` - the tail half is shifted up one slot into the
//    space that just appeared and the head index follows it, so the records
//    stay contiguous in the new, larger array.  The shift runs from the top
//    down, which is what lets it copy in place.
//  * the write index advances modulo the ring size - but only when the ring
//    is not empty, so the very first push lands at index 0 rather than at 1.
//    The modulus is a real `div` and not a mask: nothing requires the ring's
//    size to be a power of two.
//  * the live count goes up by one.
//
//  A ring already at 255 live records does none of that and simply hands
//  back the current write slot, which the next push will overwrite.  255 and
//  not 256: the count is one byte and 0xFF is its ceiling, so the ring
//  saturates rather than wrapping to zero.
//
//  The record copy inside the shift loop is the same body this repo also
//  reconstructs out of line as SAnimTrack::Assign (0x6F4D5D80,
//  Widget/animtrackassign.cpp) - two plain words and a deep copy of the
//  embedded key array, guarded on the key arrays' own addresses.  The
//  shipped build has it both ways: a real function there, inlined here.  It
//  is written out again below rather than shared, because sharing it would
//  have to be `__forceinline` in a header and that would take the out-of-line
//  body away.
//
//  The record base is hoisted into a local, which the shipped code does not
//  do - it reloads [esi+0Ch] inside the loop and again at the tail.  Both
//  spellings were measured: reloading gives the shipped instruction shapes
//  but makes MSVC put `this` in edi and the byte offset in esi, i.e. the
//  swap of the shipped allocation, and the canonicaliser compares registers,
//  so it scores 20 of 65 against this version's 38.  The hoist costs one
//  callee-saved register and its push/pop pair and nothing else.
//============================================================================
#include "animtrackring.h"

//----------------------------------------------------------------------------
//  A record, at the one level of detail the shift loop needs.
//----------------------------------------------------------------------------
struct SAnimKeyArray
{
    //  0x6F4D50E0 - `retn 8`.  Widget/animkeyarrayassign.cpp.
    void Assign(unsigned int count, const int* values);

    unsigned int m_capacity;    // +0x00
    unsigned int m_count;       // +0x04
    int*         m_keys;        // +0x08
    int          m_inline[2];   // +0x0C
};

struct SAnimTrackFields
{
    int           m_field0;     // +0x00
    int           m_field4;     // +0x04
    SAnimKeyArray m_keyArray;   // +0x08 .. +0x1C
};

//  The inlined copy.  Same guard as the out-of-line body: on the *key
//  arrays*, not on the records.
static __forceinline void CopyRecord(SAnimTrackFields& dst,
                                     const SAnimTrackFields& src)
{
    dst.m_field0 = src.m_field0;
    dst.m_field4 = src.m_field4;
    if (&dst.m_keyArray != &src.m_keyArray)
        dst.m_keyArray.Assign(src.m_keyArray.m_count, src.m_keyArray.m_keys);
}

SAnimTrackRecord* CAnimTrackRing::PushBack()
{
    SAnimTrackFields* records = (SAnimTrackFields*)m_records;

    if (m_count != 0xFF)
    {
        if ((unsigned int)m_count == m_modulus)
        {
            ((SAnimRecordArray*)&m_storage)->SetAlloc(m_count + 1);

            if (m_head > m_write)
            {
                unsigned int i = m_count;
                while (i > (unsigned int)m_head)
                {
                    CopyRecord(records[i], records[i - 1]);
                    i--;
                }

                m_head = (unsigned char)(m_head + 1);
            }
        }

        unsigned char live = m_count;
        if (live != 0)
            m_write = (unsigned char)((m_write + 1) % m_modulus);

        m_count = (unsigned char)(live + 1);
    }

    return (SAnimTrackRecord*)&records[m_write];
}

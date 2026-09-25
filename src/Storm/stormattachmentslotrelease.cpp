//============================================================================
//  0x6F741760 - drop whatever object a visual-attachment record parks at
//  +0x4C, and clear the record's "live" bit.
//
//  Was a thunk in stormattachmentgates.cpp, declared there so
//  AttachmentRelease (0x6F00D970) could reach the real code.  Nineteen
//  instructions, and the only call in it is TRefCnt::Release (0x6F4C40A0),
//  which this repo has reconstructed EXACT since the first target.
//
//  The record base at +0x2CC is re-read across the Release call - the call
//  can reallocate the array, so the compiler cannot cache the pointer past
//  it (the same reasoning stormattachments.cpp's SetAttachmentWalkableZ
//  note spells out) - but the two accesses *after* the call share one load,
//  which is a local rather than a repeated member access.  The index
//  scaling is hoisted once either way, because the index does not change.
//
//  The returned flags-word address is what the whole 0x6F741xxx family
//  hands back and no caller in this codebase reads.
//
//  Own translation unit: AttachmentRelease reaches it with a real call, and
//  it reaches Release with one.
//============================================================================
#include "refcnt.h"

//  One record of the singleton's own 0xA0-byte array at +0x2CC - the same
//  array stormsingletona.h's SAttachmentRecord and misc_leaves_batch3.cpp's
//  SIndexedArrayFlags describe, with the two fields this function touches
//  named.
struct SIndexedArrayRecord
{
    char          m_reserved00[4];
    unsigned int  m_flags;          // +0x04 - bit 0 is "live"
    char          m_reserved08[0x4C - 0x08];
    TRefCnt*      m_pObject;        // +0x4C
    char          m_reserved50[0xA0 - 0x50];
};

struct SIndexedArrayRecords
{
    void* __thiscall ReleaseSlotObject(int index);

    char                 m_reserved00[0x2CC];
    SIndexedArrayRecord* m_records;      // +0x2CC
};

void* __thiscall SIndexedArrayRecords::ReleaseSlotObject(int index)
{
    TRefCnt* held = m_records[index].m_pObject;
    if (held != 0)
    {
        held->Release();
        m_records[index].m_pObject = 0;
    }

    //  The last two accesses share one load, and the shipped code says so:
    //  `mov edx,[edi+2CCh]` once, then `and [esi+edx+4],0FFFFFFFEh` and
    //  `lea eax,[esi+edx+4]` off the same base.  Repeating the member
    //  access here instead adds a third reload and a second `lea` - the
    //  same pair stormattachments.cpp's two siblings document.
    //
    //  A pointer to the flags word, not a pointer to the record array: both
    //  spellings emit the same three instructions, but the record-array
    //  local puts the base in edi where the shipped code uses edx.
    unsigned int* flags = &m_records[index].m_flags;
    *flags &= ~1u;
    return flags;
}

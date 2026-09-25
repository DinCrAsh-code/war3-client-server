//============================================================================
//  0x6F4D50E0 - SAnimKeyArray::Assign (Widget/animtrackarray.cpp's key
//  array, the one SAnimTrack::Assign/CopyFrom already call).  Resize to
//  `count` exactly and overwrite every slot from `values`.
//
//  0x6F4D38F0 - the resize helper Assign calls first.  Unlike SetAlloc
//  (animkeyarray.cpp), which preserves the surviving elements across a
//  resize, this one never copies the old block's contents forward - the
//  caller is about to overwrite every slot - so the grow path is a single
//  SMemReAlloc with Storm's plain flags (never "resize in place or fail",
//  since Storm is free to move the block) and the shrink-to-inline path
//  frees any heap block outright rather than trying to reuse it.  The
//  shipped body reuses the `m_keys` field itself as the "old pointer" it
//  hands Storm, zeroing it first when the array is still on its inline
//  storage - there is no separate local for it.
//
//  Own translation unit: two real calls (SMemReAlloc, SMemFree); the
//  per-slot store loop is not a call.
//============================================================================
#include "storm.h"

static const char s_animKeyTag[] =
    ".?AV?$TSFixedArray@UANIMKEY@@$0?CCCCCCCD@@@";

struct SAnimKeyArray
{
    void ResizeDiscardingContents(unsigned int count);
    void Assign(unsigned int count, const int* values);

    unsigned int m_capacity;    // +0x00
    unsigned int m_count;       // +0x04
    int*         m_keys;        // +0x08
    int          m_inline[2];   // +0x0C
};

void SAnimKeyArray::ResizeDiscardingContents(unsigned int count)
{
    if (count > 2)
    {
        if (m_capacity <= 2)
            m_keys = 0;

        m_keys = (int*)SMemReAlloc(m_keys, count * 4, s_animKeyTag, -2, 0);
        m_capacity = count;
        return;
    }

    if (m_capacity > 2)
        SMemFree(m_keys, s_animKeyTag, -2, 0);

    m_keys = m_inline;
    m_capacity = count;
}

void SAnimKeyArray::Assign(unsigned int count, const int* values)
{
    ResizeDiscardingContents(count);

    for (unsigned int i = 0; i < count; i++)
    {
        int* slot = &m_keys[i];
        if (slot != 0)
            *slot = values[i];
    }

    m_count = count;
}

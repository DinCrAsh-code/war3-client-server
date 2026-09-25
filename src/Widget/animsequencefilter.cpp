//============================================================================
//  0x6F4EBA40 - SAnimSequenceTable::KeepExtremeRankedSequences.
//
//  SpriteStartAnimation (0x6F4D7E50) has gathered every sequence whose name
//  tokens match into a candidate list; this narrows that list to the ones
//  whose rank is extreme.  Which extreme is the caller's flag - bit 4 of the
//  animation flags word, passed on as 0x10 or 0 - and the starting values
//  say the same thing: 0.0f when the caller wants the highest, 1.0f when it
//  wants the lowest, so the rank is a normalised weight in [0, 1].  Nothing
//  in this call tree says what the weight *means*, which is why the field is
//  named for how it is used and not for a meaning.
//
//  Two passes over the list, both the shipped shape:
//
//    * the first walks *backwards* from the end - the shipped stream steps a
//      pointer down from `m_keys + m_count` and counts a separate index down
//      to zero - and keeps the extreme rank in a stack slot, re-testing the
//      caller's flag inside the loop rather than hoisting two loops out of
//      it;
//    * the second walks forwards and removes every candidate whose rank is
//      *not* the extreme, by overwriting it with the array's last element
//      and shortening the array by one.  The index only advances on the
//      keep path, which is why the loop has no increment of its own.
//
//  The shrink is SAnimKeyArray's own SetCount inlined - the shipped code has
//  no call there - and it is written out as a __forceinline member below
//  rather than reached through a real one, because there is no out-of-line
//  SetCount for this array anywhere in the image.  Its shape is not Storm's
//  TSGrowableArray::SetCount: the chunk rounding only happens above two
//  elements (a two-element array lives in the header's own inline storage,
//  which is what animkeyarray.cpp records), and there is no element
//  construction or destruction at all because a key is a plain dword.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "animsequence.h"

//  0x6F4D38A0 - see animsequence.h.  Rounds `count` DOWN to the nearest
//  power of two by repeatedly clearing the lowest set bit
//  (`n & (n-1)` isolates "more than one bit set"; looping it strips bits
//  until only the highest one is left), then clamps the result into
//  [1, 0x40] - callers >= 0x40 short-circuit before the loop even runs.
//  `ecx` is never read (the shipped body opens `mov eax,[esp+4]`); it is
//  still a member because the call sites leave `this` in ecx regardless,
//  and a free `__stdcall` declaration would not match that.
unsigned int SAnimKeyArray::ComputeChunk(unsigned int count)
{
    if (count < 0x40)
    {
        while ((count & (count - 1)) != 0)
            count &= count - 1;

        if (count < 1)
            return 1;

        return count;
    }

    return 0x40;
}

void SAnimSequenceTable::KeepExtremeRankedSequences(int preferHighest,
                                                    SAnimKeyArray* candidates)
{
    unsigned int remaining = candidates->m_count;
    if (remaining == 0)
        return;

    float best = preferHighest ? 0.0f : 1.0f;

    //  Backwards, off the end: the shipped stream forms `m_keys + m_count`
    //  once and steps down through it.
    unsigned int* key = candidates->m_keys + remaining;
    do
    {
        key--;
        float rank = m_sequences.m_pRecords[*key].m_rank;
        remaining--;

        if (preferHighest)
        {
            if (best <= rank)
                best = rank;
        }
        else
        {
            if (best >= rank)
                best = rank;
        }
    }
    while (remaining != 0);

    for (unsigned int i = 0; i < candidates->m_count; )
    {
        unsigned int* slot = &candidates->m_keys[i];

        if (m_sequences.m_pRecords[*slot].m_rank == best)
        {
            i++;
            continue;
        }

        *slot = *(unsigned int*)GetLastArrayElement_6F4D3740(candidates);
        candidates->SetCount(candidates->m_count - 1);
    }
}

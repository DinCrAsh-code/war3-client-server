//============================================================================
//  0x6F4C3980 - CStatus::CopyFilteredText: BuildText's second pass, filling
//  the buffer MeasureFilteredTextLength (0x6F4C39D0) sized.
//
//  Same tail-to-head walk and the same +0x0C filter as
//  MeasureFilteredTextLength (statusmeasuretext.cpp), which see for why
//  `m_list.TailLink()` is what the shipped `[ecx+0Ch]` read is.  The first
//  entry whose text would not fit in what is left of `capacity` stops the
//  whole walk (`jnb` there targets the epilogue, past the loop's own
//  continue point) rather than being skipped in favour of any shorter
//  entry still further up the list - a `continue` was tried first and
//  measured wrong (matches every instruction but the jump target).
//  Nothing in the shipped body inserts a separator between two entries'
//  text, which is why the parameter BuildText forwards here is `minType`,
//  not a character (see CStatus::BuildText's own comment in cstatus.h).
//============================================================================
#include "cstatus.h"

void __thiscall CStatus::CopyFilteredText(char* dest, int capacity, int minType)
{
    *dest = 0;

    for (int cur = m_list.TailLink(); cur > 0; )
    {
        STATUSENTRY* entry = (STATUSENTRY*)cur;

        if (entry->m_type >= minType)
        {
            unsigned int len = Storm_506(entry->m_text);

            if (len >= (unsigned int)capacity)
                break;

            Storm_501(dest, entry->m_text, capacity);
            capacity -= len;
            dest += len;
        }

        cur = entry->m_link.m_prevlink;
    }
}

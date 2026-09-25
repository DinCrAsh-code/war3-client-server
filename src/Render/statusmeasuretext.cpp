//============================================================================
//  0x6F4C39D0 - CStatus::MeasureFilteredTextLength: BuildText's own
//  size pass, so the buffer it allocates is exactly big enough.
//
//  The shipped body reads `this+0x0C` directly rather than through any
//  accessor - that is `m_list`'s own `TailLink()` (m_list sits at +0x04,
//  and TSList's `m_linkoffset`/`m_terminator` fields put the terminator's
//  `m_prevlink` at +0x0C of `this`), the same field CStatus::IsEmpty
//  already reads for the identical reason (statusisempty.cpp).  Calling
//  `m_list.TailLink()` reproduces that read exactly through inlining.
//============================================================================
#include "cstatus.h"

int __thiscall CStatus::MeasureFilteredTextLength(int minType)
{
    int total = 0;

    for (int cur = m_list.TailLink(); cur > 0; )
    {
        STATUSENTRY* entry = (STATUSENTRY*)cur;

        if (entry->m_type >= minType)
            total += Storm_506(entry->m_text);

        cur = entry->m_link.m_prevlink;
    }

    return total;
}

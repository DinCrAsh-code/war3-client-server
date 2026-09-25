//============================================================================
//  0x6F4D3E40 / 0x6F4D3E90 - see animtrackring.h.
//
//  Neither calls the other and neither calls anything else, so one
//  translation unit for the pair costs nothing.
//============================================================================
#include "animtrackring.h"

//----------------------------------------------------------------------------
//  0x6F4D3E40.  The empty answer is the out-of-line tail, reached by a
//  forward branch, so the count test is written as the guard around the
//  return rather than as an early `return 0`.
//----------------------------------------------------------------------------
SAnimTrackRecord* CAnimTrackRing::Front()
{
    if (m_count != 0)
        return &m_records[m_head];
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F4D3E90.  `--m_count` on a byte field is what puts the shipped
//  `add byte ptr [ecx], 0FFh` and its flags in front of the branch: the
//  decrement's own zero flag is the test, with no separate compare.
//----------------------------------------------------------------------------
void CAnimTrackRing::PopFront()
{
    if (--m_count == 0)
    {
        m_write = 0;
        m_head = 0;
    }
    else
    {
        m_head = (unsigned char)((m_head + 1) % m_modulus);
    }
}

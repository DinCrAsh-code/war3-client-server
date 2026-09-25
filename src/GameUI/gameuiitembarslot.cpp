//============================================================================
//  0x6F338830 - CGameUIItemBar::GetSlot0x12C.  Eight-case jump table
//  (0-7), `this` in ecx, one stack arg (`retn 4`).  Index 7 and anything
//  past it (`ja` on `index > 7`) both land on the identical `xor eax,eax`
//  body - one jump-table entry, not a separate default block - which is
//  why this is written as a `default:` case covering both rather than an
//  explicit `case 7:`.
//============================================================================
#include "gameui.h"

void* CGameUIItemBar::GetSlot0x12C(unsigned int index)
{
    switch (index)
    {
    case 0: return m_slot0x12C[0];
    case 1: return m_slot0x12C[1];
    case 2: return m_slot0x12C[2];
    case 3: return m_slot0x12C[3];
    case 4: return m_slot0x12C[4];
    case 5: return m_slot0x12C[5];
    case 6: return m_slot0x12C[6];
    case 7:
    default: return 0;
    }
}

//============================================================================
//  0x6F4C2310 - CDataStoreScratch::WriteDwordAlt2.  See cdatastorescratch.h.
//
//  A second four-byte writer at its own address, with the same body as
//  WriteDword (0x6F4C2360, cdatastorescratch.cpp): the same
//  "does it already fit" guard, the same Grow call with the same seven
//  arguments, the same store and the same `this` return.  Own translation
//  unit so that CItem::Save's (item_save.cpp) call to it survives as a
//  call - see cdatastorescratch.h for why there are four of these.
//============================================================================
#include "cdatastorescratch.h"

CDataStoreScratch* CDataStoreScratch::WriteDwordAlt2(unsigned int value)
{
    unsigned int pos = (unsigned int)m_field10;
    unsigned int end = (unsigned int)m_field8;
    if (!(pos >= end && pos + 4 <= (unsigned int)m_field0xC + end))
        Grow(pos, 4, &m_field4, &m_field8, &m_field0xC, 0, 0);

    *(unsigned int*)((char*)m_field4 - m_field8 + m_field10) = value;
    m_field10 += 4;
    return this;
}

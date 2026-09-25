//============================================================================
//  0x6F4C21B0 - CDataStoreScratch::WriteWordAlt.  See cdatastorescratch.h.
//
//  A second two-byte writer at its own address, with the same body as
//  WriteWord (0x6F4C2210, cdatastorescratch.cpp): the same "does it already
//  fit" guard, the same Grow call with the same seven arguments (offset,
//  size=2, &m_field4, &m_field8, &m_field0xC, 0, 0), the same store and the
//  same `this` return.  Reached from CWidget::Save; own translation unit so
//  that caller's own call to it survives (same reasoning as WriteDwordAlt).
//============================================================================
#include "cdatastorescratch.h"

CDataStoreScratch* CDataStoreScratch::WriteWordAlt(unsigned short value)
{
    unsigned int pos = (unsigned int)m_field10;
    unsigned int end = (unsigned int)m_field8;
    if (!(pos >= end && pos + 2 <= (unsigned int)m_field0xC + end))
        Grow(pos, 2, &m_field4, &m_field8, &m_field0xC, 0, 0);

    *(unsigned short*)((char*)m_field4 - m_field8 + m_field10) = value;
    m_field10 += 2;
    return this;
}

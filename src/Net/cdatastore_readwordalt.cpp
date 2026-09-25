//============================================================================
//  0x6F4C2C40 - CDataStore::ReadWordAlt.  See cdatastore.h.
//
//  The same body as ReadWord (0x6F4C2C70, cdatastore.cpp) - the same
//  EnsureReadable guard, the same `m_field4 - m_field8 + m_readPos` load and
//  the same advance - at a second address.  Reached from CWidget::Load; own
//  translation unit so that caller's own call to it survives (same
//  reasoning as ReadDwordAlt).
//============================================================================
#include "cdatastore.h"

CDataStore* CDataStore::ReadWordAlt(unsigned short* out)
{
    int pos = m_readPos;
    if (EnsureReadable(pos, 2) != 0)
    {
        *out = *(unsigned short*)((char*)m_field4 - m_field8 + m_readPos);
        m_readPos += 2;
    }
    return this;
}

//============================================================================
//  0x6F4C2CD0 - CDataStore::ReadDwordAlt.  See cdatastore.h.
//
//  The same body as ReadDword (0x6F4C2D30, cdatastore.cpp) - the same
//  EnsureReadable guard, the same `m_field4 - m_field8 + m_readPos` load and
//  the same advance - at a second address.  Own translation unit so that
//  ReadCFloat's (0x6F6EED50) call to it survives.
//============================================================================
#include "cdatastore.h"

CDataStore* CDataStore::ReadDwordAlt(unsigned int* out)
{
    int pos = m_readPos;
    if (EnsureReadable(pos, 4) != 0)
    {
        *out = *(unsigned int*)((char*)m_field4 - m_field8 + m_readPos);
        m_readPos += 4;
    }
    return this;
}

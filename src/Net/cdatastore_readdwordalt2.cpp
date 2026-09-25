//============================================================================
//  0x6F4C2CA0 - CDataStore::ReadDwordAlt2, and 0x6F4C2D00 -
//  CDataStore::ReadDwordAlt3.  See cdatastore.h.
//
//  The third and fourth four-byte readers, both byte for byte the same body
//  as ReadDword (0x6F4C2D30).  They share a file because neither calls the
//  other, and because they are adjacent in the shipped binary - one module,
//  one translation unit.
//============================================================================
#include "cdatastore.h"

CDataStore* CDataStore::ReadDwordAlt2(unsigned int* out)
{
    if (EnsureReadable(m_readPos, 4) != 0)
    {
        *out = *(unsigned int*)((char*)m_field4 - m_field8 + m_readPos);
        m_readPos += 4;
    }
    return this;
}

//  Byte for byte the same body as its sibling above; the two are separate
//  functions in the shipped binary, at separate addresses, and neither
//  calls the other, so they are written out twice rather than folded.
CDataStore* CDataStore::ReadDwordAlt3(unsigned int* out)
{
    if (EnsureReadable(m_readPos, 4) != 0)
    {
        *out = *(unsigned int*)((char*)m_field4 - m_field8 + m_readPos);
        m_readPos += 4;
    }
    return this;
}

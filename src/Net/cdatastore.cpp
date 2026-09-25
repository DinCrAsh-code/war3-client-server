//============================================================================
//  CDataStore - see cdatastore.h for the class-level notes.
//============================================================================
#include "cdatastore.h"

//  A cross-module function pointer (IDA's `off_6F932C0C`), not a virtual
//  call - the same one the not-reconstructed write-side family
//  (sub_6F658030 and friends) uses to check a recycled buffer back in.
extern "C" void(__stdcall* g_pfnRecycleBuffer)(void** pField4, int* pField8, int* pField0xC);

//----------------------------------------------------------------------------
//  0x6F2C8A10
//----------------------------------------------------------------------------
CDataStore::~CDataStore()
{
    bool owned = (m_field0xC != -1);
    if (owned)
        g_pfnRecycleBuffer(&m_field4, &m_field8, &m_field0xC);
}

//----------------------------------------------------------------------------
//  0x6F4C2B30
//----------------------------------------------------------------------------
int CDataStore::EnsureReadable(unsigned int offset, unsigned int size)
{
    if (offset + size > (unsigned int)m_field10)
    {
        m_readPos = m_field10 + 1;
        return 0;
    }

    unsigned int capacity = (unsigned int)m_field8;
    if (offset >= capacity && offset + size <= (unsigned int)m_field0xC + capacity)
        return 1;

    if (!Grow(offset, size, &m_field4, &m_field8, &m_field0xC))
    {
        m_readPos = m_field10 + 1;
        return 0;
    }
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4C2C10
//----------------------------------------------------------------------------
CDataStore* CDataStore::ReadByte(unsigned char* out)
{
    int pos = m_readPos;
    if (EnsureReadable(pos, 1) != 0)
    {
        *out = *((unsigned char*)m_field4 - m_field8 + m_readPos);
        m_readPos = m_readPos + 1;
    }
    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C2C70
//----------------------------------------------------------------------------
CDataStore* CDataStore::ReadWord(unsigned short* out)
{
    int pos = m_readPos;
    if (EnsureReadable(pos, 2) != 0)
    {
        *out = *(unsigned short*)((char*)m_field4 - m_field8 + m_readPos);
        m_readPos += 2;
    }
    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C2D30
//----------------------------------------------------------------------------
CDataStore* CDataStore::ReadDword(unsigned int* out)
{
    int pos = m_readPos;
    if (EnsureReadable(pos, 4) != 0)
    {
        *out = *(unsigned int*)((char*)m_field4 - m_field8 + m_readPos);
        m_readPos += 4;
    }
    return this;
}

//----------------------------------------------------------------------------
//  0x6F4C2DE0 - ReadDword's float twin.  Same guard, same advance; the value
//  travels through the x87 stack because it is a float.
//----------------------------------------------------------------------------
CDataStore* CDataStore::ReadFloat(float* out)
{
    int pos = m_readPos;
    if (EnsureReadable(pos, 4) != 0)
    {
        *out = *(float*)((char*)m_field4 - m_field8 + m_readPos);
        m_readPos += 4;
    }
    return this;
}

//============================================================================
//  0x6F4C2FD0 - CDataStore::ReadRaw.  One contiguous run at a time, with
//  EnsureReadable asked for at least one byte each time round and memcpy
//  skipped outright when the destination already *is* the source.
//============================================================================
#include "cdatastore.h"

void __stdcall SErrSetLastError(unsigned int error);
extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int n);

CDataStore* CDataStore::ReadRaw(void* dest, unsigned int size)
{
    int ok = (dest == 0 && size != 0) ? 0 : -1;
    if (ok == 0)
    {
        SErrSetLastError(87);
        return this;
    }

    if ((unsigned int)m_readPos <= (unsigned int)m_field10 && size != 0)
    {
        char* out = (char*)dest;
        unsigned int left = size;

        do
        {
            unsigned int at = m_readPos;
            unsigned int run = (unsigned int)m_field10 - at;
            if (run >= left)
                run = left;
            if (run >= (unsigned int)m_field0xC)
                run = m_field0xC;
            if (run <= 1)
                run = 1;

            if (!EnsureReadable(at, run))
                break;

            const char* src = (const char*)m_field4 - m_field8 + m_readPos;
            if (out != src)
                memcpy(out, src, run);

            m_readPos += run;
            out += run;
            left -= run;
        }
        while (left != 0);
    }

    return this;
}

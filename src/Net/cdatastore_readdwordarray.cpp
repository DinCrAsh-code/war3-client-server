//============================================================================
//  0x6F4C32A0 - CDataStore::ReadDwordArray.  See cdatastore.h.
//
//  ReadRaw's own loop (cdatastore_readraw.cpp) with `count` a dword count
//  rather than a byte count - `count * 4` is the request ReadRaw would have
//  been given - and two numbers changed inside the loop: the minimum run is
//  4 bytes and every run is rounded down to a multiple of 4, where ReadRaw's
//  minimum is 1 and it never rounds.  The same `dest == 0 && count != 0`
//  guard as ReadRaw carries over unchanged.
//============================================================================
#include "cdatastore.h"

void __stdcall SErrSetLastError(unsigned int error);
extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int n);

CDataStore* CDataStore::ReadDwordArray(void* dest, unsigned int count)
{
    int ok = (dest == 0 && count != 0) ? 0 : -1;
    if (ok == 0)
    {
        SErrSetLastError(87);
        return this;
    }

    if ((unsigned int)m_readPos <= (unsigned int)m_field10 && count != 0)
    {
        char* out = (char*)dest;
        unsigned int left = count * 4;

        do
        {
            unsigned int at = m_readPos;
            unsigned int run = (unsigned int)m_field10 - at;
            if (run >= left)
                run = left;
            if (run >= (unsigned int)m_field0xC)
                run = m_field0xC;
            if (run <= 4)
                run = 4;
            run &= ~3u;

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

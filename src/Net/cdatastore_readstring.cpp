//============================================================================
//  0x6F4C2E10 - CDataStore::ReadString.
//
//  The argument check is written as the shipped `(dest != 0 || size == 0)
//  ? 0 : -1` mask rather than as an `if`, because that is the stream: two
//  tests, a 0/-1 materialisation and then a test of *that*.  Getting the
//  sense backwards compiles clean and scores about the same, which is
//  exactly the hazard docs/msvc-vc8-idioms.md records for a branchless
//  argument mask.
//============================================================================
#include "cdatastore.h"

void __stdcall SErrSetLastError(unsigned int error);

CDataStore* CDataStore::ReadString(char* dest, unsigned int size)
{
    int ok = (dest == 0 && size != 0) ? 0 : -1;
    if (ok == 0)
    {
        SErrSetLastError(87);
        return this;
    }

    if (size != 0 && m_readPos <= m_field10)
    {
        unsigned int written = 0;

        while (EnsureReadable(m_readPos, 1))
        {
            unsigned int end = m_field8 + (unsigned int)m_field0xC;
            if (end >= (unsigned int)m_field10)
                end = m_field10;

            unsigned int run = end - m_readPos;
            unsigned int room = size - written;
            if (run >= room)
                run = room;

            const char* src = (const char*)m_field4 + (m_readPos - m_field8);
            unsigned int taken = 0;

            if (run != 0)
            {
                char c;
                do
                {
                    c = src[taken];
                    dest[written] = c;
                    written++;
                    taken++;
                }
                while (c != 0 && --run != 0);
            }

            m_readPos += taken;

            if (run != 0)
                break;
            if (written >= size)
            {
                m_readPos = m_field10 + 1;
                break;
            }
        }
    }

    if ((unsigned int)m_readPos > (unsigned int)m_field10)
        dest[0] = 0;

    return this;
}

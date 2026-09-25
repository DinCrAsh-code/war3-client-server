//============================================================================
//  0x6F60C9A0 - CSimpleFontString::SetTextLength, ".\\CSimpleRender.cpp".
//
//  A length of zero drops the reservation without freeing the buffer, which
//  is the shipped behaviour and not an omission: the free only happens inside
//  the non-zero arm, after the replacement has been allocated.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleFontString::SetTextLength(int length)
{
    if (length == m_textLength)
        return;

    if (length)
    {
        char* buffer = (char*)SMemAlloc(length + 1, ".\\CSimpleRender.cpp",
                                        0x19E, 0);
        buffer[0] = 0;

        if (m_pText)
            SMemFree(m_pText, ".\\CSimpleRender.cpp", 0x1A1, 0);

        m_pText = buffer;
        m_textCapacity = length;
    }

    m_textLength = length;
}

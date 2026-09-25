//============================================================================
//  0x6F60CA10 - CSimpleFontString::SetText, ".\\CSimpleRender.cpp".
//
//  Three cases, and they are the shipped code's and not a simplification:
//  a string with a *reserved* length is copied into the existing buffer and
//  truncated to it; one without is measured, and either replaces the buffer
//  (longer than the capacity) or is copied into it with the capacity plus one
//  as the bound.  Either way the rendered form is dropped and the frame is
//  queued for layout.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#include "refcnt.h"

void CSimpleFontString::SetText(const char* text)
{
    if (m_pText)
        m_pText[0] = 0;

    m_A4 = 0.0f;
    m_A8 = 0.0f;

    if (text && text[0])
    {
        if (m_textLength)
        {
            Storm_501(m_pText, text, m_textLength);
        }
        else
        {
            int length = (int)Storm_506(text);
            if (length > (int)m_textCapacity)
            {
                if (m_pText)
                    SMemFree(m_pText, ".\\CSimpleRender.cpp", 0x1B8, 0);
                m_pText = Storm_507(text, ".\\CSimpleRender.cpp", 0x1B9);
                m_textCapacity = length;
            }
            else
            {
                Storm_501(m_pText, text, m_textCapacity + 1);
            }
        }
    }

    if (m_pRendered)
    {
        ((TRefCnt*)m_pRendered)->Release();
        m_pRendered = 0;
    }

    QueueForLayout(0);
}

//============================================================================
//  0x6F603B10 - CSimpleMessageFrame::SetFont: duplicate the font name, keep
//  the height and the flags, and push all three down every live line.
//
//  The line array's stride is twelve bytes and the CSimpleFontString each
//  line owns sits at its +0x08 *minus eight* - the shipped code adds the
//  eight back at the call, which is what the `add ecx,8` is.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleMessageFrame::SetFont(const char* font, float height, int flags)
{
    if (m_pFont)
        SMemFree((void*)m_pFont, ".\\CSimpleMessageFrame.cpp", 0x36, 0);

    m_pFont = Storm_507(font, ".\\CSimpleMessageFrame.cpp", 0x37);
    m_fontHeight = height;
    m_fontFlags = flags;

    for (unsigned int i = 0; i < m_lineCount; ++i)
    {
        CSimpleFontString* line = (CSimpleFontString*)
            (*(char**)(m_pLines + i * 12 + 8) + 8);
        line->SetFont(font, height, flags);
    }
}

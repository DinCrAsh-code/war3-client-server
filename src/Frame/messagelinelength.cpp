//============================================================================
//  0x6F603BB0 - CSimpleMessageFrame::SetLineLength: how much text each line
//  reserves room for.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleMessageFrame::SetLineLength(int length)
{
    m_lineLength = length;

    for (unsigned int i = 0; i < m_lineCount; ++i)
    {
        CSimpleFontString* line = (CSimpleFontString*)
            (*(char**)(m_pLines + i * 12 + 8) + 8);
        line->SetTextLength(length);
    }
}

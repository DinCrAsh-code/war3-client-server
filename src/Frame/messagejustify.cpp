//============================================================================
//  0x6F603C00 - CSimpleMessageFrame::SetJustify: the same sweep as
//  SetLineLength over the other per-line setting.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleMessageFrame::SetJustify(int justify)
{
    m_justify = justify;

    for (unsigned int i = 0; i < m_lineCount; ++i)
    {
        CSimpleFontString* line = (CSimpleFontString*)
            (*(char**)(m_pLines + i * 12 + 8) + 8);
        line->SetJustify(justify);
    }
}

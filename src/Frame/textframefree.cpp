//============================================================================
//  0x6F611A80 - CTextFrame::FreeText.
//
//  Two arms, and which one runs is the style bit HasStyleBits answers for:
//  when the caption is masked, +0x1EC holds a *separate* buffer of '*' that
//  has to be freed on its own; when it is not, +0x1EC is a second pointer to
//  the same string +0x1E8 holds and freeing it would be a double free.  The
//  Storm line numbers - 344, 347 and 356 - are the shipped ones and are what
//  says the two arms are three distinct free sites in the original source.
//
//  `freeText` zero keeps the raw string: SetString passes 0 when it is about
//  to copy over the buffer in place, and 1 when it is about to replace it.
//
//  Its own translation unit: SetString calls it three times for real.
//============================================================================
#include "frame.h"
#include "framethunks.h"

static const char kTextFrameFile[] = ".\\CTextFrame.cpp";

void CTextFrame::FreeText(int freeText)
{
    if (HasStyleBits(0x100))
    {
        if (freeText)
        {
            if (m_pText != 0)
                SMemFree(m_pText, kTextFrameFile, 344, 0);
            m_pText = 0;
        }

        if (m_pDisplayText != 0)
            SMemFree(m_pDisplayText, kTextFrameFile, 347, 0);
        m_pDisplayText = 0;
    }
    else
    {
        if (freeText)
        {
            if (m_pText != 0)
                SMemFree(m_pText, kTextFrameFile, 356, 0);
            m_pText = 0;
        }

        //  Not freed: without the mask bit this is the same pointer m_pText
        //  holds, and the arm above has already released it.
        m_pDisplayText = 0;
    }
}

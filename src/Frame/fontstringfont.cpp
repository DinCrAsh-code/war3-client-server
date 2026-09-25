//============================================================================
//  0x6F60DA20 - CSimpleFontString::SetFont.
//
//  Answers whether the font was made: 1 when there was nothing to make (a
//  null name or a zero height) and 1 when the make succeeded, 0 only when it
//  was attempted and failed.
//
//  The height handed to the font factory is the *frame's* scale times the
//  requested point size (CLayoutFrame::m_scale at +0x60), which is what makes
//  a font string scale with its frame.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#include "refcnt.h"

int CSimpleFontString::SetFont(const char* font, float height, int flags)
{
    m_fontHeight = height;

    int made = 1;

    if (m_pRendered)
    {
        ((TRefCnt*)m_pRendered)->Release();
        m_pRendered = 0;
    }
    if (m_pFont)
    {
        ((TRefCnt*)m_pFont)->Release();
        m_pFont = 0;
    }

    m_A4 = 0.0f;
    m_A8 = 0.0f;
    MarkLayerDirty();

    //  Both spellings here are load-bearing and were measured.  `height`
    //  tested directly (rather than `0.0f != height`) is what puts the zero
    //  in st(0) first - `fldz` / `fcomp [height]`; writing the comparison out
    //  makes MSVC commute the constant to the right and emit
    //  `fld [height]` / `fcomp __real@0` instead, which costs two
    //  instructions.  Likewise the flag has to be an if, not `flags ? 1 : 0`:
    //  the ternary becomes `setne dl` where the shipped code branches and
    //  reuses the 1 it already has in edi (worth eight instructions).
    if (font && height)
    {
        int normalised = 0;
        if (flags)
            normalised = 1;
        m_pFont = CreateFont(font, m_scale * m_fontHeight, normalised);
        if (!m_pFont)
            made = 0;
    }

    if (m_pFont && m_pText && m_pText[0])
        Rebuild(0);

    return made;
}

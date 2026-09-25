//============================================================================
//  0x6F603C50 - CSimpleMessageFrame::SetShadow: colour and offset in one
//  call, kept on the frame and pushed down every line.
//
//  The two are handed on as *pointers into the frame's own fields*, not as
//  values - the shipped code takes the address of +0x140 and of +0x144, which
//  is why the colour has to be stored before the sweep and not copied into a
//  local.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleMessageFrame::SetShadow(int color, const float* offset)
{
    //  The offset pair is copied as two dwords, not as two floats: the
    //  shipped code moves them through a general register
    //  (`mov ecx,[eax]` / `mov [ebp],ecx`), which is what a POD copy gives.
    //  Spelling it `m_shadowOffset[i] = offset[i]` makes MSVC route both
    //  through the x87 stack (`fld` / `fstp`) instead - same bits, four more
    //  instructions and a different register.
    //  `kept` is the address of m_shadowOffset held once and used for both
    //  the copy and the per-line argument, which is what the shipped code
    //  does (`lea ebp,[esi+324h]`, then stores and the push through ebp).
    //  Without it MSVC hoists &m_shadowColor into the register instead and
    //  re-forms the offset address at every use.
    int* kept = (int*)m_shadowOffset;

    m_shadowColor = color;
    kept[0] = ((const int*)offset)[0];
    kept[1] = ((const int*)offset)[1];

    for (unsigned int i = 0; i < m_lineCount; ++i)
    {
        CSimpleFontString* line = (CSimpleFontString*)
            (*(char**)(m_pLines + i * 12 + 8) + 8);
        line->SetShadow((const unsigned int*)&m_shadowColor,
                        (const unsigned int*)kept);
    }
}

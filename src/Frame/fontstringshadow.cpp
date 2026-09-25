//============================================================================
//  0x6F60DC70 - CSimpleFontString::SetShadow: colour and offset together,
//  with style bit 8 raised to say the string has one.
//
//  The relayout is conditional on there being a *rendered* form to redo
//  (+0xA0), not on the string having changed.
//============================================================================
#include "frame.h"
#include "framethunks.h"


void CSimpleFontString::SetShadow(const unsigned int* color,
                                  const unsigned int* offset)
{
    m_style |= 0x100;
    m_shadowColor = color[0];
    *(unsigned int*)&m_shadowX = offset[0];
    *(unsigned int*)&m_shadowY = offset[1];

    if (m_pRendered)
        Rebuild(0);
}

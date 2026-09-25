//============================================================================
//  0x6F60D580 - CSimpleFontString::CSimpleFontString.
//
//  Clean: its base constructor (CSimpleRegion, 0x6F60E8C0) is `retn 0Ch` and
//  nothing here needs unwinding, so unlike most of this library it carries no
//  EH frame and can match instruction for instruction.
//
//  0xFF000000 is the shadow colour it starts with and 0x212 the style word.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CSimpleFontString::CSimpleFontString(void* parent, int layer, int style)
    : CSimpleRegion((CSimpleFrame*)parent, layer, style)
{
    m_shadowColor = 0xFF000000;
    m_shadowX = 0.0f;
    m_shadowY = 0.0f;
    m_B8 = 0.0f;
    m_BC = 0.0f;
    m_84 = 0;
    m_A4 = 0.0f;
    m_88 = 0;
    m_pFont = 0;
    m_A8 = 0.0f;
    m_textLength = 0;
    m_textCapacity = 0;
    m_pText = 0;
    m_pRendered = 0;
    m_style = 0x212;
}

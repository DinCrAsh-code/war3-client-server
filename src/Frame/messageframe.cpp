//============================================================================
//  0x6F604820 - CSimpleMessageFrame::CSimpleMessageFrame,
//  ".\\CSimpleMessageFrame.cpp".  No EH frame of its own: the base
//  constructor is the only call and it is `retn 4`, so this one is a clean
//  forty-one instructions.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CSimpleMessageFrame::CSimpleMessageFrame(void* parent)
    : CSimpleFrame(parent)
{
    m_shadowColor = 0;
    m_shadowOffset[0] = 0.0f;
    m_shadowOffset[1] = 0.0f;
    m_150 = 0.0f;
    m_154 = 0.0f;
    m_158 = 0.0f;
    m_15C = 0.0f;
    m_160 = 0.0f;
    m_164 = 0.0f;
    m_168 = 0.0f;
    m_16C = 0.0f;
    m_174 = 0;
    m_178 = 0;
    m_17C = 0;
    m_180 = 0;
    m_184 = 0;
    m_188 = 0;
    m_pLines = 0;
    m_190 = 0;
    m_fontHeight = 0.0f;
    //  flt_6F96DD14 - three.
    m_14C = 3.0f;
    m_lineCount = 0;
    m_128 = 0;
    m_pFont = 0;
    m_fontFlags = 0;
    m_lineLength = 0;
    m_justify = 1;
    m_170 = 1;
}

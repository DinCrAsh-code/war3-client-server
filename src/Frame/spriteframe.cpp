//============================================================================
//  0x6F60F030 - CSpriteFrame::CSpriteFrame.
//
//  flt_6F9361D0 is one half - the animation blend the frame starts at - and
//  the 1.0f above it is the sprite scale.
//
//  Score capped by the unified EH frame; see framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CSpriteFrame::CSpriteFrame(int a, int b, int c)
    : CFrame(a, b, c)
{
    m_168 = 0.0f;
    m_16C = 0.0f;
    m_pSprite = 0;
    m_174 = 0;
    m_178 = 0;
    m_17C = 0;

    m_slot.Init(0);

    m_scale = 1.0f;
    m_1A0 = 0;
    m_1A4 = 0;
    m_1A8 = 0.5f;
    m_1AC = 1;
}

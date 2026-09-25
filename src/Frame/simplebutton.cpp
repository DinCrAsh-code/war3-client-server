//============================================================================
//  0x6F6030E0 - CSimpleButton::CSimpleButton, ".\\CSimpleButton.cpp".
//
//  Score capped by the unified EH frame; see framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CSimpleButton::CSimpleButton(void* parent)
    : CSimpleFrame(parent)
{
    m_150 = 0.0f;
    m_154 = 0.0f;

    m_124 = 0;
    m_12C = 0;
    //  flt_6F92E7A4 and flt_6F935D98: one thousandth and its negative.
    m_150 = 0.001f;
    m_enabled = 0;
    m_13C = 0;
    m_154 = -0.001f;
    m_140 = 0x10;
    m_148 = 0;
    m_144 = 0;
    m_14C = 0;
    m_164 = 0;
    m_slots[0] = 0;
    m_slots[1] = 0;
    m_slots[2] = 0;

    SetEnabled(1);
    RaiseFlagBit(2, -1);
}

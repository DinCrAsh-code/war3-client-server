//============================================================================
//  0x6F352F00 - CGameUIItemBar::SetEnabled.
//
//  The whole body is "forward to the button list at +0x148 and remember what
//  was asked for".  Same cached-flag guard as the other four controllers.
//
//  Own translation unit: 0x6F3470E0 is a real call and a different shipped
//  module.
//============================================================================
#include "gameui.h"

void CGameUIItemBar::SetEnabled(int enabled)
{
    if (m_enabled != enabled)
    {
        m_pButtons->SetEnabled(enabled);
        m_enabled = enabled;
    }
}

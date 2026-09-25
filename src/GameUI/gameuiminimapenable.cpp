//============================================================================
//  0x6F33ACC0 - CGameUIMiniMapPanel::SetEnabled.
//
//  One frame at +0x134 and the flag it was last given at +0x150; nothing
//  happens when the flag has not changed, which is the shape all five of
//  this target's pause-time controllers share.
//
//  Own translation unit: 0x6F602FE0 is a real call into the frame framework.
//============================================================================
#include "gameui.h"
#include "frame.h"

void CGameUIMiniMapPanel::SetEnabled(int enabled)
{
    if (m_enabled != enabled)
    {
        m_pFrame->SetEnabled(enabled);
        m_enabled = enabled;
    }
}

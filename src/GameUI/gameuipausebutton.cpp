//============================================================================
//  0x6F35E670 - CGameUIPauseButton::SetPaused.
//
//  Two instructions: step to the frame at +0x18 and tail-jump into its
//  caption refresh.  The `jmp` is MSVC's tail call on a `void` call in tail
//  position.
//
//  Own translation unit: 0x6F3505D0 is a real call and a different shipped
//  module.
//============================================================================
#include "gameui.h"

void CGameUIPauseButton::SetPaused(int paused)
{
    m_pFrame->RefreshCaption(paused);
}

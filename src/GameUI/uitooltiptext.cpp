//============================================================================
//  0x6F34F5F0 - CUIToolTipFrame::SetToolTipText.
//
//  Two guards and then two calls.  The frame has to *have* a tooltip host at
//  +0x190, and the caller has to have passed a title; either missing and the
//  whole thing is skipped, including the re-layout - a tooltip with no text
//  is left exactly as it was rather than being emptied.
//
//  All five arguments go straight through to the host, in the order they
//  arrived; nothing here reads any of them.  The re-layout that follows is on
//  the frame itself, not on the host.
//
//  The one call site in this tree is the idle-worker button's set-up
//  (0x6F36AB4B), which hands it the "IDLE_PEON" and "IDLE_PEON_DESC" game
//  strings and three zeroes.
//============================================================================
#include "gameui.h"

void CUIToolTipFrame::SetToolTipText(const char* title, const char* body,
                                     int a, int b, int c)
{
    if (m_pToolTipHost == 0 || title == 0)
        return;

    m_pToolTipHost->SetText(title, body, a, b, c);
    RefreshToolTipLayout();
}

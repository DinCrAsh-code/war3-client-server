//============================================================================
//  0x6F603790 - CSimpleButton's own slot 28 (+0x70): the real visual-state
//  setter every other slot in this file calls "NotifyEnabledChanged" -
//  kept under that name because buttonenable.cpp already calls it that way
//  and this session did not want to touch a scored, working call site to
//  relabel a parameter that has been correct (an `int`) the whole time.
//
//  `a` (0 disabled, 1 normal, 2 pressed) is stored into m_enabled itself
//  and picks which of the three m_slots textures is shown: if `a` equals
//  the current m_enabled there is nothing to do but still record `b` into
//  m_13C; otherwise hide whichever slot m_164 last pointed at (only if
//  m_slots[a] actually exists - a button with no texture in the new
//  slot leaves the old one showing), show m_slots[a] and remember it in
//  m_164, reposition (UpdateVisualForState), and only then commit the new
//  state into m_enabled.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleButton::NotifyEnabledChanged(int a, int b)
{
    m_13C = b;

    if (a == m_enabled)
        return;

    if (m_164 && m_slots[a])
    {
        HideSlotTexture(m_164);
        m_164 = 0;
    }

    if (m_slots[a])
    {
        m_164 = m_slots[a];
        ShowSlotTexture(m_164);
    }

    UpdateVisualForState(a);

    m_enabled = a;
}

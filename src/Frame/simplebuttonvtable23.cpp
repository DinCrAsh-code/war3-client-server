//============================================================================
//  0x6F6036B0 - CSimpleButton's own slot 23 (+0x5C): the acquire-side
//  mirror of NotifyCaptureLost below - see that file's own comment and
//  docs/notes/pause-mouse-capture.md for why this is the strongest
//  candidate this session found for what CSimpleTop dispatches the instant
//  it sets m_pCapturedFrame to this button.
//
//  While enabled: force m_70's own m_shown to 1 and dispatch its slot 26
//  (NotifyConstructed) directly - not the usual Hide()/Show() path - raise
//  the pressed-layer priority (layer 4, RaiseLayerPriority), and reposition
//  the visual for m_enabled's *current* value (UpdateVisualForState).  Then,
//  regardless of m_enabled, fire the m_124 observer with m_130 as its
//  context if one is registered.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleButton::NotifyCaptureGained()
{
    if (m_enabled != 0)
    {
        if (m_70)
        {
            m_70->m_shown = 1;
            m_70->NotifyConstructed();
        }

        RaiseLayerPriority(4);
        UpdateVisualForState(m_enabled);
    }

    if (m_124)
    {
        g_buttonEventContext = m_130;
        g_buttonEventSource = this;

        typedef int (__thiscall *Fn)(void*, void*);
        ((Fn)(*(void***)m_124)[3])(m_124, &g_buttonEventTemplate);
    }
}

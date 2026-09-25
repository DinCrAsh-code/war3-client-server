//============================================================================
//  0x6F603720 - CSimpleButton's own slot 24 (+0x60): "you are no longer the
//  captured frame".  The exact mirror of slot 23/NotifyCaptureGained: force
//  m_70's m_shown to 0 and dispatch its slot 25 (NotifyHidden) directly,
//  drop the pressed-layer priority back to 0 (ResetLayerPriority, already
//  named), reposition the visual for the current m_enabled, then fire the
//  m_124 observer with m_134 (not m_130) as its context.
//
//  CSimpleTop::RemoveFrameFromLayer (Frame/simpletoplayers.cpp) already
//  dispatches this slot the moment it clears CSimpleTop::m_pCapturedFrame -
//  the one confirmed, already-reconstructed call site for the whole
//  acquire/lose pair.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleButton::NotifyCaptureLost()
{
    if (m_enabled != 0)
    {
        if (m_70)
        {
            m_70->m_shown = 0;
            m_70->NotifyHidden();
        }

        ResetLayerPriority(4);
        UpdateVisualForState(m_enabled);
    }

    if (m_124)
    {
        g_buttonEventContext = m_134;
        g_buttonEventSource = this;

        typedef int (__thiscall *Fn)(void*, void*);
        ((Fn)(*(void***)m_124)[3])(m_124, &g_buttonEventTemplate);
    }
}

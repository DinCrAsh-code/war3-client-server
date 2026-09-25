//============================================================================
//  0x6F609B50 - CSimpleFrame::NotifyConstructed, slot 26 (+0x68) - "the
//  I am fully built notice the constructor ends on".  The mirror of slot 25
//  (NotifyHidden): once shown, with an unattached parent or none at all,
//  and not already attached itself, join every draw layer this frame's
//  flag bits name (CSimpleTop::AddFrameToLayer), join the ten region lists
//  (CSimpleTop::AddToLayerRegions), dispatch slot 11 (Method_0x2C, a no-op
//  on this class) on self, mark attached, then cascade into the +0x120
//  list the same way slot 25/slot 5 do.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleFrame::NotifyConstructed()
{
    if (m_shown == 0)
        return;
    if (m_pParent && m_pParent->m_attached == 0)
        return;
    if (m_attached != 0)
        return;

    for (int layer = 0; layer < 4; layer++)
    {
        if (m_flags & (1 << layer))
            m_pTop->AddFrameToLayer(this, layer, -1);
    }

    m_pTop->AddToLayerRegions(this, m_depth);
    Method_0x2C();
    m_attached = 1;

    typedef void (__thiscall *Fn)(void*);
    for (char* n = *(char**)((char*)this + 0x120); n; n = *(char**)(n + 4))
    {
        void* child = *(void**)(n + 8);
        ((Fn)(*(void***)child)[26])(child);
    }
}

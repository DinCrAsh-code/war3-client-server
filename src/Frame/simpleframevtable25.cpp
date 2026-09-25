//============================================================================
//  0x6F609AD0 - CSimpleFrame::NotifyHidden, slot 25 (+0x64).  The hide half
//  of the pair slot 26 (NotifyConstructed) shows: while this frame is
//  attached, pull it out of every draw layer it is a bit-flagged member of
//  (CSimpleTop::RemoveFrameFromLayer, already named) and out of the ten
//  region lists (CSimpleTop::RemoveFromLayerRegions - which is also a third
//  confirmed CSimpleTop::m_pCapturedFrame release path, see its own
//  comment), dispatch slot 12 (Method_0x30, a no-op on this class) on self,
//  clear m_attached, then cascade the same notification into the +0x120
//  list slot 5 (PropagateScaleToChildren) already walks.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleFrame::NotifyHidden()
{
    if (m_attached == 0)
        return;

    for (int layer = 0; layer < 4; layer++)
    {
        if (m_flags & (1 << layer))
            m_pTop->RemoveFrameFromLayer(this, layer);
    }

    m_pTop->RemoveFromLayerRegions(this);
    Method_0x30();
    m_attached = 0;

    typedef void (__thiscall *Fn)(void*);
    for (char* n = *(char**)((char*)this + 0x120); n; n = *(char**)(n + 4))
    {
        void* child = *(void**)(n + 8);
        ((Fn)(*(void***)child)[25])(child);
    }
}

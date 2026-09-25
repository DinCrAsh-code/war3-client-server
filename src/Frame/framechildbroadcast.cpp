//============================================================================
//  CFrame's own slots 31-33 (+0x7C/+0x80/+0x84) - a real, shared "broadcast
//  to every child layer" family (same address on a fresh CSimpleTop and
//  CWorldFrameWar3 pull).  All three walk m_children (CLayer's own
//  CFrameNodeList<LAYERNODE,4>, clayer.h) tail-to-head - exactly the shape
//  CFrameNodeList::Tail()/Prev() already give a name to, which this file
//  uses directly rather than re-deriving the walk from raw offsets.
//============================================================================
#include "frame.h"

//  CFrame::EnsureAndConfigureScreenSprite is declared on the class in
//  frame.h and defined in Frame/framescreensprite.cpp - see that file's
//  own header comment for why it is not defined here, alongside slot 33's
//  own only in-repo caller.

extern CFrame* g_frameScreenFrameGuard;   // dword_6FACE670

//----------------------------------------------------------------------------
//  Slot 31 (+0x7C) - only while m_field30 == 0: tell every child's own
//  slot 31 the same pointer argument.
//----------------------------------------------------------------------------
void CFrame::Method_0x7C(void* arg)
{
    if (m_field30 != 0)
        return;

    typedef void (__thiscall *Fn)(void*, void*);
    for (LAYERNODE* n = m_children.Tail(); n != 0; n = m_children.Prev(n))
    {
        Fn f = (Fn)(*(void***)n->m_pLayer)[0x7C / 4];
        f(n->m_pLayer, arg);
    }
}

//----------------------------------------------------------------------------
//  Slot 32 (+0x80) - only while m_float34 != 0.0: tell every child's own
//  slot 32 the same float argument.
//----------------------------------------------------------------------------
void CFrame::Method_0x80(float arg)
{
    if (m_float34 == 0.0f)
        return;

    typedef void (__thiscall *Fn)(void*, float);
    for (LAYERNODE* n = m_children.Tail(); n != 0; n = m_children.Prev(n))
    {
        Fn f = (Fn)(*(void***)n->m_pLayer)[0x80 / 4];
        f(n->m_pLayer, arg);
    }
}

//----------------------------------------------------------------------------
//  Slot 33 (+0x84) - only while m_field2C == 0: first, unless the "system
//  message host" singleton (g_unk6FACE754/ReadUnk6FACE754, already named)
//  already IS this frame, lazily ensure its own screen-space render object
//  (CScreenFrame::EnsureRenderObject, already named) and reconfigure it
//  with this call's own argument (CSpriteFrame::ConfigureSprite, already
//  named); then broadcast the same argument to every child's own slot 33.
//
//  This is *not* CSimpleTop::m_pCapturedFrame despite the coincidental
//  shared +0x16C offset - see frame.h's own comment on CSimpleTop's
//  slots 34/35 and docs/notes/pause-mouse-capture.md's dated section.
//----------------------------------------------------------------------------
void CFrame::Method_0x84(void* arg)
{
    if (m_field2C != 0)
        return;

    if (g_frameScreenFrameGuard != this)
        EnsureAndConfigureScreenSprite(arg);

    typedef void (__thiscall *Fn)(void*, void*);
    for (LAYERNODE* n = m_children.Tail(); n != 0; n = m_children.Prev(n))
    {
        Fn f = (Fn)(*(void***)n->m_pLayer)[0x84 / 4];
        f(n->m_pLayer, arg);
    }
}

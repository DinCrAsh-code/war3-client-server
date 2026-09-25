//============================================================================
//  0x6F609C70 - CSimpleFrame::CSimpleFrame.
//
//  One base (CLayoutFrame, at offset 0 this time - see frame.h on why CFrame
//  and CSimpleFrame are two hierarchies and not one), five embedded
//  SFrameSlots (the per-layer region lists) built by the vector-constructor
//  iterator, and the registration with the one CSimpleTop.
//
//  Score capped by the unified EH frame; see framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CSimpleFrame::CSimpleFrame(void* parent)
{
    m_screenX0 = 0.0f;
    m_screenY0 = 0.0f;
    m_screenX1 = 0.0f;
    m_screenY1 = 0.0f;
    m_insetX0 = 0.0f;
    m_insetY0 = 0.0f;
    m_insetX1 = 0.0f;
    m_insetY1 = 0.0f;
    m_74 = 0;
    m_78 = 0;
    m_7C = 0;
    m_80 = 0;
    m_88 = 0xFF;
    m_flags = 0;
    m_attached = 0;
    m_pOwnedFrame = 0;

    m_pTop = g_pSimpleTop;
    g_pSimpleTop->AddFrame(this);

    m_pParent = 0;
    SetParent(parent);
    m_70 = 0;

    m_layerPriority[0] = 1;
    m_layerPriority[1] = 1;
    m_layerPriority[2] = 1;
    m_layerPriority[3] = 1;
    m_layerPriority[4] = 0;
    m_shown = 1;

    NotifyConstructed();
}

//  0x6F609DD0 - the base destructor.  Naked thunk; see frame.h's own
//  comment on the declaration for why this batch (CItem vtable-closure
//  batch 7) added the declaration without reconstructing the body.
__declspec(naked) CSimpleFrame::~CSimpleFrame()
{
    __asm { mov eax, 0x6F609DD0 }
    __asm { jmp eax }
}

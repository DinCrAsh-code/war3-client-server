//============================================================================
//  0x6F60BDF0 - CSimpleTop::CSimpleTop, the one top-level frame manager.
//
//  Four vector-constructed arrays - ten SFrameLayers, fifty SRenderBatches,
//  four CFramePriorityArrays and, between the first two, a fifty-dword dirty-layer
//  table the shipped code clears with `rep stosd` - and the store into
//  dword_6FACE758 that makes it the singleton every CSimpleFrame finds.
//
//  Score capped by the unified EH frame; see framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CSimpleTop::CSimpleTop(int a, int b, int c)
    : CFrame(a, b, c)
{
    m_168 = 0;

    g_pSimpleTop = this;

    m_pCapturedFrame = 0;
    m_170 = 0;

    for (int i = 0; i < 50; ++i)
        m_dirty[i] = 0;

    m_shortcutsOn = 1;

    m_dirtyBatches.m_alloc = 0;
    m_dirtyBatches.m_count = 0;
    m_dirtyBatches.m_data = 0;

    m_8D4 = 0;
    m_8D8 = 0;
    m_8DC = 0;
    m_8E0 = 0;
    m_8E4 = 0;
    m_8E8 = 0;
    m_8EC = 0;
    m_8F0 = 0;
    m_8F4 = 0;
}

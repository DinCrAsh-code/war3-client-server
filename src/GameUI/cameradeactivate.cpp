//============================================================================
//  0x6F308B60 - CCameraWar3::Deactivate, overriding CAgent's own slot 13.
//  `retn 0`.
//
//  Stop everything (StopAll(1), the same call SetControllerMode and PanTo
//  both already reach), unbind the nine controller-property registrations
//  by hand - the same nine calls UnbindControllerProps makes for its own
//  first nine (camerabindprops.cpp/cameraunbindprops.cpp), but written out
//  again here rather than through a call to that function: the dump has
//  nine more inline calls into the same two slot binders, not a `call` to
//  UnbindControllerProps, and it skips that function's own trailing pair
//  on the event sink entirely - and finally drop the four controller/sink
//  references the camera itself owns.
//
//  Was one of CCameraWar3's nine naked-thunk vtable overrides
//  (cameravtablethunks.cpp); wired in for real now that the body is
//  reconstructed.
//============================================================================
#include "gameui.h"
#include "cameracontrollerbind.h"
#include "refcnt.h"

void CCameraWar3::Deactivate()
{
    StopAll(1);

    BindControllerSlotDirect(m_pDefaultController, 8, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 0, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 1, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 2, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 3, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 4, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 5, 0, 0, 0.0f);
    BindControllerSlotRamp(m_pDefaultController, 6, 0, 0, 0.0f);
    BindControllerSlotDirect(m_pDefaultController, 7, 0, 0, 0.0f);

    //  Shipped order is compare-to-m_pDefaultController first, then
    //  compare-to-zero (`cmp ecx,[esi+64]` / `cmp ecx,edi`) - the operand
    //  order of the `&&` below is what the compiler evaluates left to
    //  right, so it has to be spelled with the m_pDefaultController
    //  comparison first to match.
    if (m_pController != m_pDefaultController && m_pController != 0)
    {
        ((TRefCnt*)m_pController)->Release();
        m_pController = 0;
    }
    if (m_pAltController != 0)
    {
        ((TRefCnt*)m_pAltController)->Release();
        m_pAltController = 0;
    }
    if (m_pDefaultController != 0)
    {
        ((TRefCnt*)m_pDefaultController)->Release();
        m_pDefaultController = 0;
    }
    if (m_pEventSink != 0)
    {
        ((TRefCnt*)m_pEventSink)->Release();
        m_pEventSink = 0;
    }
}

//============================================================================
//  0x6F306980 - CCameraWar3::SetControllerMode: switch between "the default
//  controller drives the properties" (modes 0 and 2) and "nothing does"
//  (mode 1), then tell the UI's camera host which controller is live.
//
//  Three things the shipped code fixes and a tidier spelling would lose:
//
//   * an early-out on an unchanged mode, *before* the field is written;
//   * modes 0 and 2 share one arm.  The shipped test is a chain of
//     `sub`/`jz` on a register, which is what a `switch` over a small dense
//     set gives, and modes 0 and 2 fall into the same block;
//   * the notification at the end runs for *every* mode that got past the
//     early-out, including one the switch has no arm for, and it is a tail
//     jump - which is why it sits after the switch and not inside it.
//
//  Own translation unit: it calls both binders and GetGameUI for real.
//============================================================================
#include "gameui.h"
#include "refcnt.h"

void CCameraWar3::SetControllerMode(int mode)
{
    if (mode == m_controllerMode)
        return;

    m_controllerMode = mode;

    switch (mode)
    {
    case 0:
    case 2:
        if (m_pController != 0 && m_pController != m_pDefaultController)
            ((TRefCnt*)m_pController)->Release();
        m_pController = m_pDefaultController;
        BindControllerProps();
        break;

    case 1:
        UnbindControllerProps();
        break;
    }

    //  The singleton is named first and the host is not.  MSVC evaluates
    //  arguments right to left, so folding GetGameUI into the receiver
    //  expression pushes m_pController *before* it runs and the call can no
    //  longer be the shipped tail jump; naming the host as well costs the
    //  other direction, an extra `mov ecx,eax` where the shipped code loads
    //  the receiver straight out of `[eax+3BCh]`.  Exactly one of the two
    //  is a local.
    CGameUI* ui = (CGameUI*)GetGameUI(1, 0);
    ui->m_pWorldFrameWar3->NotifyControllerChanged(m_pController);
}

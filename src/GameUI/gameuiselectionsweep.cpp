//============================================================================
//  0x6F333590 and 0x6F3335A0 - the two smallest members of the 0x6F333xxx
//  module.  Neither calls the other.
//============================================================================
#include "gameui.h"

//----------------------------------------------------------------------------
//  0x6F333590 - one instruction.  A whole function whose body is one `void`
//  call in tail position, which MSVC turns into a `jmp`.
//----------------------------------------------------------------------------
void RefreshAllSelectionVisuals()
{
    RefreshSelectedUnitVisuals();
}

//----------------------------------------------------------------------------
//  0x6F3335A0 - is the world view up?
//
//  Asked with `create` clear, so this never brings the UI into existence;
//  both of the singleton's two view fields have to be set.
//----------------------------------------------------------------------------
int __fastcall IsWorldViewReady()
{
    CGameUI* ui = (CGameUI*)GetGameUI(0, 0);

    if (ui != 0 && ui->m_viewReadyA != 0 && ui->m_viewReadyB != 0)
        return 1;

    return 0;
}

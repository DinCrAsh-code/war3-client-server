//============================================================================
//  0x6F0D1B90 - NotifyGameUIOfAbilityOwner, with 0x6F332F20 folded in
//  (IDA's own tail-jmp chunk of the same function, not a separate one).
//  `this`/only arg in ecx (`__fastcall`, retn 0), reached from
//  CUnit::NotifyAbilityManagerAndGameUI
//  (Unit/unit_notifyabilitymanagerandgameui.cpp).
//
//  The manager's own +0x30 is a cached "owner" pointer, the same shape
//  SAttachmentTarget::Show already establishes
//  (Widget/attachmentcounters.cpp) - use it if set, else resolve through
//  SAttachmentOwnerQuery::QueryOwner (0x6F472890, already reconstructed).
//  Once resolved, ask GetGameUI(0, 0) and, if the UI exists, forward the
//  owner to it (CGameUI::NotifyAbilityListenerOfOwner, 0x6F300CB0,
//  GameUI/gameuiabilitylistenernotify.cpp).
//============================================================================
#include "gameui.h"

//  Re-declared to match Widget/attachmentcounters.cpp's own
//  SAttachmentOwnerQuery exactly, so QueryOwner binds onto that real body.
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();   // 0x6F472890, already reconstructed
};
struct SAbilityManagerOwnerCache
{
    char  m_reserved00[0x30];
    void* m_pOwner;   // +0x30 - same shape as SAttachmentTarget's own
};

void __fastcall NotifyGameUIOfAbilityOwner(void* manager)
{
    void* cached = ((SAbilityManagerOwnerCache*)manager)->m_pOwner;
    void* owner = cached ? cached
                         : ((SAttachmentOwnerQuery*)manager)->QueryOwner();

    CGameUI* ui = (CGameUI*)GetGameUI(0, 0);
    if (ui != 0)
        ui->NotifyAbilityListenerOfOwner(owner);
}

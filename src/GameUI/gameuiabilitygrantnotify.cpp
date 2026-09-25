//============================================================================
//  0x6F332830 - NotifyGameUIAbilityGranted: after JASS_UnitAddAbility's own
//  root (Jass/jassunitaddability_native.cpp) grants a new ability to a
//  unit, tell the CGameUI singleton about it - the same "only if one
//  already exists, never build one" shape attachmentnotifygameui.cpp's own
//  notify uses (`create` is 0). __fastcall, the recipient in ecx; `retn`.
//
//  Its own 0x6F332xxx module, a different address neighbourhood from the
//  0x6F2F8xxx receiver it calls into (SGameUIAbilityGrantNotify::
//  OnAbilityGranted, 0x6F2F89C0, gameuiabilitygrantreceiver.cpp) - own
//  translation unit for that reason, not just organisation: a small
//  thiscall method defined in the same TU as its one caller is exactly
//  what this build's /Ob2 inlines away, and the shipped dump has a real
//  `call` here.
//============================================================================

//  0x6F300710 - the lazily-constructed CGameUI singleton, already declared
//  (and defined as a naked redirect) in Widget/selectable_attachcreate.cpp;
//  redeclared here against this file's own narrower view of the struct
//  rather than pulling in that file's unrelated 0x2A0-byte layout - the
//  same per-TU-local-view convention Widget/attachmentnotifygameui.cpp uses
//  for the same singleton.
struct SGameUI;
SGameUI* __fastcall GetGameUI(int create, int reset);

//  0x6F2F89C0 - gameuiabilitygrantreceiver.cpp.
struct SGameUIAbilityGrantNotify
{
    void __thiscall OnAbilityGranted(void* candidate);
};

void __fastcall NotifyGameUIAbilityGranted(void* recipient)
{
    SGameUIAbilityGrantNotify* ui = (SGameUIAbilityGrantNotify*)GetGameUI(0, 0);
    if (ui != 0)
        ui->OnAbilityGranted(recipient);
}

//============================================================================
//  0x6F2F89C0 - CGameUI's own ability-grant receiver, in its own 0x6F2F8xxx
//  module (own translation unit, per CLAUDE.md's "one TU per shipped
//  module" - NotifyGameUIAbilityGranted (0x6F332830,
//  gameuiabilitygrantnotify.cpp) is a real call in the shipped dump, not
//  something this build's own inliner is allowed to fold away by sharing a
//  TU with its callee).
//
//  `this` is the CGameUI, the candidate object (the unit/agent an ability
//  was just granted to) arrives on the stack. Check the candidate
//  (0x6F2F83D0, out of this batch's own worklist slice - not reconstructed
//  here, only thunked so this TU links) and, if it passes, mark the
//  ability panel dirty at +0x27C. `retn 4`.
//============================================================================

struct SGameUIAbilityGrantNotify
{
    char          m_reserved000[0x27C];
    unsigned int  m_abilityPanelDirty;  // +0x27C - set once a grant needs a
                                         //   panel refresh; read elsewhere in
                                         //   CGameUI, not in this batch's
                                         //   own worklist slice.

    //  0x6F2F83D0 - out of this batch's own worklist slice (no dump in this
    //  call tree): whatever check decides a grant is one the ability panel
    //  cares about. `retn 4`, one stack argument (the candidate). Thunked
    //  below so this TU links (link_check.py) without claiming the address.
    int __thiscall CheckCandidate(void* candidate);

    void __thiscall OnAbilityGranted(void* candidate);
};

__declspec(naked) int __thiscall
SGameUIAbilityGrantNotify::CheckCandidate(void*)
{
    __asm { mov eax, 06F2F83D0h }
    __asm { jmp eax }
}

void __thiscall SGameUIAbilityGrantNotify::OnAbilityGranted(void* candidate)
{
    if (CheckCandidate(candidate))
        m_abilityPanelDirty = 1;
}

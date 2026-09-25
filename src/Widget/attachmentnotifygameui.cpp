//============================================================================
//  0x6F3329D0 - SAttachmentCounterNotify::OnCountChanged (0x6F021270,
//  attachmentcounters.cpp) tail-jumps here with the same `this`, so this is
//  the same receiver: after the two unreconstructed 0x6F021xxx-module
//  helpers run, tell the CGameUI singleton (if one already exists - `create`
//  is 0, so this never builds one) about the change.
//
//  Own translation unit: a different address neighbourhood from
//  attachmentcounters.cpp's own 0x6F021xxx module, and it reaches
//  GetGameUI (already declared, selectable_attachcreate.cpp) with a real
//  call.
//============================================================================
//  0x6F300710 - the lazily-constructed CGameUI singleton.  Behind an
//  __except_handler4-shaped SEH frame this toolchain cannot reproduce, so
//  it stays a redirect (selectable_attachcreate.cpp); declared here, not
//  re-thunked - a second naked body at the same address is the
//  two-symbols-for-one-function defect docs/notes/verifier-gate-link-
//  failures.md warns about.
struct SGameUI
{
    char          m_reserved00[0x2A0];
    unsigned int  m_uiUp;           // +0x2A0
    unsigned int  m_circlesLit;     // +0x2A4
};
SGameUI* __fastcall GetGameUI(int create, int reset);

//  Same receiver OnCountChanged has, under a TU-local name: attachmentcounters.cpp
//  never exported a header to declare the real SAttachmentCounterNotify against.
struct SAttachmentChangeReceiver
{
    void __thiscall NotifyGameUI();
};

//----------------------------------------------------------------------------
//  0x6F3009F0 - whatever CGameUI does with a changed attachment owner.
//  `retn 4`, one stack argument (this call tree's own `this`); no dump in
//  this batch, so the body stays a naked redirect.
//----------------------------------------------------------------------------
struct SGameUIAttachmentNotify
{
    void __thiscall OnAttachmentOwnerChanged(void* owner);
};

__declspec(naked) void __thiscall
SGameUIAttachmentNotify::OnAttachmentOwnerChanged(void*)
{
    __asm
    {
        mov     eax, 06F3009F0h
        jmp     eax
    }
}

void __thiscall SAttachmentChangeReceiver::NotifyGameUI()
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui != 0)
        ((SGameUIAttachmentNotify*)ui)->OnAttachmentOwnerChanged(this);
}

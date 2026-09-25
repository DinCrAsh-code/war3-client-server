//============================================================================
//  0x6F3687C0 - CCommandButton::~CCommandButton.  `retn 0`.
//
//  Stamps its own vtable, releases the optional CCommandButtonData record
//  at +0x190 through its own release helper (sub_6F35E610, out of this
//  batch's own dump - declared and called, not guessed at), frees it
//  outright with Storm if +0x18C still says the button owns it, and tails
//  into the base class's own destructor (CSimpleButton::~CSimpleButton,
//  0x6F603800 - Frame/simpleframebuttondtoraliases.cpp's own alias struct,
//  reused here as the call target rather than inventing a second one for
//  the same shipped address).
//============================================================================
#include "commandbutton.h"

//  0x6F35E610 - out of this batch's own dump, confirmed by this
//  destructor's own xrefs (`worktree_store.py xrefs 0x6F3687C0 --from`);
//  link_check.py caught this as declared and called but never defined -
//  a naked redirect until its own dump lands.
__declspec(naked) void __fastcall ReleaseCommandButtonData(CCommandButton*)
{
    __asm { mov eax, 0x6F35E610 }
    __asm { jmp eax }
}

//  Frame/simpleframebuttondtoraliases.cpp's own alias struct for
//  0x6F603800 (CSimpleButton::~CSimpleButton) - declared again here to
//  match its mangled name exactly rather than shared through a header,
//  the same pattern this repo already uses for a callee reconstructed in
//  its own separate TU.
struct SCSimpleButtonDtorAlias
{
    void Dtor();
};

CCommandButton::~CCommandButton()
{
    ReleaseCommandButtonData(this);

    if (m_hasData && m_pData)
        SMemFree(m_pData, "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CCommandButton.cpp", 65, 0);

    ((SCSimpleButtonDtorAlias*)this)->Dtor();
}

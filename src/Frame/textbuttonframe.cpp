//============================================================================
//  0x6F61E910 - CTextButtonFrame::CTextButtonFrame.
//
//  The base is CButtonFrame (0x6F6220C0), called with (a, b, c) forwarded
//  unchanged - the same three-int shape every class in this chain passes
//  straight down to CFrame, where `a` is the parent.
//
//  After the base returns, this constructor:
//
//    * zeroes its own +0x1E4 caption pointer and the four floats at
//      +0x1E8..+0x1F4 (member initialisers, in declaration order - the only
//      shape that gets a `fldz`/`fst`x4 run with no intervening branch);
//    * makes its own caption CTextFrame, parented on *itself* (`this` is the
//      first argument SSelectionHost::CreatePrefixLabel forwards straight
//      into CTextFrame::CTextFrame as `a`, the same role `this` plays at
//      that thunk's one other, genuine SSelectionHost call site in
//      GameUI/chateditbar.cpp - see the struct below for why the receiver
//      type does not have to match);
//    * addrefs the new caption, releases whatever +0x1E4 held before (0,
//      here, so a no-op - the shipped stream still carries the check), and
//      stores the new one - the same addref-new/release-old/store shape
//      Unit/unitorderinit.cpp's COrder::Init already uses for a counted
//      handle;
//    * anchors the caption to cover the whole button
//      (`label->SetAllPoints(this, 1)`), unconditionally - SetAllPoints is
//      not virtual, so there is no null test to skip even though the
//      allocation two lines up was.  This is the shipped shape, not a bug
//      introduced here: Frame/buttonslottexture.cpp's own
//      CSimpleButton::SetSlotTexture has the identical unchecked-`made->`
//      pattern for the same reason.
//
//  Score capped by the unified EH frame; see framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  0x6F6121E0 - already reconstructed as SSelectionHost::CreatePrefixLabel
//  in GameUI/gameui_thunks.cpp (`ADDR_THUNK`, `retn 4`).  Read for this
//  target (ida_query calltree_asm), its own body never touches `this` as an
//  SSelectionHost - it is a plain three-int factory that forwards
//  (ecx, edx, one stack dword) straight into CTextFrame::CTextFrame as
//  (a, b, c), so `this` only ever plays the role of the new caption's
//  *parent* pointer.  Re-declared locally under the same class+method name
//  (same mangled symbol, no body here, nothing added to funcmap.py) rather
//  than pulling in GameUI/gameui.h from this module - the same technique
//  Misc/sweep_field_setters.cpp's owners are re-declared under in every TU
//  that needs one of them.
struct SSelectionHost
{
    CTextFrame* __fastcall CreatePrefixLabel(int a, int b);
};

CTextButtonFrame::CTextButtonFrame(int a, int b, int c)
    : CButtonFrame(a, b, c)
{
    m_pCaption = 0;
    m_1E8 = 0.0f;
    m_1EC = 0.0f;
    m_1F0 = 0.0f;
    m_1F4 = 0.0f;

    CTextFrame* label = ((SSelectionHost*)this)->CreatePrefixLabel(0, 0);

    if (label)
        label->m_refcount++;

    CTextFrame* old = m_pCaption;
    if (old)
    {
        if (--old->m_refcount == 0)
            old->ReleaseSelf();
    }
    m_pCaption = label;

    label->SetAllPoints(this, 1);
}

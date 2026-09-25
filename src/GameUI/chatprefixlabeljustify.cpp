//============================================================================
//  0x6F611EB0 - CChatPrefixLabel::SetJustify.  Its own translation unit,
//  separate from GameUI/chattextlines.cpp's CChatPrefixLabel::SetShadow and
//  CChatEntryBox::SetJustify (both real callers of this one): with no
//  whole-program optimisation, /O2 /Ob2 only auto-inlines a callee it can
//  see the definition of in the *same* TU, and the shipped body genuinely
//  makes a real `call` at both those sites - keeping this in its own file is
//  what keeps that call real here too, the same "one module, one TU" split
//  this codebase already uses to stop two genuinely-shipped-as-separate
//  functions from folding into one.
//
//  `this`'s own +0x1F0 is a pointer to a two-int style record: [0] the live
//  bits, [4] bits this label always forces on regardless of what is asked
//  for.  OR the new justify bits into both of those and the record's own
//  live bits, store, and - only if the live bits actually changed - tell
//  the frame (OnStyleBitsChanged) what changed.  The four dirty-flag stores
//  at the end are unconditional, same as the shipped stream.
//============================================================================
#include "gameui.h"

class CChatPrefixLabel
{
public:
    void SetJustify(int justify);   // 0x6F611EB0
};

//  0x6F611650 - CTextFrame's own masked-display-text refresh (its assert
//  string, read off `worktree_store show`, is ".\CTextFrame.cpp"): when the
//  0x100 ("masked") bit of the style word changes, it re-derives
//  m_pDisplayText from m_pText via HasStyleBits/MakeMaskedText.  Outside
//  this batch (not one of its 35 addresses) and its own module has not
//  landed, so it stays a local, declared-and-thunked callee here - the same
//  pattern chateditbar.cpp already uses for SDirtyFlagOwner's two setters.
class CTextFrameStyleNotify
{
public:
    void OnStyleBitsChanged(int changedBits);   // 0x6F611650
};

__declspec(naked) void CTextFrameStyleNotify::OnStyleBitsChanged(int)
{
    __asm { mov eax, 0x6F611650 }
    __asm { jmp eax }
}

void CChatPrefixLabel::SetJustify(int justify)
{
    struct SStyleWord
    {
        int m_bits;
        int m_forced;
    };

    SStyleWord* style = *(SStyleWord**)((char*)this + 0x1F0);
    int oldBits = style->m_bits;
    int newBits = oldBits | style->m_forced | justify;
    style->m_bits = newBits;

    int changed = newBits ^ oldBits;
    if (changed != 0)
        ((CTextFrameStyleNotify*)this)->OnStyleBitsChanged(changed);

    *(int*)((char*)this + 0x224) = 1;
    *(int*)((char*)this + 0x22C) = 1;
    *(int*)((char*)this + 0x230) = 1;
    *(int*)((char*)this + 0x23C) = 1;
}

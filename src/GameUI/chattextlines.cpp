//============================================================================
//  Four more of the small setters the chat edit bar's two children (GameUI/
//  chateditbar.cpp's `CChatPrefixLabel` / `CChatEntryBox` - both local,
//  field-free placeholder names for two shipped classes nothing in this
//  tree fully models; see that file's own header comment) reach through.
//  Every one of these was still a naked `ADDR_THUNK` in gameui_thunks.cpp;
//  this batch (CItem's vtable-closure batch 7) reached all four as still-
//  TODO addresses and had enough of each body's own fields pinned down to
//  write real bodies.  `CChatPrefixLabel::SetJustify` (0x6F611EB0, a real
//  caller of two of these) is its own TU now, GameUI/chatprefixlabeljustify.cpp
//  - see that file's own header for why.
//
//  `CChatEntryBox::SetMaxLines` (0x6F616250) is deliberately NOT here: its
//  own body treats +0x1E8/+0x1EC/+0x1F0 as an integer line-count/pointer
//  triplet and then calls CTextFrame::SetText (0x6F615B50) on the same
//  `this`, which only makes sense if CEditBox (the class its own assert
//  string ".\CEditBox.cpp" names) really does share CTextFrame's layout at
//  those offsets - contradicting the plain-int reading its own compare
//  otherwise suggests.  That is a real class-layout question this batch
//  does not have enough evidence to settle, so it stays TODO rather than
//  guessing; see docs/targets/CItem-callee-batch7.md.
//============================================================================
#include "gameui.h"
#include "renderdevice.h"       // TEXTURECOLOR

//  Matches chateditbar.cpp's own local declaration exactly (member mangling
//  only encodes the class name and the one function's own signature, not
//  the rest of the class, so two independent field-free local copies of
//  the same class name are the same symbol at every method they share).
//  SetJustify's own body is GameUI/chatprefixlabeljustify.cpp; declared
//  (not defined) here so this file's own two callers make a real call.
class CChatPrefixLabel
{
public:
    void SetJustify(int justify);                                // 0x6F611EB0
    void SetShadow(TEXTURECOLOR colour, const float* offset);   // 0x6F611F90
};

class CChatEntryBox
{
public:
    void SetFont(const char* path, float height, int flags);    // 0x6F613CA0
    void SetColor(const unsigned int* colour);                  // 0x6F613D60
    void SetJustify(int justify);                               // 0x6F613D90
};

//----------------------------------------------------------------------------
//  0x6F611F90 - CChatPrefixLabel::SetShadow.  Stores the shadow colour and
//  its two-float offset into three fields of its own, folds bit 4 (the
//  "has a shadow" flag) into the same style word SetJustify owns - the
//  shipped code reuses SetJustify itself to do that fold and its dirty-flag
//  side effects rather than repeating them - then queues this label's own
//  layout sub-object (at +0xB4, the same CLayoutFrame offset chateditbar.cpp
//  casts through) for re-layout.
//----------------------------------------------------------------------------
void CChatPrefixLabel::SetShadow(TEXTURECOLOR colour, const float* offset)
{
    *(TEXTURECOLOR*)((char*)this + 0x200) = colour;
    //  Copied as raw dwords, not floats: the shipped stream is a plain
    //  `mov`, not an `fld`/`fstp` pair - `offset` is only ever a pointer
    //  here, never loaded onto the FPU stack.
    ((unsigned int*)((char*)this + 0x210))[0] = ((const unsigned int*)offset)[0];
    ((unsigned int*)((char*)this + 0x210))[1] = ((const unsigned int*)offset)[1];

    SetJustify(4);

    ((CLayoutFrame*)((char*)this + 0xB4))->QueueForLayout(1);

    *(int*)((char*)this + 0x23C) = 1;
}

//----------------------------------------------------------------------------
//  The entry box's own eight fixed message-line children, at +0x254 through
//  +0x270 - a `CChatPrefixLabel*[8]`, not modelled as a named field because
//  no member of this placeholder class is meant to carry real layout (see
//  chateditbar.cpp's own note on why); reached the same way that file
//  reaches every other field, as a raw offset off `this`.
//----------------------------------------------------------------------------
static inline CChatPrefixLabel* const* EntryBoxLines(const CChatEntryBox* self)
{
    return (CChatPrefixLabel* const*)((const char*)self + 0x254);
}

//  0x6F5FB960's own real name is `SFrameFontHost::SetFont`
//  (Frame/messageframethunks.cpp) - the class name a member call's mangled
//  symbol carries, so calling it through `CChatPrefixLabel` here (a
//  different, if layout-identical, placeholder name) would resolve to a
//  second, unrelated symbol at the same address instead of the one real
//  callee already declared. Re-declared here matching that file exactly.
struct SFrameFontHost { void SetFont(const char* path, float size, int flags); };

//  0x6F613CA0 - propagate a font change to all eight lines.  Written out
//  longhand rather than as a loop: the shipped body is fully unrolled (eight
//  copies of the same three-push-and-call sequence), and a `for` loop here
//  compiles to an actual loop instead - verified against the diff.
void CChatEntryBox::SetFont(const char* path, float height, int flags)
{
    SFrameFontHost* const* lines = (SFrameFontHost* const*)EntryBoxLines(this);
    lines[0]->SetFont(path, height, flags);
    lines[1]->SetFont(path, height, flags);
    lines[2]->SetFont(path, height, flags);
    lines[3]->SetFont(path, height, flags);
    lines[4]->SetFont(path, height, flags);
    lines[5]->SetFont(path, height, flags);
    lines[6]->SetFont(path, height, flags);
    lines[7]->SetFont(path, height, flags);
}

//  Matches chateditbar.cpp's own local declaration.
struct SDirtyFlagOwner
{
    void __thiscall SetField21C(int value);
    void __thiscall SetField1F4IfChanged(const int* value);
};

//  0x6F613D60 - propagate a colour change to the first two lines only (the
//  shipped stream only ever touches +0x254/+0x258 - not a mistake here, the
//  entry box's own visible span is shorter than its eight-line backing
//  array).
void CChatEntryBox::SetColor(const unsigned int* colour)
{
    CChatPrefixLabel* const* lines = EntryBoxLines(this);
    ((SDirtyFlagOwner*)lines[0])->SetField1F4IfChanged((const int*)colour);
    ((SDirtyFlagOwner*)lines[1])->SetField1F4IfChanged((const int*)colour);
}

//  0x6F613D90 - propagate a justify change to the first four lines.
void CChatEntryBox::SetJustify(int justify)
{
    CChatPrefixLabel* const* lines = EntryBoxLines(this);
    lines[0]->SetJustify(justify);
    lines[1]->SetJustify(justify);
    lines[2]->SetJustify(justify);
    lines[3]->SetJustify(justify);
}

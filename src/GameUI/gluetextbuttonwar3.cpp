//============================================================================
//  0x6F556E20 - CGlueTextButtonWar3::CGlueTextButtonWar3.
//
//  docs/notes/pause-mouse-capture.md, "2026-08-28" section, has the full
//  investigation this target sits inside.  In short: tracing CGameUI's own
//  fields toward the Esc-menu/pause chain led here (CEscMenuMainPanel's own
//  +0x190, declared `void* m_pCaptionFrame` in GameUI/gameui.h, reads as a
//  CGlueTextButtonWar3*), while a *different* live investigation found the
//  object actually holding mouse capture during a pause reading back with
//  **CSimpleButton's own vtable pointer** (0x6F96DC8C) - a different address
//  from this class's own (0x6F95A00C).  This constructor settles which of
//  the two live possibilities is real:
//
//    * this class derives, single-inheritance, straight from
//      CTextButtonFrame (Frame/frame.h, Frame/textbuttonframe.cpp) - itself
//      CButtonFrame -> CControl -> CSpriteFrame -> CFrame -> CLayer ->
//      CObserver -> TRefCnt, plus CLayoutFrame as CFrame's own secondary
//      base at +0xB4.  **CSimpleButton is nowhere in that chain.**
//      CSimpleButton derives from CSimpleFrame, a *different* CLayoutFrame
//      descendant (CLayoutFrame as CSimpleFrame's *first* base, not its
//      second - see frame.h's own "two parallel hierarchies" note) that
//      shares no class with this one below CLayoutFrame itself.
//    * the constructor stamps exactly two vtable pointers, at +0 and +0xB4
//      (??_7CGlueTextButtonWar3@@6B@ / its `_0` twin) - the ordinary
//      "CFrame has two bases with vtables" shape every class in this file's
//      whole ancestor chain carries, not a *third*, additional stamp at some
//      other offset the way a genuine "also derives from CSimpleButton"
//      secondary base would need.  There is no CSimpleButton sub-object
//      anywhere in this object: its field layout runs
//      CControl(+0x1B0..+0x1E0) -> CTextButtonFrame(+0x1E4..+0x1F4) ->
//      this class's own +0x1F8, with nothing CSimpleButton-shaped spliced
//      in.
//
//  **So the two vtable pointers genuinely disagree because they are two
//  different objects.**  The Esc-menu-button identification
//  (CGameUIPauseFrame::m_pCaptionFrame -> a CGlueTextButtonWar3) is real,
//  structurally confirmed - but it is not what holds mouse capture during a
//  pause.  Something else, with CSimpleButton's own vtable, still is; see
//  the note for what that leaves open.
//
//  The base call forwards `(int)parent` as CTextButtonFrame's own `a`
//  (the parent every class in this chain threads down to CFrame), with `b`
//  and `c` hardcoded 0 - the same "only the parent survives to this class's
//  own public constructor" shape CPortraitButton and CButtonFrame already
//  use for the same three-int base.
//
//  No explicit vtable-pointer stores are written here, on purpose: this
//  class overrides nothing (zero new `virtual`s, zero call-tree evidence of
//  any), and MSVC's own implicit "stamp both this class's vtable pointers
//  on entry to its own constructor body" codegen already reproduces the
//  shipped two-store, two-symbol pattern with no C++ needed to say so - the
//  same way CPortraitButton::CPortraitButton (GameUI/portraitbutton.cpp)
//  reproduces CButtonFrame's own pair with no explicit store either.
//
//  Only this class's own constructor and its own vtable's slot->address
//  table (docs/targets/vtables/CGlueTextButtonWar3.txt) are in scope for
//  this session - not a full method sweep.  The class's own vtable is 68
//  slots long (dumped, committed) and this file declares none of them
//  `virtual`: they are inherited, mostly-unreconstructed slots belonging to
//  CTextButtonFrame's whole ancestor chain, and modelling all 68 here would
//  mean reconstructing that entire chain's virtual surface, which is a
//  separate, much larger piece of work.  tools/check_vtables.py will
//  therefore report this class as a length mismatch (a handful of slots
//  emitted against 68 expected) until that chain is done - a known,
//  documented gap, not a defect in this constructor.
//
//  Score capped by the unified EH frame; see Frame/framecore.cpp.
//============================================================================
#include "frame.h"

class CGlueTextButtonWar3 : public CTextButtonFrame
{
public:
    CGlueTextButtonWar3(void* parent);      // 0x6F556E20, `retn 4`

    //  +0x1F8 - a pointer, not a plain word: read via the destructor
    //  (0x6F556E60, xref'd off this class's own vtable symbol but not
    //  reconstructed - out of this session's scope), which frees it through
    //  Storm_403 tagged "delete" when it is non-null.  This constructor only
    //  zeroes it.
    void* m_1F8;      // +0x1F8
};

CGlueTextButtonWar3::CGlueTextButtonWar3(void* parent)
    : CTextButtonFrame((int)parent, 0, 0)
{
    m_1F8 = 0;
}

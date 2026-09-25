//============================================================================
//  CEscMenu - the real shipped identity behind `CGameUI+0x228`, which
//  `gameui.h` still declares (deliberately) as `CGameUIPauseButton*
//  m_pPauseButton`.  See that field's own comment and
//  docs/notes/pause-mouse-capture.md for the live vtable-pointer read that
//  found this.
//
//  Confirmed twice over, not just by the live read: `??_7CEscMenu@@6B@` is
//  at 0x6F93D854 (`tools/ida_query.py names CEscMenu`), and its constructor
//  (0x6F34FA20, xref'd off a `mov dword ptr [esi], offset ??_7CEscMenu@@6B@`
//  stamp) is reached from `CGameUI`'s own lazy-init routine (0x6F2F6530) at
//  exactly `[esi+228h]` - the same offset `gameui.h` already had under the
//  old name:
//
//      cmp   dword ptr [esi+228h], 0        ; esi = CGameUI* this
//      ...
//      push  esi                            ; CGameUI* owner, one stack arg
//      mov   ecx, eax                       ; SMemAlloc(0x28, ...) result
//      call  sub_6F34FA20                   ; CEscMenu::CEscMenu(owner)
//
//  `SMemAlloc(0x28, ...)` - **40 bytes** - is the object size, and it lines
//  up exactly with the field layout below with nothing left over: an
//  0x0C-byte `CObserver` base, one reserved dword, and six more dwords
//  (0x10, 0x14, 0x18, 0x1C, 0x20, 0x24) the constructor clears and the
//  destructor releases in exactly the reverse order.  `retn 4` on both the
//  constructor and destructor matches the one stack argument.
//============================================================================
#ifndef ESCMENU_H
#define ESCMENU_H

#include "observer.h"
#include "frame.h"

//  Forward-declared only.  Each is a real, RTTI-confirmed class this session
//  does not reconstruct - see docs/notes/pause-mouse-capture.md:
//    - CEscMenuMainPanel is being reconstructed in parallel by a sibling
//      session (agent-cescmenumainpanel-2026-08-28).
//    - CEscMenuOptionsPanel / CEscMenuSaveGamePanel are unclaimed.
//  All three are refcounted (TRefCnt-derived, same as every other class in
//  this chain) - CEscMenu::~CEscMenu drops a reference on each through the
//  same inlined "decrement, ReleaseSelf() if zero" shape it uses on its own
//  CFrame* and CObserver* fields, which needs nothing more than that they
//  exist as *some* type.
class CEscMenuMainPanel;
class CEscMenuOptionsPanel;
class CEscMenuSaveGamePanel;

//============================================================================
//  CEscMenu : public CObserver
//
//  Vtable @ 0x6F93D854 (??_7CEscMenu@@6B@), 11 slots - see
//  docs/targets/vtables/CEscMenu.txt.  Slots 0, 2, 4, 5 are CObserver's own
//  (0x6F00C060/0x6F62A9A0/0x6F629A90/0x6F62A7B0 - identical addresses to
//  CObserver's own vtable, i.e. not overridden, so they are not
//  re-declared here) - see observer.h.  Slots 1 and 3 are overrides; slots
//  6-10 are new.
//============================================================================
class CEscMenu : public CObserver
{
public:
    //====================================================================
    //  CEscMenu's vtable contribution.  Declaration order *is* vtable
    //  order for the five new slots (6-10) - see CObserver's own header
    //  for why that rule matters.  The two overrides (1, 3) take the slot
    //  their signature already owns in the base regardless of where they
    //  are declared, so they are grouped with their slot's neighbours
    //  above for readability, not because the compiler needs it.
    //====================================================================
    //  slot 1 / +0x04 (0x6F34FD50) - override of CObserver::DeleteSelf.
    //  escmenu_deleteself.cpp.
    virtual CEscMenu* DeleteSelf(int flags);
    //  slot 3 / +0x0C (0x6F34FDC0) - override of CObserver::Method_0x0C.
    //  Forwards to m_pActivePanel's own slot 3 when msg->m_id ==
    //  0x40060064; escmenu_method0c.cpp.
    virtual int Method_0x0C(const SAgentMessage* msg);
    //  slot 6 / +0x18 (0x6F34FC80) - new.  `return 1;`, five instructions.
    //  escmenu_smallslots.cpp.
    virtual int Method_0x18();
    //  slot 7 / +0x1C (0x6F2F1650) - new.  Byte-identical to slot 6's own
    //  body at a second address (same "trivial `return 1`" shape
    //  docs/msvc-vc8-idioms.md already documents for other pairs; the
    //  shipped link has no /OPT:ICF so two functions stay two functions).
    //  escmenu_smallslots.cpp.
    virtual int Method_0x1C();
    //  slot 8 / +0x20 (0x6F2F1660) - new.  Tail-calls `this`'s own slot 6
    //  (Method_0x18) through the vtable - i.e. `return Method_0x18();`.
    //  escmenu_smallslots.cpp.
    virtual int Method_0x20();
    //  slot 9 / +0x24 (0x6F34FDE0) - new, NOT reconstructed (0x134 bytes;
    //  out of this session's time budget - see docs/targets/CEscMenu.md).
    //  Naked redirect, escmenu_bigslots_thunks.cpp.
    virtual int Method_0x24();
    //  slot 10 / +0x28 (0x6F34FF20) - new, NOT reconstructed (0x14D bytes;
    //  same reason as slot 9).  Naked redirect,
    //  escmenu_bigslots_thunks.cpp.
    virtual int Method_0x28();

    //------------------------------------------------------------------
    //  Fields.  CObserver's own base occupies [0x00, 0x0C) - vtable,
    //  refcount, m_resource (observer.h).  Everything from here down is
    //  this class's own, established by SMemAlloc's 0x28-byte allocation
    //  at the one `new CEscMenu(owner)` call site (0x6F2F6530) plus the
    //  constructor's own field clears and the destructor's release order
    //  (reverse of declaration, as always).
    //------------------------------------------------------------------
    //  +0x0C - never touched by the constructor (not even cleared) or by
    //  anything else this session's call trees reach.  Left reserved
    //  rather than named; CLAUDE.md's own rule is not to invent a field
    //  the tree does not read or write.
    char         m_reserved0C[4];

    //  +0x10 - an unnamed CFrame the constructor builds with
    //  `CFrame::Create(owner, 0, 0x1FE)`, sized 0.8 x 0.6 (SetWidth/
    //  SetHeight) and pinned to point 4 of `owner` cast to CLayoutFrame
    //  (`SetPointTo(4, (CLayoutFrame*)owner, 4, 0.0f, 0.0f, 1)`) - a
    //  full-menu backdrop container the three named panels below are all
    //  parented to (by way of a further "EscMenuBackdrop"-named child the
    //  constructor also builds under it but never stores in a `this`
    //  field - see escmenuctor.cpp).  Released last in the destructor,
    //  i.e. declared first, matching every other field below.
    CFrame*      m_pContainer;      // +0x10

    //  +0x14 - null for the whole of the constructor (cleared, never
    //  assigned) but read by Method_0x0C's override (escmenu_method0c.cpp),
    //  which forwards a specific message id to its own vtable slot 3 -
    //  i.e. this is a CObserver-derived "currently active panel" the menu
    //  routes one message kind through, set by some other, un-dumped
    //  method (most likely whichever panel-switch entry point shows one of
    //  the three panels below).  Released by the destructor in the same
    //  refcounted-pointer shape as every other field here, which is what
    //  proves it is a real owned reference and not a raw observer pointer.
    CObserver*   m_pActivePanel;    // +0x14

    //  +0x18 / +0x1C / +0x20 - the three panels the constructor builds by
    //  name under the backdrop container above: "EscMenuMainPanel",
    //  "EscMenuOptionsPanel", "EscMenuSaveGamePanel" (CreateFrameByName,
    //  framereg.h).  Each panel's own back-pointer to this CEscMenu is
    //  stamped by the constructor too, at a *panel-specific* offset
    //  (0x178 on the main panel, 0x170 on the options panel, 0x57C on the
    //  save-game panel) - evidence for whoever reconstructs those classes,
    //  not claimed here.
    CEscMenuMainPanel*      m_pMainPanel;       // +0x18
    CEscMenuOptionsPanel*   m_pOptionsPanel;    // +0x1C
    CEscMenuSaveGamePanel*  m_pSaveGamePanel;   // +0x20

    //  +0x24 - cleared by the constructor, never otherwise touched in any
    //  call tree this session reached (same status as +0x0C, but grouped
    //  with the refcounted block above by the destructor's own release
    //  order, which drops it - as a plain "test/dec/ReleaseSelf" pointer
    //  slot - immediately before +0x20).  Left reserved for the same
    //  reason +0x0C is.
    char         m_reserved24[4];

    //  0x6F34FA20 - `retn 4`.  NOT reconstructed as real source this
    //  session; left as a naked redirect (escmenuctor.cpp) with the full
    //  field-by-field analysis in this header's own top comment and in
    //  docs/targets/CEscMenu.md.  Builds the backdrop container above,
    //  the "EscMenuBackdrop"-named child frame parented to it (0.06 wide,
    //  0.0 tall), and the three named panels above (parented to that
    //  child, each back-pointer-stamped and, for the main and save-game
    //  panels only, handed the child through two more calls this session
    //  does not own - see the header comment on m_pMainPanel/
    //  m_pSaveGamePanel).
    CEscMenu(void* owner);

    //  0x6F34FC90 - `retn 0`.  Reconstructed for real - escmenudtor.cpp.
    //  Releases all five fields above in reverse declaration order, then
    //  the CObserver base runs automatically.  Same
    //  `__except_handler4`-shaped SEH-frame mismatch every other
    //  CObserver-chain constructor/destructor in this repo already
    //  documents (docs/msvc-vc8-idioms.md) - this toolchain cannot
    //  reproduce it, so the body matches instruction for instruction and
    //  the frame furniture does not.
    ~CEscMenu();
};

#endif

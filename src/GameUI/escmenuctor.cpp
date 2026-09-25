//============================================================================
//  0x6F34FA20 - CEscMenu::CEscMenu(void* owner).  See escmenu.h for the
//  field layout this constructor establishes and the allocation-size
//  evidence (0x28 bytes, `SMemAlloc` at the one call site, 0x6F2F6530)
//  that confirms it.
//
//  NOT reconstructed as real source this session - left as a naked
//  redirect, with everything this session worked out about its body
//  recorded here so the next session does not have to re-read the dump
//  (asm/sub_6F34FA20_0x6F34FA20_calltree_asm.md, still in `asm/` -
//  70-function depth-4 walk).  docs/targets/CEscMenu.md has the full
//  write-up; this is the version worth reading with the disassembly open.
//
//  Reasons it stayed a thunk rather than real source, in order of weight:
//
//  1. Its own `__except_handler4`-shaped SEH frame is the same one
//     documented as unreproducible at this toolchain's fixed /GS-
//     /EHs-c- (docs/msvc-vc8-idioms.md) - so even a perfect body match
//     could not score EXACT, only IDENTICAL, the same ceiling
//     CObserver::CObserver/~CObserver already sit at.
//  2. Three of its call sites (below) are on `CEscMenuMainPanel`'s and
//     `CEscMenuSaveGamePanel`'s *own* methods, not CEscMenu's - reaching
//     into unclaimed territory (CEscMenuMainPanel is a sibling session's
//     live target right now) to give them real names risked exactly the
//     "one shipped address, two C++ names" defect
//     docs/notes/thunk-removal-pass.md warns about.
//  3. Two float constants and a `CFrame::Create`/`CreateFrameByName`/
//     `SetPointTo`/`SetWidth`/`SetHeight` sequence per sub-object (five
//     of them) is a lot of exacting, low-risk-of-error-but-high-cost-of-
//     verification work for a third-priority, time-boxed target whose
//     actual ask - the field layout, which resolves the identity
//     question - does not need it.
//
//  What the body does, read cold and cross-checked against every callee
//  this session could grep or dump (CFrame::Create - framecreate.cpp;
//  CLayoutFrame::SetPointTo/SetWidth/SetHeight - framesetpoint.cpp,
//  framesetsize.cpp; CreateFrameByName - framedefcreatename.cpp; the two
//  0x6F93xxxx float constants read with `ida_query bytes`):
//
//      CObserver::CObserver();                    // base ctor first
//      vtable stamp -> ??_7CEscMenu@@6B@
//      m_pContainer = m_pActivePanel = m_pMainPanel
//          = m_pOptionsPanel = m_pSaveGamePanel = 0;
//
//      m_pContainer = CFrame::Create((int)owner, 0, 0x1FE);
//      ((CLayoutFrame*)m_pContainer)->SetPointTo(
//          4, (CLayoutFrame*)owner, 4, 0.0f, 0.0f, 1);
//      ((CLayoutFrame*)m_pContainer)->SetWidth(0.8f);   // flt_6F93A8E8
//      ((CLayoutFrame*)m_pContainer)->SetHeight(0.6f);  // flt_6F93D84C
//      m_pContainer->vtbl[0xD0/4](m_pContainer);        // unidentified
//
//      //  Local only - never stored in `this`.  See m_pContainer's own
//      //  header comment.
//      CFrame* backdrop = CreateFrameByName("EscMenuBackdrop",
//                                            m_pContainer, 0, 0, 0);
//      ((CLayoutFrame*)backdrop)->SetPointTo(
//          4, (CLayoutFrame*)m_pContainer, 4, 0.0f, 0.0f, 1);
//      ((CLayoutFrame*)backdrop)->SetWidth(0.06f);      // flt_6F93D838
//      // SetHeight not called for `backdrop` - only SetPointTo/SetWidth.
//
//      m_pMainPanel = (CEscMenuMainPanel*)
//          CreateFrameByName("EscMenuMainPanel", backdrop, 0, 0, 0);
//      m_pMainPanel->m_field178 /* +0x178 */ = this;
//      MainPanel_0x6F3425C0(m_pMainPanel, backdrop);   // "SetContainer"-
//                                                       // shaped setter,
//                                                       // CEscMenuMainPanel's
//                                                       // own field +0x17C
//      MainPanel_0x6F342D20(m_pMainPanel, 1);          // ~170-instruction,
//                                                       // 8-case switch on
//                                                       // CEscMenuMainPanel's
//                                                       // own fields
//      m_pMainPanel->vtbl[0xD0/4](m_pMainPanel);
//
//      m_pOptionsPanel = (CEscMenuOptionsPanel*)
//          CreateFrameByName("EscMenuOptionsPanel", backdrop, 0, 0, 0);
//      m_pOptionsPanel->m_field170 /* +0x170 */ = this;  // no
//          // SetContainer-shaped call for this panel - goes straight to:
//      m_pOptionsPanel->vtbl[0xD0/4](m_pOptionsPanel);
//
//      m_pSaveGamePanel = (CEscMenuSaveGamePanel*)
//          CreateFrameByName("EscMenuSaveGamePanel", backdrop, 0, 0, 0);
//      m_pSaveGamePanel->m_field57C /* +0x57C */ = this;
//      SaveGamePanel_0x6F342650(m_pSaveGamePanel, backdrop);
//      m_pSaveGamePanel->vtbl[0xD0/4](m_pSaveGamePanel);
//
//      return this;
//
//  Every "AddRef new / release old" the real dump does around each of the
//  five assignments above is the same inlined shape escmenudtor.cpp's own
//  `ReleaseRefSlot` mirrors for release; the increment half is the same
//  test-then-`add [obj+4],1` "AddRefOrNull"-shaped inline, not a call.
//
//  vtbl[0xD0/4] (slot 0x34) is called on every one of the four created
//  objects (container, main, options, save-game) right after each is
//  wired up - almost certainly a generic CFrame/CWidget "finalize
//  construction" or "layout now" virtual; not identified further.
//============================================================================
#include "escmenu.h"

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK(CEscMenu::CEscMenu(void* owner), 0x6F34FA20)

//============================================================================
//  0x6F629170 - CCursorFrame::CCursorFrame.
//
//  A CSpriteFrame with nothing of its own but two flag bits and a "cover my
//  owner exactly" anchor.  Both vtable operands are symbols on both sides,
//  because CCursorFrame really does derive from a polymorphic base and MSVC
//  stamps `??_7CCursorFrame@@6B@` at +0 and `??_7CCursorFrame@@6B@_0` at
//  +0xB4 for no source at all.
//
//  `owner` is a CFrame* and SetAllPoints takes a CLayoutFrame*, which is
//  CFrame's *second* base at +0xB4 - so the implicit upcast is what emits
//  the shipped `test edi,edi` / `lea eax,[edi+0B4h]` / `jmp` / `xor eax,eax`
//  null-checked adjustment.  Writing the cast by hand would lose it.
//
//  Score capped by the unified `__except_handler4`-shaped EH frame this
//  toolchain cannot reproduce - eleven instructions of prologue and epilogue
//  plus the one state store, all of which /GS- /EHs-c- emits nothing for.
//  See framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"
#include "cursorframe.h"

CCursorFrame::CCursorFrame(CFrame* owner, int a, int b)
    : CSpriteFrame((int)owner, 0, 0x2710)
{
    m_flagMask |= 2;
    m_flags |= 2;

    CLayoutFrame::SetAllPoints(owner, 1);
}

//============================================================================
//  0x6F2F2990 - CGameUI::CreateChatBar.
//
//  The frame library's own factory shape (Frame/framecreate.cpp): 0x1EC
//  bytes from Storm tagged with the module's `__FILE__`/`__LINE__` pair, and
//  the object constructed into them, or null.  The null test in front of the
//  constructor call is placement-new's own; it is spelled out here because
//  the constructor itself is still a redirect and a redirect cannot be a
//  constructor (see GameUI/gameui.h).
//
//  0x1EC is exactly what GameUI/gameui.h models the chat bar as - the last
//  field is the colon-message target at +0x1E8 - which is one more thing
//  that says the two are the same class.
//
//  A __fastcall member: `this` in ecx and the owner in edx, with only the
//  FDF frame id pushed (`retn 4`).  Its one call site is CGameUI's own
//  constructor at 0x6F2FF6D3, which passes a null owner and frame id
//  0x2422.
//
//  The allocation's `__FILE__` tag is truncated in the dump
//  ("e:\\drive1\\temp\\buildwar3x\\war3\\sou"...), so the string below is
//  the visible prefix completed by convention - the file it belongs to is
//  the chat bar's, whose constructor is at 0x6F367FF0.  It is a Storm
//  leak-log tag, nothing reads it, and both sides canonicalise the operand
//  to an anonymous symbol either way.
//
//  The shipped body carries the `__except_handler4`-shaped frame this
//  toolchain cannot reproduce at its fixed /GS- /EHs-c- (the `push -1`,
//  the scope table, the cookie load and `xor esp`, the unwind-state stores
//  and the appended funclet); see docs/msvc-vc8-idioms.md.
//============================================================================
#include "gameui.h"
#include "storm.h"

static const char kChatBarFile[] =
    "e:\\drive1\\temp\\buildwar3x\\war3\\source\\CSimpleMessageFrame.cpp";

SSelectionHost* __fastcall CGameUI::CreateChatBar(void* owner, int frameId)
{
    SSelectionHost* bar =
        (SSelectionHost*)SMemAlloc(0x1EC, kChatBarFile, 0x46, 0);

    return bar ? new (bar) SSelectionHost(this, owner, frameId) : 0;
}

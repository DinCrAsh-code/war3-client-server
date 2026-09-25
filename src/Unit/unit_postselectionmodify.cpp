//============================================================================
//  0x6F2CC3D0 - reached from CUnit's slot107 closure (per-template
//  initialiser, 0x6F2A0E30).  Build a CNetCommandSelectableSelectionModify
//  (Net/netcommand_classes_multifield.h) with a fixed ctx (2) and sender
//  (0), then fill its three own fields from the caller's own `flag`
//  register argument and two raw fields off `this` (+0xC/+0x10, not
//  otherwise named - nothing else in this call tree reads them), and hand
//  it to a not-yet-reconstructed poster (0x6F2CA0D0, its own ~90-instruction
//  body below this slice's own BFS cut).
//
//  `this` arrives in ecx and `flag` in edx with a bare `retn` (no stack
//  arguments at all) - the same "thiscall plus one register-only extra
//  argument" shape this codebase already writes as a `__fastcall` free
//  function taking the receiver as its own first argument (InitWidgetAgentQuery,
//  widgetagentquery.h) rather than a real member, since a plain __thiscall
//  member has no way to receive a second argument in edx.
//
//  The three field overwrites after construction look redundant next to
//  the converting constructor's own defaults (m_field18=0, m_field1C=-1,
//  m_field20=-1) - the same "always-constant fields ahead of the two
//  runtime-value stores" dead-store-elimination shape
//  netcommand_classes_multifield.h's own CNetCommandBase constructor
//  comment already documents for this same class family, expected to fold
//  away under /O2 the same way.
//============================================================================
#include "unit.h"
#include "netcommand_classes_multifield.h"

//  0x6F2CA0D0 - not reconstructed (below this slice's own BFS cut, its own
//  ~90-instruction body reaching three further undumped helpers): ecx =
//  &cmd, edx = 0 at this call site.  A naked redirect rather than a bare
//  declaration - link_check.py's own "UNEXPECTED unresolved externals"
//  bucket is exactly for catching a declaration with nothing defining it
//  anywhere on the real link line, which this was before it became a
//  thunk.
__declspec(naked) void __fastcall PostSelectableSelectionModifyCommand(
    CNetCommandSelectableSelectionModify* cmd, int flag)
{
    __asm
    {
        mov     eax, 06F2CA0D0h
        jmp     eax
    }
}

void __fastcall CUnit_PostSelectableSelectionModify(CUnit* self, unsigned char flag)
{
    CNetCommandSelectableSelectionModify cmd(2, 0);
    cmd.m_field18 = flag;
    cmd.m_field1C = *(int*)((char*)self + 0x0C);
    cmd.m_field20 = *(int*)((char*)self + 0x10);

    PostSelectableSelectionModifyCommand(&cmd, 0);
}

//============================================================================
//  CEscMenu's two remaining new vtable slots - 9 (+0x24, 0x6F34FDE0) and
//  10 (+0x28, 0x6F34FF20).  See escmenu.h.
//
//  NOT reconstructed: 0x134 and 0x14D bytes respectively (both well past
//  the small-leaf slots reconstructed alongside them - Method_0x18/0x1C/
//  0x20 are three to seven instructions each), outside this session's time
//  budget for a third-priority target whose ask was the field layout and
//  constructor, not full vtable coverage.  Left as naked redirects to the
//  real, unhooked shipped bodies so the tree still links and scores; a dump
//  for neither has been pulled into `asm/` yet (unlike the constructor -
//  see escmenuctor.cpp), so a future session needs an `ida_query
//  calltree_asm` pass before reconstructing either.
//
//  Both `retn 0` - read with `tools/ida_query.py bytes` off the last few
//  bytes of each function's own range (`func_at` gave the range; CLAUDE.md's
//  own recipe for settling a `retn` count without guessing), not inferred
//  from the declaration.
//============================================================================
#include "escmenu.h"

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK(int CEscMenu::Method_0x24(), 0x6F34FDE0)
ADDR_THUNK(int CEscMenu::Method_0x28(), 0x6F34FF20)

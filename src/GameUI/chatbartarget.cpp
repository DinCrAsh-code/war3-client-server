//============================================================================
//  0x6F3412D0 - GetLocalChatTarget: which chat target the local player's own
//  console is set to, in the numbering SSelectionHost::SetColonMessage
//  switches on.
//
//    0        everyone
//    1        allies
//    2        observers / referees
//    3 + slot a private message to one player
//
//  With no console, or a console with no live record, the answer is 0 -
//  "everyone" - which is also the switch's own default and its case 3.
//
//  **The shipped body is in two pieces.**  Everything from the `[+0x1A0]`
//  load onward sits at 0x6F3367C0, 0x2F4F0 bytes away, reached by a `jmp`
//  out of the middle of the function; IDA carries it as a tail chunk of this
//  same function rather than as a function of its own.  That is the shipped
//  build's whole-program block placement, not a call, and no source shape
//  reproduces it: this file writes the one function it is, and the resulting
//  stream is the same instructions with the early-out block on the other
//  side of the switch.  See the BEHAVIOUR entry.
//============================================================================
#include "gameui.h"

int GetLocalChatTarget()
{
    SChatConsole* console = ((CGameUI*)GetGameUI(1, 0))->m_pChatConsole;
    SChatTarget*  target  = console->m_pTarget;

    if (target == 0)
        return 0;

    //  Case order is the shipped block order, which is what MSVC lays the
    //  arms out in.
    switch (target->m_kind)
    {
    case 1:
        return 1;
    case 2:
        return 2;
    case 0:
        //  A private message: the slot, biased past the three broadcast
        //  targets.  0xFFFFFFFF is "nobody selected", which falls back to
        //  everyone.
        return (target->m_slot == -1) ? 0 : target->m_slot + 3;
    case 3:
        return 0;
    default:
        return 0;
    }
}

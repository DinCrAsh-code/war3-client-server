//============================================================================
//  0x6F2AD770 - CWidget's vtable slot 61 (+0xF4).  See widget.h.
//
//  Decide whether the sprite should be shown and push the answer through
//  CWar3Image::SetSpriteVisible (widget_spritevisible.cpp).  Two things about the
//  shape are load-bearing:
//
//   * `visible` is computed branchlessly as
//     `(m_reserved24 & 0x80000000) != 0` - the shipped
//     `xor/cmp/sbb/neg` sequence, not a compare-and-branch;
//   * the two CWar3Image::SetSpriteVisible call sites are two separate calls with two
//     separate `retn` tails, differing only in the `force` argument, which
//     is what an if/else over two whole statements gives and a ternary on
//     the argument does not.
//
//  Own translation unit: three real calls to reproduce, one of them the
//  predicate in widget_visibleflags.cpp.
//============================================================================
#include "widget.h"
#include "playercolor.h"

int __fastcall IsGameModeOne();

void CWidget::RefreshSpriteVisibility()
{
    //  One boolean expression, materialised at a join (`mov eax,1` / `jmp`
    //  / `xor eax,eax`), not a flag accumulated across an if: written the
    //  second way MSVC keeps the flag in a callee-saved register across
    //  both calls and the `push edi` that comes with it is three
    //  instructions the shipped code does not have.
    //
    //  The `IsGameModeOne() && world->...` tail repeats
    //  IsVisibleToLocalPlayer's own (widget_visibleflags.cpp) rather than
    //  being factored out - the shipped code carries both copies.
    int shown;
    if ((m_flags & 0x20) == 0
        && IsVisibleToLocalPlayer() != 0
        && (IsGameModeOne() == 0
            || (*((const unsigned char*)
                    *(void**)((char*)g_unk6FAB65F4 + 0x34) + 0x24) & 1) == 0))
        shown = 1;
    else
        shown = 0;

    //  `0u < (x & mask)` and not `(x & mask) != 0`: the shipped code has
    //  the `xor`/`cmp`/`sbb`/`neg` carry trick, and `!= 0` on a
    //  single-bit-masked unsigned folds all the way down to `shr eax,31`
    //  instead.
    int visible = (0u < (m_reserved24 & 0x80000000u));
    if (visible != 0 || shown != 0)
        CWar3Image::SetSpriteVisible(1, visible);
    else
        CWar3Image::SetSpriteVisible(0, visible);
}

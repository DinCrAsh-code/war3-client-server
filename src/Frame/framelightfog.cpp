//============================================================================
//  CFrame's own slot 13 (+0x34) - a real, shared body (same address on a
//  fresh CSimpleTop and CWorldFrameWar3 pull): if this frame owns any fog
//  (m_144's own list, despite its generic field name - AddFog's own comment
//  already ties it to "+0x148", inside this same TSList) or any lights
//  (m_lights), and bit 2 of m_124 is *not* set, re-notify every matching
//  fog/light node.
//
//  The two node walks themselves (0x6F607DA0 for fog, 0x6F608020 for
//  lights) are real too - both walk their own TSList tail-to-head exactly
//  the way TSList<T,0>::Tail()/Prev() already do it (storm.h) - but the
//  call each makes for a matching node (0x6F4EA040) jumps into that node's
//  own slot 4 (Method_0x10) without first pushing the message argument
//  Method_0x10 expects, reusing whatever the *caller's own* frame already
//  left in that stack slot.  What real value ends up there depends on who
//  calls slot 13 in turn, which this closure does not reach - so both
//  walks are left naked thunks rather than a guessed argument.
//============================================================================
#include "frame.h"

#define FRAME_LIGHTFOG_THUNK(addr)   \
    {                                \
        __asm { mov eax, addr }     \
        __asm { jmp eax }           \
    }

//  0x6F606EC0 - "does m_124 have every bit of `mask` set": `(m_124 & mask)
//  == mask`, not a simple single-bit test.  Slot 13 below is this closure's
//  only caller (mask = 4).
int CFrame::HasAllFlagBits(unsigned int mask)
{
    return (m_124 & mask) == mask;
}

//  0x6F607DA0 / 0x6F608020 - see this file's own header comment for why
//  both stay naked thunks.
__declspec(naked) void CFrame::NotifyMatchingFogNodes(unsigned int, unsigned int) FRAME_LIGHTFOG_THUNK(0x6F607DA0)
__declspec(naked) void CFrame::NotifyMatchingLightNodes(unsigned int, unsigned int) FRAME_LIGHTFOG_THUNK(0x6F608020)

void CFrame::Method_0x34()
{
    //  +0x158 / +0x14C are both inside a TSList's own body (m_lights' and
    //  m_144's own "tail" word respectively) - non-zero (and positive, the
    //  same `> 0` TSList::Tail() itself tests) means "not empty".  Written
    //  as the shipped body's own nested-if shape (test the first list;
    //  only load the second, in its own nested block, when the first
    //  answered empty) rather than one `||` - docs/msvc-vc8-idioms.md's
    //  "Two `if`s share one return block; one `||` does not" entry, mirror
    //  case: here it is the *work*, not the early return, that the two
    //  branches share.
    if (*(int*)((char*)this + 0x158) <= 0)
    {
        if (*(int*)((char*)this + 0x14C) <= 0)
            return;
    }

    if (!HasAllFlagBits(4))
    {
        NotifyMatchingFogNodes(0, 0);
        NotifyMatchingLightNodes(0, 0);
    }
}

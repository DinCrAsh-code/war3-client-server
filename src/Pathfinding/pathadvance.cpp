//============================================================================
//  0x6F49E020 - CPathClient::Advance: the per-tick entry point. A route
//  already marked valid just burns down its cooldown; otherwise drop a
//  stale target node, try the straight-line shortcut, and fall back to a
//  full search.
//============================================================================
#include "pathmove.h"

int CPathClient::Advance(const CGridVec2* to, CGridVec2* out, CPathTrace* mover)
{
    //  Not `return 1`: the shipped code never sets eax on this path at all
    //  - it loads `eax = kPathFlagValid` (0x100000) to use as the `test`
    //  mask, and when the flag is already set it jumps straight past every
    //  other return-value store to the epilogue with that mask value still
    //  sitting in eax. angr confirmed this by finding the return value
    //  itself differs (`eax differs ... obj_088=0x100000`) for an object
    //  whose +0x88 flags word is exactly kPathFlagValid - this build's own
    //  literal `return 1` was the bug, not a scheduling artifact. Callers
    //  only ever test the result for zero/nonzero (a full search happened
    //  or not), so this was never a behavioural bug for any caller, only a
    //  literal mismatch - reproduced here anyway since it is what the
    //  shipped code actually does.
    if (m_flags & kPathFlagValid)
        return kPathFlagValid;

    if (m_cooldown > 0)
    {
        m_cooldown -= 1;
        return 1;
    }

    StageSearch(mover);

    if (m_targetNode != 0 && m_targetNode->m_visitStamp == -1)
        m_targetNode = 0;

    int shortcut = TryStraightLine(to);
    if (shortcut != 0)
        return shortcut;

    return RunSearch(to, out);
}

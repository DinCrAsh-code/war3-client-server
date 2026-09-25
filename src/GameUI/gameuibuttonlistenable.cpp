//============================================================================
//  0x6F3470E0 - CGameUIButtonList::SetEnabled.
//
//  A counted array of eight-byte records at +0x130 whose frame is the second
//  word of each; the list's own "suppressed" flag at +0x144 overrides the
//  caller.  The count is loaded once, before the loop, and the guard is
//  `<= 0` on a signed value, which is what an `int` count and a `for` over
//  it give.
//
//  Own translation unit: 0x6F602FE0 is a real call into the frame framework
//  and 0x6F352F00, its only caller, is a different shipped module.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"

namespace
{
    struct SButtonListEntry
    {
        char          m_reserved00[0x04];
        CSimpleButton* m_pFrame;        // +0x04
    };
}

void CGameUIButtonList::SetEnabled(int enabled)
{
    if (m_enabled == enabled)
        return;

    int count = m_count;

    for (int i = 0; i < count; ++i)
    {
        ((SButtonListEntry*)m_pEntries)[i].m_pFrame
            ->SetEnabled((enabled != 0 && m_suppressed == 0) ? 1 : 0);
    }

    m_enabled = enabled;
}

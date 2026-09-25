//============================================================================
//  0x6F342190 - CGameUICommandCard::SetEnabled.
//
//  Twelve buttons, addressed as three rows of four: the inner counter steps
//  0x10 bytes at a time up to 0x30 (three records) and the outer one four
//  bytes at a time up to 0x10 (four buttons per record), and the frame is
//  `m_pRows[row].m_pButtons[button]`.  The two `add`/`cmp` pairs are MSVC's
//  strength reduction of those two subscripts, not two byte counters in the
//  source.
//
//  Only the transition *into* the disabled state does anything: the flag is
//  written first and the loop runs only when it went to zero, which is why
//  every call in it pushes a literal 0.
//
//  Own translation unit: 0x6F602FE0 is a real call into the frame framework.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"

namespace
{
    //  One record of the +0x154 table: a pointer to this row's four button
    //  frames at +0x08.  Everything ahead of it is untouched here.
    struct SCommandCardRow
    {
        char           m_reserved00[0x08];
        CSimpleButton** m_pButtons;     // +0x08
        char           m_reserved0C[0x04];
    };
}

enum { kCommandCardRows = 3, kCommandCardColumns = 4 };

void CGameUICommandCard::SetEnabled(int enabled)
{
    if (m_enabled != enabled)
    {
        m_enabled = enabled;

        if (enabled == 0)
        {
            for (unsigned int button = 0; button < kCommandCardColumns; ++button)
            {
                for (unsigned int row = 0; row < kCommandCardRows; ++row)
                    ((SCommandCardRow*)m_pRows)[row].m_pButtons[button]
                        ->SetEnabled(0);
            }
        }
    }
}

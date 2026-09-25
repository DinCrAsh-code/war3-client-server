//============================================================================
//  0x6F3463A0 - CGameUIHeroBar::SetEnabled.
//
//  Seven frames off the same +0x154 table the command card uses, but each
//  one votes on its own state: the frame's owner at +0x1C4 carries a flag
//  (bit 0x100 of its +0x5C) that means "this slot is busy", and a busy slot
//  stays disabled even while the bar as a whole is being enabled.
//
//  **`SHeroBarSlot` is 0x10 bytes, not 0x0C.** The dump's own loop proves it
//  - `add esi, 10h` / `cmp esi, 70h` (0x70 / 0x10 = the 7 slots), not 0x0C -
//  and this file had it wrong: `m_reserved00[8]` plus one 4-byte `m_ppFrame`
//  sums to 0x0C, which is a real, silent bug rather than a cosmetic one.
//  Slot 0's `m_ppFrame` read is unaffected (`0 * stride` is 0 either way),
//  but every slot after it reads 4 bytes short of the real one, and the
//  garbage that lands in `m_ppFrame` is not reliably null - live, it was
//  the address `0x1`, and `frame = *m_ppFrame` (line below) faulted
//  dereferencing that.  Confirmed live: hooking `CGameUI::SetGamePaused`
//  alone (whose closure reaches this function through `m_pHeroBar->
//  SetEnabled`) crashed immediately on pause/menu-open with exactly this
//  fault.  There is no null check on `frame` before `frame->m_pOwner`
//  because the shipped dump has none either - not a gap this
//  reconstruction added, and not fixable by adding one: the shipped
//  invariant is that all 7 slot frames are always real, pre-allocated
//  objects, which only holds once the stride is the real 0x10.
//
//  Both votes are written as ternaries and not as `int x = 0; if (...) x =
//  1;`: the shipped code sets 1 on one edge and jumps over an `xor eax, eax`
//  on the other, which is what a ternary gives and what the
//  initialise-then-overwrite spelling does not (docs/msvc-vc8-idioms.md,
//  "An explicit `xor eax,eax` says which branch was written second").
//
//  Own translation unit: 0x6F602FE0 is a real call into the frame framework.
//============================================================================
#include "gameui.h"
#include "gameuithunks.h"

namespace
{
    struct SHeroBarOwner
    {
        char          m_reserved00[0x5C];
        unsigned int  m_flags;          // +0x5C, bit 0x100 = busy
    };

    struct SHeroBarFrame
    {
        char           m_reserved000[0x1C4];
        SHeroBarOwner* m_pOwner;        // +0x1C4
    };

    struct SHeroBarSlot
    {
        char            m_reserved00[0x08];
        SHeroBarFrame** m_ppFrame;      // +0x08
        char            m_reserved0C[0x04];
    };
}

enum { kHeroBarSlots = 7, kHeroBarBusyFlag = 0x100 };

void CGameUIHeroBar::SetEnabled(int enabled)
{
    if (m_enabled == enabled)
        return;

    for (int slot = 0; slot < kHeroBarSlots; ++slot)
    {
        SHeroBarFrame* frame = *((SHeroBarSlot*)m_pSlots)[slot].m_ppFrame;
        SHeroBarOwner* owner = frame->m_pOwner;

        int busy = (owner != 0 && (owner->m_flags & kHeroBarBusyFlag) != 0)
                       ? 1 : 0;

        ((CSimpleButton*)frame)
            ->SetEnabled((enabled != 0 && busy == 0) ? 1 : 0);
    }

    m_enabled = enabled;
}

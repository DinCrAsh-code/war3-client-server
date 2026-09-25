//============================================================================
//  0x6F39A1E0 - CWorldFrameWar3::AddSelectedUnitIfNotAlready.
//
//  Reached from SPlayerTableSubmitParts::RefreshUnitFog
//  (Widget/playertableunitfogrefresh.cpp, 0x6F40A650) through
//  `GetGameUI(1,0)->m_pWorldFrameWar3`.  Own dump:
//  asm/sub_6F40A650_0x6F40A650_calltree_asm.md.
//
//  If `unit`'s own flag 0x80000 is clear, append `unit` to
//  `m_selectedUnits` (+0x600, TSGrowableArray<SCheckedUnitSlot>, gameui.h)
//  via New()->Assign(unit) unconditionally.  If the flag is set, the same
//  append only happens when CUnit::QuerySpecialSelectionMask() is
//  *non-zero* - a zero mask with the flag set is the one case that skips
//  the append entirely.  (An earlier reading of this had the sense of the
//  mask test backwards - "non-zero mask means already covered, skip" -
//  which compiled to the same `je`/`jne` shape either way until checked
//  against the dump's own branch targets: `jz` here lands on the
//  *epilogue*, not on the append, so a zero mask is what skips.)
//
//  Own translation unit: a different shipped module (0x6F39Axxx) from
//  RefreshUnitFog's (0x6F409Exx/0x6F40Axxx), per CLAUDE.md's
//  one-TU-per-module rule.
//============================================================================
#include "gameui.h"
#include "unit.h"

void CWorldFrameWar3::AddSelectedUnitIfNotAlready(CUnit* unit)
{
    if ((unit->m_flags5C & 0x80000) != 0)
    {
        if (unit->QuerySpecialSelectionMask() == 0)
            return;
    }

    m_selectedUnits.New()->Assign(unit);
}

//============================================================================
//  0x6F40B300 / 0x6F40B260 / 0x6F40B280 - the three score fields that reach
//  the tally list through a real call.
//
//  Each is two or three instructions: load the list pointer at +0x2D0 and
//  tail-jump into the member that does the work.  That comes out as a `jmp`
//  only because CPlayerTallyList's own bodies live in another translation
//  unit (playerroster.cpp) - visible, /Ob2 would inline all three walks
//  here and there would be no call left to match.
//
//  0x6F40B280 is the odd one: it returns a `CFloat` **by value**, so its
//  own hidden return buffer is handed straight on as the callee's, and the
//  upgrade id at +0x260 is read *before* the list pointer because an
//  argument is evaluated before the object expression.
//============================================================================
#include "playerscore.h"

int SPlayerScore::TotalTalliedCost()
{
    return m_pTallies->TotalTalliedCost();
}

int SPlayerScore::CountFlag4WithCost()
{
    return m_pTallies->CountFlag4WithCost();
}

CFloat SPlayerScore::GetUpgradeRatio()
{
    return m_pTallies->UpgradeRatio(m_upgradeId260);
}

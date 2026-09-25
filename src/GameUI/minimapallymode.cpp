//============================================================================
//  0x6F2F3F50 - CGameUI::AdvanceMiniMapAllyMode.
//
//  The ally button's three-way cycle, which is what
//  RefreshMiniMapAllyButtonSkin then re-skins: preference 0x38 goes 0, 1, 2,
//  0, ... and a `step` of zero re-stores and re-applies the value it already
//  had rather than doing nothing.
//
//  The wrap is a *bounds* test and not a modulo: anything above 2 becomes 0,
//  which also repairs a preferences file that has a garbage value in that
//  key.  The `step` is normalised to 0 or 1 before it is added, so any
//  non-zero argument advances by exactly one.
//
//  `this` is never read, but the shipped call site (0x6F300079) still loads
//  it into ecx, so this is a member with an unused receiver and not a free
//  function - declaring it free would leave its one caller pushing a `this`
//  nothing cleans.
//
//  Always returns 1; no caller in this tree reads it.
//============================================================================
#include "gameui.h"
#include "preferences.h"

int CGameUI::AdvanceMiniMapAllyMode(int step)
{
    CPreferencesWar3* prefs = GetPreferences();

    int mode;
    prefs->GetIntValue(0x38, &mode);

    unsigned int next = (unsigned int)(mode + (step != 0 ? 1 : 0));
    if (next > 2)
        next = 0;

    prefs->StoreValue(0x38, (int)next);
    prefs->ApplyPreference(0x38, (int)next);
    return 1;
}

//============================================================================
//  0x6F2F3160 - CGameUI::RefreshMiniMapFormationButtonSkin.
//
//  The same two-family shape as the terrain and creep buttons, with the flag
//  coming out of the preferences file (key 0x2C) instead of off the minimap,
//  and with the "On" family's middle key spelled
//  "MiniMapFormationOnButtonEnabledPushed" rather than "...ButtonPushed" -
//  which is the shipped string and not a typo of this reconstruction's.
//
//  Own translation unit: the preference read is a real call.
//============================================================================
#include "gameui.h"
#include "frame.h"
#include "preferences.h"

void CGameUI::RefreshMiniMapFormationButtonSkin()
{
    int on;
    //  The preferences object is fetched into its own local because the
    //  shipped code calls for it *before* it pushes GetIntValue's two
    //  arguments; folded into the call it is scheduled after them.
    CPreferencesWar3* prefs = GetPreferences();
    prefs->GetIntValue(0x2C, &on);

    const char* disabledKey;

    if (on != 0)
    {
        m_pMiniMapFormationButton->SetSlotTexture(
            1, ResolveSkinValue("MiniMapFormationOnButtonEnabled", 0));
        m_pMiniMapFormationButton->SetSlotTexture(
            2, ResolveSkinValue("MiniMapFormationOnButtonEnabledPushed", 0));
        disabledKey = "MiniMapFormationOnButtonDisabled";
    }
    else
    {
        m_pMiniMapFormationButton->SetSlotTexture(
            1, ResolveSkinValue("MiniMapFormationOffButtonEnabled", 0));
        m_pMiniMapFormationButton->SetSlotTexture(
            2, ResolveSkinValue("MiniMapFormationOffButtonEnabledPushed", 0));
        disabledKey = "MiniMapFormationOffButtonDisabled";
    }

    m_pMiniMapFormationButton->SetSlotTexture(
        0, ResolveSkinValue(disabledKey, 0));
}

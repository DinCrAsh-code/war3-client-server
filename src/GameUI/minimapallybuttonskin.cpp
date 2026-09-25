//============================================================================
//  0x6F2F3000 - CGameUI::RefreshMiniMapAllyButtonSkin.
//
//  The ally button has three states, not two, and its state is a stored
//  preference (key 0x38) rather than a field: 1 is "inactive", 2 is
//  "active", and anything else - including the 0 the preference is born
//  with - is "off".
//
//  All three skins are resolved *before* any of them is applied, which the
//  shipped code makes plain by holding the first two in callee-saved
//  registers across the third lookup and only then calling the button three
//  times.  The other three buttons interleave lookup and apply; this one
//  does not, and writing it the interleaved way costs the two extra
//  registers their reason to exist.
//
//  Own translation unit: the preference read is a real call and so are the
//  three lookups.
//============================================================================
#include "gameui.h"
#include "frame.h"
#include "preferences.h"

void CGameUI::RefreshMiniMapAllyButtonSkin()
{
    int mode;
    //  The preferences object is fetched into its own local because the
    //  shipped code calls for it *before* it pushes GetIntValue's two
    //  arguments; folded into the call it is scheduled after them.
    CPreferencesWar3* prefs = GetPreferences();
    prefs->GetIntValue(0x38, &mode);

    const char* enabled;
    const char* pushed;
    const char* disabled;

    switch (mode)
    {
    case 1:
        enabled  = ResolveSkinValue("MiniMapAllyButtonInactiveEnabled", 0);
        pushed   = ResolveSkinValue("MiniMapAllyButtonInactivePushed", 0);
        disabled = ResolveSkinValue("MiniMapAllyButtonInactiveDisabled", 0);
        break;
    case 2:
        enabled  = ResolveSkinValue("MiniMapAllyButtonActiveEnabled", 0);
        pushed   = ResolveSkinValue("MiniMapAllyButtonActivePushed", 0);
        disabled = ResolveSkinValue("MiniMapAllyButtonActiveDisabled", 0);
        break;
    default:
        enabled  = ResolveSkinValue("MiniMapAllyButtonOffEnabled", 0);
        pushed   = ResolveSkinValue("MiniMapAllyButtonOffPushed", 0);
        disabled = ResolveSkinValue("MiniMapAllyButtonOffDisabled", 0);
        break;
    }

    m_pMiniMapAllyButton->SetSlotTexture(1, enabled);
    m_pMiniMapAllyButton->SetSlotTexture(2, pushed);
    m_pMiniMapAllyButton->SetSlotTexture(0, disabled);
}

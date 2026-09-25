//============================================================================
//  0x6F2F2F60 and 0x6F2F30C0 - the two minimap toggle buttons whose state
//  lives on the minimap object rather than in the preferences file.
//
//  Both are the same seventeen-line shape: pick the "Active" or the
//  "Inactive" skin family from the flag, resolve all three of that family's
//  keys through the active skin, and hand each one to the button's matching
//  texture slot.  The slots are written 1, 2, 0 - enabled, pushed, disabled -
//  and the *disabled* one is shared: the shipped code jumps both arms into
//  one trailing ResolveSkinValue / SetSlotTexture pair with only the key
//  string differing, which is why the third lookup is written after the `if`
//  and not inside either arm.
//
//  The flags are CMinimap's own display toggles at +0x62C and +0x630, which
//  GameUI/minimaprefresh.cpp already names (SetShowTerrain / SetShowCreeps),
//  and this file's own branch on them is the evidence that named them.
//
//  Two functions in one translation unit: neither calls the other, they are
//  0x160 bytes apart in the shipped image, and nothing here can be inlined
//  into anything.
//============================================================================
#include "gameui.h"
#include "frame.h"

//  CMinimap, re-declared against the two flags this file reads for the
//  reason GameUI/minimaprefresh.cpp gives for declaring it locally: a local
//  class declaration mangles identically, and no member is called on it
//  here at all.
class CMinimap
{
public:
    char m_reserved000[0x62C];
    int  m_showTerrain;         // +0x62C
    int  m_showCreeps;          // +0x630
};

void CGameUI::RefreshMiniMapTerrainButtonSkin()
{
    const char* disabledKey;

    if (((CMinimap*)m_pMinimapObject)->m_showTerrain != 0)
    {
        m_pMiniMapTerrainButton->SetSlotTexture(
            1, ResolveSkinValue("MiniMapTerrainButtonActiveEnabled", 0));
        m_pMiniMapTerrainButton->SetSlotTexture(
            2, ResolveSkinValue("MiniMapTerrainButtonActivePushed", 0));
        disabledKey = "MiniMapTerrainButtonActiveDisabled";
    }
    else
    {
        m_pMiniMapTerrainButton->SetSlotTexture(
            1, ResolveSkinValue("MiniMapTerrainButtonInactiveEnabled", 0));
        m_pMiniMapTerrainButton->SetSlotTexture(
            2, ResolveSkinValue("MiniMapTerrainButtonInactivePushed", 0));
        disabledKey = "MiniMapTerrainButtonInactiveDisabled";
    }

    m_pMiniMapTerrainButton->SetSlotTexture(0, ResolveSkinValue(disabledKey,
                                                               0));
}

void CGameUI::RefreshMiniMapCreepButtonSkin()
{
    const char* disabledKey;

    if (((CMinimap*)m_pMinimapObject)->m_showCreeps != 0)
    {
        m_pMiniMapCreepButton->SetSlotTexture(
            1, ResolveSkinValue("MiniMapCreepButtonActiveEnabled", 0));
        m_pMiniMapCreepButton->SetSlotTexture(
            2, ResolveSkinValue("MiniMapCreepButtonActivePushed", 0));
        disabledKey = "MiniMapCreepButtonActiveDisabled";
    }
    else
    {
        m_pMiniMapCreepButton->SetSlotTexture(
            1, ResolveSkinValue("MiniMapCreepButtonInactiveEnabled", 0));
        m_pMiniMapCreepButton->SetSlotTexture(
            2, ResolveSkinValue("MiniMapCreepButtonInactivePushed", 0));
        disabledKey = "MiniMapCreepButtonInactiveDisabled";
    }

    m_pMiniMapCreepButton->SetSlotTexture(0, ResolveSkinValue(disabledKey, 0));
}

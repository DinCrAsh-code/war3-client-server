//============================================================================
//  0x6F31F530 - ResolveSkinValue.
//
//  A skin lookup with three sources tried in order: the map's own
//  "CustomSkin" override, the named (or currently active) skin's section,
//  and finally the "Default" section.  The answer accumulates in one local
//  that both of the last two paths write and that the shared exit returns,
//  which is why `xor esi, esi` is hoisted above the very first test.
//
//  Own translation unit: three real calls out of it, one of them
//  (0x6F31F4C0) in this same shipped module.
//============================================================================
#include "gameuiskin.h"
#include "gameuithunks.h"
#include "CMiscCustom.h"   // CMiscCustom::GetString (0x6F002400)
#include "CMiscCustom.h"        // 0x6F002170 - CMiscCustom::HasValue

SSkinSection* __fastcall FindSkinSection(const char* name);
SSkinSection* __fastcall FindActiveSkinSection();
const char* __fastcall ResolveSkinValue(const char* key, const char* skin);

//: dword_6FAB5104 - the map-supplied configuration overlay, null when the
//: map does not ship one.
extern CMiscCustom* g_pMapCustomConfig;

const char* __fastcall ResolveSkinValue(const char* key, const char* skin)
{
    const char* value = 0;

    if (g_pMapCustomConfig != 0
        && g_pMapCustomConfig->HasValue("CustomSkin", key))
        return ((CMiscCustom*)g_pMapCustomConfig)
                   ->GetString("CustomSkin", key, 0);

    SSkinSection* section = skin != 0 ? FindSkinSection(skin)
                                      : FindActiveSkinSection();

    if (section != 0)
    {
        SSkinValue* found =
            ((CSkinValueTable*)section->m_values)->Find(key);

        value = found != 0 ? found->m_value : 0;

        if (value != 0)
            return value;
    }

    SSkinSection* fallback = FindSkinSection("Default");

    if (fallback != 0)
    {
        SSkinValue* found =
            ((CSkinValueTable*)fallback->m_values)->Find(key);

        value = found != 0 ? found->m_value : 0;
    }

    return value;
}

//============================================================================
//  0x6F31F5D0 - ResolveSkinValueByRace.  Maps a race index (1-5) to the
//  matching GameUI skin section name and tail-calls ResolveSkinValue
//  above; any other index reads the "Default" section directly instead -
//  a plain FindSkinSection("Default") + CSkinValueTable::Find, skipping
//  ResolveSkinValue's own CustomSkin-override check entirely, which is
//  why this is not simply `ResolveSkinValue(key, "Default")`.
//
//  The five race strings ("Human"/"Orc"/"Undead"/"NightElf"/"Demon") are
//  confirmed byte-for-byte via `tools/ida_query.py bytes` at each jump
//  table target (IDA's own auto-names - aHuman/aOrc/aUndead/aNightelf/
//  aDemon - only preserve the leading letter's case).
//
//  Called from CAbility::GetPlacementCursorTexture (ability_slots.cpp,
//  worklist batch E) - out of its own module (0x6F31Fxxx here vs
//  0x6F02Exx there), so declared rather than defined there.
//============================================================================
const char* __fastcall ResolveSkinValueByRace(const char* key, int race)
{
    switch (race)
    {
    case 1: return ResolveSkinValue(key, "Human");
    case 2: return ResolveSkinValue(key, "Orc");
    case 3: return ResolveSkinValue(key, "Undead");
    case 4: return ResolveSkinValue(key, "NightElf");
    case 5: return ResolveSkinValue(key, "Demon");
    default:
        {
            SSkinSection* section = FindSkinSection("Default");
            if (section == 0)
                return 0;
            SSkinValue* found = ((CSkinValueTable*)section->m_values)->Find(key);
            return found != 0 ? found->m_value : 0;
        }
    }
}

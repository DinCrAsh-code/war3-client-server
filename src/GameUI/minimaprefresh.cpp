//============================================================================
//  CMinimap's refresh timers and its two display toggles - 0x6F33AB00,
//  0x6F33AB80 and 0x6F33ABB0, three consecutive bodies of one shipped
//  module.
//
//  The class name is the shipped one: CGameUI's constructor builds the
//  object it parks at +0x3C0 with 0x6F36D9F0, whose first instructions stamp
//  ??_7CMinimap@@6B@ over both of its frame vtables.
//
//  What +0x62C and +0x630 are is read off CGameUI, not guessed: 0x6F2F2F60
//  branches on +0x62C to pick between the "MiniMapTerrainButtonActive..."
//  skins and 0x6F30C0 does the same for +0x630 with the
//  "MiniMapCreepButton..." ones.  So the two are the terrain and creep
//  display toggles, each also a stored user preference (keys 0x3A and 0x39).
//
//  The class is declared here rather than in a shared header because these
//  three are the only CMinimap bodies in this slice, and the one caller that
//  needs the class (CWorldFrameWar3::SetMinimap, worldframeminimap.cpp)
//  carries its own copy - a local class declaration mangles identically, so
//  the two still meet at link time.  Same trick, same reason, as the
//  SPointerArray declaration in gameui_thunks.cpp.
//============================================================================
#include "preferences.h"

//  dword_6FAB59CC - the module-global blink phase.  Flipped every time the
//  0.3-second timer below expires and read by whatever draws the blinking
//  minimap icons; nothing in this call tree reads it, which is why it has no
//  type beyond "one bit".
extern int g_minimapBlinkPhase;

class CMinimap
{
public:
    //  0x6F33AB00 - advance both refresh timers by `elapsed`.  Each expires
    //  at 0.3 seconds and resets itself; the first also flips the global
    //  blink phase.  CWorldFrameWar3::SetMinimap hands it 5000.0 to force
    //  both to expire at once, which is what makes a freshly-installed
    //  minimap repaint immediately.
    void AdvanceRefreshTimers(float elapsed);
    //  0x6F33AB80 / 0x6F33ABB0 - the two display toggles.  Both raise the
    //  same +0x61C repaint request and both persist themselves.
    void SetShowTerrain(int show);
    void SetShowCreeps(int show);

    char  m_reserved000[0x610];
    float m_repaintTimer;       // +0x610
    float m_blinkTimer;         // +0x614
    int   m_blinkRepaint;       // +0x618
    int   m_repaintRequest;     // +0x61C
    char  m_reserved620[0x0C];  // +0x620
    int   m_showTerrain;        // +0x62C
    int   m_showCreeps;         // +0x630
};

//----------------------------------------------------------------------------
//  0x6F33AB00.
//----------------------------------------------------------------------------
void CMinimap::AdvanceRefreshTimers(float elapsed)
{
    float blink = elapsed + m_blinkTimer;
    m_blinkTimer = blink;
    if (blink > 0.3f)
    {
        g_minimapBlinkPhase ^= 1;
        m_blinkTimer = 0.0f;
        m_blinkRepaint = 1;
    }

    float repaint = elapsed + m_repaintTimer;
    m_repaintTimer = repaint;
    if (repaint > 0.3f)
    {
        m_repaintRequest = 1;
        m_repaintTimer = 0.0f;
    }
}

//----------------------------------------------------------------------------
//  0x6F33AB80 - preference key 0x3A.
//----------------------------------------------------------------------------
void CMinimap::SetShowTerrain(int show)
{
    m_showTerrain = show;
    m_repaintRequest = 1;
    //  The singleton is fetched *before* the arguments are pushed - which is
    //  why it is a named local and not `GetPreferences()->StoreValue(...)`:
    //  written as one expression MSVC evaluates the argument list first and
    //  the two pushes land ahead of the call.
    CPreferencesWar3* preferences = GetPreferences();
    preferences->StoreValue(0x3A, show);
}

//----------------------------------------------------------------------------
//  0x6F33ABB0 - preference key 0x39.
//----------------------------------------------------------------------------
void CMinimap::SetShowCreeps(int show)
{
    m_showCreeps = show;
    m_repaintRequest = 1;
    CPreferencesWar3* preferences = GetPreferences();
    preferences->StoreValue(0x39, show);
}

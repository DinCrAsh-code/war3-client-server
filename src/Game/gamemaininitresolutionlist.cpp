//============================================================================
//  0x6F0058A0 - reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp) with a fixed argument of 0x50 (80): a
//  member of CPreferencesWar3 (`this` in ecx, the receiver is the same
//  singleton GetPreferences() returns) that, called with 80, recurses over
//  indices 0..78 calling itself once per display-mode table slot; called
//  with an index in [0,78] instead, it reads the display-mode name out of
//  three parallel tables (dword_6F8755AC/off_6F875590/dword_6F8755B0) and
//  either queries the registered width/height for that mode
//  (Storm_422/423) and stores it as a preference, or stores the mode's own
//  name as a text default (StoreTextDefault) when the table's own "kind"
//  word reads 0.  A value of 79 (0x4F) is the sentinel that stops the
//  recursion.
//============================================================================
#include "preferences.h"

//  Storm ordinal 422/423/501/503 - not otherwise declared (422) / already
//  declared this shape elsewhere (423, Game/gamemaininitgraphics.cpp;
//  501/503, Os/os.h).
int __stdcall Storm_422(char* buffer, const char* key, const char* section,
                        int width, char* out);
int __stdcall Storm_423(const char* section, const char* key, int flag, int* out);
void __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);
void __stdcall Storm_503(char* dest, const char* src, unsigned int maxLen);

//  0x6F005840 - already reconstructed (Config/preferencesdefaultcache.cpp).
struct STextDefaultOwner
{
    int StoreTextDefault(int key, const char* text);
};

extern int         g_displayModeSectionIndex[];   // dword_6F8755AC
extern const char* g_displayModeSectionTable[];   // off_6F875590 - "\Video"-style section names
extern const char* g_displayModeKeyTable[];       // off_6F8755A8 - "reswidth"-style keys

//  dword_6F8755B0 is CPreferencesWar3's own defaults array, declared once
//  in preferences.h as `g_preferenceKinds`/`SPreferenceKind` (already
//  reached from Config/preferencesctor.cpp and
//  Config/preferenceapplystored.cpp; this function's own
//  `slot = index << 4` byte stride matches the struct's own 16-byte size).
//  This file used to declare a second, differently-typed `int[]` extern
//  for the same address under the name g_displayModeKindTable -
//  gen_rename_from_funcmap.py's own conflict check caught the resulting
//  two-names-one-address funcmap.py DATA row, and a same-name-different-
//  type follow-up attempt (a local `int[]` reusing the g_preferenceKinds
//  name) mangled to a *different* symbol than preferences.h's own
//  `SPreferenceKind*` declaration and failed link_check.py's declaration-
//  mismatch check instead.  Indexed through the real struct now (`.m_kind`
//  at the real `index`, not through the byte-offset `slot` the two int/
//  char* tables above still use) - the one instruction-shape difference
//  this correction costs is folded into this function's own already-
//  documented DIFFERS verdict below.

//  __thiscall, not __fastcall: the shipped body's own `index` argument
//  arrives on the stack (`retn 4`, `arg_0` at [esp+...]) with no `edx`
//  load at any of this tree's call sites, which is the thiscall shape a
//  genuine one-int-argument member function gets - not the two-register
//  shape a free __fastcall(void*,int) would compile to.
int CPreferencesWar3::InitResolutionList(int index)
{
    if (index == 0x4F)
        return 0;

    CPreferencesWar3* prefs = this;

    if (index == 0x50)
    {
        for (unsigned int i = 0; i < 0x4F; ++i)
            InitResolutionList(i);
        return 1;
    }

    char nameBuffer[0x104];
    Storm_501(nameBuffer, "Warcraft III", 0x104);

    unsigned int slot = index << 4;
    int section = g_displayModeSectionIndex[slot];
    const char* videoName = g_displayModeSectionTable[section];
    Storm_503(nameBuffer, videoName, 0x104);

    int kind = g_preferenceKinds[index].m_kind;
    if (kind == 0)
    {
        int width = 0;
        const char* key = g_displayModeKeyTable[slot];
        if (!Storm_423(nameBuffer, key, 0, &width))
            return 0;

        prefs->StoreValue(index, width);
        return 1;
    }
    else if (kind == 1)
    {
        char widthBuffer[0x1000];
        const char* key = g_displayModeKeyTable[slot];
        if (!Storm_422(nameBuffer, key, "reswidth", 0x1000, widthBuffer))
            return 0;

        ((STextDefaultOwner*)prefs)->StoreTextDefault(index, widthBuffer);
        return 1;
    }

    return 0;
}

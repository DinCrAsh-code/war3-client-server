//============================================================================
//  0x6F005720 / 0x6F0059C0 - the game's preferences singleton and its
//  integer lookup.  Reached from GetModelDetailFlags (0x6F333760,
//  modeldetail.cpp), which asks it for preference 7 to decide how much of a
//  unit's model to build.
//
//  One translation unit: neither calls the other, and the two addresses are
//  neighbours.  Their caller is in a separate one because those calls are
//  real.
//
//  The class's own source file name falls out of the allocation tag:
//  ".\\CPreferencesWar3.cpp", line 225, 0x50 bytes.
//============================================================================
#include "storm.h"
#include "preferences.h"

//  0x6F005640 - the constructor, `this` in ecx.  Below the depth-3 cut.
CPreferencesWar3* __fastcall ConstructPreferences(void* raw);

//  __imp_atol - the CRT's own, imported rather than reconstructed.
long __cdecl atol(const char* text);

//  One 16-byte record per preference; the first field is the default, as a
//  string that has to be parsed every time the cache misses.
struct SPreferenceDefault
{
    const char* m_pDefault;    // +0x00
    char        m_reserved04[0x0C];
};

extern CPreferencesWar3*  g_pPreferences;                 // dword_6FAAE314
extern SPreferenceDefault g_preferenceDefaults[];         // off_6F8755B4
extern char               g_preferenceBucket;             // unk_6FAAE310

//----------------------------------------------------------------------------
//  0x6F005720.  Note the null store on the failure path: the global is
//  written even when the allocation fails, which is why this is not spelled
//  as "allocate, and only assign on success".
//----------------------------------------------------------------------------
CPreferencesWar3* GetPreferences()
{
    if (g_pPreferences != 0)
        return g_pPreferences;

    void* raw = SMemAlloc(0x50, ".\\CPreferencesWar3.cpp", 225, 0);
    if (raw != 0)
        return g_pPreferences = ConstructPreferences(raw);

    return g_pPreferences = 0;
}

//----------------------------------------------------------------------------
//  0x6F0059C0.  Returns 0 for an out-of-range key without touching *out,
//  and 1 in both other cases - the cached one and the parse-the-default one.
//  `key` is signed: the range test is a `jl`, so a negative key takes the
//  same path as one past the end.
//----------------------------------------------------------------------------
int CPreferencesWar3::GetIntValue(int key, int* out)
{
    if (key >= 0x4F)
        return 0;

    SPreferenceEntry* entry = m_cache.Find(key, &g_preferenceBucket);
    if (entry != 0)
    {
        *out = entry->m_value;
        return 1;
    }

    int value = atol(g_preferenceDefaults[key].m_pDefault);
    StoreValue(key, value);
    *out = value;
    return 1;
}

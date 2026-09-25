//============================================================================
//  0x6F0010E0 - reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp): read the "Allow Local Files" preference
//  and forward it to SetMpqModuleFlag, then open up to two "War3.mpq"-style
//  base archives and up to two localized patch archives, open
//  "War3Patch.mpq" and its DBC-listed sub-archives, then register the
//  active MPQ module (RegisterMpqModule) using preference #6 as the index.
//  Aborts with a fatal message if the first base archive is missing.
//============================================================================
#include <windows.h>

//  Storm ordinal 423/266/267/270/253/293 - 266/293 already declared this
//  shape (Game/gamerestartload.cpp); 267/270/253 not otherwise declared in
//  this tree.
int  __stdcall Storm_423(const char* section, const char* key, int flag, int* out);
int  __stdcall Storm_266(const char* name, unsigned int a, unsigned int b, void** outHandle);
int  __stdcall Storm_267(const char* key, int* out);
void __stdcall Storm_270(const char* text);
void __stdcall Storm_253(void* handle);
int  __stdcall Storm_293(int first, const char* name, unsigned int a,
                         unsigned int b, void** outHandle);

//  0x6F0010B0/0x6F4C19D0/0x6F001020 - already reconstructed
//  (Game/mpqmoduleflagset.cpp, Misc/gamemain_small_wrappers.cpp,
//  Game/mpqmoduleregister.cpp).
void  __fastcall SetMpqModuleFlag(int value);
char* __fastcall CacheTextAndReturn(const char* text);
int   __fastcall RegisterMpqModule(int index);

//  0x6F0059C0/0x6F005720 - already reconstructed (preferences.h).
#include "preferences.h"

//  0x6F0068C0 - already reconstructed (Game/gamemainfatalmessage.cpp).
void __fastcall GameMainShowFatalMessage(unsigned int lineCount);

extern const char* g_baseArchiveNameTable[2];    // off_6FA4F000 - "War3.mpq"-style
extern void*        g_baseArchiveHandleTable[2];  // dword_6FAAE2A0
extern int           g_localizedArchiveTable[2];   // dword_6FA4F008
extern void*        g_localizedArchiveHandleTable[2]; // dword_6FAAE298
extern void*        g_mpqModuleSlots[11];           // dword_6FAAE280 (Game/mpqmoduleregister.cpp)
extern int*          g_patchSubArchiveTable;        // dword_6FA4F024
extern void*        g_patchSubArchiveHandleTable[6]; // dword_6FAAE284

int __fastcall GameMainInitMpqModules()
{
    int allowLocalFiles;
    if (Storm_423("Warcraft III", "Allow Local Files", 0, &allowLocalFiles) && allowLocalFiles != 0)
        allowLocalFiles = 1;
    else
        allowLocalFiles = 0;

    SetMpqModuleFlag(allowLocalFiles);

    if (allowLocalFiles)
    {
        char* teen = CacheTextAndReturn((const char*)4);
        if (teen && *teen)
            Storm_270(teen);
    }

    for (int i = 0; i < 2; ++i)
    {
        if (!g_baseArchiveNameTable[i])
            continue;

        if (Storm_266(g_baseArchiveNameTable[i], 2, i, (void**)&g_baseArchiveHandleTable[i]) == 0)
        {
            if (i == 0)
            {
                GameMainShowFatalMessage(3);
                return 0;
            }
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        if (g_localizedArchiveTable[i])
            Storm_266((const char*)(long)g_localizedArchiveTable[i], 2, i + 2, (void**)&g_localizedArchiveHandleTable[i]);
    }

    Storm_266("War3Patch.mpq", 8, 6, &g_mpqModuleSlots[0]);

    int* teenListHandle = 0;
    Storm_267("(teen)", (int*)&teenListHandle);
    if (teenListHandle)
    {
        Storm_253(teenListHandle);

        for (int i = 2; i > 0; --i)
        {
            int value = g_patchSubArchiveTable[i - 1];
            if (value)
                Storm_293(0, (const char*)(long)value, 6, i + 0x12 - 1, &g_patchSubArchiveHandleTable[i - 1]);
        }
    }

    int index = 2;
    GetPreferences()->GetIntValue(6, &index);
    RegisterMpqModule(index);

    return 1;
}

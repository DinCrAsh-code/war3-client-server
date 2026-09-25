//============================================================================
//  0x6F001020 - register mpq module `index` if it is not already the
//  current one: free g_mpqModuleSlotsA's two entries (the same pair
//  mpqmoduleslotsteardown.cpp's own TeardownMpqModuleSlots walks), reset
//  the file cache (0x6F4BEC20 - Render/filecacheclear.cpp,
//  ResetFileCache(), one of this batch's own other 42), then walk the two
//  8-byte records off_6FA4F018[index] backwards, validating each half
//  through Storm ordinal 293 and AND-ing every validation result together.
//============================================================================

//  Matching Game/gamerestartload.cpp's own signatures exactly - a
//  differing parameter or return type mangles to a different symbol and
//  only fails at link time.
int __stdcall Storm_252(void* ptr);

//  Same combined table mpqmoduleslotsteardown.cpp declares
//  (dword_6FAAE280 + index*4); elements [4]/[5] are dword_6FAAE290/294.
extern void* g_mpqModuleSlots[11];
#define g_mpqRegisteredIndexPlus1 (*(int*)(g_mpqModuleSlots + 3))   // dword_6FAAE28C
extern int   g_mpqModuleRecordTable[][2];  // off_6FA4F018 - "War3_med.mpq" comment

void ResetFileCache();   // Render/filecacheclear.cpp - this batch's own 0x6F4BEC20

//  Storm ordinal 293 - matching Game/gamerestartload.cpp's own signature
//  exactly.  Five stack arguments in the order this call tree's one call
//  site pushes: `(0, record-as-name, count, index+1, &remainingSlots)`.
int __stdcall Storm_293(int first, const char* name, unsigned int a,
                         unsigned int b, void** outHandle);

int __fastcall RegisterMpqModule(int index)
{
    if (g_mpqRegisteredIndexPlus1 == index + 1)
        return 1;

    g_mpqRegisteredIndexPlus1 = index + 1;

    for (int i = 4; i < 6; i += 1)
    {
        void* elem = g_mpqModuleSlots[i];
        if (elem != 0)
        {
            Storm_252(elem);
            g_mpqModuleSlots[i] = 0;
        }
    }

    ResetFileCache();

    int ok = 1;
    int remaining = 2;
    int* record = g_mpqModuleRecordTable[index] + 2;   // one past the record, walked backward

    while (remaining != 0)
    {
        record -= 1;
        remaining -= 1;
        int value = *record;
        if (value != 0)
        {
            ok &= Storm_293(0, (const char*)value, 6, remaining + 1, (void**)(long)remaining);
        }
    }

    return ok;
}

//============================================================================
//  0x6F009950 - GameMain's own GPU-hardware compatibility scan: load the
//  font metric records (LoadFontRecordsThunk, 0x6F553CC0 - a genuinely
//  separate font subsystem call reached from here as one of this
//  function's own startup steps, not because the two are related), then
//  for each of the first two "War3.mpq"/"War3_med.mpq"-style archive slots
//  present (Storm ordinal 289) that is still within the loaded font
//  record's own count, hash a chunk of device-id text (HashDeviceStringThunk,
//  0x6F685B40) and check it against a small table of known device ids,
//  writing a tri-state compatibility flag (0 not found / 1 found and
//  matching a known-bad marker / 2 - via the `sbb+1` idiom below) into
//  g_slotTableB3[slot] (dword_6FAAE370 - 0x6F009920's own input, see
//  mpqhardwareprofileindex.cpp).
//
//  The function-level SEH frame is the same `__except_handler4`-shaped
//  frame this toolchain's fixed /GS- /EHs-c- never reproduces byte for
//  byte (docs/msvc-vc8-idioms.md); not chased further, see the BEHAVIOUR
//  entry.  Both local structs this function builds are opaque scratch this
//  call tree never names a type for, so only the offsets actually read or
//  written are asserted.
//============================================================================

int  __fastcall LoadFontRecordsThunk(void*, void*, int);
void __fastcall HashDeviceStringThunk(void*, void*, void*, void*);
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);
int  __stdcall Storm_289(int a, const char* name, int b);

extern const char* g_archiveNameTable[3];      // off_6FA4F708 - "War3.mpq"-style names
extern int         g_deviceIdTable[3];         // dword_6FA4F710
extern int         g_knownBadDeviceTable[3];   // dword_6FA4F700
//  void* to match Misc/gamemainslottable.cpp's own declaration exactly - a
//  differing element type mangles this extern to a different symbol and
//  only fails at link time.
extern void*       g_slotTableB3[];          // dword_6FAAE370 - 0x6F009920's own input

void GameArchiveHardwareCheck()
{
    //  Two opaque scratch structs; only the fields this function itself
    //  reads back are named.
    struct FontLoadResult { int f0; int slotCount; int f8, fC, f10, f14, f18; } fontResult;
    struct HashScratch { char text[8]; int idOut; } hashBuf;

    fontResult.f0 = 0;
    LoadFontRecordsThunk(&fontResult, &hashBuf, 0);

    int slotCount = fontResult.slotCount;
    void* releasePtr = 0;
    int weight = 0;

    for (int slot = 0; slot < 2; slot += 1)
    {
        if (Storm_289(0, g_archiveNameTable[slot], 2) != 0 && slot < slotCount)
        {
            HashDeviceStringThunk((void*)((char*)&fontResult + slot * 0x20),
                                   &hashBuf, &hashBuf.idOut, &releasePtr);

            int matched = 0;
            for (int i = 0; i < 3; i += 1)
            {
                int candidate = g_deviceIdTable[weight + i];
                if (candidate != 0 && hashBuf.idOut == candidate)
                    matched = 1;
            }

            if (matched)
            {
                //  `cmp slotCount, table[slot]; sbb ecx,ecx; add ecx,1` -
                //  1 when slotCount < table[slot], else 0... reproduced
                //  as the equivalent comparison directly.
                g_slotTableB3[slot] = (void*)(long)((slotCount < g_knownBadDeviceTable[slot]) ? 1 : 0);
            }
        }

        weight += 3;
    }

    if (releasePtr != 0)
        SMemFree(releasePtr, "delete", -2, 0);
}

//============================================================================
//  0x6F6C5730 - CPU-ticks-per-second init, reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp).  Allocates and constructs a small
//  object (0x20 bytes, tagged ".\W32\Time.cpp" line 0x273), reads the
//  "CpuTicksPerSecond" config value under section "Internal" into the
//  8-byte global qword_6FAD1428 (defaulting to 0 on a miss), then Sleep(0).
//
//  The shipped body carries the __except_handler4-shaped SEH frame
//  (push -1/push offset SEH_.../mov large fs:0 on entry, the matching
//  restore, and an appended scope-table trampoline at 0x6F8487E0 that
//  tail-calls nullsub_45 for its `__finally` and one at 0x6F848802 that
//  tail-calls SecurityCheckCookie/__CxxFrameHandler3) that this fixed
//  /GS- /EHs-c- toolchain cannot reproduce byte-for-byte from any source
//  shape - see docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped
//  frame this toolchain cannot reproduce". Written as the plain body a
//  `__try { ... } __finally { }` around the allocate-and-construct call
//  would lower to past that frame.
//============================================================================
#include <windows.h>

//  Storm ordinal 401/421 - already declared this shape elsewhere
//  (SMemAlloc) / not otherwise declared in this tree.
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename, int logline, unsigned int flags);
int   __stdcall Storm_421(const char* section, const char* key, int flag,
                          void* defaultValue, unsigned int size, void* out);

//  0x6F6C56A0 - already reconstructed (Sync/ostimethreadinit.cpp).
void __fastcall InitOsTimeThread(void* self);

extern unsigned __int64 g_cpuTicksPerSecond;   // qword_6FAD1428

void __fastcall GameMainInitCpuTicksPerSecond()
{
    void* obj = SMemAlloc(0x20, ".\\W32\\Time.cpp", 0x273, 8);
    if (obj)
        InitOsTimeThread(obj);

    if (!Storm_421("Internal", "CpuTicksPerSecond", 0, &g_cpuTicksPerSecond,
                   8, &g_cpuTicksPerSecond))
    {
        g_cpuTicksPerSecond = 0;
    }

    Sleep(0);
}

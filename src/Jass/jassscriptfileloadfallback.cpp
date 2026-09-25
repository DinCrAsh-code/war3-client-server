//============================================================================
//  0x6F3B1970 - the low-level ".j" script file reader: try the bare name
//  through the shared file cache first, and on failure retry once with a
//  "scripts\" prefix concatenated on.  This is the function
//  sub_6F3B1D00/sub_6F3B1A20 (this round's own root, and the actual
//  common.j/blizzard.j/map-script reader - not reconstructed in this
//  session's own batch) call once per script to get its bytes.
//
//  Confirmed from 0x6F3B1A20's own call sites (`lea edi,[esp+var_C]` /
//  `mov esi, offset aCommonJ` / `call sub_6F3B1970`, once each for
//  "common.j" and "blizzard.j", with the third stack local reused for the
//  map's own script): the shipped function takes its filename in `esi` and
//  an `int*` output-length pointer in `edi`, with nothing passed through
//  `ecx`/`edx`/the stack at all - the internal-linkage register convention
//  docs/msvc-vc8-idioms.md's "MSVC invents a convention for any
//  internal-linkage function whose every call site it can see" describes,
//  chosen for a static helper whose two real callers are outside this
//  session's own batch. With no call site in this translation unit to give
//  the compiler the same two live registers to reuse, that exact register
//  choice cannot be reproduced; declared here as an ordinary
//  `__fastcall(name, outLength)` instead (the closest normal C++ shape).
//  That is a real, permanent ABI difference from the shipped `esi`/`edi`
//  convention, not cosmetic register-allocation noise, so this stays
//  unhookable until a session with those two callers in view can pin the
//  real convention down and reconstruct all three together.
//
//  A second, unrelated gap: the shipped body opens with the game's own
//  `/GS` stack cookie (`mov eax, dword_6FAAE140` / `xor eax, esp` ...
//  `call sub_6F7E1059` at the tail) around its 0x104-byte "scripts\"+name
//  buffer.  This build is fixed at `/GS-` (pipeline/CLAUDE.md), so no
//  reconstruction can ever emit that pair - see Misc/crtsec.cpp and
//  Jass/jassregisternative.cpp's own `RegisterNative` for the same,
//  already-documented ceiling.
//
//  The two `LoadCachedFile` calls are otherwise a literal transcription:
//  the first arm returns early only along the shipped body's own real
//  conditional branch (`jz` over the fallback block), so it is spelled as
//  a plain `if`; the second is the branchless `rc ? buffer : 0` idiom
//  (`neg`/`sbb`/`and`) the shipped compiler produced for the tail
//  expression - what a ternary pointer return compiles to here.
//============================================================================
#include "storm.h"
#include "filecache.h"

void __stdcall Storm_501(char* dest, const char* src, unsigned int maxLen);
void __stdcall Storm_503(char* dest, const char* src, unsigned int maxLen);

void* __fastcall LoadScriptFileWithFallback(const char* name, int* outLength)
{
    void* buffer = 0;
    if (LoadCachedFile(name, &buffer, outLength, 1))
        return buffer;

    char path[0x104];
    Storm_501(path, "scripts\\", 0x100);
    Storm_503(path, name, 0x100);

    buffer = 0;
    int rc = LoadCachedFile(path, &buffer, outLength, 1);
    return rc ? buffer : 0;
}

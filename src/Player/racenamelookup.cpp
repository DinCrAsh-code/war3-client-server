//============================================================================
//  0x6F39F710 - RaceNameLookup.  Clamps a race index to [0,6) (an
//  out-of-range index resets to 0; the comparison is signed, so a negative
//  index is *not* clamped - reproduced exactly via `!(raceIndex < 6)` -
//  matching MSVC8's own `jl`/fallthrough shape), then picks one of two
//  6-entry string tables by `useLongName`.  Called from
//  CPlayerWar3::AppendPlayerSummaryLine (playerwar3_appendsummaryline.cpp,
//  vtable slot 31) with `useLongName == 0` always, so only the short-name
//  table is exercised on that path - the long-name table is still declared
//  and touched by this function's own body (the dump names both), just
//  never actually indexed at runtime by the one call site in this batch.
//
//  __fastcall(ecx, edx): both inputs are plain ints and the callee cleans
//  nothing extra (bare `retn`), matching a two-register-argument leaf with
//  no stack parameters.  Own translation unit: 0x6F39F710 is nowhere near
//  the 0x6F40xxxx/0x6F41xxxx CPlayerWar3 module neighbourhood.
//============================================================================
#include "racenamelookup.h"

//  off_6FA79470 / off_6FA79458 - two 6-entry string tables (one per
//  playable race + the neutral/random slots, matching the 6-index clamp).
//  Neither table's actual string contents were visible in this dump (only
//  the table addresses and the indexed load are), so nothing here can name
//  which race is at which index - declared `extern`, not defined, per
//  CLAUDE.md's DATA-global rule (link_check.py flags a defined copy as
//  silently shadowing the real shipped table).
extern const char* g_raceNameTableLong[6];   // off_6FA79470
extern const char* g_raceNameTableShort[6];  // off_6FA79458

const char* __fastcall RaceNameLookup(int raceIndex, int useLongName)
{
    if (!(raceIndex < 6))
        raceIndex = 0;

    if (useLongName)
        return g_raceNameTableLong[raceIndex];

    return g_raceNameTableShort[raceIndex];
}

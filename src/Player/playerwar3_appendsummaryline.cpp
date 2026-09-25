//============================================================================
//  0x6F40B4A0 - CPlayerWar3 vtable slot 31 (+0x7C).  Formats one debug
//  summary line - "Player %2d %-18s\tRace %s\tStartLoc %2d" - into a local
//  0x104-byte buffer via Storm_578, then appends it onto the caller's own
//  buffer via Storm_503.
//
//  The player's own name comes from GetHandleOrZeroAlias on the
//  SHandleHolder at +0x24 (handleobject.h, already reconstructed - the
//  same helper CPlayerWar3::Method_0x24's own array walk is unrelated to;
//  this is the object's *own* +0x24 field, not the ability array at
//  +0x40), falling back to the literal "<Unused>" when that comes back
//  zero. The race name is RaceNameLookup(this->+0x260, /*useLongName=*/0)
//  - see racenamelookup.cpp. StartLoc is this->+0x274 (already an
//  SIntMiniValue field per playerwar3.h/DumpState, read here as a plain
//  int - the same bare-dword shape SIntMiniValue itself is). The player
//  number is the single byte at +0x30.
//
//  Same plain (non-SEH) /GS local-buffer cookie every large-stack-buffer
//  function in this repo is missing under the fixed /GS- build
//  (docs/msvc-vc8-idioms.md and tools/funcmap.py's own RCString::Concat/
//  SSelectionHost::SetColonMessage/BuildMoviesPathB3 entries) - ~9
//  instructions (the cookie load/XOR/store in the prologue, the reload/
//  XOR/SecurityCheckCookie call in the epilogue) this toolchain cannot
//  reproduce with /GS- fixed. Past that gap the body matches the dump
//  instruction for instruction and in the same order.
//============================================================================
#include "playerwar3.h"
#include "handleobject.h"
#include "racenamelookup.h"

int __cdecl Storm_578(char* dest, unsigned int size, const char* format, ...);
void __stdcall Storm_503(char* dest, const char* src, unsigned int maxLen);

void CPlayerWar3::AppendPlayerSummaryLine(char* dest, unsigned int maxLen)
{
    const char* name = (const char*)GetHandleOrZeroAlias(
        (const SHandleHolder*)((char*)this + 0x24));
    if (!name)
        name = "<Unused>";

    //  Arguments passed inline (not through named locals) so the compiler
    //  evaluates/pushes them in its own natural right-to-left __cdecl
    //  order, matching the dump's own push-startLoc-before-the-
    //  RaceNameLookup-call scheduling exactly - pre-computing them into
    //  separate locals instead makes the compiler hold startLoc live
    //  across the call in a register and push it after, which is a real,
    //  scoreable instruction-order difference.
    char buffer[0x104];
    Storm_578(buffer, 0x104, "Player %2d %-18s\tRace %s\tStartLoc %2d",
              *(unsigned char*)((char*)this + 0x30),
              name,
              RaceNameLookup(*(int*)((char*)this + 0x260), 0),
              *(int*)((char*)this + 0x274));

    Storm_503(dest, buffer, maxLen);
}

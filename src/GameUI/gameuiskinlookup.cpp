//============================================================================
//  0x6F31B4B0 - FindSkinSection.
//
//  Four instructions: push the caller's own name straight through as the
//  table lookup's stack argument and hand the table over in ecx.  The
//  section table is a global *object* (`mov ecx, offset unk_6FAB5564`), not
//  a pointer to one - see docs/msvc-vc8-idioms.md, "`offset dword_X` versus
//  `dword_X`".
//
//  Own translation unit: 0x6F319920 is a real call, and its two callers here
//  (0x6F31F4C0 and 0x6F31F530) are in a third module again.
//============================================================================
#include "gameuiskin.h"

SSkinSection* __fastcall FindSkinSection(const char* name);

SSkinSection* __fastcall FindSkinSection(const char* name)
{
    return g_skinSectionTable.Find(name);
}

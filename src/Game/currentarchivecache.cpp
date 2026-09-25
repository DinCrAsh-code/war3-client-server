//============================================================================
//  0x6F00E580 - a one-slot "currently open named archive" cache: reopen
//  only when the requested name differs from whatever this global handle
//  already points at, closing the old one first.
//
//  GetGlobalPointer_6F01F590() (misc_trivial_getters.cpp) hands back
//  `unk_6FAAE788` with no `this`/args at all - the same no-argument shape
//  this function's own first call reaches, so it is called through rather
//  than the global re-declared and read directly (that would fold to a
//  plain `mov offset`, not the `call` the dump actually has).  +0x00 of
//  that global is read here as the open archive handle, null when nothing
//  is open; nothing else in this closure establishes the rest of its
//  layout.
//
//  Storm_293/Storm_266 are the two archive-open shapes gamerestartload.cpp/
//  gamemaininitmpqmodules.cpp already declare (patch-chained vs. plain);
//  Storm_252 the close, both already declared there too.  Storm_275 is not
//  otherwise declared in this tree - a `(handle, buffer, size)` shape read
//  straight off this call site's own pushes, most likely "get this
//  archive's own name/path", since its result is immediately compared
//  against the requested name via SStrnICmp (Storm_509, Config/config.h).
//
//  `/GS` stack cookie (`dword_6FAAE140 ^ esp`, checked via `sub_6F7E1059`
//  at both tails) around the 0x104-byte local path buffer - this repo's
//  toolchain is fixed at `/GS-`, so no source spelling earns that cookie
//  here; DIFFERS for exactly that, same as GameUI/frameartpath.cpp's own
//  note.  The logic past the missing prologue/epilogue cookie matches
//  instruction for instruction, including the shipped duplicate epilogue
//  (one for the "already open" early-outs, a second copy for the
//  Storm_266 tail - not /OPT:ICF, so the identical cookie-check-and-return
//  sequence is compiled out twice).
//============================================================================
#include "config.h"   // SStrnICmp - Storm_509

void* GetGlobalPointer_6F01F590();

int  __stdcall Storm_275(void* handle, char* outBuffer, unsigned int bufSize);
int  __stdcall Storm_266(const char* name, unsigned int a, unsigned int b, void** outHandle);
int  __stdcall Storm_293(int first, const char* name, unsigned int a,
                         unsigned int b, void** outHandle);
int  __stdcall Storm_252(void* handle);

//  0x6F00E580 - `retn 0`, `__fastcall(name in ecx)`.  Returns nonzero on
//  success (already open with this name, or freshly (re)opened); 0 on a
//  failed (re)open.
int __fastcall EnsureNamedArchiveOpen(const char* name)
{
    void** handleSlot = (void**)GetGlobalPointer_6F01F590();

    if (name != 0 && *handleSlot != 0)
    {
        char currentName[0x104];
        Storm_275(*handleSlot, currentName, 0x104);
        if (SStrnICmp(currentName, name, 0x7FFFFFFF) == 0)
            return 1;
    }

    if (*handleSlot != 0)
    {
        Storm_252(*handleSlot);
        *handleSlot = 0;
    }

    if (name == 0 || *name == 0)
        return 1;

    if (Storm_293(0, name, 0x10, 4, handleSlot) != 0)
        return 1;
    return Storm_266(name, 0x10, 4, handleSlot);
}

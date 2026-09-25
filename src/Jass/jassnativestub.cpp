//============================================================================
//  0x6F71D9D0 - StubNative: the trivial "always false/0/null" entry point
//  every one of the 1167 rows in RegisterAllJassNativesAsStubs
//  (jassregisterallnativestubs.cpp) points at, instead of each native's own
//  real implementation. Two instructions, `xor eax, eax` / `retn` - no
//  prologue, no argument access, no locals, so it never even looks at
//  whatever the caller pushed. See RegisterAllJassNativesAsStubs' own header
//  comment for what this whole second registration table appears to be for.
//
//  Declared `__cdecl` with no parameters (the shipped body never touches an
//  argument, so the calling convention only affects the caller's own
//  cleanup, which the JASS dispatcher already does for every native
//  regardless of its real signature - jass-natives-registration-table.md's
//  own header: "every native in the table ends in a bare retn, which makes
//  all of them __cdecl"). Passed to RegisterNative as `(void*)&StubNative`,
//  the same way any other entry point becomes the `void*` NativeFunc stores.
//============================================================================
#include "jassnativestub.h"

int __cdecl StubNative()
{
    return 0;
}

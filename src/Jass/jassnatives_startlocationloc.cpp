//============================================================================
//  0x6F3D04B0 - JASS_GetStartLocationLoc, `GetStartLocationLoc`, "(I)Hlocation;".
//
//  The second of the two natives that hand JASS a fresh `location` handle,
//  and the only caller of the first: it reads a start-location slot's
//  coordinate pair straight out of the game state and forwards the two to
//  `Location` (jassnatives_locationmake.cpp), which is what makes the
//  handle.  Nothing here validates that the slot was ever defined - an
//  undefined one yields a location at whatever the record holds.
//
//  Two things the disassembly settles rather than the signature:
//
//   * the bound is *unsigned*.  The shipped `cmp eax,0Ch` / `jb` rejects a
//     negative index as well as one past the end, where a signed `<` would
//     have been `jl` and would have let -1 through into the array maths;
//
//   * the record is *named*, and that is worth two instructions.  Written as
//     two `m_startLocations[index]` subscripts the compiler strength-reduces
//     the second one into `(index + 4) * 2Ch`, so it multiplies twice (0.565
//     against 0.682); a named `slot` pointer gives the shipped
//     `mov edx,[eax+ecx+0ACh]` / `lea eax,[eax+ecx+0ACh]` / `mov eax,[eax+4]`
//     exactly.  Naming the *singleton* instead of the record is worse again
//     (0.435).
//
//  What is left is a stack-slot choice and nothing else: this build notices
//  that `index` is dead once the pair has been read and puts `y` in the
//  incoming argument's own slot (`push ecx` for the one remaining local),
//  where the shipped build reserves two fresh words (`sub esp,8`).  Same 22
//  instructions in the same order either way; declaring both locals ahead of
//  the bound check does not move it, and reading `y` before `x` is worse.
//
//  Own translation unit even though 0x6F3D03B0 is 0x100 bytes away in the
//  same module: it issues a genuine `call sub_6F3D03B0`, so the two must not
//  be in a position to be inlined into each other.
//============================================================================
#include "jassnatives.h"

//  0x6F3D03B0 - jassnatives_locationmake.cpp.
int __cdecl JASS_Location(const int* x, const int* y);

//  The shipped bound, and the only thing in this family that states how many
//  start-location slots the record array has.
const unsigned int kStartLocationCount = 12;

int __cdecl JASS_GetStartLocationLoc(int index)
{
    if ((unsigned int)index >= kStartLocationCount)
        return 0;

    const SStartLocation* slot = &g_pJassGameState->m_startLocations[index];

    int x = slot->m_x;
    int y = slot->m_y;

    return JASS_Location(&x, &y);
}

//============================================================================
//  0x6F39F240 - "end the game" as the world object asks for it: event 11,
//  argument 0, on the record index the caller names.
//
//  Three instructions, and one of them has no source spelling: the shipped
//  code materialises the 11 as `lea ecx, [edx+0Bh]` off the `edx` it has
//  just zeroed for the second argument (docs/msvc-vc8-idioms.md, "Constants
//  formed off a register the compiler knows is zero").  The third is the
//  tail `jmp`, which only comes out that way because the callee lives in
//  another translation unit.
//============================================================================
#include "netdata.h"

//  0x6F54C7E0 - Net/netsessionend.cpp.
void __fastcall EndSessionGame(unsigned int eventId, int eventArg, int index);

//: The client event id the world object's end-game request posts.  Nothing
//: in this call tree names it; 11 is what the shipped code passes.
const unsigned int kNetClientEventGameOver = 11;

void __stdcall RequestSessionGameOver(int index)
{
    EndSessionGame(kNetClientEventGameOver, 0, index);
}

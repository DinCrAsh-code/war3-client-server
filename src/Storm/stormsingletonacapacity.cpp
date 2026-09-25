//============================================================================
//  0x6F73AFA0 - SStormSingletonA::EnsureStormSingletonACapacity.
//
//  Was a `mov eax,<address> / jmp eax` redirect, on the stated grounds that
//  it is "deep in Storm's own address range".  It is: it is also seventeen
//  instructions of signed divide-by-four, with no calls, no globals and no
//  reads through `this` at all - the whole body is the four stack
//  arguments.  The dump that carries it (asm/sub_6F2AE690_...) has been
//  here all along.
//
//  What it actually does is convert a pair of fine coordinates to the
//  coarse grid: `(v + 2) / 4`, *signed* - the `cdq` / `and edx,3` /
//  `add` / `sar 2` sequence is MSVC's signed division by a power of two,
//  which rounds towards zero, and an unsigned `>> 2` here would differ for
//  every negative input.  So a coordinate two cells left of the origin
//  still lands in cell 0, not in cell -1.
//
//  The two out-pointers really are crossed: QueryCell calls this as
//  `(x, y, &y, &x)` (stormsingletona.h records the call site), so the
//  x-derived answer is written through the third argument and the y-derived
//  one through the fourth.  Naming them after the arguments they are
//  written *through* rather than after the values they carry is the only
//  spelling that does not invite a "fix".
//
//  `this` is declared and unused - the shipped code overwrites ecx with the
//  third argument as its first act.  It stays a member because that is what
//  the call site's `mov ecx,esi` makes it, and because a __fastcall free
//  function here was a live crash once already (see stormsingletona.h).
//
//  Own translation unit: QueryCell (stormsingletonaquery.cpp) reaches it
//  with a real call.
//============================================================================
#include "stormsingletona.h"

void SStormSingletonA::EnsureStormSingletonACapacity(int x, int y,
                                                     int* outThird,
                                                     int* outFourth)
{
    *outThird = (x + 2) / 4;
    *outFourth = (y + 2) / 4;
}

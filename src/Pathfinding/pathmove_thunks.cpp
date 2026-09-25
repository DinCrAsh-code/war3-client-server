//============================================================================
//  Naked pass-through thunks for the movement-pathfinder callees that
//  src/pathmove.h declares and the reconstructed bodies call, but that no
//  session has reconstructed yet.
//
//  Without these the tree does not link at all (LNK2019/LNK1120), so no
//  reconstruction anywhere in the repo can be tested in a running game -
//  which is exactly what happened when the pathfinder branch was merged:
//  `verify.py` never links, so it scored every one of these callers green
//  while the .mix could not be built at all.  Same shape and same reasoning
//  as src/w3gsaction_thunks.cpp: jump to the shipped function at the address
//  the declaration's own comment in pathmove.h already records, so the
//  original code runs.
//
//  These are NOT reconstructions and must never be offered as hookable - a
//  detour cannot detour a detour, so hooking one JMP-patches the very
//  address it jumps to and re-enters itself forever.  `abi_audit.py` sees
//  them as SELF-REDIRECT THUNKs and `generate_mix_config.py` defaults every
//  new key to false; leave them false.  Delete each one as its real body
//  lands.
//
//  Every address below was cross-checked against a `### <name> @ 0x<addr>`
//  dump heading before being written here.
//============================================================================
#include "pathmove.h"

//  0x6F49BB90 - shipped code
__declspec(naked) int CPathClient::RunTrace(const CGridVec2*, const CGridVec2*)
{
    __asm { mov eax, 06F49BB90h
            jmp eax }
}

//  0x6F49BEE0 - shipped code
__declspec(naked) int CPathClient::FirstBlockedPoint(const CGridVec2*)
{
    __asm { mov eax, 06F49BEE0h
            jmp eax }
}

//  0x6F49C460 CPathClient::SnapToCell is NOT a thunk any more: it is
//  reconstructed in pathsnaptocell.cpp.  The thunk that used to stand here
//  outlived its replacement and was a second definition of the same symbol -
//  invisible to verify.py (which scored the real body) and downgraded to a
//  LNK4006 warning by the .mix link's /FORCE:MULTIPLE, so whichever object
//  came first on the link line won.  Do not put it back.

//  0x6F495A00 - shipped code.  Four stack arguments (retn 10h); see
//  game.h's own note on why this one stays a thunk.
__declspec(naked) int CPathTrace::TryStep(const CGridVec2*, int, int, int)
{
    __asm { mov eax, 06F495A00h
            jmp eax }
}

//  0x6F6EF3B0 - shipped code, and deliberately still a redirect.
//
//  It *is* composable from the ordinary CFloat operators after all - the
//  325 instructions are operator+ twice, operator* twice and operator+
//  once more, all five inlined, over `(-a) + b` per component, with only
//  CountLeadingZeros left as a real call.  The whole shape was derived and
//  measured with the IsUnitInRangeLoc target; see
//  docs/targets/JASS_UnitPositionAndRange.md, "0x6F6EF3B0".  What stops it
//  being a body here is that this toolchain will not inline a
//  hundred-instruction operator on its own judgement, so reproducing the
//  shipped stream needs operator+ and operator* split into __forceinline
//  helpers inside softfloat.cpp - a structural change to a finished,
//  matching target - and even then the measured result was 54 of 325, all
//  of the gap register allocation across five inlinings.  A real body at
//  that score is not worth destabilising softfloat.cpp for; the note says
//  exactly what to do when someone wants to finish it.
__declspec(naked) CFloat __fastcall CFloatDistanceSquared(const CGridVec2&,
                                                          const CGridVec2&)
{
    __asm { mov eax, 06F6EF3B0h
            jmp eax }
}


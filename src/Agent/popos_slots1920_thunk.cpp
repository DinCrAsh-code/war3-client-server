//============================================================================
//  NIpse::CPoPos - vtable slot 19 (0x6F4A7480). Left `THUNK` across a
//  THIRD follow-up session on this family now: its own body (a CFloat
//  conversion through sub_6F493910, stamped into +0x8C) reaches a small
//  `CFloat` value-mod-threshold helper (sub_6F493910 -> sub_6F6EFE50,
//  using the already-real CFloatReciprocal/CFloatFrac/operator* /+/-
//  quintet) whose exact sign-branch spelling this session traced two
//  levels deeper than either prior session (far enough to see it is a
//  generalised sibling of Math/floatmodstep.cpp's own `CFloatModStep`/
//  Math/cfloatwrappeddelta.cpp's own `CFloatWrappedDelta` - "wrap a value
//  into a symmetric range around a runtime threshold" rather than the
//  fixed `g_stepSize` those two use) but did not reach a confidently
//  provable branch-direction reading for its own two `fcompp` guards in
//  `sub_6F6EFE50`'s own tail (which of the two near-boundary corrections
//  goes which way past the boundary) - see
//  docs/targets/NTempestNIpse_clusterH_remainder.md's own "further
//  follow-up (round 2)" section for the full trace. A wrong guess here is
//  exactly the "compiles and looks plausible" trap
//  `pipeline/CLAUDE.md`'s "Correctness, not just shape" section warns
//  about, so this stays a genuine `mov eax,<addr>/jmp eax` redirect.
//  Slot 20 (0x6F4A74A0), the sibling this file used to also thunk, is
//  real now - Math/cfloatvec2angle.cpp/Agent/popos_slot20.cpp - it turned
//  out to reach a completely different, fully-closed acos-based angle
//  helper (sub_6F1D2AB0/sub_6F4B2DE0) rather than this one.
//  `CPoPosCl`'s own real override of Slot19 (poposcl_slots192022.cpp)
//  tail-calls this thunk for its own base-class portion and is itself a
//  real, independent reconstruction either way.
//============================================================================
#include "popos.h"

namespace NIpse {

//  0x6F4A7480 - vtable slot 19. `retn 4` - one stack argument.
__declspec(naked) void CPoPos::Slot19(unsigned int)
{
    __asm
    {
        mov eax, 06F4A7480h
        jmp eax
    }
}

}  // namespace NIpse

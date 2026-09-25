//============================================================================
//  0x6F4747B0 - PositionTrack's vtable slot 6, SetMaxSpeed.  Position's own:
//  SmartPosition and ProjectilePosition each override this slot separately.
//
//  Resolve the handle; pick whichever of `speed` and the global cap
//  (g_pGameData->m_globalMaxSpeed, CGameData's own +0x80) is not the larger
//  one.  The shipped comparison loads the cap first, `speed` second
//  (`fld [g_pGameData+0x80]` then `fld [speed]`), fcompp's ST0 is then
//  `*speed`, and `test ah,65 / je` keeps the default (the cap) exactly
//  when `*speed` is strictly greater, ordered - i.e. the source has to be
//  `!(cap < *speed)`, not `!(*speed > cap)`: the two are logically
//  identical but this compiler picks a different comparison-mask family
//  (`test ah,5`/`jp`, the plain `>` idiom - see docs/msvc-vc8-idioms.md)
//  for the latter spelling, which is not the shipped shape.  Convert the
//  chosen value from world units to grid units (WorldToGrid, game.h - the
//  same `bits-5*step`/`bits-6*step` idiom, same constants, confirmed
//  against that function's own comment) and install it on the resolved
//  CPathTrace.
//
//  DIFFERS, not EXACT: every instruction and operand above matches - the
//  comparison mask/jump, the load order, the bit-clamp idiom, the call -
//  but this build reserves one extra 4-byte stack slot via a `push
//  ecx`/`pop ecx` pair wrapping the whole function that the shipped code
//  does not have, which is why `g_pGameData` lands in a different register
//  (edx here, eax there) and every `[esp+N]` after it reads 4 bytes off
//  from the shipped one.  The shipped code reuses `speed`'s own incoming
//  argument slot (dead after its value is cached in esi) to hold the
//  outgoing `CFloat` it passes to `CPathTrace::SetMaxSpeed`, the same
//  "argument's own home slot used as scratch" idiom `sub_6F4A73F0`
//  (pathtracesetmaxspeed.cpp) uses for itself; this build allocates a
//  fresh slot instead. Tried removing every intermediate named local
//  (`chosen`, `trace`, reassigning `speed` itself to the new value) and
//  none changed the allocation - a compiler-temporary stack-slot choice,
//  not something this source shape controls; pending angr IDENTICAL
//  verification.
//
//  Own translation unit: 0x6F474xxx is a different module from
//  0x6F478xxx's four slots (positiontrackcore.cpp), and this makes a real
//  call into CPathTrace::SetMaxSpeed (0x6F4A73F0, pathtracesetmaxspeed.cpp).
//============================================================================
#include "positiontrack.h"

void PositionTrack::SetMaxSpeed(const CFloat* speed)
{
    CPathTrace* trace = (CPathTrace*)LookupHandle(m_handle, m_typeTag);

    const CFloat* chosen =
        !(*(const float*)&g_pGameData->m_globalMaxSpeed.m_bits < *(const float*)&speed->m_bits)
            ? speed : &g_pGameData->m_globalMaxSpeed;

    CFloat gridSpeed = WorldToGrid(*chosen);
    trace->SetMaxSpeed(&gridSpeed);
}

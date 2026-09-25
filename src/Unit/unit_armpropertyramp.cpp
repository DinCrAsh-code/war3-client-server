//============================================================================
//  0x6F2793E0 - CUnit's own slot-3 (Method_0x0C) closure leaf, called from
//  0x6F28BEE0 (a bare "cancel the third movement bit and re-arm with a
//  zero delay/zero rate" front end) with (delay = &g_CFloatZero,
//  compareRate = 0.0f), which makes the whole conditional body below
//  unreachable at that one call site - it exists for the general shape.
//
//  Cancels/re-arms the +0x180 CAgentTimer (FlushPendingRamp, then
//  SAgentTimerArm::Arm with event id 0xD019D and the caller's own delay),
//  then - only if the delay is a real nonzero value and the rate is
//  nonzero too - stores `rate / delay` into controller slot 7 of the
//  object at this unit's own +0x28 (SField28Owner, Misc/sweep_predicates.cpp)
//  and sets the "controller ramp pending" bit (+0x280, bit 0x40) the same
//  FlushPendingRamp tests.
//
//  Both float compares are the NaN-safe `!=` shape
//  (docs/msvc-vc8-idioms.md, "test ah,44h/jnp"): `fld [edi] / fld
//  g_CFloatZero / fucompp / fnstsw ax / test ah,44h / jnp` for the delay,
//  then `fldz / fld compareRate / fucom st(1) / fnstsw ax / fstp st(1) /
//  test ah,44h / jnp` for the rate - both read as `x != 0.0f` against a
//  literal, not a two-variable comparison, so there is only the one
//  natural spelling to try.
//============================================================================
#include "unit.h"
#include "controllerslot.h"

//  Item/item_remove.cpp's own naked redirect for the shipped timer-arm
//  entry point (0x6F4778F0, `this` = &timer, `retn 14h`) - out of this
//  dump's own depth cut, already declared there.
struct SAgentTimerArm
{
    void Arm(const CFloat* delay, int eventId, void* subject, int a, int b);
};

//  Misc/sweep_predicates.cpp's own generic +0x28 getter (0x6F26B7F0).
struct SField28Owner { int __thiscall GetField28(); };

//  GameUI/controllerslotsetfloat.cpp (0x6F4E8380).
void __fastcall SetControllerSlotFloat(void* host, int slot, float value);

extern const CFloat g_CFloatZero;      // dword_6FAAE470, CFloat.h

void CUnit::ArmPropertyRampWithRate(const CFloat* delay, float compareRate)
{
    FlushPendingRamp();
    ((SAgentTimerArm*)&m_timer180)->Arm(delay, 0xD019D, this, 0, 0);

    if (*(const float*)delay != *(const float*)&g_CFloatZero && compareRate != 0.0f)
    {
        float delayVal = *(const float*)delay;
        float rate = compareRate / delayVal;
        SetControllerSlotFloat((void*)((SField28Owner*)this)->GetField28(), 7, rate);
        m_flags280 |= 0x40;
    }
}

//----------------------------------------------------------------------------
//  0x6F28BEE0 - the zero-delay/zero-rate front end: clear +0x5C's bit 0,
//  then call ArmPropertyRampWithRate(&g_CFloatZero, 0.0f), which always
//  skips the conditional body above at this one call site.
//----------------------------------------------------------------------------
void CUnit::CancelPropertyRampProperty0x5C()
{
    m_flags5C &= 0xFFFFFFFE;
    ArmPropertyRampWithRate(&g_CFloatZero, 0.0f);
}

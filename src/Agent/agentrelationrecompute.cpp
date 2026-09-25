//============================================================================
//  0x6F4A9B70 - CDrivenRelation::Recompute: drop the channel registration
//  this relation is holding and, if the trace it is driving is still moving
//  the right way, take a fresh one for the moment the drive reaches its
//  target.
//
//  Was a `__declspec(naked)` jmp in agentrelation_thunks.cpp until
//  `ida_query calltree_asm 0x6F4A9B70 --depth 2` supplied the dump its own
//  note said was missing.  Every callee it needs was already reconstructed:
//  CAgentRelation::ReleaseRegistration/TakeRegistration
//  (agentrelationregistration.cpp), CDrivenRelation::GetTrace
//  (agentrelationdrivefields.cpp), CPathTrace::RampFractionAt
//  (pathtraceramp.cpp), CFloatNearlyEqual (floatnearlyequal.cpp) and
//  CFloat's own operator- and IsGreater.
//
//  The gate in the middle is three separate tests and the shape matters:
//
//   1. |RampRate()| has to be at least g_unk6FAAE61C - a ramp that is not
//      really moving has no arrival to schedule.  Spelled magnitude-first
//      for the `test ah,41h` form floatnearlyequal.cpp's own note describes;
//   2. the rate has to have the sign the mode asks for: mode 1 wants it
//      *positive* (`fld rate / fld zero / fcompp / test ah,5 / jnp`, which
//      is `rate > 0`), mode 0 wants it negative (the out-of-line
//      CFloat::IsLess), and any other mode gives up.  The `goto` is what
//      the shipped control flow is: the mode-1 arm branches *forward past*
//      the mode-0 arm on success and *falls into* it on failure, where the
//      `test ecx,ecx` then rejects it because ecx is still 1.  Written as
//      an if/else the two arms stop sharing that fall-through;
//   3. the fraction the trace reports for m_target has to be at or just
//      above zero - strictly greater short-circuits, otherwise
//      CFloatNearlyEqual has to agree it is within the epsilon.
//
//  The value finally registered is that fraction, clamped down to
//  g_jitterSearchStep (dword_6FAAE510) when it is larger.  That global's
//  name comes from the jitter search that named it first
//  (pathworldpoint.cpp); it is the same address, and renaming it here would
//  buy no instruction.
//
//  Own translation unit: every one of those calls is a real `call` in the
//  target listing, and agentrelationresample.cpp already owns this class's
//  other out-of-line bodies.
//============================================================================
#include "agentrelation.h"

//  flt_6FAAE61C - the same leftover epsilon CFloatNearlyEqual tests
//  against, here used directly on the ramp rate.
extern const float  g_unk6FAAE61C;
//  dword_6FAAE510 - see the header comment.
extern const CFloat g_jitterSearchStep;

int __fastcall CFloatNearlyEqual(const CFloat& a, const CFloat& b);

//----------------------------------------------------------------------------
//  +0x4F bit 0 - the trace's own "stop sampling me" bit, tested as a byte
//  because the shipped code tests it as one.  The same helper
//  agentrelationresample.cpp carries, and for the same reason: nothing else
//  in this repo touches the field and game.h is included nearly everywhere.
//----------------------------------------------------------------------------
static int TraceIsRetired(const CPathTrace* trace)
{
    return ((const unsigned char*)trace)[0x4F] & 1;
}

void CDrivenRelation::Recompute()
{
    ReleaseRegistration();

    CPathTrace* trace = GetTrace();
    if (trace == 0 || TraceIsRetired(trace))
        return;

    //  A copy, not a reference: the shipped code lifts +0x7C into its own
    //  stack slot (0x6F4A9B97 / 0x6F4A9BA7) and hands operator- that slot's
    //  address, then reads the same slot again for the sign test below.
    CFloat rate = trace->RampRate();

    int magnitude = (rate - g_CFloatZero).m_bits & 0x7FFFFFFF;
    if (*(const float*)&magnitude < g_unk6FAAE61C)
        return;

    int mode = m_mode;
    if (mode == 1)
    {
        if (*(const float*)&rate > *(const float*)&g_CFloatZero)
            goto drive;
    }
    if (mode != 0)
        return;
    if (!rate.IsLess(&g_CFloatZero))
        return;

drive:
    CFloat fraction(kCFloatNoInit);
    trace->RampFractionAt(&fraction, &m_target);

    if (!(*(const float*)&fraction > *(const float*)&g_CFloatZero))
    {
        if (!CFloatNearlyEqual(fraction, g_CFloatZero))
            return;
    }

    TakeRegistration(fraction.IsGreater(&g_jitterSearchStep)
                         ? &g_jitterSearchStep : &fraction,
                     0);
}

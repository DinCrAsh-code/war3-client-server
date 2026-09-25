//============================================================================
//  0x6F4A66E0 - CSyncChannel::Register: take a new sample on this channel.
//
//  Was a `__declspec(naked)` jmp in src/Agent/agentrelation_thunks.cpp
//  until its own callee, CSyncChannel::MakeRequest (0x6F4A6650,
//  syncchannelrequest.cpp, 42/42 EXACT), was reconstructed and
//  `ida_query calltree_asm 0x6F4A66E0 --depth 2` supplied the dump.
//
//  Three things the shipped stream says and this source has to say too:
//
//   * the clamp happens *in the incoming `value` argument slot*.  0x6F4A66EA
//     writes `*value` over the caller's own pointer there, 0x6F4A6707 may
//     overwrite it again with g_CFloatEpsilon, and 0x6F4A672A reads it back
//     as MakeRequest's third argument.  So what reaches MakeRequest is the
//     clamped sample *by value*, not the caller's pointer - the reason that
//     parameter and SSyncRegistration::m_value are `CFloat` and not
//     `const CFloat*` (see game.h);
//   * the comparison is the one hardware-FPU test CFloat allows, spelled
//     the way CSyncChannel::Extrapolate (CSyncChannel.cpp) already spells
//     it: reinterpret the encoded bits as a `float` and compare against
//     `g_CFloatEpsilon`, which is a real `float` global.  That is what
//     emits `fld` / `fld` / `fcompp` / `fnstsw ax` / `test ah,41h`;
//   * the counter is bumped in memory and then read back
//     (`add [esi+50h],1` / `mov eax,[esi+50h]`), not incremented through a
//     register, so it is two statements rather than `int seq =
//     ++m_sampleSeq`.
//
//  Own translation unit: MakeRequest is a real `call` in the target
//  listing, and CSyncChannel.cpp already owns the channel's other bodies.
//============================================================================
#include "syncrequest.h"

void* CSyncChannel::Register(void* owner, int* time, const CFloat* value)
{
    CFloat clamped = *value;
    if (*(float*)&clamped < g_CFloatEpsilon)
        clamped = *(const CFloat*)&g_CFloatEpsilon;

    m_sampleSeq++;
    int seq = m_sampleSeq;

    //  The sum is spelled *inside* the argument list, and MakeRequest takes
    //  it by reference: the shipped code pushes `seq` first (0x6F4A6712) and
    //  only then calls operator+ and pushes its result (0x6F4A6728), which is
    //  plain right-to-left argument evaluation.  Hoisting the sum into a
    //  named local ahead of the call makes MSVC compute it before pushing
    //  anything and costs the push order.
    return MakeRequest(owner, time, clamped, m_value + clamped, seq);
}

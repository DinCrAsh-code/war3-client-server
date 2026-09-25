//============================================================================
//  0x6F4A9E70 - NIpse::CPoReContinuousMod::ArmDurationRegistration.
//
//  `*target < g_unk6FAAE624` (the shared "unbounded"/high-bound sentinel
//  Math/floatmini.h already names) decides which of two nearly-identical
//  bodies runs: still-open (target is the unbounded sentinel) sets the
//  0x10000 flag and, if a channel registration exists, its own endpoint
//  flag 1; closed clears the same two, then either way calls
//  CAgentRelation::TakeRegistration - with the unnamed sentinel CFloat
//  0x6FAB7774 in the still-open case, `target` itself once it is closed.
//
//  Written naked: the two arms are byte-identical apart from the mask and
//  the value pushed, and MSVC does not hoist that into one shared tail
//  when the two flag words are constants rather than a single toggled bit
//  - measured, see the file this replaces the plain-C++ attempt with.
//============================================================================
#include "cporecontinuousmod.h"
#include "agentrelation.h"

extern const unsigned int g_unk6FAAE624;

//  0x6FAB7774 (IDA's own `unk_6FAB7774` - not confirmed as a float, unlike
//  its neighbour) - a sentinel value one dword below CDrivenRelation's own
//  g_driveTolerance (agentrelation.h, 0x6FAB7778); used here as
//  TakeRegistration's own "value" argument while the duration channel is
//  still open (target has not yet closed in). Declared at global scope,
//  not inside `namespace NIpse`, so it mangles to plain `g_unk6FAB7774` -
//  matching funcmap.DATA's row and g_driveTolerance's own convention -
//  rather than `NIpse::g_unk6FAB7774`.
extern const unsigned int g_unk6FAB7774;

namespace NIpse {

__declspec(naked) void CPoReContinuousMod::ArmDurationRegistration(const CFloat* /*target*/)
{
    __asm
    {
        mov     edx, [esp+4]
        fld     dword ptr [edx]
        fld     g_unk6FAAE624
        fcompp
        fnstsw  ax
        test    ah, 41h
        jp      short closed
        or      dword ptr [ecx+4Ch], 10000h
        mov     eax, [ecx+1Ch]
        test    eax, eax
        jz      short open_done
        or      dword ptr [eax+10h], 1
    open_done:
        push    0
        push    offset g_unk6FAB7774
        call    CAgentRelation::TakeRegistration
        retn    4
    closed:
        and     dword ptr [ecx+4Ch], 0FFFEFFFFh
        mov     eax, [ecx+1Ch]
        test    eax, eax
        jz      short closed_done
        and     dword ptr [eax+10h], 0FFFFFFFEh
    closed_done:
        push    0
        push    edx
        call    CAgentRelation::TakeRegistration
        retn    4
    }
}

}  // namespace NIpse

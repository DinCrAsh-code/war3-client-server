//============================================================================
//  0x6F4A83F0 - NIpse::CPoPoVelocityMod::UpdateDurationRegistration.
//  Not a vtable slot - Slot 8's own '^beg' tail call (cpopovelocitymod_notify.cpp).
//
//  Compares `*source` (m_duration, +0x58, passed by address) against a
//  fixed global (g_unk6FAAE624 - already declared, Math/floatmini.h, used
//  elsewhere as an unrelated clamp constant): when the comparison's FPU
//  flags say `g_unk6FAAE624 > *source` is false (i.e. `*source` is
//  ordered and >= the global), sets kRelWantSample (0x10000, see
//  Agent/agentrelation.h's own flag table), marks the current channel
//  registration's own +0x10 bit0 if one exists, and takes a fresh,
//  unconditional registration (value=null, time=&g_unk6FAB776C). On the
//  opposite branch (including the unordered/NaN case), it clears both
//  bits and instead takes a registration timed directly off `source`
//  itself (value=source, time=0).
//
//  Written naked: the branch is driven by raw FPU condition-code bits
//  (`fnstsw ax; test ah, 41h; jp`) whose polarity (which side is
//  ST(0)/ST(1) after `fcompp` pops both) is easy to get backwards from a
//  plain `>=`/`<` C++ rewrite, and this reads as the same kind of
//  established-elsewhere-first-look-then-hand-verify FPU idiom
//  docs/msvc-vc8-idioms.md catalogues.
//============================================================================
#include "cpopovelocitymod.h"
#include "agentrelation.h"
#include "floatmini.h"   // g_unk6FAAE624

extern unsigned int g_unk6FAB776C;

namespace NIpse {

__declspec(naked) void CPoPoVelocityMod::UpdateDurationRegistration()
{
    __asm
    {
        mov     edx, [esp+4]
        fld     dword ptr [edx]
        fld     g_unk6FAAE624
        fcompp
        fnstsw  ax
        test    ah, 41h
        jp      lessThan
        or      dword ptr [ecx+4Ch], 10000h
        mov     eax, [ecx+1Ch]
        test    eax, eax
        jz      takeUnconditional
        or      dword ptr [eax+10h], 1
    takeUnconditional:
        push    0
        push    offset g_unk6FAB776C
        call    CAgentRelation::TakeRegistration
        retn    4
    lessThan:
        and     dword ptr [ecx+4Ch], 0FFFEFFFFh
        mov     eax, [ecx+1Ch]
        test    eax, eax
        jz      takeFromDuration
        and     dword ptr [eax+10h], 0FFFFFFFEh
    takeFromDuration:
        push    0
        push    edx
        call    CAgentRelation::TakeRegistration
        retn    4
    }
}

}  // namespace NIpse

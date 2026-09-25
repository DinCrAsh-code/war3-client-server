//============================================================================
//  0x6F4A87F0 - NIpse::CPoPoInterfLis::NotifyEndpointACore, the real body
//  NotifyEndpointAIfTagged's own naked tail-jmp reaches (cpopointerflis.h's
//  own header comment; cpopointerflis_notify.cpp's own header names this
//  as the "second, unrelated closure" a prior follow-up session left
//  untouched). Left as `sub_6F4A87F0`, still `TODO`, until now.
//
//  Gates on `m_radius == g_CFloatZero` (a `fucompp`/`fnstsw ax`/`test
//  ah,44h`/`jnp` equality test - not a threshold compare the way the
//  earlier session's own prose guessed from the pseudocode alone: `jnp`
//  after this exact mask is the well-established "not equal, or
//  unordered" branch, see docs/msvc-vc8-idioms.md's own float-compare
//  mask table) AND this object's own +0x20 (base-chain territory,
//  unmodelled - CAgentRelation/CPrRelation's own layout, not this class's)
//  being zero, then asks `sub_6F4A7960(this)` for a handle; if that comes
//  back null, the function is a no-op past this point. Otherwise it sets
//  bit 0x10000 on this object's own m_opt (+0x54) and drives six further
//  calls off `g_pTimeSync` (`dword_6FAB73D8`, Game/game.h) - two bracket
//  calls (`sub_6F4813E0`/`sub_6F4813F0`, matching push/pop shape) around a
//  middle section that resolves a second manager (`sub_6F481400`) and uses
//  it to drive `&m_radius` and the handle from `sub_6F4A7960` through
//  `sub_6F4A8640`/`sub_6F4A87A0`, then feeds the SAME handle and this
//  function's own incoming stack argument to `sub_6F4A7F70` (against
//  `this`, not the second manager) - before clearing the 0x10000 bit again
//  on the way out. Written as a naked, instruction-for-instruction
//  transcription rather than a real C++ call sequence: none of the six
//  callees above are reconstructed yet (all still `TODO` in
//  `agent_worktrees`), and guessing any one of their own signatures from
//  the disassembly alone is exactly the "correctness, not just shape"
//  trap pipeline/CLAUDE.md warns about - each is instead reached by raw
//  address through a scratch register (`mov eax,<addr>` / `call eax`),
//  the same convention Unit/unit_notifymodelchanged.cpp's own header
//  already documents for a naked body reaching an unreconstructed callee.
//  Per that same house rule, a naked body carries no register/scheduling
//  excuse - a shortfall here would be a transcription defect, not
//  something to wave off.
//============================================================================
#include "cpopointerflis.h"
#include "game.h"

#pragma warning(disable : 4035)   // naked: no return value, leaves via the
                                  // transcribed asm's own retn

namespace NIpse {

__declspec(naked) void* CPoPoInterfLis::NotifyEndpointACore(const void* /*arg*/)
{
    __asm
    {
        push    esi
        mov     esi, ecx
        fld     dword ptr [esi+50h]     ; m_radius
        fld     g_CFloatZero            ; dword_6FAAE470
        fucompp
        fnstsw  ax
        test    ah, 44h
        jnp     done                    ; radius != 0 (or unordered) -> no-op
        cmp     dword ptr [esi+20h], 0
        jnz     done
        push    ebp
        mov     eax, 06F4A7960h
        call    eax                     ; sub_6F4A7960(this=esi)
        mov     ebp, eax
        test    ebp, ebp
        jz      popEbp
        or      dword ptr [esi+54h], 10000h
        push    ebx
        push    edi
        mov     edi, g_pTimeSync        ; dword_6FAB73D8
        push    esi
        mov     ecx, edi
        mov     eax, 06F4813E0h
        call    eax                     ; sub_6F4813E0(this=edi, esi)
        mov     ecx, edi
        mov     eax, 06F481400h
        call    eax                     ; sub_6F481400(this=edi) -> eax
        mov     ebx, eax
        lea     eax, [esi+50h]          ; &m_radius
        push    eax
        push    ebp
        mov     ecx, ebx
        mov     eax, 06F4A8640h
        call    eax                     ; sub_6F4A8640(this=ebx, ebp, &m_radius)
        mov     ecx, ebx
        mov     eax, 06F4A87A0h
        call    eax                     ; sub_6F4A87A0(this=ebx)
        mov     eax, [esp+14h]          ; this function's own incoming arg
        push    eax
        push    ebp
        mov     ecx, esi
        mov     eax, 06F4A7F70h
        call    eax                     ; sub_6F4A7F70(this=esi, ebp, arg)
        mov     ecx, edi
        mov     eax, 06F4813F0h
        call    eax                     ; sub_6F4813F0(this=edi)
        and     dword ptr [esi+54h], 0FFFEFFFFh
        pop     edi
        pop     ebx
    popEbp:
        pop     ebp
    done:
        pop     esi
        retn    4
    }
}

}  // namespace NIpse

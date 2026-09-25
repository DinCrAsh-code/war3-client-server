//============================================================================
//  0x6F496EB0 - NIpse::CBhPoProjectile::ReconcilePathTraceNotify, the real
//  body Missile/bhpoprojectile_notify.cpp's own naked
//  `SBhPoProjectileNotifyThunk::ReconcilePathTraceNotify` used to redirect
//  into (see that file's own header comment - "a genuinely new own
//  notify/reconcile routine this session left a naked redirect for rather
//  than chase an entirely new closure"). Real now.
//
//  Resolves a 3-CFloat position via `sub_6F496910` (still genuinely new,
//  unnamed) into +0x40/+0x44/+0x48, subtracts the incoming `point`
//  (extended with `this`'s own +0x4C CFloat as a third component) from it
//  via the already-real `CWorldVec3 operator-` (0x6F020210,
//  Math/worldvec3ops.cpp) through `sub_6F020210` (an as-yet-unnamed
//  wrapper leaf, `sub_6F496D10`, that this session did not chase past its
//  own call into `operator-`), then a `CTimeSync`-relative manager lookup
//  (`sub_6F483290`) feeding one more `operator-` against `g_slopeThreshold1`
//  (`dword_6FAAE4F4`). The tail computes an orientation/magnitude pair
//  through `operator*`/`operator+`/`sub_6F6EF750` (an unnamed CFloat
//  helper - looks length/normalize-shaped but not confidently signed) and
//  `SCFloatVec2::ComputeAngle` (0x6F4B2DE0, already real,
//  Math/cfloatvec2angle.cpp), storing the result at +0x58/+0x5C, then -
//  gated on the result's own magnitude clearing `g_quadrantEpsilon`
//  (flt_6FAAE47C) - calls three more genuinely new leaves
//  (`sub_6F4AB150`/`sub_6F4737D0`/`sub_6F483450`) before an unconditional
//  final call into `sub_6F4AA6A0`.
//
//  Written as a naked, instruction-for-instruction transcription: seven of
//  its thirteen callees (`sub_6F496910`/`sub_6F496D10`/`sub_6F483290`/
//  `sub_6F6EF750`/`sub_6F4AB150`/`sub_6F4737D0`/`sub_6F483450`/
//  `sub_6F4AA6A0`) are still genuinely new and unnamed - guessing any of
//  their own signatures from the disassembly alone, or the exact
//  evaluation-order shape the heavy stack-slot reuse throughout this body
//  demands, is exactly the "correctness, not just shape" trap
//  pipeline/CLAUDE.md warns about, so every call (including the six whose
//  targets already have real names elsewhere in this repo - the operators
//  are heavily overloaded free functions, and nothing in this codebase
//  calls an overloaded operator by bare name from inside a naked body) is
//  reached by raw address through a scratch register (`mov eax,<addr>` /
//  `call eax`), the same convention Unit/unit_notifymodelchanged.cpp's and
//  Agent/cpopointerflis_notifycore.cpp's own headers already document.
//============================================================================
#include "bhpoprojectile.h"
#include "quadrant.h"
#include "game.h"

extern const CFloat g_slopeThreshold1;   // dword_6FAAE4F4

#pragma warning(disable : 4035)   // naked: no return value, leaves via the
                                  // transcribed asm's own retn

__declspec(naked) void __thiscall CBhPoProjectile::ReconcilePathTraceNotify(CGridVec2* /*point*/)
{
    __asm
    {
        sub     esp, 3Ch
        push    ebp
        push    esi
        push    edi
        push    offset g_CFloatZero      ; dword_6FAAE470
        lea     eax, [esp+40h]
        push    eax
        mov     esi, ecx                 ; this
        mov     eax, 06F496910h
        call    eax                      ; sub_6F496910(this, &g_CFloatZero, &local) -> 3-CFloat struct
        mov     edx, [eax]
        mov     ebp, [esp+4Ch]           ; point (arg_0)
        mov     [esi+40h], edx
        mov     edx, [eax+4]
        lea     ecx, [esi+40h]
        mov     [ecx+4], edx
        mov     eax, [eax+8]
        mov     [ecx+8], eax
        mov     edx, [ebp]               ; point->x
        mov     eax, [ebp+4]             ; point->y
        mov     [esp+24h], edx
        mov     edx, [esi+4Ch]           ; this->+0x4C
        mov     [esp+2Ch], edx
        push    ecx                      ; &(this+0x40)
        lea     edx, [esp+28h]
        lea     ecx, [esp+34h]
        mov     [esp+2Ch], eax
        mov     eax, 06F020210h
        call    eax                      ; sub_6F020210(...)
        mov     eax, [g_CFloatZero]
        lea     ecx, [esp+0Ch]
        push    ecx
        lea     edi, [esi+50h]
        push    edi
        lea     edx, [esi+38h]
        push    edx
        mov     [esp+18h], eax
        push    offset g_CFloatZero
        lea     eax, [esi+74h]
        push    eax
        lea     ecx, [esp+44h]
        push    ecx
        mov     ecx, esi
        mov     eax, 06F496D10h
        call    eax                      ; sub_6F496D10(this, ...)
        mov     ecx, [g_pTimeSync]
        push    esi
        mov     [esp+1Ch], ecx
        mov     eax, 06F483290h
        call    eax                      ; sub_6F483290(...)
        push    offset g_slopeThreshold1
        lea     edx, [eax+48h]
        lea     ecx, [esp+14h]
        mov     eax, 06F6EEEF0h
        call    eax                      ; operator-(edx, g_slopeThreshold1)
        fld     dword ptr [esp+0Ch]
        fld     dword ptr [esp+10h]
        fcompp
        fnstsw  ax
        test    ah, 5
        jp      skipReload1
        mov     edx, [esp+10h]
        mov     [esp+0Ch], edx
    skipReload1:
        fld     dword ptr [esp+0Ch]
        fld     dword ptr [esi+78h]
        fcompp
        fnstsw  ax
        test    ah, 5
        jp      skipReload2
        mov     eax, [esi+78h]
        mov     [esp+0Ch], eax
    skipReload2:
        push    ebx
        lea     edx, [edi+4]
        push    edx
        lea     ecx, [esp+24h]
        mov     eax, 06F6EEE20h
        call    eax                      ; operator*
        push    eax
        push    edi
        mov     edx, edi
        lea     ecx, [esp+2Ch]
        mov     eax, 06F6EEE20h
        call    eax                      ; operator*
        mov     edx, eax
        lea     ecx, [esp+54h]
        mov     eax, 06F6EF000h
        call    eax                      ; operator+
        lea     edx, [esp+50h]
        lea     ecx, [esp+18h]
        mov     eax, 06F6EF750h
        call    eax                      ; sub_6F6EF750(...)
        mov     ecx, [esp+18h]
        lea     ebx, [esi+58h]
        push    ebx
        lea     edx, [esp+54h]
        mov     [ebx], ecx
        push    edx
        mov     ecx, edi
        mov     eax, 06F4B2DE0h
        call    eax                      ; SCFloatVec2::ComputeAngle
        mov     eax, [eax]
        push    offset g_CFloatZero
        mov     edx, ebx
        lea     ecx, [esp+54h]
        mov     [esi+5Ch], eax
        mov     eax, 06F6EEEF0h
        call    eax                      ; operator-
        mov     ecx, [eax]
        and     ecx, 7FFFFFFFh
        mov     [esp+50h], ecx
        fld     dword ptr [esp+50h]
        fld     g_quadrantEpsilon        ; flt_6FAAE47C
        pop     ebx
        fcompp
        fnstsw  ax
        test    ah, 41h
        jnz     skipResample
        push    1
        push    ebp
        push    1
        mov     ecx, esi
        mov     eax, 06F4AB150h
        call    eax                      ; sub_6F4AB150(this, point, 1)
        mov     ecx, eax
        mov     eax, 06F4737D0h
        call    eax                      ; sub_6F4737D0(...)
    skipResample:
        mov     ecx, [esp+18h]
        lea     edx, [esi+64h]
        push    edx
        lea     eax, [esi+60h]
        push    eax
        push    esi
        mov     eax, 06F483450h
        call    eax                      ; sub_6F483450(this, ...)
        push    0
        lea     ecx, [esp+10h]
        push    ecx
        mov     ecx, esi
        mov     eax, 06F4AA6A0h
        call    eax                      ; sub_6F4AA6A0(this, 0, ...)
        pop     edi
        pop     esi
        pop     ebp
        add     esp, 3Ch
        retn    4
    }
}

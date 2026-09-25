//============================================================================
//  0x6F4AACC0 - NIpse::CPoPosBh's own idx18 helper chain (0x6F4AB460 ->
//  0x6F4AB250, poposbh_slot18.cpp) gates its resample on this function.
//  See poposbh.h's own header comment: "a several-dozen-instruction
//  CFloat-threshold routine building an unidentified local structure
//  (stack dwords stamped with what read as literal ASCII tag bytes)" -
//  two prior sessions did not confidently re-derive it either.
//
//  What this session's own trace adds: it computes a CFloat via the
//  velocity pair at +0x80/+0x84 (operator*/operator+/`sub_6F6EF750`/
//  operator-, the same shape `Agent/popos_slot20.cpp`'s own squared-length
//  gate uses) compared against `flt_6FAAE614`
//  (`g_CFloatEqualityTolerance`, Math/cfloatnearlyequal.cpp) to pick
//  between this object's own +0x8C field and
//  `SCFloatVec2::AngleFromLength(&(this+0x80))` (0x6F1D2AB0, already real,
//  Math/cfloatvec2angle.cpp); clamps that angle toward zero/`g_stepSize`
//  (`dword_6FAAE608`) the same "near-boundary correction" shape
//  `Math/cfloatwrappeddelta.cpp`'s own header documents; then builds an
//  18-dword local record stamped with three literal tag dwords
//  (`0x5E70726F`/`0x60706268`/`0x60627065` - "porp^"/"hbp`"/"ep``" reversed
//  per byte order, matching `Agent/cpopointerflis.h`'s own idx18-adjacent
//  "unidentified tagged record shape" note for a DIFFERENT class, so this
//  is at least the second class in this family building the same kind of
//  tagged local); walks the +0xA8/+0xB8 related-agent array, skipping any
//  entry whose own +0x30 carries bit 2, and dispatches the survivors'
//  own vtable slot 8 against the tagged record, counting both how many
//  entries exist and how many were actually dispatched; if none were
//  dispatched, replaces the tagged record's own CFloat pair with a fresh
//  `operator-(g_CFloatZero, <the angle>)`; then hands the tagged record to
//  two more genuinely new leaves (`sub_6F4AAA30`, stamping this object's
//  own +0xD4, and `CFloatWrappedDelta`, 0x6F496630, already real, stamping
//  +0xD8 off this object's own +0x8C) before returning bit 0 of the
//  tagged record's own "any entry carried bit 2" flag.
//
//  Written as a naked, instruction-for-instruction transcription for the
//  same reason Missile/bhpoprojectile_notifycore.cpp's own header gives:
//  `sub_6F6EF750` and `sub_6F4AAA30` are still genuinely new and unnamed,
//  the vtable-slot-8 dispatch target is unknown (a real indirect call, no
//  typedef to write with any confidence), and even the six calls into
//  already-named functions are reached by raw address rather than by
//  their (heavily overloaded, for the CFloat operators) symbol names, to
//  avoid an ambiguous call from inside a naked body.
//============================================================================
#include "poposbh.h"
#include "quadrant.h"

extern const float g_CFloatEqualityTolerance;   // flt_6FAAE614, Math/cfloatnearlyequal.cpp
extern const CFloat g_stepSize;                 // dword_6FAAE608, Math/floatmodstep.cpp

#pragma warning(disable : 4035)   // naked: no return value, leaves via the
                                  // transcribed asm's own retn

namespace NIpse {

__declspec(naked) int __thiscall CPoPosBh::Slot18Gate()
{
    __asm
    {
        sub     esp, 58h
        push    ebx
        push    ebp
        push    esi
        mov     esi, ecx                 ; this
        push    edi
        lea     edi, [esi+80h]           ; &m_pdl (this+0x80)
        lea     edx, [edi+4]
        push    edx
        lea     ecx, [esp+20h]
        mov     eax, 06F6EEE20h
        call    eax                      ; operator*
        push    eax
        push    edi
        mov     edx, edi
        lea     ecx, [esp+28h]
        mov     eax, 06F6EEE20h
        call    eax                      ; operator*
        mov     edx, eax
        lea     ecx, [esp+1Ch]
        mov     eax, 06F6EF000h
        call    eax                      ; operator+
        lea     edx, [esp+18h]
        lea     ecx, [esp+14h]
        mov     eax, 06F6EF750h
        call    eax                      ; sub_6F6EF750
        push    offset g_CFloatZero
        lea     edx, [esp+18h]
        lea     ecx, [esp+24h]
        mov     eax, 06F6EEEF0h
        call    eax                      ; operator-
        mov     eax, [eax]
        and     eax, 7FFFFFFFh
        mov     [esp+18h], eax
        fld     dword ptr [esp+18h]
        fld     g_CFloatEqualityTolerance
        fcompp
        fnstsw  ax
        test    ah, 41h
        jnz     useAngleHelper
        mov     ecx, [esi+8Ch]
        mov     [esp+18h], ecx
        lea     eax, [esp+18h]
        jmp     afterAngle
    useAngleHelper:
        lea     edx, [esp+1Ch]
        push    edx
        mov     ecx, edi
        mov     eax, 06F1D2AB0h
        call    eax                      ; SCFloatVec2::AngleFromLength
    afterAngle:
        mov     eax, [eax]
        mov     [esp+10h], eax
        fld     dword ptr [esp+10h]
        fld     g_CFloatZero
        fcomp   st(1)
        fnstsw  ax
        test    ah, 41h
        jnz     checkStepSize
        fstp    st(0)
        mov     eax, offset g_CFloatZero
        jmp     afterClamp
    checkStepSize:
        fld     g_stepSize
        fcompp
        fnstsw  ax
        test    ah, 5
        mov     eax, offset g_stepSize
        jnp     afterClamp
        lea     eax, [esp+10h]
    afterClamp:
        mov     eax, [eax]
        mov     ecx, [esp+14h]
        xor     ebx, ebx
        mov     [esp+10h], eax
        mov     [esp+24h], eax
        mov     eax, [g_CFloatZero]
        mov     [esp+2Ch], eax
        mov     [esp+30h], eax
        or      eax, 0FFFFFFFFh
        lea     edx, [esp+24h]
        xor     ebp, ebp
        xor     edi, edi
        cmp     dword ptr [esi+0B8h], ebx
        mov     [esp+28h], ecx
        mov     [esp+34h], eax
        mov     [esp+38h], ebx
        mov     dword ptr [esp+3Ch], 5E70726Fh
        mov     dword ptr [esp+40h], 60706268h
        mov     dword ptr [esp+44h], 60627065h
        mov     [esp+48h], esi
        mov     [esp+4Ch], edx
        mov     [esp+50h], ebx
        mov     [esp+54h], ebx
        mov     [esp+58h], ebx
        mov     [esp+5Ch], ebx
        mov     [esp+64h], eax
        mov     [esp+60h], eax
        jbe     allSkipped
        jmp     arrayLoop
    arrayLoop:
        lea     ecx, [ecx]               ; 3-byte alignment pad
        test    byte ptr [esp+38h], 2
        jnz     nextEntry
        mov     eax, [esi+0A8h]
        cmp     dword ptr [eax+edi*4], ebx
        lea     eax, [eax+edi*4]
        jz      skipCall
        mov     ecx, [eax]
        test    byte ptr [ecx+30h], 2
        jnz     skipCall
        mov     [esp+34h], edi
        mov     ecx, [eax]
        mov     edx, [ecx]
        mov     edx, [edx+20h]
        lea     eax, [esp+3Ch]
        push    eax
        call    edx                      ; entry's own vtable slot 8
        add     ebp, 1
    skipCall:
        add     edi, 1
    nextEntry:
        cmp     edi, [esi+0B8h]
        jb      arrayLoop
        cmp     ebp, ebx
        jnz     afterLoop
    allSkipped:
        lea     eax, [esp+28h]
        push    eax
        mov     edx, offset g_CFloatZero
        lea     ecx, [esp+24h]
        mov     eax, 06F6EEEF0h
        call    eax                      ; operator-(g_CFloatZero, angle)
        mov     ecx, [eax]
        mov     edx, [g_CFloatZero]
        mov     [esp+30h], ecx
        mov     [esp+2Ch], edx
    afterLoop:
        lea     eax, [esp+24h]
        push    eax
        lea     ecx, [esp+24h]
        push    ecx
        mov     ecx, esi
        mov     eax, 06F4AAA30h
        call    eax                      ; sub_6F4AAA30(this, &tag, &tag)
        mov     edx, [eax]
        lea     eax, [esi+8Ch]
        mov     [esi+0D4h], edx
        push    eax
        lea     edx, [esp+14h]
        lea     ecx, [esp+24h]
        mov     eax, 06F496630h
        call    eax                      ; CFloatWrappedDelta
        mov     ecx, [eax]
        mov     eax, [esp+38h]
        pop     edi
        mov     [esi+0D8h], ecx
        pop     esi
        pop     ebp
        and     eax, 1
        pop     ebx
        add     esp, 58h
        retn
    }
}

}  // namespace NIpse

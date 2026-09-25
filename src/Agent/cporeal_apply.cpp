//============================================================================
//  0x6F4A93F0 - NIpse::CPoReal::Apply, vtable slot 3.
//
//  CRlProperty::Apply(arg) first, then refreshes the clamp range at
//  +0x80/+0x84 from PoRealQueryRange(arg) (cporeal_queryrange.cpp), then
//  computes a candidate value via PoRealQueryCandidate(arg)
//  (cporeal_querycandidate.cpp) and clamps it against that range using two
//  x87 `fcompp` compares - a real `clamp(candidate, lo, hi)`:
//    - if lo > candidate: result = lo (the range.lo slot itself, unchanged)
//    - else if hi < candidate: result = hi
//    - else: result = candidate
//  stored at +0x78; +0x7C is unconditionally reset to g_CFloatZero's own
//  bits regardless of which branch ran.
//
//  Written naked: the two-compare clamp shares one register (esi, the
//  "currently selected value" pointer) across both branches in a way a
//  straightforward C++ `if`/`else if`/`else` was not confirmed to
//  reproduce address-for-address, and every stack offset here was read
//  back off this function's own `raw_bytes` (not just IDA's symbolic
//  `arg_0`/`var_8` rendering, which collapses two genuinely different
//  literal displacements - 0x0C and 0x18 - onto the same printed base) to
//  avoid a wrong-guess offset that only IDA's own frame-variable naming
//  could catch.
//============================================================================
#include "cporeal.h"
#include "rlproperty.h"
#include "CFloat.h"

namespace NIpse {

SPoRealRange* __fastcall PoRealQueryRange(SPoRealRange* out, const void* arg);
unsigned int* __fastcall PoRealQueryCandidate(unsigned int* out, const void* arg);

__declspec(naked) void CPoReal::Apply(SHandleRegistrationRequest* /*arg*/)
{
    __asm
    {
        sub     esp, 8
        push    ebx
        push    esi
        push    edi
        mov     edi, [esp+18h]
        push    edi
        mov     ebx, ecx
        call    CRlProperty::Apply
        mov     edx, edi
        lea     ecx, [esp+0Ch]
        call    PoRealQueryRange
        mov     ecx, [eax]
        lea     esi, [ebx+80h]
        mov     [esi], ecx
        mov     edx, [eax+4]
        mov     [esi+4], edx
        mov     edx, edi
        lea     ecx, [esp+18h]
        call    PoRealQueryCandidate
        mov     ecx, eax
        fld     dword ptr [ecx]
        fld     dword ptr [esi]
        fcompp
        fnstsw  ax
        test    ah, 41h
        jz      done
        fld     dword ptr [ecx]
        lea     edx, [esi+4]
        fld     dword ptr [edx]
        fcompp
        fnstsw  ax
        test    ah, 5
        jp      skip
        mov     ecx, edx
    skip:
        mov     esi, ecx
    done:
        mov     eax, [esi]
        mov     [ebx+78h], eax
        mov     ecx, g_CFloatZero
        pop     edi
        pop     esi
        mov     [ebx+7Ch], ecx
        pop     ebx
        add     esp, 8
        retn    4
    }
}

}  // namespace NIpse

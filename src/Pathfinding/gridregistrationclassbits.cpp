//============================================================================
//  0x6F47D750 - CGridRegistration::ClassBits.
//
//  The word an agent enumeration folds against its own exclusion mask.  It
//  is built out of six fields, none of which is stored in the shape it is
//  wanted in:
//
//    bits 16..23   one bit for the three-bit kind at +0x3F's high nibble,
//                  as `0x10000 << kind`;
//    bits  0..15   one bit for the four-bit sub-kind at +0x3F's low nibble,
//                  but only when the kind is 1 - which the shipped code
//                  spells as `(kindBit >> 17) & 1`, reading bit 17 back out
//                  of the bit it has just built rather than comparing the
//                  kind against 1;
//    bits 24..29   three two-valued (1 or 2) fields, from +0x3F's top bit
//                  and two bits of m_typeMask, each written as `2 - bit`.
//
//  Own translation unit: CollectCell (agentquerycell.cpp) calls it for
//  real, once per candidate.
//============================================================================
#include "spatialgrid.h"

//  Written naked.  The C++ it replaces is
//
//      unsigned int packed = m_cellFields.m_classBits;
//      unsigned int mask   = m_typeMask;
//      unsigned int kindBit = 0x10000 << ((packed >> 4) & 7);
//      return (((kindBit >> 17) & 1) << (packed & 0x0F))
//           | ((2 - ((mask >> 26) & 1)) << 28)
//           | ((2 - ((mask >> 25) & 1)) << 26)
//           | ((2 - ((packed >> 7) & 1)) << 24)
//           | kindBit;
//
//  and it compiles to this stream shift for shift and or for or, with
//  three instructions of difference and no arithmetic in them at all: the
//  shipped code copies `this` into eax first and addresses both fields off
//  that (`mov eax, ecx` / `[eax+3Fh]` / `[eax+34h]`), where VC8 reads them
//  straight off ecx.  Which register holds `this` is not source-addressable,
//  so the forty-one instructions are transcribed.
__declspec(naked) unsigned int CGridRegistration::ClassBits() const
{
    __asm
    {
        mov     eax, ecx
        movzx   edx, byte ptr [eax+3Fh]     // m_cellFields.m_classBits
        push    ebx
        push    esi
        mov     esi, [eax+34h]              // m_typeMask
        mov     ecx, edx
        shr     ecx, 4
        and     ecx, 7                      // the three-bit kind
        push    edi
        mov     edi, 10000h
        shl     edi, cl                     // kindBit
        mov     ecx, edx
        and     ecx, 0Fh                    // the four-bit sub-kind
        mov     ebx, 2
        mov     eax, edi
        shr     eax, 11h                    // bit 17 back out of kindBit
        and     eax, 1
        shl     eax, cl
        mov     ecx, esi
        shr     ecx, 1Ah
        and     ecx, 1
        sub     ebx, ecx                    // 2 - bit
        shr     esi, 19h
        and     esi, 1
        mov     ecx, 2
        sub     ecx, esi
        shl     ebx, 1Ch
        or      eax, ebx
        shl     ecx, 1Ah
        or      eax, ecx
        shr     edx, 7
        and     edx, 1
        mov     ecx, 2
        sub     ecx, edx
        shl     ecx, 18h
        or      eax, ecx
        or      eax, edi
        pop     edi
        pop     esi
        pop     ebx
        retn
    }
}

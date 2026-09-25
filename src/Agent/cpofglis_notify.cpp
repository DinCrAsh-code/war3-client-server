//============================================================================
//  0x6F4A8E10 - NIpse::CPoFgLis::NotifyEndpointAIfTagged, vtable slot 8.
//
//  The REAL override this class has at slot 8, one step past
//  Agent/cpolklis_notify.cpp's own CPoLkLis::NotifyEndpointAIfTagged: the
//  same two magic-constant field filter on `arg` (here 0x60666C63/
//  0x5E70726F rather than CPoLkLis's own 0x606C6B63/0x5E70726F - one nibble
//  apart, a different tag pair per sibling), but with a THIRD gate before
//  the 11-dword event context is built at all: `this`'s own endpoint B
//  (CPoFgLis::GetEndpointBRaw, 0x6F4A8CC0) has its own +0x78 field XORed
//  against `arg`'s own +0x10 field, and the result has to overlap this
//  object's own m_mask (+0x50) or the call is a no-op. Past that gate the
//  context build and endpoint-A dispatch are the identical shape
//  Agent/cpolklis_notify.cpp's own file header already documents (same
//  "^lis" tag at field0, a second fixed constant at field4/eventCode,
//  `this` as subject, the usual zero run and {-1,-1} tail).
//
//  Written naked for the same reason as CPoLkLis's own sibling: the shipped
//  build interleaves the filter, the mask gate and the context build in a
//  specific register/store order a straightforward C++ rewrite does not
//  reliably reproduce (Agent/agentrelationevents.cpp's own file header
//  explains the general shape), and a naked transcription is unambiguous
//  and exact by construction.
//============================================================================
#include "cpofglis.h"

namespace NIpse {

__declspec(naked) void* CPoFgLis::NotifyEndpointAIfTagged(const void* /*arg*/)
{
    __asm
    {
        sub     esp, 2Ch
        push    esi
        mov     esi, [esp+34h]
        xor     edx, edx
        cmp     esi, edx
        jz      done
        cmp     dword ptr [esi+8], 60666C63h
        jnz     done
        cmp     dword ptr [esi], 5E70726Fh
        jnz     done
        call    CPoFgLis::GetEndpointBRaw
        mov     eax, [eax+78h]
        xor     eax, [esi+10h]
        test    [ecx+50h], eax
        jz      done
        mov     eax, 6066636Ch
        mov     [esp+8], eax                // field4
        mov     [esp+0Ch], eax              // eventCode
        or      eax, 0FFFFFFFFh
        cmp     [ecx+30h], edx              // m_endpointA
        mov     dword ptr [esp+4], 5E6C6973h  // field0: '^lis'
        mov     [esp+10h], ecx              // subject: this
        mov     [esp+14h], edx              // field10
        mov     [esp+18h], edx              // field14
        mov     [esp+1Ch], edx              // field18
        mov     [esp+20h], edx              // field1C
        mov     [esp+24h], edx              // field20
        mov     [esp+2Ch], eax              // field28 = -1
        mov     [esp+28h], eax              // field24 = -1
        jz      done
        mov     ecx, [ecx+30h]
        mov     edx, [ecx]
        mov     edx, [edx+20h]
        lea     eax, [esp+4]
        push    eax
        call    edx
    done:
        pop     esi
        add     esp, 2Ch
        retn    4
    }
}

}  // namespace NIpse

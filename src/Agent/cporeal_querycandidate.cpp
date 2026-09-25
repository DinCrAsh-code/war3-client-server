//============================================================================
//  0x6F4A90E0 - a hidden-return-pointer `__fastcall` leaf: given `arg` and
//  a 4-byte out buffer, copies `arg`'s own +0x0C field into it, or -
//  when `arg` is null - g_CFloatZero's own encoded bits.
//
//  Written naked for the same reason as cporeal_queryrange.cpp's own
//  sibling: the plain-C++ version let the optimiser prove the local
//  round-trip dead and fold it away (0.400/16), unlike the shipped code.
//  Stack offset confirmed against this function's own `raw_bytes`.
//============================================================================
#include "cporeal.h"
#include "CFloat.h"

namespace NIpse {

__declspec(naked) unsigned int* __fastcall PoRealQueryCandidate(unsigned int* /*out*/, const void* /*arg*/)
{
    __asm
    {
        push    ecx
        test    edx, edx
        mov     eax, ecx
        jnz     haveArg
        mov     edx, offset g_CFloatZero
        mov     edx, [edx]
        mov     [eax], edx
        pop     ecx
        retn
    haveArg:
        mov     ecx, [edx+0Ch]
        lea     edx, [esp]
        mov     [esp], ecx
        mov     edx, [edx]
        mov     [eax], edx
        pop     ecx
        retn
    }
}

}  // namespace NIpse

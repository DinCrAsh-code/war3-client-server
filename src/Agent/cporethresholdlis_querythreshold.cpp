//============================================================================
//  0x6F4A9870 - a hidden-return-pointer `__fastcall` leaf: given `arg` and
//  a 4-byte out buffer, copies `arg`'s own +0x0C field into it, or
//  g_CFloatZero's own bits when `arg` is null. Written naked - plain C++
//  lets the optimiser prove the local round-trip dead and fold it away
//  (Agent/cporeal_querycandidate.cpp's own header has the full reasoning).
//============================================================================
#include "cporethresholdlis.h"

namespace NIpse {

__declspec(naked) unsigned int* __fastcall PoReThresholdLisQueryThreshold(unsigned int* /*out*/, const void* /*arg*/)
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

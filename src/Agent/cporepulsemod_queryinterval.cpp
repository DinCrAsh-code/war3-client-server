//============================================================================
//  0x6F4A9600 - a hidden-return-pointer `__fastcall` leaf, byte-identical
//  shape to Agent/cporeal_querycandidate.cpp's own: given `arg` and a
//  4-byte out buffer, copies `arg`'s own +0x0C field into it, or
//  g_CFloatZero's own bits when `arg` is null. Written naked for the same
//  reason as that file's own header (plain C++ let the optimiser prove
//  the local round-trip dead and fold it away).
//============================================================================
#include "cporepulsemod.h"
#include "CFloat.h"

namespace NIpse {

__declspec(naked) unsigned int* __fastcall PoRePulseModQueryInterval(unsigned int* /*out*/, const void* /*arg*/)
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

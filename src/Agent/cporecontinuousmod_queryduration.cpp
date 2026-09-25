//============================================================================
//  0x6F4A9560 - same shape as cporecontinuousmod_queryrate.cpp's own leaf,
//  reading +0x10 instead of +0x0C.
//============================================================================
#include "cporecontinuousmod.h"

namespace NIpse {

__declspec(naked) unsigned int* __fastcall PoReContinuousModQueryDuration(unsigned int* /*out*/, const void* /*arg*/)
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
        mov     ecx, [edx+10h]
        lea     edx, [esp]
        mov     [esp], ecx
        mov     edx, [edx]
        mov     [eax], edx
        pop     ecx
        retn
    }
}

}  // namespace NIpse

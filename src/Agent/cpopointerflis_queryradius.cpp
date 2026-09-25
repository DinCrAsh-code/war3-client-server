//============================================================================
//  0x6F4A78E0 - a hidden-return-pointer `__fastcall` leaf: given `arg` and
//  a 4-byte out buffer, copies `arg`'s own +0x0C field into it, or
//  g_CFloatZero's own bits when `arg` is null. Same shape as
//  Agent/cporethresholdlis_querythreshold.cpp's own
//  PoReThresholdLisQueryThreshold - written naked for the same reason
//  (that file's own header has the full "optimiser proves the round trip
//  dead" reasoning).
//============================================================================
#include "cpopointerflis.h"
#include "CFloat.h"

namespace NIpse {

__declspec(naked) unsigned int* __fastcall PoPoInterfLisQueryRadius(unsigned int* /*out*/, const void* /*arg*/)
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

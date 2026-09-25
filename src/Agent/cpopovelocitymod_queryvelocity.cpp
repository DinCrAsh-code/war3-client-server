//============================================================================
//  0x6F4A7C60 - a hidden-return-pointer `__fastcall` leaf: given `arg` and
//  an 8-byte out buffer, copies `arg`'s own +0x0C/+0x10 CFloat pair into
//  it, or {g_CFloatZero, g_CFloatZero} when `arg` is null. Written naked -
//  the round trip through a local stack temp is provably dead to a plain
//  C++ version, the same reasoning
//  Agent/cporethresholdlis_querythreshold.cpp's own header gives for its
//  single-CFloat sibling.
//============================================================================
#include "cpopovelocitymod.h"
#include "CFloat.h"

namespace NIpse {

__declspec(naked) void* __fastcall PoPoVelocityModQueryVelocity(void* /*out*/, const void* /*arg*/)
{
    __asm
    {
        sub     esp, 8
        test    edx, edx
        mov     eax, ecx
        jnz     haveArg
        mov     ecx, dword ptr [g_CFloatZero]
        lea     edx, [esp]
        mov     [esp+4], ecx
        mov     [esp], ecx
        mov     ecx, [edx]
        mov     edx, [edx+4]
        mov     [eax], ecx
        mov     [eax+4], edx
        add     esp, 8
        retn
    haveArg:
        mov     ecx, [edx+0Ch]
        mov     edx, [edx+10h]
        mov     [esp+4], edx
        lea     edx, [esp]
        mov     [esp], ecx
        mov     ecx, [edx]
        mov     edx, [edx+4]
        mov     [eax], ecx
        mov     [eax+4], edx
        add     esp, 8
        retn
    }
}

}  // namespace NIpse

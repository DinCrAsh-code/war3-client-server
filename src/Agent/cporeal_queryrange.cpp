//============================================================================
//  0x6F4A9120 - a hidden-return-pointer `__fastcall` leaf: given `arg` (a
//  `SHandleRegistrationRequest*`, opaque here) and an 8-byte out buffer,
//  copies `arg`'s own +0x10/+0x14 dword pair into it, or - when `arg` is
//  null - the two "unbounded range" globals FloatMiniB already declares
//  (g_unk6FAAE628 as the low bound, g_unk6FAAE624 as the high bound,
//  Math/floatmini.h).
//
//  Written naked: the plain-C++ attempt (0.387/18) let the optimiser prove
//  the round-trip through a local pair was dead and fold it away entirely,
//  which is not what the shipped code does - it genuinely stores the
//  {lo, hi} pair to an 8-byte local and reloads it before writing *out*,
//  the same "prove nothing about a cross-TU pointer" shape
//  Agent/agentbasechain_staticpools.h's own BaseMapCPaCell_GetStaticPool
//  note documents for a different function. Stack offsets confirmed
//  against this function's own `raw_bytes`.
//============================================================================
#include "cporeal.h"
#include "floatmini.h"

namespace NIpse {

__declspec(naked) SPoRealRange* __fastcall PoRealQueryRange(SPoRealRange* /*out*/, const void* /*arg*/)
{
    __asm
    {
        sub     esp, 8
        test    edx, edx
        mov     eax, ecx
        jnz     haveArg
        mov     ecx, g_unk6FAAE628
        mov     edx, g_unk6FAAE624
        jmp     store
    haveArg:
        mov     ecx, [edx+10h]
        mov     edx, [edx+14h]
    store:
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

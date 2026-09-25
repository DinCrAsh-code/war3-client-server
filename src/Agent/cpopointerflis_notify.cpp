//============================================================================
//  0x6F4A8870 - NIpse::CPoPoInterfLis::NotifyEndpointAIfTagged, vtable
//  slot 8. See cpopointerflis.h's own file header for the full shape: a
//  short, real '^sig'-tag (0x5E736967) filter that reads and clears bit 1
//  of this object's own m_opt (+0x54), storing the PRE-clear bit-1 value
//  back into its own stack argument slot, then a genuine MSVC sibling-call
//  tail-jmp (docs/msvc-vc8-idioms.md) into a second, substantially larger
//  and unrelated function (sub_6F4A87F0) this session left untouched
//  rather than chase - real substance (an m_radius-vs-fixed-threshold
//  compare driving a further 6 callees off a `g_pTimeSync`-relative
//  manager object), not an MSVC/CRT rabbit hole, but a second, unrelated
//  closure past this follow-up session's own scope (the +0x58 array is),
//  the same "left a naked redirect rather than chase an entirely new
//  closure this far past the two classes actually in scope" call
//  Missile/bhpoprojectile_notify.cpp's own header already makes for
//  sub_6F496EB0. Written naked to preserve the real tail-jmp rather than a
//  call+return that would leave an extra return address on the stack
//  sub_6F4A87F0's own `retn 4` does not expect.
//
//  `arg == 0` short-circuits before even reading the tag (a null context
//  never carries it) - `retn 4` either way.
//============================================================================
#include "cpopointerflis.h"

namespace NIpse {

__declspec(naked) void* CPoPoInterfLis::NotifyEndpointAIfTagged(const void* /*arg*/)
{
    __asm
    {
        mov     eax, [esp+4]
        test    eax, eax
        jz      short done
        cmp     dword ptr [eax+8], 5E736967h  // '^sig'
        jnz     short done
        mov     eax, [ecx+54h]
        shr     eax, 1
        and     eax, 1
        and     dword ptr [ecx+54h], 0FFFFFFFDh
        mov     [esp+4], eax
        mov     eax, 06F4A87F0h
        jmp     eax
done:
        retn    4
    }
}

}  // namespace NIpse

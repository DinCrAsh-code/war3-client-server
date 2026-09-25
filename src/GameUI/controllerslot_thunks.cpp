//============================================================================
//  Redirects for the slot layer's two private-ABI apply bodies.  Neither is
//  in this dump.
//
//  **Neither is the plain `mov eax,<addr>` / `jmp eax` form, and cannot be.**
//  Both shipped functions take their host in *eax* - their shipped call
//  sites are `mov eax,ecx` immediately before the call - which is a register
//  no MSVC calling convention delivers, and which the plain redirect needs
//  for the address itself.  So each moves the receiver into eax, pushes the
//  target address and returns to it: `ret` pops that address into eip while
//  the caller's own return address stays untouched below it, so the real
//  function runs with the registers it expects, returns straight to our
//  caller, and cleans its own arguments.  Writing these as ordinary
//  redirects would hand the receiver in ecx to a function reading eax.
//
//  NOT reconstructions.  The stack argument each declaration pushes is the
//  one dword its shipped `retn 4` cleans, fixed by the call sites, which
//  push exactly one and never adjust esp.
//============================================================================
#include "controllerslot.h"

//  0x6F4E85F0 - the slot-binding registration the three binders share.  An
//  ordinary redirect: this one's receiver really does arrive in ecx.
__declspec(naked) void __fastcall RegisterControllerSlotBinding(
                        void* /*controller*/, int /*slot*/, void* /*callback*/,
                        void* /*context*/, float /*blend*/)
{
    __asm { mov eax, 06F4E85F0h }
    __asm { jmp eax }
}

__declspec(naked) void __fastcall ApplyControllerSlotVec3(void* /*host*/,
                                                          int /*slot*/,
                                                          const float* /*v*/)
{
    __asm
    {
        mov  eax, ecx
        push 06F4E81C0h
        ret
    }
}

__declspec(naked) void __fastcall ApplyControllerSlotFloat(void* /*host*/,
                                                           int /*slot*/,
                                                           const float* /*v*/)
{
    __asm
    {
        mov  eax, ecx
        push 06F4E8200h
        ret
    }
}

//  0x6F4E8160 - the single-int apply, the exact twin of the float one above
//  and the same private ABI: host in eax.  Reached only through
//  SetControllerSlotInt (GameUI/controllerslotsetint.cpp).
__declspec(naked) void __fastcall ApplyControllerSlotInt(void* /*host*/,
                                                         int /*slot*/,
                                                         const int* /*v*/)
{
    __asm
    {
        mov  eax, ecx
        push 06F4E8160h
        ret
    }
}

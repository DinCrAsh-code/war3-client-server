//============================================================================
//  0x6F480D50 - a 6-stack-argument `__thiscall` helper that binds a freshly
//  built IntegerListener (`this`, possibly null) into what looks like a
//  named-trigger lookup table (`sub_6F03FA30` inside it hashes a receiver
//  field through a global at `dword_6FAB7788`, `sub_6F48AB80` allocates a
//  fresh node tagged "[list]"/"[cfg]", and `sub_6F471A30` finishes the
//  registration).  This is depth 4 from the CPlayerWar3 slot-30 root
//  (root -> sub_6F02D800 -> sub_6F40FAF0 -> sub_6F473180 -> here), one
//  level past this session's depth-3 window - the dump queried for this
//  address on its own (asm/sub_6F480D50_0x6F480D50_calltree_asm.md,
//  depth 1) is what pins the six-stack-argument, `retn 18h` signature
//  below, not a real reconstruction.
//
//  Redirected rather than reconstructed for that reason alone; see
//  playerwar3_ability5integerlistener.cpp (0x6F473180, the one caller in
//  scope) for how the argument types below were read off its own push
//  sequence.
//============================================================================
#include "integerlistener.h"

__declspec(naked) void IntegerListenerBindHelper::Bind(
    void*, void*, int, void*, int, int)
{
    __asm
    {
        mov     eax, 06F480D50h
        jmp     eax
    }
}

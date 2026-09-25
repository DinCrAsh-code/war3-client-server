//============================================================================
//  0x6F26FE40 - GetGameUI(0,0), and if it exists, forward `this` and the
//  incoming edx (both zeroed on the path this entry point takes) into
//  sub_6F2F84D0 (SField284Owner::OrField284SetField26C's own two-argument
//  sibling, not yet reconstructed - out of this agent's own worklist).
//  Written naked: the shared tail at 0x6F332720 (folded into this same
//  agent_worktrees entry) is reached by more than one caller with a
//  different edx each time, so its own instructions are transcribed
//  in-line here as local labels rather than reconstructed as a second,
//  separately addressed C++ function this entry point cannot see.
//============================================================================
#include "gameui.h"

//  0x6F2F84D0 - out of this agent's own 60-address worklist; a naked
//  redirect to the real, unhooked body (src/Item/abilityfieldflagclosure_
//  thunks.cpp's own established convention), `retn 8` read off its own
//  raw_asm.
__declspec(naked) void Agent4Thunk_6F2F84D0()
{
    __asm { mov eax, 06F2F84D0h }
    __asm { jmp eax }
}

__declspec(naked) void __fastcall ForwardZeroFilterToGameUI_6F26FE40(void* /*this, ecx*/)
{
    __asm
    {
        xor     edx, edx
        push    esi
        push    edi
        mov     esi, edx
        mov     edi, ecx
        xor     edx, edx
        xor     ecx, ecx
        call    GetGameUI
        test    eax, eax
        jz      done
        push    esi
        push    edi
        mov     ecx, eax
        call    Agent4Thunk_6F2F84D0
done:
        pop     edi
        pop     esi
        retn
    }
}

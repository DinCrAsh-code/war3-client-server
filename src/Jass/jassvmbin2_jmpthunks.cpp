//============================================================================
//  Six single-instruction "jmp sub_X" bodies in the JASS VM module's own
//  address neighbourhood (this round's assigned range) - each real shipped
//  function is nothing but a tail redirect to another real address, so the
//  reconstruction here is the standard self-redirect THUNK
//  (`mov eax, <own address> / jmp eax`) rather than a hand-transcription of
//  the shipped `E9 rel32` (which would score 0 against that shape anyway,
//  and THUNK makes no behavioural claim either way).
//============================================================================

//  0x6F44F460 -> sub_6F44ECD0
__declspec(naked) void Thunk_6F44F460()
{
    __asm { mov eax, 06F44F460h }
    __asm { jmp eax }
}

//  0x6F452D10 -> sub_6F450370
__declspec(naked) void Thunk_6F452D10()
{
    __asm { mov eax, 06F452D10h }
    __asm { jmp eax }
}

//  0x6F457B40 -> sub_6F457A70
__declspec(naked) void Thunk_6F457B40()
{
    __asm { mov eax, 06F457B40h }
    __asm { jmp eax }
}

//  0x6F4582E0 -> sub_6F457E70
__declspec(naked) void Thunk_6F4582E0()
{
    __asm { mov eax, 06F4582E0h }
    __asm { jmp eax }
}

//  0x6F460530 - `yy_scan`'s own destructor tail (jass-multiscript-A:
//  called from `CompileScriptChain`/0x6F458900 on every path, success or
//  failure, after `CompileOneScriptChainPass` returns - see
//  Jass/jassscanner.h/.cpp and docs/notes/jass-multi-script-compile-mechanism.md).
//  Restamps the vtable, frees two `Storm_403`-tagged buffers, then tail-
//  jumps to sub_6F4583C0 (unreconstructed) for the rest of the teardown.
__declspec(naked) void Thunk_6F460530()
{
    __asm { mov eax, 06F460530h }
    __asm { jmp eax }
}

//  0x6F455A40 -> sub_6F460530 directly (one instruction) - its own
//  redirect rather than sharing Thunk_6F460530's body so each address's
//  own funcmap entry points at a distinct symbol.
__declspec(naked) void Thunk_6F455A40()
{
    __asm { mov eax, 06F455A40h }
    __asm { jmp eax }
}

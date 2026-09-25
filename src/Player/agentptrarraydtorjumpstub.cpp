//============================================================================
//  0x6F28D4E0 - a genuine one-instruction `jmp` stub IDA finds *in the
//  shipped binary itself* (its own auto-generated "j_" name), tail-jumping
//  straight into TSGrowableArray<CAgentPtr>::~TSGrowableArray<CAgentPtr>
//  (0x6F286590, Agent/agentptrarray.cpp).  Not this repo's own placeholder
//  convention - a real, separate machine address the shipped compiler
//  itself emitted as a plain redirect, reached as a callee of
//  PlayerAbilityAgentTrackerChecksum's own closure
//  (Player/playerabilityagenttracker.cpp).  A single `jmp`, so it is a
//  thunk by construction rather than by choice - there is nothing else to
//  reconstruct at this address.
//============================================================================
//  Deliberately *not* `extern "C"`: nothing else in this closure calls it
//  by name (it only appears as a call-graph node in the dump itself), so
//  there is no cross-TU declaration to keep consistent, and an ordinary
//  mangled C++ symbol is what lets MASM annotate the PUBLIC line with a
//  readable name comment - an `extern "C" __fastcall` symbol carries only
//  its raw `@Name@4`-decorated form with no such comment, which is what
//  left this address permanently unscored the first time this file was
//  written (verify.py's own name lookup keys off that comment).
void __fastcall TSGrowableArrayCAgentPtrDtorJumpStub(void*);

__declspec(naked) void __fastcall TSGrowableArrayCAgentPtrDtorJumpStub(void*)
{
    __asm
    {
        mov     eax, 06F286590h
        jmp     eax
    }
}

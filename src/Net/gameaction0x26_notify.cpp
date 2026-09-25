//============================================================================
//  0x6F6D8950 - a thin wrapper (`this`, one stack dword) around
//  sub_6F6D8340, a synchronisation primitive this session did not
//  reconstruct (a companion to the wait/contention-tracked lock family
//  sub_6F6D9820/sub_6F6D8340 themselves belong to - real implementations,
//  not simple leaves). Reached from GameAction_0x26_PacketHandler
//  (gameaction0x26_handler.cpp) to signal `unk_6FACFF20` after inserting a
//  new new per-player record into the action queue's hash table.
//
//  sub_6F6D8340 is NOT a normal thiscall member - it never sets up its own
//  context register, it just reads `esi` directly from the first
//  instruction that touches it. Its only real call site (this function)
//  supplies that context by explicitly doing `mov esi, ecx` right before
//  the call:
//
//      mov     eax, [esp+arg_0]
//      push    esi
//      push    eax
//      mov     esi, ecx
//      call    sub_6F6D8340
//      pop     esi
//      retn    4
//
//  A plain C++ member-call spelling (`NotifyOne(value)` on a declared
//  member) compiles to an ordinary thiscall - `this` stays in ecx, esi is
//  never touched - which leaves sub_6F6D8340 reading whatever garbage was
//  already sitting in esi and crashing on its first real dereference
//  (`mov edi, [esi+8]`). Written naked, reproducing the real register
//  shuffle exactly, instead.
//
//  The call target is still the qualified C++ name `ConditionVariable::
//  NotifyOne`, not a hand-typed `sub_6F6D8340` extern - a raw `extern "C"`
//  symbol with no funcmap.py FUNCTIONS entry has nothing to bind it to the
//  real address at link time (unresolved external in the injection repo's
//  own build, distinct from this repo's own build which only needs the
//  /FA listing and never actually links). `NotifyOne`'s own mangled name
//  already resolves correctly - it's what the previous version called -
//  so keep using it, just with `esi` set up by hand first instead of left
//  to the compiler's ordinary thiscall codegen.
//============================================================================

struct ConditionVariable
{
    void NotifyOne(unsigned int value);   // sub_6F6D8340, not reconstructed
    void SignalOne(unsigned int value);   // sub_6F6D8950 (this function)
};

__declspec(naked) void ConditionVariable::SignalOne(unsigned int /*value*/)
{
    __asm
    {
        mov     eax, [esp+4]
        push    esi
        push    eax
        mov     esi, ecx
        call    ConditionVariable::NotifyOne
        pop     esi
        retn    4
    }
}

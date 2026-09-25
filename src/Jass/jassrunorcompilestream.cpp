//============================================================================
//  0x6F44B640 - dispatch between re-running an already-compiled JASS
//  opcode stream and compiling+running one for the first time.  Reached
//  from this session's own `agent_worktrees` sweep of JassInstance's own
//  closure, not from any calltree dump in this session's `asm/` (no dump
//  reaches it), so it is scored from a hand-assembled dump the same way
//  jassscriptframepool.cpp's own functions are - see that file's note.
//
//  The two things this reconstruction is confident about, straight from
//  the instruction stream: the dispatch condition itself (a looked-up
//  JassInstance's own m_streamCursor, +0x20, tested non-null, gated by a
//  caller-supplied flag) and which of the two known members it calls on
//  each arm (RunOpcodeStream vs. sub_6F45F8B0, still TODO - not attempted
//  this session).  What this function's four stack parameters actually
//  mean is not reachable from this closure alone (sub_6F45F8B0 - the
//  "compile" arm - is the function that would settle it), so they are
//  named for their evidence (`lookupKey`, `resumeAllowed`) or left as
//  plain positional parameters rather than guessed at further - the
//  "QueryFieldNN" rule (docs/notes/naming-standard.md) applied to
//  parameters instead of a member name.
//============================================================================
#include "jassexecutestream.h"
#include "jassthreadstate.h"   // JassInstance forward use via jassinstance.h

//  0x6F44B2E0 - jassthreadinstanceslot.cpp.  Resolves `lookupKey` to a
//  JassInstance* off the running thread's own +0x90 handle array.
JassInstance* __fastcall LookupInstanceBySlot(unsigned int lookupKey);

//  0x6F45F8B0 - still TODO, not attempted this session (139 instructions,
//  a real function, not a thunk in the shipped binary - kept a naked
//  redirect here purely so this closure's own caller links and behaves,
//  the same way this repo thunks any not-yet-reached callee).  `retn 14h`
//  (5 stack dwords) with `ecx` carrying `lookupKey` straight through
//  unexamined by this call site - RunOrCompileStream below never loads a
//  new value into ecx between the lookup call and this one - which is
//  not a real `this` pointer in any type sense this session can name;
//  CompileStreamTarget below exists only to spell that unchanged-ecx
//  handoff in C++ (the same reinterpret-the-key-as-`this` shape
//  JassFuncAddrTableView, jassinstance.h, already uses for an unrelated
//  reason), reinterpreting `lookupKey`'s own bit pattern as the pointer.
struct CompileStreamTarget
{
    //  0x6F45F8B0 - kept THUNK: see this file's own header comment.  Return
    //  type corrected this round (jass-instance-targeting-D) from `void` to
    //  `int` - RunOrCompileStream's own body never re-touches eax after
    //  either of its two calls, and GetOrRunJassInstanceForScript
    //  (jassgetorrunscriptinstance.cpp), the caller that motivated this
    //  round's own read of this file, genuinely tests RunOrCompileStream's
    //  return value for success - see docs/notes/
    //  jass-runtime-instance-targeting.md.
    int CompileAndRun(void* param0, void* arg0, void* arg4, void* argC,
                       int zero);
};

__declspec(naked) int CompileStreamTarget::CompileAndRun(
    void* /*param0*/, void* /*arg0*/, void* /*arg4*/, void* /*argC*/,
    int /*zero*/)
{
    __asm
    {
        mov eax, 06F45F8B0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F44B640 - `retn 10h`.  `ecx` = lookupKey, `edx` = param0; stack
//  parameters named by their own IDA offsets (arg_0/arg_4/arg_8/arg_C) -
//  see this file's own header note on why arg_8 alone gets a real name
//  (`resumeAllowed`, the only one this closure's own control flow pins
//  down) and the rest do not.  Return type corrected this round
//  (jass-instance-targeting-D) from `void` to `int`: both arms end with
//  `call <callee>` immediately followed by `pop esi; retn 10h` with no
//  further write to eax, so whatever RunOpcodeStream/CompileAndRun
//  returns in eax genuinely is this function's own return value -
//  GetOrRunJassInstanceForScript (jassgetorrunscriptinstance.cpp) is the
//  caller that reads it.
//----------------------------------------------------------------------------
int __fastcall RunOrCompileStream(unsigned int lookupKey, void* param0,
                                   void* arg0, void* arg4, int resumeAllowed,
                                   void* argC)
{
    JassInstance* instance = LookupInstanceBySlot(lookupKey);

    if (resumeAllowed != 0 && instance->m_streamCursor != 0)
    {
        return instance->RunOpcodeStream();
    }

    return ((CompileStreamTarget*)(void*)lookupKey)->CompileAndRun(param0, arg0,
                                                                     arg4, argC, 0);
}

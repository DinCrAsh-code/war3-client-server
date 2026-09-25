//============================================================================
//  0x6F26FE50 - cunit_agent5_worklist.md, CUnit closure BFS depth 1-2.
//
//  `mov edx, [esp+4]` / `call sub_6F332ED0` / `retn 4` - exactly what a
//  __thiscall member taking one pointer argument compiles to when it
//  tail-forwards straight into a two-register __fastcall(ecx,edx) callee:
//  `this` stays in ecx across the call (never re-loaded), the one
//  explicit stack argument gets reloaded into edx for the callee, and the
//  bare `retn 4` is __thiscall's own single-argument cleanup.  No callers
//  in this batch's worklist (grep says zero references outside this
//  file), so the receiver type is a minimal stand-in - the same shape
//  attachmentrendernotify.cpp already uses for an address reached only
//  through an unnamed object.  sub_6F332ED0 itself has no dump body in
//  this batch (out of scope: it goes on to call sub_6F300710/
//  sub_6F2F9C90, neither reconstructed here).
//============================================================================

//  0x6F332ED0 - __fastcall(ecx, edx), bare `retn` (0 stack bytes, read off
//  agent_worktrees' own raw_asm for it) - a naked thunk, no dump body: it
//  goes on to call sub_6F300710/sub_6F2F9C90, neither reconstructed here.
__declspec(naked) void __fastcall UnknownFastcallHelper_0x6F332ED0(void*, void*)
{
    __asm { mov eax, 06F332ED0h }
    __asm { jmp eax }
}

struct SUnknownForwarder_0x6F26FE50
{
    void Method(void* candidate);
};

void SUnknownForwarder_0x6F26FE50::Method(void* candidate)
{
    UnknownFastcallHelper_0x6F332ED0(this, candidate);
}

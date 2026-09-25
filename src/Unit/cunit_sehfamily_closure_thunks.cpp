//============================================================================
//  Naked redirects for a batch of CUnit vtable-closure leaves (slots 3, 72
//  and 107, depth 1-2) that open a real __except_handler4 SEH frame - the
//  same unreproducible ceiling documented in
//  docs/msvc-vc8-idioms.md ("An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce") and already applied to the slot 91/103/
//  115/116/117 family in unit_blockedclosureleaves.cpp.  This build's fixed
//  /GS- /EHs-c- cannot emit the cookie-XOR/`fs:[0]`-chain/appended-
//  scope-table shape the dump shows for any of these, so each is a direct
//  redirect rather than a guess at a frame the bundled compiler will not
//  produce. Every retn byte count below is read off the function's own
//  tail in agent_worktrees/funcs/<addr>.json's raw_bytes, not inferred.
//
//  Eighteen of these (the 0x6F239xxx-0x6F24Cxxx run, all reached from slot
//  107's own root at 0x6F2A0E30) share one 78-instruction template to the
//  instruction: same SEH furniture, same refcounted-handle release tail
//  (`add [x+4],-1` / `jnz` / vtbl-slot-0 call), same trailing
//  `stru_...`/`__CxxFrameHandler3` frame descriptor, differing only in the
//  two callees each pulls in past the frame (a different
//  dword_6FAB7368-relative accessor pair and a different RTTI/frame
//  descriptor). No instruction in any of the eighteen ever reads `ecx`
//  before overwriting it, so none of them takes `this` or any other
//  register argument; the singleton outside that run (0x6F078970) and the
//  two reached from slots 3/107 individually (0x6F208310, 0x6F29B560) each
//  carry their own real `ecx`/stack-argument shape, read directly off
//  their own dumps. 0x6F385F80 (also SEH-blocked, per the same worklist
//  note) is deliberately not included here - it was already claimed by a
//  concurrent session at the time of this pass.
//============================================================================
#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//----------------------------------------------------------------------------
//  0x6F078970 (`retn 4`) - reached from slot 107's own root (0x6F2A0E30).
//  `this` (ecx) plus one stack dword; opens directly with the
//  __except_handler4/GS-cookie SEH frame.
//----------------------------------------------------------------------------
struct SCUnitSEHFamilyLeaf078970
{
    void* Run(void* a);
};
ADDR_THUNK(void* SCUnitSEHFamilyLeaf078970::Run(void*), 0x6F078970)

//  0x6F385F80 is deliberately NOT here: agent_worktrees shows it already
//  claimed by a concurrent session (cunit-agent7) as of this pass. Left
//  TODO for that session to finish rather than racing it.

//----------------------------------------------------------------------------
//  0x6F208310 (`retn 8`) - reached from slot 3's own root (0x6F2A7E60).
//  `this` (ecx) plus two stack dwords; opens directly with the
//  __except_handler4/GS-cookie SEH frame around a checked-handle
//  release/reassign (`sub_6F477550`-constructed object at [esi+0x24]).
//----------------------------------------------------------------------------
struct SCUnitSEHFamilyLeaf208310
{
    void Run(void* a, void* b);
};
ADDR_THUNK(void SCUnitSEHFamilyLeaf208310::Run(void*, void*), 0x6F208310)

//----------------------------------------------------------------------------
//  0x6F29B560 (`retn 0`) - reached from slot 3's own root (0x6F2A7E60).
//  Same 78-instruction template as the eighteen below (no register
//  argument at all), but reached from a different root - kept separate so
//  its own `reached_from` stays honest in the commit history.
//----------------------------------------------------------------------------
struct SCUnitSEHFamilyLeaf29B560
{
    void* Run();
};
ADDR_THUNK(void* SCUnitSEHFamilyLeaf29B560::Run(), 0x6F29B560)

//----------------------------------------------------------------------------
//  The eighteen-member 78-instruction template, all reached from slot 107's
//  own root (0x6F2A0E30), all `retn 0`, all no-argument (see file header).
//----------------------------------------------------------------------------
struct SCUnitSEHFamilyTemplate18
{
    void* Run_239A00();
    void* Run_23B9B0();
    void* Run_23C440();
    void* Run_23CB90();
    void* Run_23D210();
    void* Run_23D960();
    void* Run_23DFE0();
    void* Run_23EC10();
    void* Run_23F290();
    void* Run_23F910();
    void* Run_23FF90();
    void* Run_2406E0();
    void* Run_240FD0();
    void* Run_241720();
    void* Run_241F40();
    void* Run_2425C0();
    void* Run_244E60();
    void* Run_247490();
    void* Run_249FA0();
    void* Run_24CB80();
};
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_239A00(), 0x6F239A00)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23B9B0(), 0x6F23B9B0)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23C440(), 0x6F23C440)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23CB90(), 0x6F23CB90)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23D210(), 0x6F23D210)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23D960(), 0x6F23D960)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23DFE0(), 0x6F23DFE0)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23EC10(), 0x6F23EC10)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23F290(), 0x6F23F290)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23F910(), 0x6F23F910)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_23FF90(), 0x6F23FF90)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_2406E0(), 0x6F2406E0)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_240FD0(), 0x6F240FD0)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_241720(), 0x6F241720)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_241F40(), 0x6F241F40)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_2425C0(), 0x6F2425C0)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_244E60(), 0x6F244E60)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_247490(), 0x6F247490)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_249FA0(), 0x6F249FA0)
ADDR_THUNK(void* SCUnitSEHFamilyTemplate18::Run_24CB80(), 0x6F24CB80)

#undef ADDR_THUNK

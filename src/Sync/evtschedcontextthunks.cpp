//============================================================================
//  0x6F630460, thunked rather than reconstructed this session - a
//  deliberate judgement call, not a shortcut past tedium (see CLAUDE.md's
//  own line on that).  259 instructions: EvtSched's own worker-context
//  array constructor - a C++ `new[]` (through Storm_401, `eh vector
//  constructor iterator`), a power-of-two bucket-count computation reused
//  twice (grow/shrink), a named-object registration loop (Storm_578
//  building "EvtSched#%d" tags) and a thread launch through sub_6F6D8540
//  (LaunchWorkerThread, Sync/evtsched.h) - all inside a full
//  `__except_handler4`-shaped frame this toolchain's fixed /GS- /EHs-c-
//  never reproduces byte for byte regardless of source shape
//  (docs/msvc-vc8-idioms.md).  Five more callees this batch's own 43 does
//  not otherwise reach (sub_6F6CE250, sub_6F62FAE0, sub_6F62D230,
//  sub_6F6D8980, sub_6F6D8530) would each need reconstructing cold first.
//  Reached only from GameMain's own root closure below this batch's own
//  depth, not from anywhere else in this batch's 43.
//
//  0x6F630760, the matching per-context "log and dispatch" method, is
//  reconstructed for real in evtschedcontextlogdispatch.cpp instead - it
//  turned out to be reachable from an *already-finished* target
//  (Probably_W3GS_ActionHandler_MaybeServerVersion) that a thunk here would
//  have regressed (CLAUDE.md: "no finished target lost instructions...
//  outranks any progress on the new target").
//============================================================================

//  0x6F630460 - retn 0 (fastcall, both args in registers, no stack
//  arguments - `retn` bare in its own dump).  Types corrected against the
//  body itself (`mov ebp,ecx` then `cmp esi,ebp`/`add esi,esi` - a
//  power-of-two bucket-count computation over ecx, so ecx is the count, not
//  a pointer; `mov dword_6FACEB64,edx` stashes edx straight into a global
//  with no arithmetic on it, consistent with a context pointer) - the
//  ecx/edx roles this file originally guessed were swapped, caught when
//  Net/netproviderltcptimerforward.cpp's own call site (`mov ecx,esi
//  /*count, just clamped >=1*/; mov edx,edi /*context*/`) needed this same
//  address and read the opposite roles from its own dump evidence.
__declspec(naked) void __fastcall ConstructEvtSchedContextArray(unsigned int /*ecx, count*/, void* /*edx, context*/)
{
    __asm { mov eax, 06F630460h }
    __asm { jmp eax }
}


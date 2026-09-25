//============================================================================
//  JASS VM/compiler core internals (jassvm-core-A) - the six largest
//  functions in this session's own worklist, every one either
//  `__except_handler4`-shaped SEH (this fixed /GS- /EHs-c- toolchain
//  cannot emit that frame at all - docs/msvc-vc8-idioms.md's own
//  cautionary note on that shape) or, for 0x6F45E140, large enough and
//  deeply enough coupled to further unreconstructed callees/globals that
//  a from-scratch transcription is its own session's work, not a budget
//  item here.  Kept naked redirects with real, evidence-backed names and
//  signatures (from this session's own reading of their call sites and
//  bodies - see the per-function notes below) rather than left `sub_`.
//============================================================================
#include "jassinstance.h"

//----------------------------------------------------------------------------
//  0x6F4549E0 - the master VM-environment allocator: JassInstance's own
//  0x28A8-byte constructor (jassinstance.h's own header note).  `retn 8`,
//  two stack args.  **Three call sites, not one** (corrected this round,
//  jass-instance-targeting-D - `tools/worktree_store.py xrefs 0x6F4549E0
//  --to` lists all three): 0x6F455250 (SpawnChildInstance) and 0x6F45E300
//  (CreateChildInstance) both pass `flag=0` and the *calling* instance's
//  own +0x2858 as `globalsBase` (`push 0; push [esi+2858h]; mov
//  ecx,newMem; call`) - cloning a child off an already-running instance.
//  The third, 0x6F455760 (Jass_ctor_SEH_6F455760,
//  jassvmbatch4_thunks.cpp), passes `flag=1` and a *freshly resolved
//  JassThreadLocal::m_scripts record* (not another instance's own
//  globals-base) as `globalsBase` - this is the ROOT instance
//  construction a compiled script chain drives, and `flag`'s own value is
//  what gates whether `Construct` calls into `ExecuteDeclStream` at all
//  (see this function's own body: `cmp arg_4,0 / jz skip / call
//  sub_6F45D720`) - only the flag=1 (root) path ever installs a decl
//  stream into the instance it just built; both flag=0 (spawn/clone)
//  paths skip it entirely.  See docs/notes/
//  jass-runtime-instance-targeting.md for the full trace and why this is
//  the reason the shipped game has no call site that re-runs
//  ExecuteDeclStream against an *already-existing* instance.
//----------------------------------------------------------------------------
__declspec(naked) void JassInstance::Construct(void** globalsBase, int flag)
{
    __asm
    {
        mov eax, 06F4549E0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F45E300 - clone the running instance's own compile-time state
//  (ScriptDataTable/FuncAddr/hash tables and a dozen more +0x28xx fields,
//  field-by-field per this function's own dump) into a freshly allocated
//  JassInstance (JassInstance::JassInstance above), for the child
//  instance a "spawn" site (0x6F455250) builds.  `retn 0`, `__thiscall`,
//  no stack args - `this` is the source instance being cloned.
//----------------------------------------------------------------------------
__declspec(naked) JassInstance* JassInstance::CreateChildInstance()
{
    __asm
    {
        mov eax, 06F45E300h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F455250 - the "spawn a child instance" driver: deserializes a
//  parameter/local layout off a CDataStore, builds the child instance
//  (JassInstance_CreateChildInstance above), registers it
//  (JassThreadRegisterInstance) and runs it (JassInvokeInstanceRun,
//  0x6F4484F0 - jassnativecalldispatch.cpp).  `retn 0x10`, four stack
//  args (this session's own read of its own trailing `retn`; the
//  argument shapes themselves are not resolved from this closure).
//----------------------------------------------------------------------------
__declspec(naked) void JassInstance::SpawnChildInstance(void*, void*, void*, void*)
{
    __asm
    {
        mov eax, 06F455250h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F45D860 - plausibly JassInstance::Run, the interpreter's own
//  per-instance execution entry (691 instructions, SEH-wrapped,
//  `__fastcall(ecx=this, one stack arg)`, `retn 4` - user_knowledge.json's
//  own finding).  The single largest function in this session's own
//  worklist; declared on JassInstance in jassinstance.h already
//  (jassnativecalldispatch.cpp's own JassInvokeInstanceRun calls it).
//----------------------------------------------------------------------------
__declspec(naked) void* JassInstance::Run(void*)
{
    __asm
    {
        mov eax, 06F45D860h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F45D410 - deserializes an array of ScriptDataTable records off a
//  CDataStore (allocating each from g_jassScriptDataPool and dispatching
//  through JassSymbolNode::DispatchOrEvaluate, 0x6F45C170 -
//  jassnativecalldispatch.cpp), immediately ahead of 0x6F45D720
//  (JassInstance::ExecuteDeclStream - jassinstancedispatch.cpp) in the
//  image, and its own trailing `retn 0Ch` (three stack args) is what
//  pins that function's own start address in this session's own reading.
//  `retn 0Ch`, three stack args - `__stdcall` here rather than a member,
//  since this session's own closure has no evidence either way for what,
//  if anything, arrives in ecx.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall JassInstance_LoadScriptDataRecordArray(void*, void*, void*)
{
    __asm
    {
        mov eax, 06F45D410h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F45E140 - deserializes one ScriptDataTable record (get-or-create off
//  a free list rooted at dword_6FAB72A0/dword_6FAB72A4, else a fresh
//  g_jassFramePool allocation) and its own field list, dispatching each
//  through JassSymbolNode::DispatchOrEvaluate in a loop.  `retn 4`.  Two
//  of its own callees (sub_6F450450, sub_6F6EED50) and the free-list pair
//  above are outside this session's own reach; a from-scratch
//  transcription is its own session's work.  `retn 4`, `__thiscall` - the
//  function's own entry reads its one stack arg (a CDataStore*) before
//  touching `this` at all, and later reaches `this+0x2850`, a JassInstance
//  field jassthreadstate.h's own AssignInstanceSlot already names.
//----------------------------------------------------------------------------
__declspec(naked) void JassInstance::LoadScriptDataRecord(void* store)
{
    __asm
    {
        mov eax, 06F45E140h
        jmp eax
    }
}

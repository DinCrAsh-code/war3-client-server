//============================================================================
//  JASS `ExecuteFunc`'s own name -> handle resolve step - see
//  docs/notes/jass-runtime-execute-function.md for the full trace and the
//  "does this work for a runtime-compiled script" answer this closure feeds.
//
//  Two real calls deep, both real reconstructions:
//
//    CGameWar3::ResolveJassFunctionHandle (0x6F39F5B0, ExecuteFunc's own
//    direct callee)
//      -> ResolveFuncHandleForCurrentInstance (0x6F44B620)
//           -> LookupInstanceBySlot (0x6F44B2E0, already reconstructed,
//              jassinstancespawn.cpp/jassthreadinstanceslot.cpp)
//           -> JassInstance::ResolveFuncHandle (0x6F45CEE0) - kept a naked
//              redirect rather than reconstructed: it is itself only two
//              calls (FindFuncAddr, already real - jassfuncaddrlookup.cpp -
//              then a second, larger memoization step at 0x6F45CE50 this
//              session traced but did not confirm the struct layout of well
//              enough to write real source for, the same caution
//              CLAUDE.md's own struct-offset rule asks for).  Nothing in
//              this closure needs its body modelled beyond "it resolves to
//              the same handle every time for the same function", which the
//              shipped code (and this redirect) both guarantee by
//              construction.
//============================================================================
#include "jassinstance.h"
#include "gamewar3.h"

//  0x6F44B2E0 - jassinstancespawn.cpp/jassthreadinstanceslot.cpp.  Resolves
//  a JASS thread-instance slot index to the JassInstance currently running
//  in it.
JassInstance* __fastcall LookupInstanceBySlot(unsigned int lookupKey);

//----------------------------------------------------------------------------
//  0x6F45CEE0 - `retn 4`.  JassInstance::ResolveFuncHandle: resolve a
//  function name to its instance-scoped, memoized JASS handle value.  Traced
//  but not reconstructed this session - see this file's own header comment.
//----------------------------------------------------------------------------
__declspec(naked) unsigned int __fastcall
    JassInstanceResolveFuncHandle(JassInstance* /*instance*/,
                                   const char* /*funcName*/)
{
    __asm
    {
        mov eax, 06F45CEE0h
        jmp eax
    }
}

//----------------------------------------------------------------------------
//  0x6F44B620 - `retn 0`, `__fastcall(ecx=instanceSlot, edx=funcName)`.
//  ResolveFuncHandleForCurrentInstance: find the JassInstance the calling
//  thread is currently running in (via its own slot index) and resolve
//  `funcName` to a handle inside *that* instance's own symbol tables - not
//  a global/static table, so a name declared into that same live instance
//  by a mid-game recompile resolves here exactly like one declared at map
//  load.  See the doc above for the caveat this depends on (same instance,
//  not a spawned child one).
//----------------------------------------------------------------------------
unsigned int __fastcall ResolveFuncHandleForCurrentInstance(unsigned int instanceSlot,
                                                              const char* funcName)
{
    JassInstance* instance = LookupInstanceBySlot(instanceSlot);
    return JassInstanceResolveFuncHandle(instance, funcName);
}

//----------------------------------------------------------------------------
//  0x6F39F5B0 - `retn 4`.  CGameWar3::ResolveJassFunctionHandle
//  (gamewar3.h): ExecuteFunc's own direct callee.  `this` is the CGameWar3
//  game-data singleton (g_unk6FAB65F4/dword_6FAB65F4, lazily created by
//  ExecuteFunc itself); its own +0x08 field (m_currentJassInstanceSlot) is
//  "which JASS thread slot is currently running", read here and handed
//  straight through.
//----------------------------------------------------------------------------
unsigned int CGameWar3::ResolveJassFunctionHandle(const char* funcName)
{
    return ResolveFuncHandleForCurrentInstance(m_currentJassInstanceSlot,
                                                funcName);
}

//============================================================================
//  0x6F453C00 - the lazy per-thread accessor: return the running thread's
//  JassThreadLocal (TLS slot 5) if it already has one, otherwise allocate
//  and construct one and publish it to the slot before returning it.
//============================================================================
#include "jassthreadstate.h"
#include "gamecontext.h"    // GetThreadLocalSlot - 0x6F4C34D0
#include "storm.h"          // SMemAlloc - Storm_401

//  0x6F4C34F0 - the writer twin of GetThreadLocalSlot, declared here rather
//  than pulled from a header: Sync/tlsslot.cpp (its own reconstruction)
//  keeps it file-local too.
void __fastcall SetThreadLocalSlot(unsigned int slot, void* value);

//  `retn` with no epilogue `mov eax, ...` on the already-exists path, and a
//  tail `jmp` into the void-returning SetThreadLocalSlot on both of the
//  other two - so this is a `void` primer/publisher, not a getter (callers
//  needing the pointer back call GetThreadLocalSlot(kThreadLocalJass)
//  themselves afterwards, the way JassThreadSleep already does).
void __fastcall EnsureJassThreadLocal()
{
    JassThreadLocal* jass = (JassThreadLocal*)GetThreadLocalSlot(kThreadLocalJass);
    if (jass != 0)
        return;

    void* mem = SMemAlloc(0xB0, ".\\Jass.cpp", 0x29C, 0);
    if (mem == 0)
    {
        SetThreadLocalSlot(kThreadLocalJass, 0);
        return;
    }

    jass = new (mem) JassThreadLocal();
    SetThreadLocalSlot(kThreadLocalJass, jass);
}

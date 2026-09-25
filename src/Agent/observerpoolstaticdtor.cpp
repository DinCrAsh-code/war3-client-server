//============================================================================
//  0x6F62ABB0 - a module-teardown chain: EvtSched's own shutdown
//  (EvtSchedShutdown, Sync/evtschedshutdown.cpp, already reconstructed),
//  three further module teardowns outside this batch's own 43, then a tail
//  merge (the dump's own `jmp loc_6F629E40`, IDA showing the shared chunk
//  inline) into the observer pool's own static destructor: release two
//  named CDataAllocator instances (AVObserverRegi.../AUObserverEven...,
//  matching the same ReleaseAllBlocks ABI gamemain_small_wrappers.cpp's own
//  two static-dtor stubs already use) and then five more, unnamed, walked
//  as a fixed 0x14-byte-stride array from 0x6FACEA64 down to (exclusive)
//  0x6FACEA00.
//============================================================================
#include "evtsched.h"
#include "dataallocator.h"

void EvtSchedShutdown();   // Sync/evtschedshutdown.cpp, 0x6F62E5B0

//  Three further module teardowns, below this batch's own 43; declared and
//  thunked to their own real addresses (`retn 0`, no arguments at either
//  call site).
__declspec(naked) void ObserverModuleTeardownA()   // 0x6F631210
{
    __asm { mov eax, 06F631210h }
    __asm { jmp eax }
}
__declspec(naked) void ObserverModuleTeardownB()   // 0x6F631820
{
    __asm { mov eax, 06F631820h }
    __asm { jmp eax }
}
__declspec(naked) void ObserverModuleTeardownC()   // 0x6F62C060
{
    __asm { mov eax, 06F62C060h }
    __asm { jmp eax }
}

//  0x6F4C1A60 is CDataAllocator::ReleaseAllBlocks (Net/dataallocator.cpp,
//  gamemain-batch-3's own real reconstruction of this address).  This file
//  used to carry its own naked redirect to the same address, routed through
//  a minimal opaque `ObserverAllocatorReleaser` receiver (this file
//  predates that reconstruction landing) - a duplicate; every call below
//  now goes through the real method directly on the already-typed
//  `CDataAllocator` globals/pointer.

//  g_observerResourcePool/g_observerEventNodePool - Agent/observer.cpp and
//  Agent/observer_slots.cpp's own CDataAllocator globals (unk_6FACE9D8,
//  dword_6FACE9EC).
extern CDataAllocator g_observerResourcePool;
extern CDataAllocator g_observerEventNodePool;
static const char aAvobserverregi[] = ".?AVObserverRegistry@@";   // type descriptor name
static const char aAuobservereven[] = ".?AUObserverEvent@@";      // type descriptor name

//  g_observerEventArrayPools - Agent/observereventreg.cpp's own
//  CDataAllocator[8] (unk_6FACEA00); this function only tears down the
//  first five of the eight (0x6FACEA00..0x6FACEA64), matching this call
//  tree's own bound exactly.
extern CDataAllocator g_observerEventArrayPools[8];

void ObserverPoolStaticDtor()
{
    EvtSchedShutdown();
    ObserverModuleTeardownA();
    ObserverModuleTeardownB();
    ObserverModuleTeardownC();

    g_observerResourcePool.ReleaseAllBlocks(aAvobserverregi, -2, 1);
    g_observerEventNodePool.ReleaseAllBlocks(aAuobservereven, -2, 1);

    CDataAllocator* p = g_observerEventArrayPools + 5;
    do
    {
        p -= 1;
        p->ReleaseAllBlocks(0, 0, 1);
    } while (p != g_observerEventArrayPools);
}

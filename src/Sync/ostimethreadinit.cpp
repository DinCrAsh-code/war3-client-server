//============================================================================
//  0x6F6C56A0 - construct the "OsTime" worker thread's own state block and
//  launch it.  `this` (ecx) is an opaque receiver this call tree never
//  names a class for; only the three offsets it actually touches are
//  asserted, per CLAUDE.md's "only touched members get names" rule:
//    +0x10  a bare HANDLE, zeroed via Event::ZeroHandle rather than fully
//           constructed - LaunchWorkerThread fills it in as the new
//           thread's own handle (its `outHandle` out-parameter).
//    +0x14  a manual-reset, initially-unsignalled Event (the thread's own
//           "time to stop" signal - g_pauseLatch is set to `this`, not to
//           this Event, so nothing in this call tree names what reads it).
//    +0x18  a plain dword, stamped 0x32 (50) right before the thread
//           launches - most likely a poll interval in milliseconds the
//           spawned thread (0x6F6C5660, below this dump's depth cut) reads
//           back through `this`.
//
//  The function-level SEH frame (the `push -1 / push offset SEH_.../
//  mov eax,fs:0` prologue, the matching `mov fs:0,ecx` epilogue restore and
//  the scope-table funclets after the `retn`) is the same
//  `__except_handler4`-shaped frame this toolchain's fixed /GS- /EHs-c-
//  never reproduces byte for byte regardless of source shape (see
//  docs/msvc-vc8-idioms.md and Sync/evtschedshutdown.cpp's own note) - the
//  Event constructor two lines below is enough by itself to make MSVC
//  synthesize one, whether or not this source spells out a `__try`.  Not
//  chased further; see the BEHAVIOUR entry.
//
//  g_pauseLatch is threadpausewait.cpp's own dword_6FAD1408 - the same
//  global sub_6F6C5440 (PauseAndWaitBriefly) swaps and re-arms.
//============================================================================
#include "storm.h"    // placement operator new
#include "event.h"

extern long g_pauseLatch;         // dword_6FAD1408

//  0x6F6C5660 - the spawned thread's own entry point.  Below this dump's
//  depth cut (its own call tree is not in this batch) and only ever taken
//  by address here, never called directly - a naked redirect to its own
//  real address all the same, since a declaration alone leaves nothing for
//  the linker to bind &OsTimeThreadMain to.
__declspec(naked) unsigned int __stdcall OsTimeThreadMain(void* /*arg*/)
{
    __asm { mov eax, 06F6C5660h }
    __asm { jmp eax }
}

//  0x6F6D8540 - _beginthreadex wrapper, already reconstructed generically
//  in Sync/evtsched.h (LaunchWorkerThread) for EvtSched's own worker
//  threads; the same function, reused here for the "OsTime" thread.
BOOL __fastcall LaunchWorkerThread(void* startAddress, void* argList,
                                    HANDLE* outHandle, void* unused);

void __fastcall InitOsTimeThread(void* self)
{
    char* base = (char*)self;

    HANDLE* handleSlot = (HANDLE*)(base + 0x10);
    ((Event*)handleSlot)->ZeroHandle();

    Event* stopEvent = (Event*)(base + 0x14);
    new (stopEvent) Event(TRUE, FALSE);   // manual-reset, initially unsignalled

    g_pauseLatch = (long)self;

    stopEvent->Reset();
    *(unsigned int*)(base + 0x18) = 0x32;

    LaunchWorkerThread((void*)&OsTimeThreadMain, (void*)0,
                        handleSlot, (void*)"OsTime");
}

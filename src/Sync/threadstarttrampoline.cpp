//============================================================================
//  0x6F6E2B10 - a generic __fastcall(ecx, edx, stack: owner, callback,
//  stackSize) trampoline that relays all five values, plus a lazily-created
//  manager object read from an unidentified global, into a fixed
//  `__thiscall` entry point (sub_6F6E2050 - the LAN net provider's
//  listener-thread body, not reconstructed this session: only the calling
//  shape here is established, not what it does once the manager object
//  exists).
//
//  IDA's own name for the global (`lpAddend`) is a leftover from wherever
//  its FLIRT signature match came from, not a real identifier - there is no
//  numeric address for it in this dump (the same situation
//  docs/msvc-vc8-idioms.md already documents for `dwTlsIndex`), so it is
//  left undeclared as a `DATA` entry and reached here as a plain, unbound
//  `extern` the way any other not-yet-identified global would be. The null
//  check right after reading it (skip the whole relay if the manager does
//  not exist yet) is the only thing this call tree proves about it.
//============================================================================

//  0x6F6E2050 - only its calling convention is established: `this` (the
//  manager object `g_lpAddend` points at) in ecx, five plain stack
//  arguments and no use of edx at all - i.e. a plain non-virtual member
//  function (`__thiscall`), not a 2-register `__fastcall` free function.
//  Modelled as a method of a minimal, otherwise-unknown manager class so
//  the compiler picks that convention; nothing here reaches its body.
class UnidentifiedThreadManager
{
public:
    void NetThreadStart(int a0, int a4, int a8, void* callback, void* owner);
};

extern "C" UnidentifiedThreadManager* g_lpAddend;

void __fastcall SpawnListenerThread(int origEcx, int origEdx, int stackSize, void* callback, void* owner)
{
    UnidentifiedThreadManager* mgr = g_lpAddend;
    if (mgr)
        mgr->NetThreadStart(origEcx, origEdx, stackSize, callback, owner);
}

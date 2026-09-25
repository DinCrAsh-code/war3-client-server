//============================================================================
//  sub_6F6D9500/sub_6F6D9510 (0x6F6D9500/0x6F6D9510) - two near-identical
//  three-call sequences reached from the same "[%u] Suspected deadlock"
//  wait/contention-tracked lock family sub_6F6D9820 belongs to (this
//  codebase's earlier sessions left that whole family unreconstructed -
//  see docs/targets/Probably_W3GS_ActionHandler.md). Each: lazily
//  initialise a fixed log-record table (sub_6F6D8A70/sub_6F6D8AD0, a
//  memset-then-stride-init loop over one of two disjoint globals), walk a
//  linked list of records off a second fixed global calling one more
//  per-node handler on each (sub_6F6D8CC0/sub_6F6D8F60), then release a
//  critical section - one already-identified as a known, shared global
//  (LeaveGlobalCritSec6FADA5E4, critsec.cpp), the other left ambiguous on
//  purpose (see critsec.cpp's own note on sub_6F6D8AC0: ecx names
//  `lpCriticalSection` with no concrete address, so guessing which lock
//  would be worse than leaving it declared, not defined).
//
//  ServerTock_0x27_PacketHandler's own dump later reconstructed the first
//  three of these four callees for real (debuglock.cpp: DeadlockLog0_Init,
//  DeadlockLog0_WalkActive, DeadlockLog1_Init, DeadlockLog1_WalkActive) -
//  this file now calls those instead of the raw sub_ thunks. sub_6F6D8AC0
//  is still deliberately not reconstructed - see critsec.cpp's own note:
//  ecx names `lpCriticalSection` with no concrete address, so guessing
//  which lock would be worse than leaving it declared, not defined.
//============================================================================

extern "C" void sub_6F6D8AC0();   // deliberately not reconstructed - see header
void DeadlockLog0_Init();
void DeadlockLog0_WalkActive();
void DeadlockLog1_Init();
void DeadlockLog1_WalkActive();
void __fastcall LeaveGlobalCritSec6FADA5E4();

void FlushDeadlockLog_6F6D9500()
{
    DeadlockLog0_Init();
    DeadlockLog0_WalkActive();
    sub_6F6D8AC0();
}

void FlushDeadlockLog_6F6D9510()
{
    DeadlockLog1_Init();
    DeadlockLog1_WalkActive();
    LeaveGlobalCritSec6FADA5E4();
}

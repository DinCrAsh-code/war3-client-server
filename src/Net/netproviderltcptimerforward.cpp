//============================================================================
//  0x6F62AB80/0x6F630440 - two small functions in the same address
//  neighbourhood as Net/netproviderltcp_notify.cpp's own sixteen vtable
//  slots (0x6F62ABF0, right next door), reached out of GameMain's own init
//  sequence rather than through any of those slots.  Their real owning
//  class/module is not established by this call tree; every out-of-scope
//  callee below is thunked to the real, unhooked body.
//============================================================================
#include "netproviderltcp.h"

//  0x6F629760/0x6F62BDA0/0x6F631680 are nullsub_23/24/25 - already real,
//  non-naked reconstructions (Misc/gamemain_trivial_leaves.cpp) under
//  their funcmap names EmptyStub23/24/25.  This file used to carry its own
//  second, naked set of redirects to the same three addresses
//  (NoOp_6F629760/NoOp_6F62BDA0/NoOp_6F631680) - a plain duplicate, not a
//  different signature this time, so just call the real bodies instead.
void __fastcall EmptyStub23();
void __fastcall EmptyStub24();
void __fastcall EmptyStub25();

//  0x6F631220 is TailRedirectToThreadPoolWorker (Misc/gamemain2_
//  trivialglobals.cpp) - a smarter thunk than a naive self-redirect would
//  be: the shipped body at 0x6F631220 is itself the single instruction
//  `jmp sub_6F6C6A60`, so that file encodes the real destination directly
//  rather than re-entering 0x6F631220.  This file used to carry a second,
//  ordinary naked redirect to 0x6F631220 under the name
//  PrepareTimerForward_6F631220 - a duplicate, and a strictly worse one
//  (self-redirect-shaped, so not even hookable per CLAUDE.md's own rule).
void TailRedirectToThreadPoolWorker();

//  0x6F630460 is ConstructEvtSchedContextArray (Sync/evtschedcontextthunks.
//  cpp) - fastcall, `retn 0`.  This file used to carry a second, naked
//  redirect to the same address under the name ForwardTimerCount_6F630460,
//  declared (count, context) in (ecx, edx) - which turned out to be the
//  *correct* reading of 0x6F630460's own body (a power-of-two bucket-count
//  computation over ecx, a plain pointer stash of edx into a global); the
//  canonical evtschedcontextthunks.cpp declaration had ecx/edx swapped and
//  is now fixed there instead of duplicated here.
void __fastcall ConstructEvtSchedContextArray(unsigned int count, void* context);

//----------------------------------------------------------------------------
//  0x6F62AB80 - run the three no-op callees and the (also out of scope)
//  preparation call, clamp the count argument up to at least 1, then
//  tail-forward (count, context) on.
//----------------------------------------------------------------------------
void __fastcall ClampAndForwardTimerCount(unsigned int count, void* context)
{
    EmptyStub23();
    EmptyStub24();
    EmptyStub25();
    TailRedirectToThreadPoolWorker();

    if (count < 1)
        count = 1;

    ConstructEvtSchedContextArray(count, context);
}

//  0x6F630200 is GameMain_sub_6F630200 (Misc/gamemainenginethunks.cpp) -
//  __stdcall, one stack dword, `retn 4`.  This file used to carry a second,
//  naked redirect to the same address under the name
//  MarkTimerModuleStarted_6F630200; a plain duplicate, same signature.
unsigned int __stdcall GameMain_sub_6F630200(unsigned int);

extern Event g_ltcpTimerReadyEvent;   // unk_6FACEB90
extern int   g_ltcpTimerModuleFlag;   // dword_6FACEB4C

//----------------------------------------------------------------------------
//  0x6F630440 - signal the module's own ready event, mark it started, then
//  clear the flag GameMain reads through sub_6F62ABD0's own alias.
//----------------------------------------------------------------------------
void SignalTimerModuleReady()
{
    g_ltcpTimerReadyEvent.Set();
    GameMain_sub_6F630200(1);
    g_ltcpTimerModuleFlag = 0;
}

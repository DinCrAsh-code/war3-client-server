//============================================================================
//  Net wave 13 - the ground-truth pause call chain.
//
//  The operator captured a real, live-game call stack (LAN/loopback, so
//  `Net::NetProviderLOOP`) at the instant `CGameUI::SetGamePaused` executed
//  from a pause-button press.  Sixteen return addresses above it, all new to
//  this pipeline.  See docs/targets/PacketToPause_call_trace.md for the full
//  17-frame writeup - this file is the ABI-safe placeholder for the nine
//  frames of it that are not already reconstructed elsewhere in `src/`.
//
//  Three of the sixteen were *already* reconstructed, under names this wave
//  did not know going in - a grep-first hit, not new work:
//
//      0x6F629A90  CObserver::Method_0x10   (observer_slots.cpp)
//      0x6F62A7B0  CObserver::Method_0x14   (observer_slots.cpp)
//      0x6F62A5D0  SObserverRecord::Invoke  (observer_slots.cpp, since
//                                            reconstructed - DIFFERS, see
//                                            funcmap.py's BEHAVIOUR entry)
//
//  Two more (0x6F5378E0 CNetPlayerRecord::FireToObserverLists and
//  0x6F53B790 CNetData_FireCommand) and the actual pause wire-command
//  builder, 0x6F5402B0 CNetCommandPause_BuildAndFire, were already
//  reconstructed (EXACT/EXACT/IDENTICAL) by the session that produced
//  docs/targets/PauseGame_LAN_networking.md - this wave's live stack is the
//  first hard confirmation that closure is the real one the pause button
//  drives, not just a plausible static path.
//
//  What is left, and is thunked here, is the surrounding tick/event pump
//  (frames far from the verb itself) plus the two functions that *are* the
//  verb decode:
//
//      0x6F3012E0  CGameUICommandRouter::Method_0x0C  - the ~230-instruction,
//                  many-chunk message-id switch this observer callback
//                  is.  Cases 3 and 4 of its low switch (edx in [0,0x20])
//                  are the actual pause/resume decode - see the doc for the
//                  exact case bodies (0x6F2FB760/0x6F2FB780) and the
//                  argument mapping onto SetGamePaused's own five
//                  parameters, confirmed against the already-reconstructed
//                  signature in gameuisetgamepaused.cpp.  Not compiled here:
//                  its ~18 other case chunks scatter across a much wider
//                  address range (0x6F2F3xxx-0x6F301xxx) unrelated to pause,
//                  and reproducing the whole multi-thousand-instruction
//                  module was out of this wave's budget - a genuine future
//                  target, now that its real name and its two load-bearing
//                  cases are on record.
//      0x6F550730  CNetData::DispatchActionByte - the ~133-case turn-stream
//                  action-byte dispatcher docs/targets/PauseGame_LAN_
//                  networking.md already named as the caller of
//                  CNetCommandPause_BuildAndFire, itself now confirmed live
//                  by this stack.  1107 instructions; already flagged TODO
//                  by that doc, unchanged here.
//
//  The other seven are the `CNetEventGameTick`/`CNetEvent` tick-accumulation
//  and queued-record drain machinery waves 8-12 already mapped as a whole
//  (docs/targets/NetRouter_NetPlayer_NetClient_oneslot_sweep.md, "wave 12"),
//  now walked end to end by a real stack for the first time and each given
//  its first real name.  All nine `this`-taking frames operate on the same
//  object waves before this one call `SNetSessionInfo`/`CNetData`
//  (netdata.h) - offsets 0x278 (m_gameState), 0x610 (m_activeIndex) and
//  several more this wave did not name are read by more than one of them,
//  but none of these bodies is reconstructed here, so nothing is claimed
//  about which view is the right one; `void*` stands in for all of them.
//
//  **Every retn byte count below comes off this wave's own call-chain
//  capture** (the real stack, not a guess) or off asm/sub_*.md - both agree
//  where both exist.  A `__fastcall` free function with an unused `edx`
//  parameter is the same pattern src/Agent/vslot4_refcnt_forwards.cpp and
//  src/Config/parseconfigint.cpp already use for "this in ecx, one more
//  stack argument than __fastcall's own two registers would give it" -
//  copied here rather than modelling a class for objects whose layout this
//  wave did not chase.  None of these nine is called from anywhere else in
//  `src/` yet, so nothing downstream depends on the exact argument *types*
//  below - only the byte count, which is what thunk_abi_audit.py checks.
//============================================================================
#include <cstddef>
#include <windows.h>
#include "netdata.h"
#include "netdataeventqueue.h"   // CNetEventQueue::Pop, CNetDataEventOwner::ReleaseNetEvent

//  RecordCtor_6F6515B0/NetMsgReadBytePair_6F6516F0/NetMsgReadDwordPair_
//  6F651790 - already reconstructed, netevent_replaystream_records.cpp /
//  netmsgio_651.cpp (re-declared identically here, no shared header -
//  CLAUDE.md's own "the awkward ones carry a note").
extern CDataStore* __fastcall RecordCtor_6F6515B0(CDataStore* store, void* record);
extern CDataStore* __fastcall NetMsgReadBytePair_6F6516F0(CDataStore* self, unsigned char* dst);
extern CDataStore* __fastcall NetMsgReadDwordPair_6F651790(CDataStore* self, unsigned char* dst);

//  0x6F39E5C0 - Containers/rotatinghash.cpp.
extern unsigned int __fastcall RotatingHash(const void* data, unsigned int length);

//  0x6F280B90 - Containers/rotatingbytemix.cpp.
extern void __fastcall RotatingByteMixAccumulate_6F280B90(unsigned int value, unsigned int* acc);

//  0x6F4434A0 - Net/netdata_recordview_ctor.cpp.
extern CDataStore* __fastcall ConstructRecordView_6F4434A0(void* self, void* unused_edx,
                                                            void* ptr, unsigned int len);

//  0x6F551D80 - Net/netevent_dispatch.cpp.
extern int __fastcall CNetEventTypeDispatcher_ConstructAndBroadcast(
    SNetSessionInfo* self, void* unused_edx, void* record);

//  0x6F54CD40 - Net/battlenetevent_dispatch.cpp - this session's own target.
extern int __fastcall CBattleNetEventTypeDispatcher_ConstructAndBroadcast(
    SNetSessionInfo* self, void* unused_edx, void* record);

//  Overlays only the fields CNetData_DispatchOneQueuedRecord itself reads
//  off a queued CNetEventNode - that type stays deliberately opaque
//  (netdataeventqueue.h's own header comment) so this is a local view, not
//  an addition to it.
struct SQueuedNetEventRecordView
{
    char          m_reservedLink[8];   // +0x00 - the TSLink<T> pair itself
    void*         m_dataPtr;           // +0x08
    unsigned int  m_dataLen;           // +0x0C
    char          m_reserved10[4];
    unsigned char m_typeByte;          // +0x14
    unsigned char m_slotFlag;          // +0x15
};

#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  QueryScaledPerformanceTick_6F6C4E10's own state - see its own header
//  below and the DATA table entry in tools/funcmap.py.
extern int   g_perfTickLatched;      // dword_6FAD141C
extern int   g_perfCounterUsable;    // dword_6FAD1418
extern float g_perfCounterScale;     // flt_6FAD140C

//  0x6F553470 - CNetData_AccumulateAndDrainTicks, defined further below in
//  this file (still an ADDR_THUNK, plain C++ linkage - no extern "C", to
//  match its own definition's mangled name exactly); forward-declared here
//  so PumpFromTls (which precedes it in this file's own frame-number
//  order) can call it.
void __fastcall CNetData_AccumulateAndDrainTicks(
                          void* self, void* /*unused_edx*/, void* stackArg);

//  CNetData_DispatchOneQueuedRecord (0x6F5530D0) and the new
//  CNetData_BadReplayEventCheck_6F54EB00 thunk are both defined further
//  below/here; forward-declared so CNetData_DrainQueuedNetEvents (which
//  precedes them in this file's own frame-number order) can call them.
int __fastcall CNetData_DispatchOneQueuedRecord(
                          void* self, void* /*unused_edx*/, unsigned int allow);

//  0x6F54EB00 - "CNetData::EnqueueReplayTurn: Bad replay event %u" -
//  genuinely deep, out of this batch's own scope (see
//  CNetData_DrainQueuedNetEvents's own header comment below): a
//  0x908-byte SEH-protected frame around a 34-case wire-format-tag jump
//  table, each case a further out-of-line decode routine with no dump of
//  its own.  `this` in ecx, bare `retn` (no stack args) confirmed off its
//  own dump tail.
extern "C" __declspec(naked) int __fastcall
CNetData_BadReplayEventCheck_6F54EB00(void* self)
{
    __asm { mov eax, 0x6F54EB00 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  [16] 0x6F62D710 - outermost captured frame.  A critical-section-guarded,
//  once-per-poll check: enters the lock at `this+0x10`, tests a "shutting
//  down" flag at `this+0x2C` and an elapsed-`GetTickCount` gate at
//  `this+0x40`, and on the slow path posts input message id 5 through
//  PostQueuedInputMessage (0x6F632460, already reconstructed as a thunk in
//  src/Input/inputeventbusthunk.cpp) with a computed float delta as its
//  payload.  Bare `retn` (bytes cleaned by the *shipped* function are 0)
//  - no stack arguments confirmed by the dump's own `retn` with no operand.
//
//  Attempted this session (batch-F-net-misc-2) and found a genuine,
//  confirmed register-ABI blocker, the same structural class
//  tools/unhookable.py documents elsewhere: **the shipped body never
//  loads `this` from `ecx` at all** - `lea edi,[esi+10h]` is its own
//  first real instruction, reading `esi` directly with no `mov esi,ecx`
//  anywhere in the whole 44-instruction stream (confirmed with
//  `tools/dumpfn.py --calls`, not just the top few lines). The whole-
//  program optimiser has proven `esi` already holds the right object at
//  every real call site to this function and hard-wired that assumption
//  the same way it does for the register-passed arguments
//  `tools/unhookable.py` already names (`CConfigFile::GetIntValue`'s own
//  `edx`, `ValueAt`'s own `eax`/`esi`) - no MSVC `__fastcall`/`__thiscall`
//  spelling can put an object pointer in `esi` on entry, so a real C++
//  reconstruction would silently read garbage the moment any caller this
//  session cannot see relies on the same assumption. The declared
//  `__fastcall(void* self)` signature below is therefore not a claim
//  about the real calling convention, only about argument *byte count*
//  for thunk_abi_audit.py's own purposes (`retn 0`) - left a thunk on
//  purpose rather than guessed at, and NOT added to tools/unhookable.py
//  without a debugger confirming it (that file's own standard, not met
//  by static disassembly alone).
//----------------------------------------------------------------------------
ADDR_THUNK(void __fastcall NetTickPulse_MaybeFire(void* self),
                       0x6F62D710)

//----------------------------------------------------------------------------
//  0x6F4BEAD0 - a genuinely separate, deep subsystem PumpFromTls's own
//  entry calls first and whose result it never reads: a 0x2188-byte
//  SEH-protected frame (`__except_handler4`-shaped, this build's fixed
//  /GS- /EHs-c- cannot reproduce - docs/msvc-vc8-idioms.md) walking a
//  linked structure of up to 0x20 records, each carrying its own
//  callback-pointer/argument pair invoked through `call ecx`, past a
//  Storm_501 formatted-string call - reads as a periodic flush of some
//  buffered diagnostic/log queue, not otherwise reached by this session's
//  nine tick/dispatch frames.  Below this session's scope - naked redirect,
//  no arguments (nothing pushed before the call, `retn 0` off its own tail).
extern "C" __declspec(naked) void __cdecl
PumpDeferredDiagnosticQueue_6F4BEAD0()
{
    __asm { mov eax, 0x6F4BEAD0 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F6C4E10 - the "current tick, high-resolution where available" helper
//  PumpFromTls's own third call feeds into [13] as its `stackArg`: latch
//  once whether QueryPerformanceCounter is usable on this machine, and
//  either return `GetTickCount()` directly or the QueryPerformanceCounter
//  count scaled by a fixed factor and rounded to the nearest integer (the
//  `fnstcw`/round-to-nearest-bit-set/`fldcw`/`fistp`/restore dance is
//  MSVC's own `(int)round(scaled)` idiom for x87 without touching the
//  ambient control word) - written naked because that FPU control-word
//  save/restore sequence is exact-instruction load-bearing and not worth
//  re-deriving through a C++ spelling.  `retn 0`, no arguments.
//----------------------------------------------------------------------------
//  Not extern "C" - a real, scored reconstruction (funcmap.py maps
//  sub_6F6C4E10 to it); extract_gen.py reads a generated function's C++
//  name off the friendly-name comment normal C++ decoration carries after
//  `PUBLIC ?Name@@...@Z`, which extern "C" suppresses (see
//  playertable_installhandler.cpp's own note on PlayerTable_Method679460
//  for the same fix).
__declspec(naked) unsigned int __cdecl
QueryScaledPerformanceTick_6F6C4E10()
{
    __asm
    {
        sub     esp, 0Ch
        cmp     dword ptr g_perfTickLatched, 0
        jnz     short have_latched
        mov     dword ptr g_perfTickLatched, 1
have_latched:
        cmp     dword ptr g_perfCounterUsable, 0
        jz      short use_tick_count
        lea     eax, [esp]
        push    eax
        call    DWORD PTR [QueryPerformanceCounter]
        fild    qword ptr [esp+0Ch]
        fnstcw  [esp+2]
        movzx   eax, word ptr [esp+2]
        fmul    dword ptr g_perfCounterScale
        or      eax, 0C00h
        mov     dword ptr [esp], eax
        fldcw   word ptr [esp]
        fistp   qword ptr [esp]
        mov     eax, dword ptr [esp]
        fldcw   [esp+2]
        add     esp, 0Ch
        retn
use_tick_count:
        add     esp, 0Ch
        jmp     DWORD PTR [GetTickCount]
    }
}

//----------------------------------------------------------------------------
//  [14] 0x6F5538F0 - reads the current thread's `CNetData` record the same
//  way netdata.h's `NetDataRecords()` does (TLS slot 0x0D, `+0x10`, `+0x8`),
//  after first pumping the unrelated diagnostic queue above, then calls
//  [13] with the current tick (QueryScaledPerformanceTick_6F6C4E10, above)
//  as its `stackArg`.  Bare `retn`: no incoming `ecx` is read and no stack
//  argument is used, so it is a plain zero-argument entry point rather than
//  a member function.
//----------------------------------------------------------------------------
void CNetData_PumpFromTls()
{
    PumpDeferredDiagnosticQueue_6F4BEAD0();

    CNetData* records = NetDataRecords();

    unsigned int nowTicks = QueryScaledPerformanceTick_6F6C4E10();
    CNetData_AccumulateAndDrainTicks(records, 0, (void*)nowTicks);
}

//----------------------------------------------------------------------------
//  [13] 0x6F553470 - "the master decoder/tick-accumulator" docs/targets/
//  PauseGame_LAN_networking.md already named without a body.  Confirmed
//  live: accumulates elapsed time into a per-session tick budget, and once
//  it clears a threshold, calls [12] to drain whatever queued records that
//  much simulated time has made ready.  `retn 4` per the dump's own
//  `6F5538E6  retn  4` (asm/sub_6F553470_0x6F553470_calltree_asm.md).
//----------------------------------------------------------------------------
ADDR_THUNK(void __fastcall CNetData_AccumulateAndDrainTicks(
                          void* self, void* /*unused_edx*/, void* stackArg),
                       0x6F553470)

//----------------------------------------------------------------------------
//  [12] 0x6F553440 - drains one queued record via [11] with a literal
//  `true` "allow" flag; if none was ready, checks for a bad/oversized
//  replay event (`sub_6F54EB00`, string "CNetData::EnqueueReplayTurn: Bad
//  replay event %u") and retries [11] once more on that path.  Bare
//  `retn` - two of its three exits in the dump (0x553454, 0x55346A,
//  0x55346E) all end in a bare `retn`, so no stack arguments.
//
//  Reconstructed for real (batch-K-netprovider-misc) - this root itself
//  is 21 instructions and does nothing but sequence the two calls below,
//  both of which stay thunked: sub_6F54EB00 is a genuine structural
//  blocker, not scope triage - a 0x908-byte SEH-protected frame around a
//  34-case wire-format-tag jump table, each case its own out-of-line
//  decode routine with no dump of its own (`ida_query calltree_asm` at
//  --depth 2 would pull in 34+ more undumped callees for one function),
//  and [11] (CNetData_DispatchOneQueuedRecord) is 251 instructions of the
//  identical shape one level up.
//----------------------------------------------------------------------------
int __fastcall CNetData_DrainQueuedNetEvents(void* self)
{
    if (CNetData_DispatchOneQueuedRecord(self, 0, 1))
        return 1;

    if (!CNetData_BadReplayEventCheck_6F54EB00(self))
        return 0;

    return CNetData_DispatchOneQueuedRecord(self, 0, 1);
}

//----------------------------------------------------------------------------
//  [11] 0x6F5530D0 - wave 12's own find, now given a name, and now a real
//  reconstruction (cnd-wave2-b, the Battle.net-dispatch session).  Walks
//  the +0xF38 CNetEventQueue's own TSList<CNetEventNode,0> from the tail
//  (queue->m_list.Prev(), peeked one node ahead of `current` so unlinking
//  it mid-body cannot break the walk - matches TSList<T,0>::Tail()'s own
//  "clamp to >0 or stop" shape (Storm/storm.h), just re-derived by hand at
//  the call site instead of calling it, presumably because the compiler
//  had already spilled the peeked value once and reused the spill rather
//  than re-deriving `Tail()`'s own short-circuit a second time), reads its
//  one-byte type tag at `+0x14`: below 0x40 it is a `CNetEvent` and goes to
//  CNetEventTypeDispatcher_ConstructAndBroadcast (netevent_dispatch.cpp,
//  [10] below); 0x40 and up it is a `CBattleNetEvent` and goes to the
//  mirror CBattleNetEventTypeDispatcher_ConstructAndBroadcast
//  (Net/battlenetevent_dispatch.cpp, this session's own new target -
//  Battle.net-only, so never reachable over the LAN/loopback path wave 13's
//  own capture used).
//
//  Five wire-format tags (0x17/0x1E/0x1F/0x21/0x2F), and only when the
//  record's slot matches self->m_activeIndex (XNOR'd against the record's
//  own +0x15 flag byte - see the gating expression below) and an
//  unidentified self+0x278 dword reads >= 4, get an extra step first: wrap
//  the same record's bytes in a second, short-lived CDataStore (or, for
//  0x1E/0x1F, run them straight through RotatingHash, Containers/
//  rotatinghash.cpp) and fold a small rolling byte-mix hash
//  (RotatingByteMixAccumulate_6F280B90, Containers/rotatingbytemix.cpp -
//  seeded with the raw type byte) over a couple of its wire fields.  If
//  `allow` (this function's own stack argument, always 1 at both call
//  sites in this closure) is 0 at that point, the *whole call* returns
//  immediately - the record is left on the queue, undispatched, for a
//  later call to pick back up; every real caller passes 1, so that branch
//  is dead in practice but reproduced literally since nothing here proves
//  it can never fire.  Otherwise the hash folds into
//  self->m_syncData.m_checksum (NTempest::CSyncData, Net/syncdata.h) via
//  the exact same byte-mix, inlined at this one call site rather than
//  calling RotatingByteMixAccumulate_6F280B90 out of line - the dump's own
//  register-level shape at 0x6F55336E-0x6F5533BA is that function's body
//  duplicated instruction for instruction, the same /OPT:ICF-less
//  duplication this neighbourhood is already full of.
//
//  Every record, gated or not, then falls into the same tail: pop it off
//  the queue (CNetEventQueue::Pop, netdataeventqueuepop.cpp), dispatch it,
//  release it (CNetDataEventOwner::ReleaseNetEvent, netdataeventrelease.cpp)
//  and continue to the peeked next node.  The function's own return value
//  (`didWork`) is 1 only if *some* processed record hit the 0x1E/0x1F
//  branch above (`var_60` in the dump) - and that per-iteration flag is
//  only *reset* inside the gated branch, so a gated 0x1E/0x1F hit on one
//  iteration can still read back as "true" on a later, non-gated iteration
//  that never touches it again; reproduced as a persistent local rather
//  than "fixed", since that stale-carry is exactly what the dump does.
//
//  `retn 4` confirmed both by the dump (`6F553407 retn 4`) and by [12]'s
//  own call site (`push 1` before every call).  DIFFERS, not EXACT: the
//  type-0x21/0x2F local CDataStore is built through a raw stack buffer
//  cast (ConstructRecordView_6F4434A0 placement-constructs into it) rather
//  than a plain `CDataStore` local the way the type-0x17 branch's own
//  manual field sequence is, and the self+0x278 gate and the two ~15-byte
//  local record-header shapes (SQueuedNetEventRecordView's own reserved
//  gaps, and the type-0x17 header struct below) are read straight off the
//  disassembly rather than independently confirmed - see
//  tools/funcmap.py's own BEHAVIOUR entry for the exact reason string.
//----------------------------------------------------------------------------
int __fastcall CNetData_DispatchOneQueuedRecord(
    void* selfVoid, void* /*unused_edx*/, unsigned int allow)
{
    SNetSessionInfo* self = (SNetSessionInfo*)selfVoid;
    CNetEventQueue* queue = (CNetEventQueue*)((char*)self + 0xF38);

    int didWork = 0;
    int tail = queue->m_list.TailLink();
    if (tail <= 0)
        return didWork;

    SQueuedNetEventRecordView* current = (SQueuedNetEventRecordView*)tail;
    int turnHashFlag = 0;   // var_60 - see the header comment on its stale-carry

    do
    {
        int p = (int)queue->m_list.Prev((CNetEventNode*)current);
        SQueuedNetEventRecordView* next = (p > 0) ? (SQueuedNetEventRecordView*)p : 0;

        unsigned char rawType = current->m_typeByte;

        bool gated = ((current->m_slotFlag != 0) == (self->m_activeIndex == 0)) &&
                     (*(int*)((char*)self + 0x278) >= 4) &&
                     (rawType < 0x40);

        if (gated)
        {
            unsigned int hashResult = rawType;
            turnHashFlag = 0;

            if (rawType == 0x1E || rawType == 0x1F)
            {
                turnHashFlag = 1;
                unsigned int h = RotatingHash(current->m_dataPtr, current->m_dataLen);
                RotatingByteMixAccumulate_6F280B90(h, &hashResult);
            }
            else if (rawType == 0x17)
            {
                CDataStore localStore;
                localStore.m_field4   = current->m_dataPtr;
                localStore.m_field8   = 0;
                localStore.m_field0xC = -1;
                localStore.m_field10  = (int)current->m_dataLen;
                localStore.m_readPos  = 0;

                struct
                {
                    unsigned int  f0;
                    unsigned char f4;
                    char          pad[3];
                    unsigned int  f8;
                    unsigned int  fC;
                } hdr;
                RecordCtor_6F6515B0(&localStore, &hdr);

                RotatingByteMixAccumulate_6F280B90(hdr.f0, &hashResult);
                RotatingByteMixAccumulate_6F280B90(hdr.f4, &hashResult);
                RotatingByteMixAccumulate_6F280B90(hdr.f8, &hashResult);
            }
            else if (rawType == 0x21)
            {
                unsigned char storeBuf[sizeof(CDataStore)];
                CDataStore* view = ConstructRecordView_6F4434A0(
                    storeBuf, 0, current->m_dataPtr, current->m_dataLen);
                unsigned char bytes[2];
                NetMsgReadBytePair_6F6516F0(view, bytes);
                RotatingByteMixAccumulate_6F280B90(bytes[0], &hashResult);
                RotatingByteMixAccumulate_6F280B90(bytes[1], &hashResult);
                view->~CDataStore();
            }
            else if (rawType == 0x2F)
            {
                unsigned char storeBuf[sizeof(CDataStore)];
                CDataStore* view = ConstructRecordView_6F4434A0(
                    storeBuf, 0, current->m_dataPtr, current->m_dataLen);
                unsigned int dwords[2];
                NetMsgReadDwordPair_6F651790(view, (unsigned char*)dwords);
                RotatingByteMixAccumulate_6F280B90(dwords[0], &hashResult);
                RotatingByteMixAccumulate_6F280B90(dwords[1], &hashResult);
                view->~CDataStore();
            }

            if (!allow)
                return didWork;

            RotatingByteMixAccumulate_6F280B90(
                hashResult, (unsigned int*)&self->m_syncData.m_checksum);
        }

        queue->Pop(current);
        if (rawType < 0x40)
            CNetEventTypeDispatcher_ConstructAndBroadcast(self, 0, current);
        else
            CBattleNetEventTypeDispatcher_ConstructAndBroadcast(self, 0, current);
        ((CNetDataEventOwner*)self)->ReleaseNetEvent(current);

        if (turnHashFlag)
            didWork = 1;

        current = next;
    } while (current != 0);

    return didWork;
}

//----------------------------------------------------------------------------
//  [10] 0x6F551D80 - CNetEventTypeDispatcher_ConstructAndBroadcast, real as
//  of the session that reconstructed the type-0x1B replay-streaming state
//  machine and the two-jump-table CNetEvent dispatch (netevent_dispatch.cpp)
//  and its type-0x1B streaming-activation callee,
//  CNetEventTypeDispatcher_ReplayAckDispatch, 0x6F54EB00
//  (netevent_replaystream.cpp).  See both files' own header comments and
//  docs/targets/CNetEventTypeDispatcher.md.
//----------------------------------------------------------------------------

//  [9] 0x6F5516E0 and [8] 0x6F550730 - both reconstructed for real now, in
//  Net/netcommand_dispatch.cpp:
//
//      0x6F5516E0  CNetData::DispatchQueuedTurnRecord  (renamed from this
//                  wave's own CNetData_ParseTurnCommandStream - reading
//                  the dump showed it calls [8] exactly *once* per call,
//                  not in a loop over an embedded stream the old name
//                  implied; see netcommand_dispatch.cpp's own header
//                  comment and docs/targets/CNetData_DispatchActionByte.md)
//      0x6F550730  CNetData::DispatchActionByte  (the 133-case turn-stream
//                  action-byte dispatcher this wave's live stack confirmed
//                  calls CNetCommandPause_BuildAndFire from jump-table
//                  index 1)
//
//  See docs/targets/CNetData_DispatchActionByte.md for the full writeup:
//  which of the 133 cases call an already-reconstructed builder, which
//  eleven are new this session, and which small leaf helpers are still
//  correctly-ABI'd naked thunks rather than full reconstructions.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//  [1] 0x6F3012E0 - `CObserver::Method_0x0C`'s override for the class this
//  wave calls `CGameUICommandRouter` (no RTTI/vtable evidence was chased
//  for its real shipped name - "CGame UI-command router" is docs/targets/
//  PauseGame_LAN_networking.md's own working name, kept here rather than
//  invented fresh).  Switches on a packed dword at `msg+8`: values in
//  [0,0x20] are a flat ~33-case UI-command enum, decoded by a jump table
//  at `0x6F301308`; a handful of larger packed values (0x1xxxx/0x3xxxx/
//  0x4xxxxxxx-shaped) are a second, unrelated message family collapsed to
//  a generic virtual-call fallback through `this+0x1B4`.
//
//  **Cases 3 and 4 of the flat enum are the pause/resume verb decode this
//  whole search was for** - not a generic `GameActionId`, but this
//  router's own small command-id space:
//
//      case 3 (0x6F2FB760): SetGamePaused(paused=1, actingSlot=[msg+0x15],
//                            notify=1, latch=0, record=0);  msg=1 -> "PAUSE"
//      case 4 (0x6F2FB780): SetGamePaused(paused=0, actingSlot=[msg+0x15],
//                            notify=1, latch=0, record=0);  msg=0 -> "RESUME"
//
//  read directly off the dump's own push order (`push 0; push 0; push 1;
//  push edx; push <1|0>` before `call sub_6F2FA7E0`) and matching
//  gameuisetgamepaused.cpp's own five-parameter signature slot for slot -
//  the same 1/0 in the fifth push is the only difference between the two
//  cases.  `retn 4`, `this` in `ecx`, one stack argument (the message
//  pointer) - confirmed by every case's own `retn 4` and by the vtable
//  slot this fills (`Method_0x0C`, offset 0x0C, observer.h).
//
//  Not compiled: IDA attributes ~18 more `loc_` case chunks to this same
//  function, physically scattered from 0x6F2F3xxx to 0x6F301xxx (chat,
//  minimap tooltips, latency display, cheat-toggle echoes and more,
//  addresses in asm/sub_6F3012E0_0x6F3012E0_calltree_asm.md) - none of
//  them pause-shaped, and reproducing the whole module was out of this
//  wave's budget.  A genuine future target now that it has a name.
//----------------------------------------------------------------------------
ADDR_THUNK(int __fastcall CGameUICommandRouter_Method_0x0C(
                          void* self, void* /*unused_edx*/, const void* msg),
                       0x6F3012E0)

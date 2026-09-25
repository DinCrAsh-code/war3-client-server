//============================================================================
//  A hand-tuned wait/event-slot pool underlying ConditionVariable::Wait and
//  ::NotifyOne (0x6F6D9820/0x6F6D8340) and the two FlushDeadlockLog helpers'
//  own callees. Every function in this cluster uses a non-standard,
//  hand-picked register convention instead of an ordinary calling
//  convention (usually the object pointer in a register the compiler would
//  never choose for `this` - esi, eax, or nothing at all, expecting a fixed
//  global base already sitting in ebx from an outer caller) - the same
//  discovery gameaction0x26_notify.cpp's own ConditionVariable::SignalOne/
//  NotifyOne pair already documents for this exact family. Reproduced as
//  __declspec(naked) bodies, transcribed instruction-for-instruction from
//  the dump, rather than guessed at as ordinary C++ that the compiler would
//  never actually emit this way.
//
//  Named for what each does, with its real address in a comment; nothing
//  here is inferred type information beyond what the instructions
//  themselves establish.
//============================================================================
#include <windows.h>

//  These eight are reconstructed for real elsewhere in this repo - the first
//  six in critsec.cpp, the last two in deadlocklog_flush.cpp - and every one
//  of them is defined there as ordinary C++, not `extern "C"`.  Declaring
//  them `extern "C"` here asked the linker for @ThreadSleep@4 / _SpinPause /
//  @InterlockedExchangeAt@8 ... while those translation units emit
//  ?ThreadSleep@@YIXI@Z and friends, so the two never met and every build
//  died with LNK2019 on all eight.  The C++ spelling below is what actually
//  matches their definitions; do not "tidy" the extern "C" back on.
long __fastcall InterlockedExchangeAt(volatile long*, long);
long __fastcall InterlockedCompareExchangeAt(volatile long*, long, long);
long __fastcall InterlockedIncrementAt(volatile long*);
long __fastcall InterlockedDecrementAt(volatile long*);
void __fastcall ThreadSleep(unsigned int);
void SpinPause();
void FlushDeadlockLog_6F6D9500();
void FlushDeadlockLog_6F6D9510();

//  nullsub_1 (0x6F20BC60) is reconstructed in nullsub.cpp - a single `ret`.
//  It stays `extern "C"` because the call sites below are transcribed
//  `__asm { call nullsub_1 }`, which name the C-decorated symbol.
extern "C" void nullsub_1();

extern "C" long dword_6FAD1D78[];
extern "C" long dword_6FAD1D88[];
extern "C" long dword_6FAD1D80[];
extern "C" long unk_6FAD1D90[];
extern "C" long dword_6FA9FF04;
extern "C" void* hHandle[];
extern "C" void* hEvent[];
extern "C" long dword_6FADA608;

static const char aUSuspectedDeadWait[] = "[%u] Suspected deadlock, thread %u\n";

void Wait_DeadlockTracked();

struct ConditionVariable
{
    void Wait(unsigned int);
    void NotifyOne(unsigned int);
};

//----------------------------------------------------------------------------
//  0x6F6D8080 - acquire one wait-event slot for the calling thread: bump a
//  per-caller reference count (dword_6FAD1D78[esi]) down and a generation
//  count (dword_6FAD1D88[esi]) up, then spin/sleep-wait on a per-slot busy
//  flag (unk_6FAD1D90 + a computed index) until it is claimed. Takes its
//  "index" in eax (not ecx/this) - only ever reached from sub_6F6D81E0,
//  itself part of this same hand-written family.
//----------------------------------------------------------------------------
__declspec(naked) void EventPool_Acquire()
{
    __asm
    {
        push    esi
        mov     esi, eax
        lea     ecx, dword_6FAD1D78[esi*4]
        call    InterlockedDecrementAt
        lea     ecx, dword_6FAD1D88[esi*4]
        call    InterlockedIncrementAt
        and     eax, 7FFh
        shl     esi, 0Bh
        add     esi, eax
        lea     esi, unk_6FAD1D90[esi*4]
        xor     edx, edx
        mov     ecx, esi
        call    InterlockedExchangeAt
        test    eax, eax
        jnz     short acquired
        push    edi
        mov     edi, dword_6FA9FF04
    spin:
        xor     edx, edx
        mov     ecx, esi
        call    InterlockedExchangeAt
        test    eax, eax
        jnz     short gotit
        call    SpinPause
        sub     edi, 1
        jnz     short spin
        xor     ecx, ecx
        call    ThreadSleep
        jmp     short spin
    gotit:
        pop     edi
    acquired:
        pop     esi
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6D80F0 - release a wait-event slot (`eax` = the value returned by
//  Acquire above): optionally ResetEvent the slot's own handle first (when
//  the stack argument is nonzero), then spin/sleep-wait to hand the slot's
//  busy flag back.
//----------------------------------------------------------------------------
__declspec(naked) void EventPool_Release()
{
    __asm
    {
        push    ebx
        push    esi
        mov     esi, ecx
        push    edi
        lea     ecx, dword_6FAD1D78[esi*4]
        mov     ebx, eax
        call    InterlockedIncrementAt
        cmp     dword ptr [esp+0Ch+8], 0
        jz      short skipReset
        mov     eax, ebx
        mov     ecx, esi
        shr     eax, 15h
        shl     ecx, 0Ah
        add     eax, ecx
        mov     edx, hHandle[eax*4]
        push    edx
        //  `call DWORD PTR [X]`, not `call X`.  In inline asm the bare form
        //  assembles to `E8 rel32` **at the import table slot itself**
        //  (REL32 relocation), so it executes the four bytes of the
        //  function pointer as code.  The bracketed form is `FF 15`
        //  (DIR32) - the indirect call through the slot that the dump's
        //  own `call ds:__imp_...` does.  Both canonicalise to `call SYM`,
        //  so the instruction score cannot tell them apart.  See
        //  docs/notes/import-call-indirection.md.
        call    DWORD PTR [ResetEvent]
    skipReset:
        lea     ecx, dword_6FAD1D80[esi*4]
        call    InterlockedIncrementAt
        and     eax, 7FFh
        shl     esi, 0Bh
        add     esi, eax
        lea     edi, unk_6FAD1D90[esi*4]
        mov     edx, ebx
        and     edx, 0FFE00000h
        mov     ecx, edi
        call    InterlockedExchangeAt
        mov     esi, eax
        test    esi, esi
        jz      short done
        mov     ebx, dword_6FA9FF04
    spin:
        mov     edx, esi
        mov     ecx, edi
        call    InterlockedExchangeAt
        mov     esi, eax
        test    esi, esi
        jz      short done
        call    SpinPause
        sub     ebx, 1
        jnz     short spin
        xor     ecx, ecx
        call    ThreadSleep
        jmp     short spin
    done:
        pop     edi
        pop     esi
        pop     ebx
        retn    4
    }
}

//----------------------------------------------------------------------------
//  0x6F6D81E0 - claim a wait-event slot from the pool (via EventPool_Acquire,
//  which reads its own index from `eax`), then CAS the two-field record at
//  `ebx` (an address the *caller* sets up before this call - `&Wait's own
//  this+8` from sub_6F6D9820, `&this+4` from sub_6F6D95D0 - a fourth,
//  implicit argument by fixed register rather than by the stack, the same
//  hand-written-family convention ConditionVariable::NotifyOne's own header
//  comment documents) from 0 to the claimed slot; on a collision, retries
//  against the *other* half of that same record (`ebx+ebp`, ebp being the
//  second stack argument) before giving the slot back and retrying from
//  the top.
//----------------------------------------------------------------------------
__declspec(naked) void EventSlot_Claim()
{
    __asm
    {
        mov     eax, [esp+4]
        push    ebp
        mov     ebp, [esp+4+8]
        push    esi
        push    edi
        call    EventPool_Acquire
        mov     edi, eax
        push    0
        mov     edx, edi
        mov     ecx, ebx
        call    InterlockedCompareExchangeAt
        mov     esi, eax
        test    esi, esi
        jz      short claimed
    retry:
        push    esi
        lea     edx, [esi+ebp]
        mov     ecx, ebx
        call    InterlockedCompareExchangeAt
        cmp     esi, eax
        push    0
        jz      short giveBack
        mov     edx, edi
        mov     ecx, ebx
        call    InterlockedCompareExchangeAt
        mov     esi, eax
        test    esi, esi
        jnz     short retry
    claimed:
        mov     eax, edi
        pop     edi
        pop     esi
        pop     ebp
        retn    8
    giveBack:
        mov     ecx, [esp+10h+4]
        mov     eax, edi
        call    EventPool_Release
        pop     edi
        lea     eax, [esi+ebp]
        pop     esi
        pop     ebp
        retn    8
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8240 - release the CAS-based record at `ebx` if it currently reads
//  `edi` (both again caller-set implicit arguments - `edi` a computed
//  claimant id, `ebx` the same per-instance record address
//  EventSlot_Claim's own callers set up), retrying against the record's
//  `ebx+ebp` half on a collision, then hands the slot itself back through
//  EventPool_Release. Only reached from ConditionVariable::NotifyOne
//  (0x6F6D8340) and sub_6F6D95D0, both of which set `edi`/`ebx` immediately
//  before the call.
//----------------------------------------------------------------------------
__declspec(naked) void EventSlot_Release()
{
    __asm
    {
        push    ebp
        mov     ebp, [esp+4+8]
        push    esi
        push    edi
        xor     edx, edx
        mov     ecx, ebx
        call    InterlockedCompareExchangeAt
        mov     esi, eax
        cmp     esi, edi
        jz      short notFound
    retry:
        mov     edx, esi
        push    esi
        sub     edx, ebp
        mov     ecx, ebx
        call    InterlockedCompareExchangeAt
        cmp     esi, eax
        jz      short gaveUp
        push    edi
        xor     edx, edx
        mov     ecx, ebx
        call    InterlockedCompareExchangeAt
        mov     esi, eax
        cmp     esi, edi
        jnz     short retry
    notFound:
        mov     ecx, [esp+8+4]
        push    1
        mov     eax, edi
        call    EventPool_Release
        pop     esi
        mov     eax, 1
        pop     ebp
        retn    8
    gaveUp:
        pop     esi
        xor     eax, eax
        pop     ebp
        retn    8
    }
}

//----------------------------------------------------------------------------
//  0x6F6D82E0 - wake one waiter on the record at `eax` (yet another
//  caller-supplied implicit self, this time in eax rather than ecx/esi):
//  mark it claimed (*eax = 1), and if a waiting thread's index is recorded
//  in the record's second field, SetEvent its slot's own handle.
//----------------------------------------------------------------------------
__declspec(naked) void EventSlot_Signal()
{
    __asm
    {
        mov     dword ptr [eax], 1
        mov     eax, [eax+4]
        test    eax, eax
        jz      short noWaiter
        shr     eax, 15h
        mov     eax, hHandle[eax*4]
        push    eax
        call    DWORD PTR [SetEvent]
    noWaiter:
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8340 - ConditionVariable::NotifyOne. Not a normal thiscall member -
//  see gameaction0x26_notify.cpp's own header comment for why (its only
//  caller, ConditionVariable::SignalOne, sets `esi` = `this` by hand rather
//  than relying on the compiler's ordinary ecx-based thiscall). `value==0`
//  wakes one already-waiting thread directly (EventSlot_Signal); otherwise
//  it releases this instance's own claimed slot (EventSlot_Release, with
//  `edi`/`ebx` set up here exactly as EventSlot_Release's own header
//  documents) and, only if that release found nothing to release, falls
//  through to the same direct signal.
//----------------------------------------------------------------------------
__declspec(naked) void ConditionVariable::NotifyOne(unsigned int)
{
    __asm
    {
        cmp     dword ptr [esp+4], 0
        jnz     short signalDirect
        push    ebx
        push    edi
        mov     edi, [esi+8]
        lea     ebx, [esi+8]
        push    2
        and     edi, 0FFE00000h
        add     edi, 1
        push    1
        call    EventSlot_Release
        test    eax, eax
        pop     edi
        pop     ebx
        jz      short done
    signalDirect:
        mov     eax, esi
        call    EventSlot_Signal
    done:
        retn    4
    }
}

//----------------------------------------------------------------------------
//  0x6F6D9820 - ConditionVariable::Wait. `value==0` goes straight to the
//  deadlock-tracked wait (sub_6F6D95D0); otherwise it claims a slot
//  (EventSlot_Claim, this+8/this+0xC being the same two-field record
//  EventSlot_Release's own callers use), and if the claimed id already has
//  its high bits set (a lock-free "already satisfied" encoding), releases
//  it again and signals the corresponding hEvent handle directly rather
//  than actually blocking; otherwise it polls `this+8`'s own low bit with
//  a bounded spin before falling back to a real WaitForSingleObject, and on
//  a real timeout logs a "suspected deadlock" line and flushes the deadlock
//  log before resuming the poll.
//----------------------------------------------------------------------------
__declspec(naked) void ConditionVariable::Wait(unsigned int)
{
    __asm
    {
        mov     eax, ecx
        jmp     coldEntry
    coldEntry:
        cmp     dword ptr [esp+4], 0
        push    edi
        mov     edi, eax
        jz      short deadlockTracked
        push    edi
        call    Wait_DeadlockTracked
        pop     edi
        retn    4
    deadlockTracked:
        push    ebx
        push    esi
        push    2
        lea     ebx, [edi+8]
        push    1
        call    EventSlot_Claim
        mov     esi, eax
        and     eax, 0FFE00000h
        cmp     esi, eax
        jnz     short realWait
        push    edi
        call    Wait_DeadlockTracked
        mov     ecx, ebx
        call    InterlockedIncrementAt
        test    esi, esi
        jz      short doneNoEvent
        shr     esi, 15h
        mov     ecx, hEvent[esi*4]
        pop     esi
        pop     ebx
        pop     edi
        mov     [esp+4], ecx
        jmp     DWORD PTR [SetEvent]
    realWait:
        push    ebp
        mov     ebp, WaitForSingleObject
        //  The timeout path at the bottom jumps back *here*, to the reload -
        //  the dump's `jmp short loc_6F6D9710`, and 6F6D9710 is this
        //  instruction, not the `test` after it.  Jumping to `spin` instead
        //  re-enters the poll with edi already 0 (that is how it fell out of
        //  the `jnz`), so `sub edi, 1` wraps to 0FFFFFFFFh and every timed-out
        //  wait spins ~4 billion times before it would wait again.
        //
        //  This was applied once before as part of a four-function batch and
        //  reverted with it - at the time the whole family was broken by the
        //  `call Api` import-slot defect (import-call-indirection.md), which
        //  masked whether any of the four helped.  With that fixed, five of
        //  the six Sync crashers pass and this is the only one left, so this
        //  goes in on its own.
    reloadSpin:
        mov     edi, dword_6FA9FF04
    spin:
        test    byte ptr [ebx], 1
        jnz     short gotIt
        call    SpinPause
        sub     edi, 1
        jnz     short spin
        mov     edx, esi
        shr     edx, 15h
        mov     eax, hEvent[edx*4]
        push    0EA60h
        push    eax
        call    ebp
        test    eax, eax
        jz      short gotIt
        mov     ecx, offset dword_6FADA608
        call    InterlockedIncrementAt
        call    DWORD PTR [GetCurrentThreadId]
        push    eax
        mov     eax, dword_6FADA608
        push    eax
        push    offset aUSuspectedDeadWait
        call    nullsub_1
        add     esp, 0Ch
        call    FlushDeadlockLog_6F6D9500
        call    FlushDeadlockLog_6F6D9510
        jmp     short reloadSpin
    gotIt:
        pop     ebp
    doneNoEvent:
        pop     esi
        pop     ebx
        pop     edi
        retn    4
    }
}

//----------------------------------------------------------------------------
//  0x6F6D95D0 - the deadlock-tracked wait ConditionVariable::Wait(0) always
//  goes through, and the same wait its own hEvent-signal fast path (`value
//  != 0` with a spent id) reaches too. First spins/sleeps on this+4's own
//  low bit directly (no slot claimed yet); once set, claims a slot
//  (EventSlot_Claim(self=0, offset=1) with `this+4` as the implicit CAS
//  record, matching this function's own `ebx`), then WaitForSingleObject-
//  polls that slot's own hHandle with the same "suspected deadlock" logging
//  ConditionVariable::Wait's own real-wait path has, releasing the slot
//  (EventSlot_Release) once the wait actually succeeds.
//----------------------------------------------------------------------------
__declspec(naked) void Wait_DeadlockTracked()
{
    __asm
    {
        push    ebx
        push    esi
        push    edi
        mov     edi, [esp+0Ch+4]
        mov     eax, [edi+4]
        test    eax, eax
        lea     ebx, [edi+4]
        jnz     short claimSlot
        mov     esi, dword_6FA9FF04
    spinLow:
        xor     edx, edx
        mov     ecx, edi
        call    InterlockedExchangeAt
        test    eax, eax
        jnz     short done
        call    SpinPause
        sub     esi, 1
        jz      short claimSlot
        mov     ecx, [ebx]
        test    ecx, ecx
        jz      short spinLow
    claimSlot:
        push    ebp
        push    1
        push    0
        call    EventSlot_Claim
        mov     ebp, WaitForSingleObject
        mov     edi, eax
        and     edi, 0FFE00000h
        mov     esi, dword_6FA9FF04
        jmp     short pollSlot
    pollSlot:
        mov     ecx, [esp+10h+4]
        xor     edx, edx
        call    InterlockedExchangeAt
        test    eax, eax
        jnz     short releaseSlot
        call    SpinPause
        sub     esi, 1
        jnz     short pollSlot
        mov     eax, [ebx]
        shr     eax, 15h
        mov     eax, hHandle[eax*4]
        push    0EA60h
        push    eax
        call    ebp
        test    eax, eax
        jz      short pollSlot
        mov     ecx, offset dword_6FADA608
        call    InterlockedIncrementAt
        call    DWORD PTR [GetCurrentThreadId]
        mov     edx, dword_6FADA608
        push    eax
        push    edx
        push    offset aUSuspectedDeadWait
        call    nullsub_1
        add     esp, 0Ch
        call    FlushDeadlockLog_6F6D9500
        call    FlushDeadlockLog_6F6D9510
        jmp     short pollSlot
    releaseSlot:
        push    1
        push    0
        call    EventSlot_Release
        pop     ebp
    done:
        pop     edi
        pop     esi
        pop     ebx
        retn    4
    }
}

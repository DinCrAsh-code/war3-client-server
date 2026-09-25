//============================================================================
//  A handful of one-line __fastcall wrappers around plain Win32
//  synchronisation calls, all reached from elsewhere in this call tree's
//  0x6F6Cxxxx/0x6F6Dxxxx neighbourhood. Nothing in this call tree says what
//  class (if any) originally wrapped these - each is a bare `__fastcall`
//  free function taking exactly the register(s) the target's own calling
//  convention shows, named for what it forwards to.
//============================================================================
#include <windows.h>
#include <intrin.h>

#pragma intrinsic(_InterlockedExchange, _InterlockedCompareExchange, _InterlockedIncrement, _InterlockedDecrement)

//----------------------------------------------------------------------------
//  0x6F6C40A0 - `mov eax, edx / lock xchg eax, [ecx] / retn`: exchange the
//  dword at `ecx` with `edx`, returning the old value. The compiler
//  intrinsic for `lock xchg` reproduces this exactly at this build's flags.
//----------------------------------------------------------------------------
long __fastcall InterlockedExchangeAt(volatile long* target, long value)
{
    return _InterlockedExchange(target, value);
}

//----------------------------------------------------------------------------
//  0x6F6C40B0 - `mov eax, [esp+arg_0] / lock cmpxchg [ecx], edx / retn 4`:
//  compare-and-swap the dword at `ecx`: if it equals the stack argument
//  (the comparand), replace it with `edx`; either way return the value that
//  was there. Same intrinsic reasoning as InterlockedExchangeAt.
//----------------------------------------------------------------------------
long __fastcall InterlockedCompareExchangeAt(volatile long* target, long exchange, long comparand)
{
    return _InterlockedCompareExchange(target, exchange, comparand);
}

//----------------------------------------------------------------------------
//  0x6F6CDEA0 - `push ecx / call Sleep / retn`: Sleep(milliseconds), the
//  register argument going straight onto the stack for the real stdcall
//  API.
//----------------------------------------------------------------------------
void __fastcall ThreadSleep(unsigned int milliseconds)
{
    Sleep(milliseconds);
}

//----------------------------------------------------------------------------
//  0x6F6D83A0 - `push ecx / call EnterCriticalSection / retn`.
//----------------------------------------------------------------------------
void __fastcall EnterCritSec(CRITICAL_SECTION* cs)
{
    EnterCriticalSection(cs);
}

//----------------------------------------------------------------------------
//  0x6F6D83B0 - `push ecx / call LeaveCriticalSection / retn`.
//----------------------------------------------------------------------------
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs)
{
    LeaveCriticalSection(cs);
}

//----------------------------------------------------------------------------
//  0x6F6C4060 - `mov eax,1 / lock xadd [ecx],eax / inc eax / retn`: atomic
//  increment of the dword at `ecx`, returning the *new* value (xadd leaves
//  the pre-increment value in eax, incremented once more after the atomic
//  op to report the post-increment count).
//----------------------------------------------------------------------------
long __fastcall InterlockedIncrementAt(volatile long* target)
{
    return _InterlockedIncrement(target);
}

//----------------------------------------------------------------------------
//  0x6F6C4070 - the decrementing twin of InterlockedIncrementAt: `mov
//  eax,-1 / lock xadd [ecx],eax / dec eax / retn`, same reasoning with the
//  step negated.
//----------------------------------------------------------------------------
long __fastcall InterlockedDecrementAt(volatile long* target)
{
    return _InterlockedDecrement(target);
}

//----------------------------------------------------------------------------
//  0x6F6D8B20 - `mov eax, dword_6FADA5E4 / push eax / call
//  LeaveCriticalSection / retn`: leaves a single fixed, module-global
//  critical section (dword_ prefixed, so matching_decomp_injection's
//  binder maps it to its real live address - unlike the sibling at
//  0x6F6D8AC0, whose operand IDA names `lpCriticalSection` with no numeric
//  address at all and which stays unreconstructed for exactly that reason:
//  guessing a *different* local CRITICAL_SECTION for a lock other code
//  really shares would be worse than leaving it TODO).
//----------------------------------------------------------------------------
extern CRITICAL_SECTION* g_critSec6FADA5E4;   // dword_6FADA5E4 - a pointer, not the struct itself

void __fastcall LeaveGlobalCritSec6FADA5E4()
{
    LeaveCriticalSection(g_critSec6FADA5E4);
}

//----------------------------------------------------------------------------
//  0x6F6C42B0 - `mov eax,[esp+4] / push eax / push edx / push ecx / call
//  InterlockedCompareExchange / retn 4`: the pointer-sized CAS every
//  lock-free-stack push/pop in the CDataRecycler family (agent-networking-
//  wave4-2026-08-29's cdatarecycler_stackprims.cpp) drives in a retry loop.
//  Same intrinsic-wrapper reasoning as InterlockedExchangeAt/
//  InterlockedCompareExchangeAt above, one level up (pointer rather than
//  `long`) - `_InterlockedCompareExchangePointer` compiles to the identical
//  three-push-then-call shape at this build's flags.
//----------------------------------------------------------------------------
//  Written naked rather than through the `_InterlockedCompareExchange`
//  intrinsic: at this build's flags the intrinsic inlines a real `lock
//  cmpxchg`, where the shipped code (like every other Interlocked* wrapper
//  in this file) genuinely calls through the imported
//  `InterlockedCompareExchange` - a real API call, not an inlined
//  instruction, on whatever Platform SDK this image links against.
__declspec(naked) void* __fastcall CompareExchangePointer(void* volatile* /*Destination*/, void* /*Exchange*/, void* /*Comperand*/)
{
    __asm
    {
        mov     eax, [esp+4]
        push    eax
        push    edx
        push    ecx
        call    DWORD PTR [InterlockedCompareExchange]
        retn    4
    }
}

//----------------------------------------------------------------------------
//  0x6F6CDEF0 - forty bare `nop`s then `retn`: a calibrated busy-wait spin,
//  reached from the same wait/contention-tracked lock family sub_6F6D9820
//  belongs to (this codebase's earlier sessions left that whole family
//  unreconstructed - see docs/targets/Probably_W3GS_ActionHandler.md).
//  Not expressible as ordinary C++ (no operation here has any effect a
//  compiler would preserve); reproduced literally with inline asm rather
//  than guessed at with an intrinsic like `_mm_pause()`, which emits a
//  `pause` instruction, not a `nop`.
//----------------------------------------------------------------------------
__declspec(naked) void SpinPause()
{
    __asm
    {
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        ret
    }
}

//----------------------------------------------------------------------------
//  0x6F6C40C0 - a 64-bit atomic increment via `lock cmpxchg8b`: MSVC has no
//  64-bit interlocked-increment intrinsic at this build's target
//  architecture, so the shipped code (and this reconstruction) spins its
//  own compare-and-swap loop instead, matching how a 64-bit
//  read-modify-write is hand-written pre-64-bit-intrinsic MSVC. Returns the
//  new (post-increment) 64-bit value in edx:eax, the ordinary MSVC ABI for
//  a 64-bit return.
//----------------------------------------------------------------------------
__declspec(naked) unsigned __int64 __fastcall Interlocked64IncrementAt(volatile unsigned __int64*)
{
    __asm
    {
        push    ebx
        push    edi
        mov     edi, ecx
        mov     eax, [edi]
        mov     edx, [edi+4]
    retry:
        mov     ebx, eax
        mov     ecx, edx
        add     ebx, 1
        adc     ecx, 0
        lock cmpxchg8b qword ptr [edi]
        jnz     short retry
        mov     eax, ebx
        mov     edx, ecx
        pop     edi
        pop     ebx
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6C40E0 - the decrementing twin of Interlocked64IncrementAt.
//----------------------------------------------------------------------------
__declspec(naked) unsigned __int64 __fastcall Interlocked64DecrementAt(volatile unsigned __int64*)
{
    __asm
    {
        push    ebx
        push    edi
        mov     edi, ecx
        mov     eax, [edi]
        mov     edx, [edi+4]
    retry:
        mov     ebx, eax
        mov     ecx, edx
        sub     ebx, 1
        sbb     ecx, 0
        lock cmpxchg8b qword ptr [edi]
        jnz     short retry
        mov     eax, ebx
        mov     edx, ecx
        pop     edi
        pop     ebx
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8380 (`unknown_libname_947`) - `push ecx / call
//  InitializeCriticalSection / mov eax,esi(=ecx) / retn`: initialises the
//  CRITICAL_SECTION at `ecx` in place and returns the same pointer, the
//  fluent-constructor shape this codebase's own Event/OsNet types use
//  elsewhere.
//----------------------------------------------------------------------------
CRITICAL_SECTION* __fastcall InitCritSec(CRITICAL_SECTION* cs)
{
    InitializeCriticalSection(cs);
    return cs;
}

//----------------------------------------------------------------------------
//  0x6F6D8390 (`unknown_libname_948`) - `push ecx / call
//  DeleteCriticalSection / retn`.
//----------------------------------------------------------------------------
void __fastcall DeleteCritSec(CRITICAL_SECTION* cs)
{
    DeleteCriticalSection(cs);
}

//============================================================================
//  0x6F6C4CD0 - CalibrateHighResTimer (gamemain-batch-3).  Measure how many
//  RDTSC ticks elapse in one QueryPerformanceCounter-timed interval and
//  cache the result in qword_6FAD1428, so a later caller can convert a raw
//  RDTSC delta into real time without paying for QueryPerformanceCounter
//  again.  Runs the measurement exactly once: every later call sees the
//  cached 64-bit value already nonzero and returns immediately.
//
//  The wait is boosted to the calling thread's own current priority (a
//  self-set, not an escalation - GetThreadPriority's result is handed
//  straight back to SetThreadPriority, which is what the shipped code
//  spells; presumably a leftover from a version of this routine that raised
//  the priority to a constant) before and after the sleep, and the sleep
//  length itself is 50ms when the priority read/restore succeeded or 500ms
//  when GetThreadPriority failed (returned THREAD_PRIORITY_ERROR_RETURN,
//  0x7FFFFFFF) - a longer wait to average out scheduling noise when the
//  thread could not be pinned.  `docs/msvc-vc8-idioms.md`'s `(x != 0) ? C :
//  0` mask idiom is what the 50-vs-500 selection compiles to.
//
//  sub_6F6C4CC0 is the bare `rdtsc`/`retn` leaf (timerreadtsc.cpp).
//============================================================================
#include <windows.h>

//----------------------------------------------------------------------------
//  0x6F6C4CC0 - the bare RDTSC leaf.  `rdtsc` leaves the 64-bit counter in
//  edx:eax, which is exactly what a `__cdecl` function returning
//  `unsigned __int64` hands back, so no epilogue is needed at all.
//----------------------------------------------------------------------------
#pragma warning(disable : 4035)   // naked: return value left by `rdtsc` itself
__declspec(naked) unsigned __int64 __cdecl ReadTimeStampCounter()
{
    __asm
    {
        rdtsc
        retn
    }
}
#pragma warning(default : 4035)

static unsigned __int64 g_tscPerSecond;   // qword_6FAD1428

void CalibrateHighResTimer()
{
    if (g_tscPerSecond != 0)
        return;

    HANDLE thisThread = GetCurrentThread();
    int priority = GetThreadPriority(thisThread);
    if (priority != THREAD_PRIORITY_ERROR_RETURN)
        SetThreadPriority(GetCurrentThread(), priority);

    unsigned __int64 tsc0 = ReadTimeStampCounter();

    LARGE_INTEGER qpc0;
    QueryPerformanceCounter(&qpc0);

    unsigned int sleepMs = (priority != THREAD_PRIORITY_ERROR_RETURN
                                ? 0xFFFFFE3Eu : 0u) + 0x1F4u;
    Sleep(sleepMs);

    int priority2 = GetThreadPriority(GetCurrentThread());
    if (priority2 != THREAD_PRIORITY_ERROR_RETURN)
        SetThreadPriority(GetCurrentThread(), priority2);

    unsigned __int64 tsc1 = ReadTimeStampCounter();

    LARGE_INTEGER qpc1;
    QueryPerformanceCounter(&qpc1);

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    __int64 tscDelta = (__int64)(tsc1 - tsc0);
    __int64 qpcDelta = qpc1.QuadPart - qpc0.QuadPart;

    g_tscPerSecond = (unsigned __int64)((tscDelta * freq.QuadPart) / qpcDelta);
}

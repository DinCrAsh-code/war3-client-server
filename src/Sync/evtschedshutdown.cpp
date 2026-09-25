//============================================================================
//  0x6F62E5B0 (root) - EvtSched's shutdown/teardown routine.  A no-op
//  unless g_evtSchedActive is set; otherwise it spawns
//  g_evtSchedThreadCount worker threads (each running EvtWorkerThreadMain,
//  i.e. the other dump's own root, sub_6F62CF50, with a shared "shutdown"
//  Event as its argument and "EvtShutdown" as a name-ish tag), waits for
//  all of them via WaitForEvents/WaitForMultipleObjects, then releases
//  every Event and the CRITICAL_SECTION[] block this subsystem has ever
//  tracked (both the batch just launched and whatever g_evtSchedThreads
//  already held) and clears g_evtSchedActive.
//
//  Ground truth (asm/'s own `## Ground truth` section): something in this
//  call tree calls `__imp_send` - not visible in THIS 27-function dump
//  (sub_6F62E5B0 never touches ws2_32 directly), so it must be reached
//  through EvtWorkerThreadMain/sub_6F62CF50's own call tree in the sibling
//  dump, consistent with this function's role as "spawn worker threads that
//  do real network I/O, then wait for them to finish."
//
//  The per-iteration __try/__finally (dump: a one-byte state variable at
//  var_4 stepping 0/1/2/3 as the iteration progresses, read only by the
//  compiler-synthesized unwind dispatcher) is the same `__except_handler4`
//  -shaped frame docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped
//  frame this toolchain cannot reproduce" already establishes this fixed
//  /GS- /EHs-c- toolchain never emits byte-for-byte, on every function that
//  has one, regardless of source shape - kept in source as genuine
//  __try/__finally below (matching what the dump's own control flow
//  implies: clean up a not-yet-array-owned heap Event if anything in the
//  try block raises), not chased further once that limitation applies,
//  same as sub_6F42CB50/sub_6F02F780's own entries.
//============================================================================
#include "evtsched.h"

unsigned int            g_evtSchedActive;
unsigned int            g_evtSchedThreadCount;
TSGrowableArray<HANDLE> g_evtSchedThreads;
void*                   g_evtSchedUnknown5C;
void*                   g_evtSchedCritSecBlock;

//  CRITICAL_SECTION*, not void*: critsec.cpp defines this as
//  DeleteCritSec(CRITICAL_SECTION*), and MSVC mangles the parameter type in,
//  so a void* declaration here asked for ?DeleteCritSec@@YIXPAX@Z while that
//  TU emitted ?DeleteCritSec@@YIXPAU_RTL_CRITICAL_SECTION@@@Z - LNK2019 on
//  every link.  The call site below casts the pointer anyway.
void __fastcall DeleteCritSec(CRITICAL_SECTION* cs);   // critsec.cpp (0x6F6D8390)

//  crtseh4.cpp's own transcription of the `eh vector destructor iterator`
//  (0x6F7E1662) - declared here with its real 4-argument signature so this
//  call site pushes the same four arguments the dump's own call does.
void __stdcall EhVectorDestructorIterator(void* arrayEnd, unsigned int elementSize,
                                           unsigned int count,
                                           void(__thiscall* dtor)(void*));

//  Debug-tag strings the shipped code passes to Storm's allocator/free
//  calls.  These canonicalise to an unresolved data symbol on both the
//  dump and this build regardless of content (same as every other
//  string-literal operand in this repo, e.g. tsarray.inl's own
//  s_typeName), so the exact bytes are not score-relevant - see
//  docs/msvc-vc8-idioms.md.  Content here is the readable prefix IDA's own
//  display gives for each (".\\EvtSched.cpp", "EvtShutdown", "delete",
//  "delete[]", and the RTTI-descriptor-shaped tag IDA names aPavcosthread,
//  already used unchanged by TSGrowableArray<T>::SetAlloc elsewhere).
static const char kEvtSchedCpp[] = ".\\EvtSched.cpp";
static const char kEvtShutdown[] = "EvtShutdown";
static const char kDelete[]      = "delete";
static const char kDeleteArray[] = "delete[]";
static const char kTypeTag[]     = ".?AVCOSThread@@";   // aPavcosthread - see note above

//  C4509: this function's own __try/__finally coexists with local objects
//  that have destructors (shutdownEvent, launched) - the exact tension
//  that produces the dump's own unreproducible SEH frame in the first
//  place (docs/msvc-vc8-idioms.md).  Silenced rather than restructured:
//  the destructors never fire through the compiler's own synthesized
//  unwind path at this build's /EHs-c- (that whole mechanism does not
//  reproduce here, see the file header comment), only at the function's
//  own explicit end, which is exactly what the dump's own explicit
//  Close()/SMemFree() calls do too.
#pragma warning(disable : 4509)

void EvtSchedShutdown()
{
    if (!g_evtSchedActive)
        return;

    Event shutdownEvent(TRUE, FALSE);   // manual-reset, initially unsignaled

    TSGrowableArray<HANDLE> launched;
    launched.m_alloc = 0;
    launched.m_count = 0;
    launched.m_data  = 0;
    launched.m_chunk = 0;

    if (g_evtSchedThreadCount != 0)
    {
        unsigned int remaining = GetProcessorCount();

        for (;;)
        {
            remaining -= 1;

            //  trackedHandle aliases whatever pointer the try block has not
            //  yet handed off to `launched`, read back through Event's own
            //  layout (a single HANDLE member, so a HANDLE slot and an
            //  Event are interchangeable); the __finally below closes it
            //  (a no-op once ownership transfers and it is zeroed).
            HANDLE trackedHandle;

            __try
            {
                Event* newEvent = (Event*)SMemAlloc(sizeof(Event), kEvtSchedCpp, 0x337, 0);
                if (newEvent)
                    newEvent->ZeroHandle();
                trackedHandle = (HANDLE)newEvent;

                HANDLE* slot = HandleArrayAppend(&launched);
                *slot = (HANDLE)newEvent;

                BOOL ok = LaunchWorkerThread((void*)&EvtWorkerThreadMain,
                                              &shutdownEvent,
                                              (HANDLE*)newEvent,
                                              (void*)kEvtShutdown);
                if (!ok)
                {
                    unsigned int lastIndex = launched.m_count - 1;
                    HANDLE last = (launched.m_count != 0) ? launched[lastIndex] : 0;
                    Event* failedEvent = (Event*)last;
                    if (failedEvent)
                    {
                        failedEvent->CloseForward();
                        SMemFree(failedEvent, kDelete, -1, 0);
                    }

                    //  Shrink `launched` back down by one, the same
                    //  chunked-rounding SetAlloc/ComputeChunk shape
                    //  HandleArrayAppend's own growth path uses, reused
                    //  here for the symmetrical shrink.
                    unsigned int newCount = lastIndex;
                    if (newCount < launched.m_count && newCount > launched.m_alloc)
                    {
                        unsigned int chunk = launched.m_chunk;
                        if (chunk == 0)
                            chunk = launched.ComputeChunk(newCount);
                        unsigned int alloc = newCount;
                        unsigned int over = newCount % chunk;
                        if (over != 0)
                            alloc = newCount + (chunk - over);
                        launched.SetAlloc(alloc);
                    }
                    launched.m_count = newCount;
                }
            }
            __finally
            {
                ((Event*)&trackedHandle)->CloseForward();
            }

            if (remaining == 0)
                break;
        }
    }

    WaitForEvents(g_evtSchedThreads.m_count, (Event* const*)g_evtSchedThreads.m_data, TRUE, (DWORD)-1);

    if (launched.m_count != 0)
    {
        shutdownEvent.Set();

        WaitForEvents(launched.m_count, (Event* const*)launched.m_data, TRUE, (DWORD)-1);

        unsigned int i = launched.m_count;
        do
        {
            Event* e = (Event*)launched[i - 1];
            i -= 1;
            if (e)
            {
                e->CloseForward();
                SMemFree(e, kDelete, -1, 0);
            }
        } while (i != 0);
    }

    //  The dump's own tail (right before its epilogue) shows a real
    //  `call TSGrowableArray<T>::~TSGrowableArray` followed by an
    //  unresolved-name destructor call in this position, meaning the true
    //  source relies on `launched`/`shutdownEvent` going out of scope
    //  rather than tearing them down by hand.  This build's toolchain
    //  warns C4509 for exactly that combination (a __try/__finally earlier
    //  in the same function suppresses automatic destructor calls here
    //  entirely rather than reproducing them at scope exit - confirmed by
    //  testing: removing these two explicit calls does not reproduce the
    //  dump's destructor calls, it just drops the cleanup from this build
    //  altogether and the instruction count falls instead of rising), so
    //  the equivalent cleanup is spelled out explicitly instead - the same
    //  "this toolchain's fixed /GS- /EHs-c- cannot reproduce this exact
    //  SEH/destructor interaction" limitation docs/msvc-vc8-idioms.md
    //  already documents, recurring here as a missing pair of calls rather
    //  than a missing frame.
    if (launched.m_data)
        SMemFree(launched.m_data, kTypeTag, -2, 0);

    shutdownEvent.CloseForward();

    if (g_evtSchedThreads.m_count != 0)
    {
        unsigned int i = g_evtSchedThreads.m_count;
        do
        {
            Event* e = (Event*)g_evtSchedThreads.m_data[i - 1];
            i -= 1;
            if (e)
            {
                e->CloseForward();
                SMemFree(e, kDelete, -1, 0);
            }
        } while (i != 0);
    }

    if (g_evtSchedThreads.m_data)
        SMemFree(g_evtSchedThreads.m_data, kTypeTag, -2, 0);

    void* unknown5C = g_evtSchedUnknown5C;
    g_evtSchedThreads.m_alloc = 0;
    g_evtSchedThreads.m_count = 0;
    g_evtSchedThreads.m_data  = 0;
    if (unknown5C)
        SMemFree(unknown5C, kDeleteArray, -1, 0);

    void* critBlock = g_evtSchedCritSecBlock;
    g_evtSchedUnknown5C = 0;
    if (critBlock)
    {
        unsigned int count = *((unsigned int*)critBlock - 1);
        void* header = (unsigned int*)critBlock - 1;
        EhVectorDestructorIterator(critBlock, sizeof(CRITICAL_SECTION), count,
                                    (void(__thiscall*)(void*))&DeleteCritSec);
        if (header)
            SMemFree(header, kDeleteArray, -1, 0);
    }

    g_evtSchedCritSecBlock = 0;
    g_evtSchedActive = 0;
}

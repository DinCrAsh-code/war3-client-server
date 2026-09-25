//============================================================================
//  0x6F630760 - EvtSched context "log and dispatch": optionally format and
//  log a context line ("Context: interactive = %u, idleTime = %u...",
//  sub_6F6E8420), allocate a new dispatch context (Storm_401/SMemAlloc,
//  already reconstructed) and hand it to sub_6F62F610 to actually route,
//  bump a shared counter (InterlockedIncrementAt, Net/playertable.h,
//  already reconstructed) if this context is the interactive one, insert
//  up to two event-bucket handlers (CEventBucketList::InsertHandler,
//  Frame/eventbucket.cpp, already reconstructed, slots 7 and 4) for the
//  two out-pointer arguments the caller supplied, and finally tick
//  sub_6F62FE20.
//
//  Reached both from GameMain's own root closure (this batch's own 43) and
//  from Probably_W3GS_ActionHandler_MaybeServerVersion (already finished) -
//  the second is why this is reconstructed for real rather than thunked;
//  see evtschedcontextthunks.cpp's own header comment.
//
//  The function-level SEH frame (the `push -1 / push offset SEH_.../
//  mov eax,fs:0` prologue and the matching epilogue/scope-table funclets)
//  is the same `__except_handler4`-shaped frame this toolchain's fixed
//  /GS- /EHs-c- never reproduces byte for byte regardless of source shape
//  (docs/msvc-vc8-idioms.md); not chased further, see the BEHAVIOUR entry.
//============================================================================
#include "storm.h"
#include "playertable.h"
#include "frame.h"

int Storm_578(char* dest, unsigned int size, const char* format, ...);

//  0x6F6E8420 - format the context line into the caller's own buffer and
//  return whatever value this call tree threads through as `arg`.  Below
//  this batch's own 43; declared and thunked to its own real address
//  (`retn 0`, ecx=buffer only, confirmed by the call site).
__declspec(naked) int __fastcall LogEvtSchedContextLine(char* /*buffer*/)
{
    __asm { mov eax, 06F6E8420h }
    __asm { jmp eax }
}

//  0x6F62F610 - actually build/route the dispatch context.  Below this
//  batch's own 43; declared and thunked to its own real address (`retn
//  14h` - five stack arguments, ecx-only for `this`, confirmed by the call
//  site's own five pushes).  `__fastcall` would put the first of them in
//  edx, so this is a method on a minimal opaque receiver instead.
struct EvtSchedDispatchContextFactory
{
    void* Create(int, int, int, int, int);
};

void* CreateEvtSchedDispatchContext(void* block, int a, int b, int c, int d, int e)
{
    return ((EvtSchedDispatchContextFactory*)block)->Create(a, b, c, d, e);
}

__declspec(naked) void* EvtSchedDispatchContextFactory::Create(int, int, int, int, int)
{
    __asm { mov eax, 06F62F610h }
    __asm { jmp eax }
}

//  0x6F6C4060 - Net/playertable.h's own InterlockedIncrementAt, reused
//  here on a different counter.
extern int g_evtSchedDispatchCount;   // unk_6FACEB54

//  0x6F62FE20 - below this batch's own 43; declared and thunked to its own
//  real address (`retn 0`, no arguments at the call site).
__declspec(naked) void TickEvtSchedDispatch()
{
    __asm { mov eax, 06F62FE20h }
    __asm { jmp eax }
}

//  flt_6F938440 - the constant `when` every InsertHandler call below hands
//  through unchanged; this call tree never shows its own value being
//  computed, only reloaded via `fld`.
extern float g_evtSchedHandlerWhen;

//  ecx=self, edx=idleTimePtr (a register argument, not stack); three stack
//  arguments (otherPtr, interactiveRaw, flagsArg) - `retn 0Ch` confirmed by
//  this function's own dump.
int __fastcall EvtSchedContextLogAndDispatch(void* self, int idleTimePtr,
                                              int otherPtr, int interactiveRaw,
                                              int flagsArg)
{
    int interactive = (interactiveRaw < 1) ? 1 : interactiveRaw;

    int loggedValue = 0;
    if (flagsArg & 1)
    {
        char line[0x100];
        Storm_578(line, sizeof(line),
                  "Context: interactive = %u, idleTime = %u", self, interactive);
        loggedValue = LogEvtSchedContextLine(line);
    }

    int flag = (flagsArg >> 1) & 1;
    int delayMs = self ? 1000 : 1;

    void* block = SMemAlloc(0x214, ".\\EvtSched.cpp", 0x39F, 0);

    void* context = 0;
    if (block != 0)
    {
        int otherFlag = self ? 2 : 0;
        context = CreateEvtSchedDispatchContext(block, otherFlag, interactive,
                                                 delayMs, flag, loggedValue);
    }

    if (self != 0)
        InterlockedIncrementAt((volatile long*)&g_evtSchedDispatchCount);

    if (idleTimePtr != 0)
    {
        CEventBucketList* buckets = (CEventBucketList*)context;
        buckets->InsertHandler(7, (void*)idleTimePtr, 0, g_evtSchedHandlerWhen);
    }
    if (otherPtr != 0)
    {
        CEventBucketList* buckets = (CEventBucketList*)context;
        buckets->InsertHandler(4, (void*)otherPtr, 0, g_evtSchedHandlerWhen);
    }

    TickEvtSchedDispatch();

    return (int)context;
}

//============================================================================
//  0x6F62B390 - RegisterEventHandler: bind `handler` to one of the 0x1D
//  handler slots of this thread's own event table, ordered by `when`.
//
//  A slot outside [0, 0x1D) or a null handler is Storm error 0x57
//  (ERROR_INVALID_PARAMETER) and nothing else - the shipped code forms both
//  tests as sign masks and ands them together rather than branching twice,
//  which is what the setl/setnl pair is.
//============================================================================
#include "frame.h"
#include "framethunks.h"

#include "gamecontext.h"

void __fastcall RegisterEventHandler(int slot, void* handler, int a,
                                     float when)
{
    //  Every part of this test is spelled to match the shipped branchless
    //  form, and each was measured:
    //    * bitwise `&`, not `&&` - a short-circuit chain branches twice;
    //    * `(cond) - 1` rather than `!(cond)` - the subtraction is what makes
    //      MSVC emit `setge al` / `sub eax,1` (a 0/-1 mask).  `!(slot >= 29)`
    //      is folded straight back to `setl` and loses both `sub`s;
    //    * `handler ? -1 : 0` rather than `handler != 0` - the ternary is what
    //      gives `neg edx` / `sbb edx,edx`, where the comparison gives a
    //      four-instruction `xor`/`test`/`setne`/`neg`.
    //  The one instruction still missing is the `test`'s operand order
    //  (`test edx,eax` shipped, `test eax,edx` here); MSVC picks that itself
    //  and writing the `&` the other way round does not move it.
    int inRange = ((slot >= 0x1D) - 1) & ((slot < 0) - 1);
    if (!(inRange & (handler ? -1 : 0)))
    {
        SErrSetLastError(0x57);
        return;
    }

    int lockIndex;
    void* context = GetThreadLocalSlot(0);
    CEventBucketList* list = (CEventBucketList*)g_eventRegistry.AcquireBucket(
        context, 0, &lockIndex, 0, 1);

    if (list)
    {
        list->InsertHandler(slot, handler, a, when);
        if (lockIndex != -1)
            EventRegistryLock(lockIndex & 7)->SignalOne((unsigned int)lockIndex >= 8u);
    }
}

//============================================================================
//  0x6F2F7880 - release every live TRefCntPtr<CTimerDialog> in a small
//  growable array, then free the array's own backing block.
//
//  CItem vtable-closure batch 2.  Reached from CGameUI's own real
//  destructor (0x6F300220, `??_7CGameUI@@6B@`/`_0` - not itself in this
//  batch) against the sub-object at CGameUI+0x398: a 0xC-byte array header
//  ({reserved, count, data}, no chunk field - a plain, non-growing pointer
//  vector rather than the SPointerArray/TSGrowableArray shape) whose data
//  block is tagged ".?AV?$TRefCntPtr@VCTimerDialog@@@@" (agent_worktrees'
//  resolve of aAvTrefcntptrVc_3, 0x6FA75CE0).  Written as a free function
//  over a raw base pointer rather than added to gameui.h: nothing in this
//  call tree needs the field named or the class touched, and CGameUI's own
//  layout is a much larger, already-committed shared header not to
//  perturb for one leaf.
//
//  Each live element is released the plain TRefCnt way (decrement, and on
//  the last reference a virtual call through the object's own slot 0 -
//  DeleteSelf) rather than through a smart-pointer destructor call, which
//  is why this reads as a raw refcount walk and not a per-element dtor
//  loop.
//
//  Own translation unit: called for real out of CGameUI's own destructor.
//============================================================================
#include "storm.h"

static const char s_typeName[] = ".?AV?$TRefCntPtr@VCTimerDialog@@@@";

void __fastcall ReleaseTimerDialogPtrArray(void* array)
{
    unsigned int* header = (unsigned int*)array;
    unsigned int count = header[1];
    void** data = (void**)header[2];

    for (unsigned int i = 0; i < count; i++)
    {
        void* element = data[i];
        if (element != 0)
        {
            unsigned int* refcnt = (unsigned int*)element;
            refcnt[1] -= 1;
            if (refcnt[1] == 0)
            {
                void** vtbl = *(void***)element;
                typedef void (__thiscall *DeleteSelfFn)(void*);
                ((DeleteSelfFn)vtbl[0])(element);
            }
        }
    }

    if (data != 0)
        SMemFree(data, s_typeName, -2, 0);
}

//----------------------------------------------------------------------------
//  0x6F2F83C0 - `jmp sub_6F2F7880`, one instruction.  Not a call site of a
//  distinct function: it is the alternate entry CGameUI's own SEH scope
//  table (0x6F81B840, the `??_7CGameUI@@6B@_0`-side unwind funclet region)
//  reaches during exception unwinding, tail-jumping straight into the same
//  body above.  Naked redirect, THUNK - nothing of its own to reconstruct.
//----------------------------------------------------------------------------
__declspec(naked) void ReleaseTimerDialogPtrArrayAlias()
{
    __asm { jmp ReleaseTimerDialogPtrArray }
}

//============================================================================
//  0x6F29DA00 - CUnit::CancelPendingEventGroup: cancel four events by id
//  through CancelEventById, then a fifth call with argument 0 through
//  Method_0x2963F0.  CUnit batch-13, slot 3 closure (0x6F2A7E60).
//
//  Both callees (0x6F295AC0, 0x6F2963F0) open their own real
//  `__except_handler4`/security-cookie/scope-table prologue at their own
//  address - the confirmed SEH-frame ceiling documented in
//  docs/targets/CUnit__vtable.md - so neither is a reconstruction target;
//  they are thunked here with the argument byte count read straight off
//  the tail `retn 4` in each one's own asm/ dump body.
//============================================================================
#include "unit.h"
#include "unitorder.h"     // kEventOrderFinished

void CUnit::CancelPendingEventGroup()
{
    CancelEventById(0xD0148);
    CancelEventById(0xD014A);
    CancelEventById(0xD0178);
    CancelEventById(kEventOrderFinished);   // 0xD0144
    Method_0x2963F0(0);
}

#define ADDR_THUNK(sig, addr)              \
    __declspec(naked) sig                  \
    {                                       \
        __asm { mov eax, addr }            \
        __asm { jmp eax }                  \
    }

ADDR_THUNK(void CUnit::CancelEventById(unsigned int eventId), 0x6F295AC0)
ADDR_THUNK(void CUnit::Method_0x2963F0(int arg), 0x6F2963F0)

#undef ADDR_THUNK

//============================================================================
//  sub_6F673720 (0x6F673720, 12 instructions) - `self->m_nextRetryTick =
//  GetTickCount() + timeoutMs`, coerced away from the literal 0 the ADD can
//  land on (`jnz` tests the ADD's own flags straight through the following
//  MOV, which does not touch them) so a retry deadline of exactly 0 never
//  reads back as "no deadline armed". Called from PlayerTable_InstallHandler
//  (playertable_installhandler.cpp) on both the "no live connection yet"
//  and the "no pending request queued yet" give-up paths.
//============================================================================
#include "playertable.h"

extern "C" unsigned int __stdcall GetTickCount();

void __fastcall ArmRetryTimer(PlayerTable* self, unsigned int timeoutMs)
{
    unsigned int deadline = GetTickCount() + timeoutMs;
    self->m_nextRetryTick = deadline;
    if (deadline == 0)
        self->m_nextRetryTick = 1;
}

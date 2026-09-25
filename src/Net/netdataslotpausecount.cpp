//============================================================================
//  0x6F53EE90 - the pause counter of one participant, or zero when either
//  the game data or the slot is missing.
//
//  Both negative answers share one `xor eax,eax` and one exit and the hit
//  gets its own - the guarded-success-path shape, not a chain of refusals.
//
//  Own translation unit: SLOTTABLE::Find is a real call, and
//  IsSlotParticipating calls this one.
//============================================================================
#include "netdata.h"

int __fastcall CNetData::GetSlotPauseCount(unsigned char slot, int index)
{
    CNetData* records = NetDataRecords();
    if (records != 0)
    {
        SLOT* record = records[index].m_slots.Find(slot);
        if (record != 0)
            return record->m_pauseCount;
    }

    return 0;
}

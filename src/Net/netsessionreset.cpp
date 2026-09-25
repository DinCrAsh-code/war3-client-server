//============================================================================
//  0x6F54C7A0 - reset a record that never got past GAME_STATE_FAKED.
//
//  Answers 1 when it actually reset something and 0 otherwise, and the two
//  refusals - no record array at all, and a record in some other state -
//  share one `xor eax, eax` tail the shipped code parks after the body.
//
//  Own translation unit: CNetPlayerRecord::Reset is a real call here, and
//  0x6F54C7E0 calls this one for real in turn.
//============================================================================
#include "netdata.h"

int __fastcall ResetFakedSessionRecord(int index)
{
    CNetData* records = NetDataRecords();
    if (records != 0)
    {
        CNetData* record = &records[index];
        if (record->m_gameState == kGameStateFaked)
        {
            record->Session()->Reset();
            return 1;
        }
    }

    return 0;
}

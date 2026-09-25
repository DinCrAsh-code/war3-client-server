//============================================================================
//  0x6F53ED30 / 0x6F53EDD0 - the two neighbouring slot bytes of a CNetData
//  record, each behind the same three-hop thread-local fetch.
//
//  +0x2DD is the *pausing* player's slot: RequestPause writes the requesting
//  slot into it and RequestResume writes 0xFF back, and
//  CGameUI::SetGamePaused reads it to refuse a second pause ("somebody
//  already has it") and to refuse a resume ("nobody paused it").  Batch A of
//  this closure reached it as a redirect and called it GetPlayerSlotState.
//
//  +0x2DC is the local player's own slot.  Nothing in this closure writes
//  it; what types it is the one place that reads it -
//  CEscMenuMainPanel::RefreshCaption (renamed from CGameUIPauseFrame,
//  2026-08-28) asks IsSlotParticipating about it to decide whether the
//  *local* player may press pause.
//
//  Both answer 0xFF when this thread has no game data at all, formed as
//  `or al, 0FFh` off the byte return rather than as a fresh `mov`.
//
//  Two functions in one translation unit because neither calls the other and
//  they are 0xA0 bytes apart in the shipped image; nothing here can inline
//  into anything else.
//============================================================================
#include "netdata.h"

unsigned char __fastcall CNetData::GetPausingSlot(int index)
{
    //  The guarded success path, not a refusal first: the shipped code
    //  falls through into the read and parks `or al,0FFh` at the end.
    CNetData* records = NetDataRecords();
    if (records != 0)
        return records[index].m_pausingSlot;

    return 0xFF;
}

unsigned char __fastcall CNetData::GetLocalSlot(int index)
{
    CNetData* records = NetDataRecords();
    if (records != 0)
        return records[index].m_localSlot;

    return 0xFF;
}

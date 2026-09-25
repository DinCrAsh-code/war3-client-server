//============================================================================
//  0x6F53EA60 / 0x6F53EAF0 - the two halves of the pause request, and the
//  only two writers of CNetData::m_pausingSlot.
//
//  RequestPause refuses outright if the session is already PAUSING or
//  PAUSED, finds the requesting participant, moves the session to PAUSING,
//  records who did it, and - unless the caller says otherwise - charges the
//  pause to that participant's allowance.  CGameUI::SetGamePaused passes
//  `countIt` as "the acting player is not a referee", so a referee's pause is
//  free.  SetGameState's own answer is discarded: the state test above it has
//  already established that the transition is legal.
//
//  Its two outer refusals - no game data, and a session already paused -
//  share one zero held in a register from the top, which is why they are one
//  `refused` local and a single trailing `return` rather than two literal
//  `return 0`s; the third, a slot that is not in the table, really is its own
//  `xor eax,eax` in the shipped stream.
//
//  RequestResume is its opposite and answers 1 for every case it does *not*
//  act on - no game data, or a session that was not paused in the first
//  place - because "the game is running" is what the caller is asking.  The
//  one refusal is a session whose resume is blocked at +0x2E0.
//
//  Batch A reached the second one as a redirect and called it
//  SubmitObserverPauseRequest; it has nothing to do with observers.  Its one
//  caller is CGameUI::SetGamePaused's `paused == 0` arm.
//
//  Two functions in one translation unit: neither calls the other, and both
//  call CNetPlayerRecord::SetGameState and SLOTTABLE::Find for real.
//============================================================================
#include "netdata.h"

int __fastcall CNetData::RequestPause(unsigned char slot, int index,
                                      int countIt)
{
    int refused = 0;

    CNetData* records = NetDataRecords();
    if (records != 0)
    {
        CNetData* self = &records[index];

        CNetPlayerRecord* session = self->Session();
        int state = self->m_gameState;

        if (state != kGameStatePausing && state != kGameStatePaused)
        {
            SLOT* record = self->m_slots.Find(slot);
            if (record == 0)
                return 0;

            session->SetGameState(kGameStatePausing);
            self->m_pausingSlot = slot;

            if (countIt != 0)
                record->m_pauseCount += 1;

            return 1;
        }
    }

    return refused;
}

int __fastcall CNetData::RequestResume(int index)
{
    CNetData* records = NetDataRecords();
    if (records != 0)
    {
        CNetData* self = &records[index];

        int state = self->m_gameState;
        CNetPlayerRecord* session = self->Session();

        if (state == kGameStatePausing || state == kGameStatePaused)
        {
            if (self->m_resumeBlocked != 0)
                return 0;

            session->SetGameState(kGameStatePlaying);
            self->m_pausingSlot = 0xFF;
        }
    }

    return 1;
}

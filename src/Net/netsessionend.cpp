//============================================================================
//  0x6F54C7E0 - end the session's game.
//
//  Three outcomes, in the order the shipped code tests them:
//
//    * a record still in GAME_STATE_FAKED never started, so it is simply
//      reset and nothing is announced;
//    * a record that is not the live one (`+0x610`) is left alone entirely;
//    * otherwise, a game that got past GAME_STATE_FAKED+1 has its send
//      queue drained and closed and its accumulated results shipped, and
//      then - in every live case, results or not - the event the caller
//      handed in is posted to the network client.
//
//  The event id arrives in ecx and its argument in edx and both are held
//  across the whole body for that last call, which is why the shipped
//  prologue's first act is to park ecx in a frame slot.
//============================================================================
#include "netdata.h"
#include "netclient.h"

//  0x6F54C7A0 - Net/netsessionreset.cpp.
int __fastcall ResetFakedSessionRecord(int index);

typedef void (__thiscall *NetSendQueueCloseFn)(void* self);

void __fastcall EndSessionGame(unsigned int eventId, int eventArg, int index)
{
    CNetData* records = NetDataRecords();

    int state = records[index].m_gameState;
    if (state == kGameStateFaked)
    {
        ResetFakedSessionRecord(index);
        return;
    }

    SNetSessionInfo* session = (SNetSessionInfo*)records;
    if (index == session->m_activeIndex)
    {
        if (state >= 3)
        {
            CNetSendQueue* queue = &session->m_sendQueue;
            queue->Flush();
            ((NetSendQueueCloseFn)(*(void***)queue)[0x1C / 4])(queue);
            session->SubmitGameResults(index);
        }

        PostNetClientEvent(eventId, eventArg, 0);
    }
}

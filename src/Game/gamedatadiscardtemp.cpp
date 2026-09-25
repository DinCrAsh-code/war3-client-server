//============================================================================
//  0x6F53F830 - discard the temporary game-data file, reached from the
//  session's own thread-local context.
//
//  Eight instructions and a tail jump: the thread's slot 0x0D, its cache at
//  +0x10, the net data at +0x08 of that, and then the discard itself - which
//  it *tail-calls*, so it answers whatever that answers even though its own
//  callers ignore it.
//============================================================================
#include "gamecontext.h"

//  0x6F53E2A0 - Net/netdatadiscardtemp.cpp.
int __fastcall NetDataDiscardTempFile(void* self);

struct SNetDataHolder
{
    char  m_reserved00[0x08];
    void* m_pNetData;       // +0x08
};

void __fastcall GameDataDiscardTempFile()
{
    CGameDataContext* context = (CGameDataContext*)GetThreadLocalSlot(0x0D);

    SNetDataHolder* holder = (SNetDataHolder*)context->m_pCache;
    if (holder->m_pNetData != 0)
        NetDataDiscardTempFile(holder->m_pNetData);
}

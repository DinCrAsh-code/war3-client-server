//============================================================================
//  0x6F657340 - CNetClient::SendPacket.
//
//  Four instructions and every one of them is addressing: load the sink at
//  +0x2B4, load its vtable, load slot 0, jump.  The jump is what says the
//  two stack words the caller pushed are the slot's own arguments and that
//  nothing here cleans them - an argument-rewriting tail call with the
//  argument list unchanged (docs/msvc-vc8-idioms.md).
//
//  Own translation unit so that the call from netclientevent.cpp stays a
//  real call.
//============================================================================
#include "netclient.h"

typedef void (__thiscall *NetSinkSendFn)(void* self, void* data,
                                         unsigned int size);

void CNetClient::SendPacket(void* data, unsigned int size)
{
    void* sink = m_pPacketSink;
    ((NetSinkSendFn)(*(void***)sink)[0])(sink, data, size);
}

//----------------------------------------------------------------------------
//  0x6F657350 - CNetClient::Method_0x44.  Same four-instruction shape as
//  SendPacket above, one slot further along (0x44/4 = 17) on the same sink
//  object.  Reached from CTurnsSyncStore::Flush (netturnsyncstore.cpp).
//----------------------------------------------------------------------------
void CNetClient::Method_0x44(void* data, unsigned int size)
{
    void* sink = m_pPacketSink;
    ((NetSinkSendFn)(*(void***)sink)[0x44 / 4])(sink, data, size);
}

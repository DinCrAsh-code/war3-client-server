//============================================================================
//  Net::CTurnsSyncStore's own two used members - see netclient.h for the
//  class-level notes (why it stays a plain hand-dispatched struct rather
//  than a real polymorphic class, and what is/isn't known about its
//  vtable).  0x6F652080 and 0x6F6504F0 are not one module by address, but
//  share this file anyway: both are CTurnsSyncStore's own members, reached
//  only from CNetData_AppliedDeltaReporterCtor/Flush
//  (netcommand_dispatch.cpp), and neither calls the other.
//============================================================================
#include "netclient.h"

typedef int (__thiscall *CTurnsSyncStoreGrowFn)(CTurnsSyncStore* self, int count,
                                                 int increment, void** pField4,
                                                 int* pField8, int* pField0xC,
                                                 int flagA, int flagB);
typedef void (__thiscall *CTurnsSyncStoreGetBufferFn)(CTurnsSyncStore* self,
                                                       void** outBuffer,
                                                       unsigned int* outSize,
                                                       int flags);

//----------------------------------------------------------------------------
//  0x6F652080 - reserve room for one more 5-byte record.
//
//  The two-part guard is the shipped code's own, not an equivalent
//  rewrite: `count < m_field8` alone forces a Grow call regardless of
//  m_field0xC, and only when that first half already holds does the
//  `count+5 <= m_field0xC+m_field8` half get checked at all - written here
//  as the same nested shape so the branch that decides "already have room"
//  stays the single path the dump takes.
//----------------------------------------------------------------------------
void CTurnsSyncStore::ReserveRecord()
{
    unsigned int count = (unsigned int)m_count;

    if (count >= (unsigned int)m_field8)
    {
        if (count + 5 <= (unsigned int)m_field0xC + (unsigned int)m_field8)
            goto haveRoom;
    }

    ((CTurnsSyncStoreGrowFn)(*(void***)this)[0xC / 4])(
        this, count, 5, &m_field4, &m_field8, &m_field0xC, 0, 0);

haveRoom:
    m_count += 5;
}

//----------------------------------------------------------------------------
//  0x6F6504F0 - drain the queue and forward what's left to the network.
//  Same shape as CNetSendQueue::Flush (netclientevent.cpp): ask the queue
//  for its buffer through vtable slot 9, skip the record's own header (5
//  bytes here, 8 there), bail silently on an empty remainder, reject
//  anything outside (0, 0x60] (0x400 there), then hand what is left to the
//  thread's network client - here through its own vtable slot 0x44
//  (Method_0x44) rather than slot 0 (SendPacket).
//----------------------------------------------------------------------------
void CTurnsSyncStore::Flush()
{
    void* buffer;
    unsigned int size;

    ((CTurnsSyncStoreGetBufferFn)(*(void***)this)[0x24 / 4])(this, &buffer,
                                                              &size, 0);

    buffer = (char*)buffer + 5;
    size -= 5;
    if (size == 0)
        return;

    if (((0u < size ? -1 : 0) & ((0x60u < size) - 1)) == 0)
    {
        SErrSetLastError(kErrorInvalidParameter);
        return;
    }

    CNetClient* client = (CNetClient*)GetThreadLocalSlot(kThreadLocalNetClient);
    if (client != 0)
        client->Method_0x44(buffer, size);
}

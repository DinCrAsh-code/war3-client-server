//============================================================================
//  OsNet::CONNSTATE::Insert (0x6F6DF540) and ::Remove (0x6F6DF5F0) - the
//  sharded add/remove pair under one of TCPMGR's four connection states.
//
//  One translation unit: neither calls the other, and their one caller
//  (TCPMGR::MoveConn, ostcp_setstate.cpp) is in a different one so both calls
//  survive.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DF540 - retn 4, returns the shard index.
//
//  The counter is bumped and then *re-read* rather than the increment's own
//  return value being used - two statements in the source, and it has to stay
//  two, because using the returned value would drop the reload the shipped
//  code has.  Under a concurrent insert the two can disagree; that is the
//  shipped behaviour and it is harmless here, since either value is a legal
//  shard and the one actually used is the one remembered in m_stateBucket.
//
//  m_count is bumped after the list is released, not inside the lock.
//----------------------------------------------------------------------------
unsigned char CONNSTATE::Insert(NETCONN* conn)
{
    InterlockedIncrement(&m_cursor);
    int bucket = m_cursor & 7;

    EnterCriticalSection(&m_locks[bucket]);
    m_lists[bucket].LinkToHead(conn);
    LeaveCriticalSection(&m_locks[bucket]);

    InterlockedIncrement(&m_count);
    return (unsigned char)bucket;
}

//----------------------------------------------------------------------------
//  0x6F6DF5F0 - retn 8.
//
//  `bucket` is the shard Insert() handed back, kept in NETCONN::m_stateBucket
//  because the list encoding gives removal no other way to find which of the
//  eight locks this connection is under.  It is read back as a byte
//  (`movzx esi, byte ptr [esp+...]`) even though the caller pushes a full
//  dword, which is what an `unsigned char` parameter looks like.
//----------------------------------------------------------------------------
void CONNSTATE::Remove(NETCONN* conn, unsigned char bucket)
{
    EnterCriticalSection(&m_locks[bucket]);
    m_lists[bucket].Link(conn)->Unlink();
    LeaveCriticalSection(&m_locks[bucket]);

    InterlockedDecrement(&m_count);
}

}  // namespace OsNet

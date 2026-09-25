//============================================================================
//  OsNet::TCPMGR::MoveConn (0x6F6DFFB0) - move a connection from whichever of
//  the four state lists it is on to `newState`.
//
//  Its own translation unit so the two CONNSTATE calls (ostcp_bucket.cpp) stay
//  real calls.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DFFB0 - retn 8.
//
//  The lock taken is the *connection's*, not the state's - the shards have
//  their own locks inside Insert/Remove.  State 4 is the "on no list"
//  sentinel at both ends, so a move from 4 to 4 does nothing but rewrite the
//  two bytes.
//
//  Both bytes are written after the calls, from the values that were already
//  in registers: `bucket` is zero unless Insert ran, and the state byte is the
//  low byte of the `int` parameter (the shipped code reloads the parameter
//  after the Insert call and stores `al`).
//----------------------------------------------------------------------------
void TCPMGR::MoveConn(NETCONN* conn, int newState)
{
    EnterCriticalSection(&conn->m_critSec);

    unsigned char bucket = 0;
    unsigned char state = (unsigned char)conn->m_state;
    if (state != kConnStateNone)
        m_states[state].Remove(conn, (unsigned char)conn->m_stateBucket);

    if (newState != kConnStateNone)
        bucket = m_states[newState].Insert(conn);

    conn->m_state = (char)newState;
    conn->m_stateBucket = (char)bucket;

    LeaveCriticalSection(&conn->m_critSec);
}

}  // namespace OsNet

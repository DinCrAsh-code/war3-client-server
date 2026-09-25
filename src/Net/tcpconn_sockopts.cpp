//============================================================================
//  OsNet::TCPCONN's three plain setsockopt wrapper slots - 12, 13 and 14
//  (0x6F6DA800, 0x6F6DA830, 0x6F6DA890).  TCPCONN's own, inherited unchanged
//  by IOTCPCONN, and called by nothing in either dump - names and
//  signatures are inferred from each body.  Address-adjacent in the shipped
//  image (all three within 0x100 bytes of each other), so kept in one
//  translation unit together; none calls another, so there is nothing for
//  that to accidentally inline.
//============================================================================
#include "ostcp.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DA800 - retn 4.  `setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY,
//  &flag, 4)` with `flag = !coalesce` - a nonzero `coalesce` leaves Nagle's
//  algorithm on (TCP_NODELAY cleared), zero disables it (TCP_NODELAY set).
//----------------------------------------------------------------------------
void TCPCONN::SetCoalesce(int coalesce)
{
    BOOL flag = !coalesce;
    setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY,
               (const char*)&flag, sizeof(flag));
}

//----------------------------------------------------------------------------
//  0x6F6DA830 - retn 4.  Sets SO_SNDBUF to `size`; only on success sets
//  SO_RCVBUF to the same value.  Returns whether the second call succeeded -
//  false if either one failed.
//----------------------------------------------------------------------------
BOOL TCPCONN::SetBufferSizes(int size)
{
    int value = size;
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF,
                   (const char*)&value, sizeof(value)) != 0)
        return FALSE;
    value = size;
    return setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF,
                      (const char*)&value, sizeof(value)) == 0;
}

//----------------------------------------------------------------------------
//  0x6F6DA890 - retn 4.  `return setsockopt(m_socket, SOL_SOCKET,
//  SO_RCVTIMEO, &ms, 4);` - the raw setsockopt result, unlike
//  SetBufferSizes' boolean.
//----------------------------------------------------------------------------
int TCPCONN::SetRecvTimeout(int ms)
{
    int value = ms;
    return setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,
                      (const char*)&value, sizeof(value));
}

}  // namespace OsNet

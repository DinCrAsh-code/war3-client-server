//============================================================================
//  OsNet::NETCONN::AddRefRaw/ReleaseRaw - the unchecked, no-null-test halves
//  of the refcount pair (see netconn.h). Its own translation unit, separate
//  from the null-checked AddRef/Release (netconnref.cpp), so those callers
//  keep a real `call`/tail-`jmp` into these instead of inlining them.
//============================================================================
#include "netconn.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DA3D0
//----------------------------------------------------------------------------
void NETCONN::AddRefRaw()
{
    InterlockedIncrement((LONG*)&m_refcount);
}

//----------------------------------------------------------------------------
//  0x6F6DA3E0
//----------------------------------------------------------------------------
void NETCONN::ReleaseRaw()
{
    if (InterlockedDecrement((LONG*)&m_refcount) == 0 && this != 0)
        DeleteSelf(1);
}

}  // namespace OsNet

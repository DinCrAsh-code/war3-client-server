//============================================================================
//  OsNet::NETCONN - see netconn.h for the class-level notes. This file has
//  the constructor, destructor and the intrusive-refcount cluster
//  (AddRef/Release and their raw, unchecked twins) - everything reached
//  from NetProviderLTCP__Method_0xC's own listener-thread bootstrap
//  (0x6F6639D0) without going through UDPCONN's extra socket setup, which
//  lives in its own translation unit (netconnctor.cpp) precisely so the
//  base-class call from there stays a real `call` instead of being inlined
//  away.
//============================================================================
#include "netconn.h"

//  Storm.dll ordinal 465 - same declaration shape as tls.cpp/refcnt.cpp's.
void __stdcall SErrSetLastError(unsigned int code);

//  The CRT intrinsic, recognised by name/signature under /O2 without
//  <string.h> (which /Zl keeps out of this build).
extern "C" void* __cdecl memcpy(void* dst, const void* src, unsigned int count);
#pragma intrinsic(memcpy)

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DCEC0 - retn 14h (5 stack args, owner..addrs in that order).
//----------------------------------------------------------------------------
NETCONN::NETCONN(TCPMGR* owner, SOCKET s, CONNNOTIFYPROC proc, void* ctx,
                 const void* addrs)
{
    m_socket = s;
    m_linkNext = 0;
    m_linkPrevlink = 0;
    m_stateBucket = 0;
    m_notifySeq = 0;
    m_state = 4;
    m_refcount = 1;
    m_notifyProc = proc;
    m_notifyCtx = ctx;
    InitializeCriticalSection(&m_critSec);
    m_owner = owner;
    m_createTick = GetTickCount();
    memcpy(&m_addrs, addrs, 32);
}

//  ~NETCONN (0x6F6DCE00) lives in ostcp_conndtor.cpp - see netconn.h.

}  // namespace OsNet

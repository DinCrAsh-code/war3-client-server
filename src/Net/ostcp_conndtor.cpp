//============================================================================
//  OsNet::TCPCONN::~TCPCONN (0x6F6E0520), OsNet::NETCONN::~NETCONN
//  (0x6F6DCE00) and OsNet::NETCONN::DeleteSelf (0x6F6DCD90, vtable slot 4).
//
//  The three are together deliberately.  0x6F6E0520 ends with the whole of
//  the base destructor *inlined* - the NETCONN vftable store,
//  DeleteCriticalSection and the link Unlink all appear in it with no call -
//  which only happens when the compiler can see the base body, i.e. when the
//  two are compiled in one translation unit.  0x6F6DCE00 still exists as a
//  standalone function for the other translation units that call it, which
//  is exactly what a non-inline member function defined here gives: MSVC
//  emits the out-of-line definition and inlines it into the caller in this
//  file at the same time.
//
//  DeleteSelf belongs here for the identical reason: its shipped body
//  (0x6F6DCD90) has `~NETCONN()`'s whole call *inlined* too - the vftable
//  store comes first, then DeleteCriticalSection, then the link Unlink, with
//  no `call` in between - which only happens with the destructor's
//  definition visible in this translation unit.  That inlining is also why
//  this override has no `this != 0` test where TCPCONN's and IOTCPCONN's
//  do: the vftable store proves the pointer non-null before the compiler
//  would reach the test (docs/msvc-vc8-idioms.md, "A deleting destructor's
//  `this != 0` test tracks whether the base destructor inlined").
//============================================================================
#include "ostcp.h"

//  Storm.dll ordinal 403 - same declaration shape as refcnt_deleteself.cpp's.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E0520.
//
//  The queue is drained explicitly first; ~OUTPUTLIST (the member destructor)
//  then runs on an already-empty list, which is why the UnlinkAll call and the
//  inlined terminator Unlink that follow it do nothing here in practice and
//  are still both in the stream.
//----------------------------------------------------------------------------
TCPCONN::~TCPCONN()
{
    m_sendQueue.FreeAll();
    m_owner->MoveConn(this, kConnStateNone);
}

//----------------------------------------------------------------------------
//  0x6F6DCE00 - tear down the critical section, then unlink from whatever
//  intrusive list still holds this connection.
//
//  That order is not a guess and it is not the order netconn.h used to
//  describe: ~TCPCONN above has this body inlined into it, and the dump has
//  the DeleteCriticalSection call ahead of the link walk there.  The two
//  cannot have been reordered by the compiler either way round - the walk is
//  a run of plain memory accesses and DeleteCriticalSection is an opaque
//  external call, so neither can be moved across the other - so the shipped
//  source really did delete first.
//----------------------------------------------------------------------------
NETCONN::~NETCONN()
{
    DeleteCriticalSection(&m_critSec);
    ((TSLink<NETCONN>*)&m_linkNext)->Unlink();
}

//----------------------------------------------------------------------------
//  0x6F6DCD90 - vtable slot 4, retn 4.  `~NETCONN()`'s call inlines (see the
//  file header), so this is the whole thing: vftable stamp, critical-section
//  delete, link unlink, then free on flags&1 with no null test.
//----------------------------------------------------------------------------
NETCONN* NETCONN::DeleteSelf(int flags)
{
    this->NETCONN::~NETCONN();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

}  // namespace OsNet

//============================================================================
//  OsNet::NETCONNLESS::DeleteSelf (0x6F6DE8E0) and
//  OsNet::NETCONNFULL::DeleteSelf (0x6F6DE9B0) - vtable slot 4 for each.
//
//  Both classes are `: public NETCONN` with no data of their own (see
//  osnetfamily.h), so their implicit destructors are trivial calls straight
//  into NETCONN::~NETCONN(), which is why each body here is byte-for-byte
//  NETCONN::DeleteSelf's own (ostcp_conndtor.cpp) - a fresh compiled
//  instance at its own address because each is a distinct derived class,
//  not because either does anything different.
//
//  ~NETCONN()'s own body (DeleteCriticalSection then the link Unlink) is
//  inlined directly here rather than called, matching NETCONN::DeleteSelf's
//  own shape (netconn.h's slot 4 comment, docs/msvc-vc8-idioms.md's "A
//  deleting destructor's `this != 0` test tracks whether the base
//  destructor inlined") - a real out-of-line call to `~NETCONN()` (declared
//  in netconn.h, defined in a different TU) would not inline the same way
//  it does inside ostcp_conndtor.cpp, where NETCONN::DeleteSelf and
//  NETCONN::~NETCONN() are compiled together.
//============================================================================
#include "osnetfamily.h"

//  Storm.dll ordinal 403 - same declaration ostcp_conndtor.cpp uses.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DE8E0 - retn 4h.
//----------------------------------------------------------------------------
NETCONNLESS* NETCONNLESS::DeleteSelf(int flags)
{
    DeleteCriticalSection(&m_critSec);
    ((TSLink<NETCONN>*)&m_linkNext)->Unlink();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F6DE9B0 - retn 4h.
//----------------------------------------------------------------------------
NETCONNFULL* NETCONNFULL::DeleteSelf(int flags)
{
    DeleteCriticalSection(&m_critSec);
    ((TSLink<NETCONN>*)&m_linkNext)->Unlink();
    if (flags & 1)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

}  // namespace OsNet

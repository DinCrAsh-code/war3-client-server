//============================================================================
//  OsNet::TCPCONN::DeleteSelf - vtable slot 4, 0x6F6E24C0.
//
//  Overrides NETCONN::DeleteSelf (netconn.h) for the scalar deleting
//  destructor of a `virtual ~TCPCONN()`.  Deliberately its own translation
//  unit, separate from ostcp_conndtor.cpp (which defines ~TCPCONN()): the
//  shipped body's `this->TCPCONN::~TCPCONN()` is a real `call`, not an
//  inlined body, which only happens with the destructor's definition NOT
//  visible here - see docs/msvc-vc8-idioms.md, "The corollary is a
//  placement rule". That real call is also why this override carries the
//  `this != 0` test NETCONN::DeleteSelf does not.
//============================================================================
#include "ostcp.h"

//  Storm.dll ordinal 403 - same declaration shape as refcnt_deleteself.cpp's.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

static const char kDelete[] = "delete";

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6E24C0 - retn 4.
//----------------------------------------------------------------------------
TCPCONN* TCPCONN::DeleteSelf(int flags)
{
    this->TCPCONN::~TCPCONN();
    if ((flags & 1) && this != 0)
        SMemFree(this, kDelete, -1, 0);
    return this;
}

}  // namespace OsNet

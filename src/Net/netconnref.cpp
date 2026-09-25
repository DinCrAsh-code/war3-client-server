//============================================================================
//  OsNet::NETCONN::AddRef/Release - the null-checked, diagnostic halves of
//  the refcount pair (see netconn.h). Release() is a plain tail-call into
//  ReleaseRaw() (netconnrefraw.cpp, its own TU so the call survives),
//  matching the dump's own `jmp` on the non-null path.
//============================================================================
#include "netconn.h"

//  Storm.dll ordinal 465 - same declaration shape as tls.cpp/refcnt.cpp's.
void __stdcall SErrSetLastError(unsigned int code);

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DACD0
//----------------------------------------------------------------------------
NETCONN* NETCONN::AddRef()
{
    if (this == 0)
    {
        SErrSetLastError(0x57);
        return 0;
    }
    AddRefRaw();
    return this;
}

//----------------------------------------------------------------------------
//  0x6F6DACF0
//----------------------------------------------------------------------------
void NETCONN::Release()
{
    if (this == 0)
    {
        SErrSetLastError(0x57);
        return;
    }
    ReleaseRaw();
}

}  // namespace OsNet

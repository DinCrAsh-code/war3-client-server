//============================================================================
//  OsNet::FILECONN::DeleteSelf (0x6F6E3970), OsNet::IOFILECONN::DeleteSelf
//  (0x6F6E39A0) and OsNet::SLFILECONN::DeleteSelf (0x6F6E39D0) - slot 4 for
//  each.  All three call a real, out-of-line destructor rather than
//  inlining one - FILECONN/IOFILECONN share the same destructor
//  (0x6F6E2650, out of this session's scope, thunked); SLFILECONN's own
//  (0x6F6E2A10) tears down its extra wait-event handle first and then tail-
//  jumps into that same 0x6F6E2650, so it is thunked separately here rather
//  than reused, matching what the dump actually does.
//============================================================================
#include "osnetfamily.h"

//  Storm.dll ordinal 403 - same declaration shape as ostcp_conndtor.cpp's.
//  Declared here, ahead of `namespace OsNet`, rather than block-scoped
//  inside each DeleteSelf below - a block-scope `extern` inside the
//  namespace mangles as `OsNet::SMemFree` (a symbol nothing provides),
//  not the global Storm import `link_check.py` actually resolves.
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline,
                        unsigned int flags);

namespace OsNet
{

typedef void (__thiscall *DestructFileConnFn)(void* obj);
typedef void (__thiscall *DestructSlFileConnFn)(void* obj);

//----------------------------------------------------------------------------
//  0x6F6E07F0 - slot 1, retn 0.
//----------------------------------------------------------------------------
BOOL FILECONN::IsSocketClosed()
{
    return m_hFile == (HANDLE)-1;
}

//----------------------------------------------------------------------------
//  0x6F6DAB30 - slot 5, retn 0.
//----------------------------------------------------------------------------
void FILECONN::AddPending()
{
    InterlockedIncrement((LONG*)&m_pending);
}

//----------------------------------------------------------------------------
//  0x6F6E3970 - retn 4h.
//----------------------------------------------------------------------------
FILECONN* FILECONN::DeleteSelf(int flags)
{
    ((DestructFileConnFn)0x6F6E2650)(this);
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F6E39A0 - retn 4h.
//----------------------------------------------------------------------------
IOFILECONN* IOFILECONN::DeleteSelf(int flags)
{
    ((DestructFileConnFn)0x6F6E2650)(this);
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

//----------------------------------------------------------------------------
//  0x6F6E39D0 - retn 4h.
//----------------------------------------------------------------------------
SLFILECONN* SLFILECONN::DeleteSelf(int flags)
{
    ((DestructSlFileConnFn)0x6F6E2A10)(this);
    if ((flags & 1) && this != 0)
        SMemFree(this, "delete", -1, 0);
    return this;
}

}  // namespace OsNet

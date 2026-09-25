//============================================================================
//  OsNet::IOFILECONN - the overlapped-I/O completion-port file connection,
//  `: public FILECONN`.  See osnetfamily.h for the class.
//============================================================================
#include "osnetfamily.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DAC00 - slot 3, retn 0.  CloseHandle the file if open, mark it
//  closed, release the connection lock, then post a zero-byte completion to
//  the I/O completion port (at m_owner+0x618) so the pump notices and
//  finishes tearing this connection down.  If the handle was already
//  closed, just release the lock.
//----------------------------------------------------------------------------
void IOFILECONN::OnPendingDrained()
{
    if (m_hFile != (HANDLE)-1)
    {
        CloseHandle(m_hFile);
        m_hFile = (HANDLE)-1;
        LeaveCriticalSection(&m_critSec);

        HANDLE port = *(HANDLE*)((char*)m_owner + 0x618);
        PostQueuedCompletionStatus(port, 0, (ULONG_PTR)this, 0);
        return;
    }

    LeaveCriticalSection(&m_critSec);
}

//----------------------------------------------------------------------------
//  0x6F6DAB40 - slot 10, retn 4h.  AddPending() through the vtable (slot
//  5), then an overlapped WriteFile using the queue node's own buffer/size
//  fields (+0x28/+0x30) and its embedded OVERLAPPED (+0x08); on a
//  synchronous failure that is not ERROR_IO_PENDING, posts a zero-byte
//  completion so the pump notices anyway.
//----------------------------------------------------------------------------
void IOFILECONN::PostWrite(void* node)
{
    typedef void (__thiscall *AddPendingFn)(void*);
    ((AddPendingFn)(*(void***)this)[5])(this);

    LPOVERLAPPED overlapped = (LPOVERLAPPED)((char*)node + 8);
    unsigned int size = *(unsigned int*)((char*)node + 0x24);
    void* buffer = *(void**)((char*)node + 0x30);

    if (!WriteFile(m_hFile, buffer, size, 0, overlapped) &&
        GetLastError() != ERROR_IO_PENDING)
    {
        HANDLE port = *(HANDLE*)((char*)m_owner + 0x618);
        PostQueuedCompletionStatus(port, 0, (ULONG_PTR)this, overlapped);
    }
}

//----------------------------------------------------------------------------
//  0x6F6DABB0 - slot 11, retn 4h.  Same shape as PostWrite, an overlapped
//  ReadFile.
//----------------------------------------------------------------------------
void IOFILECONN::PostRead(void* node)
{
    unsigned int size = *(unsigned int*)((char*)node + 0x28);
    void* buffer = *(void**)((char*)node + 0x24);
    LPOVERLAPPED overlapped = (LPOVERLAPPED)((char*)node + 8);

    if (!ReadFile(m_hFile, buffer, size, 0, overlapped) &&
        GetLastError() != ERROR_IO_PENDING)
    {
        HANDLE port = *(HANDLE*)((char*)m_owner + 0x618);
        PostQueuedCompletionStatus(port, 0, (ULONG_PTR)this, overlapped);
    }
}

}  // namespace OsNet

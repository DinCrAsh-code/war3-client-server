//============================================================================
//  OsNet::SLFILECONN - the event-driven (select-style) file connection,
//  `: public FILECONN`.  See osnetfamily.h for the class.
//============================================================================
#include "osnetfamily.h"

namespace OsNet
{

//----------------------------------------------------------------------------
//  0x6F6DAC90 - slot 3, retn 0.  CloseHandle the file if open and signal
//  the wait event, then release the connection lock.
//----------------------------------------------------------------------------
void SLFILECONN::OnPendingDrained()
{
    if (m_hFile != (HANDLE)-1)
    {
        CloseHandle(m_hFile);
        SetEvent(m_waitEvent);
        m_hFile = (HANDLE)-1;
    }

    LeaveCriticalSection(&m_critSec);
}

//----------------------------------------------------------------------------
//  0x6F6DAC50 - slot 10, retn 4h.  AddPending() through the vtable (slot
//  5), release the connection lock, then signal the wait event - the
//  actual write happens on whatever thread the event wakes, not here.
//----------------------------------------------------------------------------
void SLFILECONN::PostWrite(void* /*node*/)
{
    typedef void (__thiscall *AddPendingFn)(void*);
    ((AddPendingFn)(*(void***)this)[5])(this);

    LeaveCriticalSection(&m_critSec);
    SetEvent(m_waitEvent);
}

//----------------------------------------------------------------------------
//  0x6F6DAC80 - slot 11, retn 0.  Just SetEvent - no lock, no AddPending.
//----------------------------------------------------------------------------
void SLFILECONN::PostRead()
{
    SetEvent(m_waitEvent);
}

}  // namespace OsNet

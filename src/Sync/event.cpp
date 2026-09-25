//============================================================================
//  Event - a one-HANDLE wrapper around {CreateEventA, ResetEvent,
//  WaitForSingleObject, CloseHandle}, reached from the LAN TCP net
//  provider's listener-thread start/stop path (NetProviderLTCP__Method_0xC,
//  0x6F6639D0, and its callee sub_6F661CD0). Nothing in either call tree
//  names the owning class; every use is through a bare pointer to just this
//  one field, so a one-member class is all the evidence supports.
//
//  ~Event() (0x6F6D8450) is reached two ways in the dump: directly, and
//  through a one-instruction `jmp` alias at 0x6F6D8980 that is itself
//  `jmp`ed to from 0x6F62D3D0 - the __finally trampoline
//  NetProviderLTCP__Method_0xC's stop path (sub_6F661CD0) tail-calls into to
//  destroy its local Event on an abnormal exit. That whole SEH-frame shape
//  (a local object with a real destructor, unwound through a per-function
//  trampoline that tail-calls __CxxFrameHandler3) is the one this toolchain
//  cannot reproduce byte-for-byte even from the right source - see
//  docs/msvc-vc8-idioms.md, "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce".
//============================================================================
#include "event.h"

//----------------------------------------------------------------------------
//  0x6F6D89B0 - ZeroHandle() (eventzero.cpp) then
//  CreateEventA(0, bManualReset, bInitialState, 0).
//----------------------------------------------------------------------------
Event::Event(BOOL bManualReset, BOOL bInitialState)
{
    ZeroHandle();
    m_handle = CreateEventA(0, bManualReset, bInitialState, 0);
}

//----------------------------------------------------------------------------
//  0x6F6D8450 (this destructor's real body; reached under the aliases
//  described above).
//----------------------------------------------------------------------------
Event::~Event()
{
    Close();
}

//----------------------------------------------------------------------------
//  0x6F6D8450
//----------------------------------------------------------------------------
void Event::Close()
{
    if (m_handle)
    {
        CloseHandle(m_handle);
        m_handle = 0;
    }
}

//----------------------------------------------------------------------------
//  0x6F6D8530
//----------------------------------------------------------------------------
void Event::Reset()
{
    ResetEvent(m_handle);
}

//----------------------------------------------------------------------------
//  0x6F6D8520
//----------------------------------------------------------------------------
void Event::Set()
{
    SetEvent(m_handle);
}

//----------------------------------------------------------------------------
//  0x6F6D8470
//----------------------------------------------------------------------------
DWORD Event::Wait(DWORD dwMilliseconds)
{
    return WaitForSingleObject(m_handle, dwMilliseconds);
}

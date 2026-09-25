//============================================================================
//  Net::NetProviderLTCP::StopListenerThread - 0x6F661CD0. Waits for a
//  previously-running listener thread to actually exit: first the thread's
//  own "I'm done" event (if one was ever started), then, if the thread
//  still holds a live listen handle, signals it and waits again on a fresh,
//  local completion event the signalled side is expected to fire.
//
//  The local `Event` here is a real automatic-storage C++ object with a
//  non-trivial destructor, which is exactly the shape
//  docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" already covers: the dump's SEH_6F661CD0
//  frame (cookie-XORed, frame-pointer-omitted, unwinding through a
//  per-function trampoline that tail-calls __CxxFrameHandler3) is the
//  modern EH model this repo's fixed /GS- /EHs-c- toolchain cannot emit
//  from any source shape, even the correct one.
//============================================================================
#include "netproviderltcp.h"

//  critsec.cpp
void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

namespace Net
{

void NetProviderLTCP::StopListenerThread()
{
    Event doneSignal(0, 0);

    EnterCritSec(&m_lock);
    int wasRunning = m_threadRunning;
    LeaveCritSec(&m_lock);

    if (wasRunning)
        m_threadDoneEvent.Wait(INFINITE);

    EnterCritSec(&m_lock);
    void* handle = m_listenHandle;
    bool hasHandle = (handle != 0);
    if (hasHandle)
    {
        m_pendingSignalTarget = &doneSignal;
        SignalWaitHandle((IWaitHandleSignal*)handle);
    }
    LeaveCritSec(&m_lock);

    if (hasHandle)
    {
        ((Event*)m_pendingSignalTarget)->Wait(INFINITE);
        m_pendingSignalTarget = 0;
    }
}

}  // namespace Net

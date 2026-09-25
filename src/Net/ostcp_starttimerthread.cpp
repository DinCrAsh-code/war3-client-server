//============================================================================
//  0x6F6E4F60 - TCPMGR::StartTimerThread.  An auto-reset event and one
//  thread, with the manager's own reference taken before the thread starts
//  and given back if it does not.
//============================================================================
#include "ostcp.h"
#include <process.h>

namespace OsNet
{

extern "C" void __stdcall OsTcpTimerThreadMain(void*);   // 0x6F6E4FE0

int TCPMGR::StartTimerThread(int /*flags*/)
{
    unsigned int threadId;

    m_timerEvent = CreateEventA(0, FALSE, FALSE, 0);
    if (m_timerEvent != 0)
    {
        OsInterlockedIncrement(&m_refcount);

        HANDLE thread = (HANDLE)_beginthreadex(0, 0, (unsigned (__stdcall*)(void*))OsTcpTimerThreadMain,
                                               this, 0, &threadId);
        if (thread != 0)
        {
            m_timerThread = thread;
            return 1;
        }

        Release();
    }

    return 0;
}

}  // namespace OsNet

//============================================================================
//  0x6F6E6C40 - TCPMGR::StartListenThread.  Two auto-reset events - one to
//  wake the thread, one it signals when it has really gone - and the thread
//  itself, again with the reference taken up front and handed back on
//  failure.
//============================================================================
#include "ostcp.h"
#include <process.h>

namespace OsNet
{

extern "C" void __stdcall OsTcpListenThreadMain(void*);   // 0x6F6E5FB0

int TCPMGR::StartListenThread(int /*flags*/)
{
    unsigned int threadId;

    m_listenWakeEvent = CreateEventA(0, FALSE, FALSE, 0);
    if (m_listenWakeEvent != 0)
    {
        m_listenDoneEvent = CreateEventA(0, FALSE, FALSE, 0);
        if (m_listenDoneEvent != 0)
        {
            OsInterlockedIncrement(&m_refcount);

            HANDLE thread = (HANDLE)_beginthreadex(0, 0,
                                (unsigned (__stdcall*)(void*))OsTcpListenThreadMain,
                                this, 0, &threadId);
            if (thread != 0)
            {
                m_listenThread = thread;
                return 1;
            }

            Release();
        }
    }

    return 0;
}

}  // namespace OsNet

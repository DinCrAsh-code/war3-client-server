//============================================================================
//  0x6F6E5ED0 - TCPMGR::StartPumpThreads.  One thread per configured worker,
//  each holding a reference, each appended to the manager's own handle array
//  through TSGrowableArray::Add.
//
//  Which body the thread runs is decided per thread rather than once: a
//  manager with a completion port pumps it, one without runs the select
//  pump instead.
//============================================================================
#include "ostcp.h"
#include <process.h>

namespace OsNet
{

extern "C" void __stdcall OsTcpIocpPumpMain(void*);     // 0x6F6E4F30
extern "C" void __stdcall OsTcpSelectPumpMain(void*);   // 0x6F6E5910

int TCPMGR::StartPumpThreads()
{
    unsigned int started = 0;
    unsigned int threadId;

    if (m_workerCount == 0)
        return 1;

    do
    {
        OsInterlockedIncrement(&m_refcount);

        void* entry = m_iocp != 0 ? (void*)OsTcpIocpPumpMain
                                  : (void*)OsTcpSelectPumpMain;

        HANDLE thread = (HANDLE)_beginthreadex(0, 0,
                            (unsigned (__stdcall*)(void*))entry,
                            this, 0, &threadId);
        if (thread == 0)
        {
            Release();
            return 0;
        }

        m_pumpThreads.Add(thread);
        started++;
    }
    while (started < m_workerCount);

    return 1;
}

}  // namespace OsNet

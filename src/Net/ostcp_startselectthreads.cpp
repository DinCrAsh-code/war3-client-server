//============================================================================
//  0x6F6E6D30 - TCPMGR::StartSelectThreads.  Two threads: the select loop
//  itself (only when bit 1 is set, and at below-normal priority), and the
//  notify thread that always runs.
//
//  A manager with no completion port has to start its pumps here instead,
//  which is the first thing this does.
//============================================================================
#include "ostcp.h"
#include <process.h>

namespace OsNet
{

extern "C" void __stdcall OsTcpSelectThreadMain(void*);   // 0x6F6E5CF0
extern "C" void __stdcall OsTcpNotifyThreadMain(void*);   // 0x6F6E68E0

int TCPMGR::StartSelectThreads(int flags)
{
    unsigned int threadId;

    if (m_iocp == 0 && !StartPumpThreads())
        return 0;

    if (flags & 2)
    {
        OsInterlockedIncrement(&m_refcount);

        HANDLE select = (HANDLE)_beginthreadex(0, 0,
                            (unsigned (__stdcall*)(void*))OsTcpSelectThreadMain,
                            this, 0, &threadId);
        if (select == 0)
        {
            Release();
            return 0;
        }

        m_selectThread = select;
        SetThreadPriority(select, THREAD_PRIORITY_BELOW_NORMAL);
    }

    m_pollEvent = CreateEventA(0, FALSE, FALSE, 0);
    if (m_pollEvent == 0)
        return 0;

    OsInterlockedIncrement(&m_refcount);

    HANDLE poll = (HANDLE)_beginthreadex(0, 0,
                      (unsigned (__stdcall*)(void*))OsTcpNotifyThreadMain,
                      this, 0, &threadId);
    if (poll == 0)
    {
        Release();
        return 0;
    }

    m_pollThread = poll;
    return 1;
}

}  // namespace OsNet

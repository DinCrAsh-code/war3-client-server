//============================================================================
//  0x6F6DECF0 - TCPMGR::JoinPumpThreads.  One WaitForMultipleObjects over
//  the whole array, then a CloseHandle each.
//
//  The two loops read `m_pumpThreads.m_count` afresh: the wait is skipped
//  entirely when it is zero, and the close loop re-tests it rather than
//  caching the count, which is what the shipped stream shows.
//============================================================================
#include "ostcp.h"

namespace OsNet
{


void TCPMGR::JoinPumpThreads()
{
    if (m_pumpThreads.m_count == 0)
        return;

    WaitForMultipleObjects(m_pumpThreads.m_count, m_pumpThreads.m_data, TRUE, INFINITE);

    for (unsigned int i = 0; i < m_pumpThreads.m_count; i++)
        CloseHandle(m_pumpThreads.m_data[i]);

    m_pumpThreads.m_count = 0;
}

}  // namespace OsNet

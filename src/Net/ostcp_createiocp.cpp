//============================================================================
//  0x6F6D9ED0 - create the completion port and say how many pump threads to
//  run on it: twice the processor count, and at least two.
//
//  The whole SYSTEM_INFO is zeroed by hand before GetSystemInfo fills it -
//  nine stores in the shipped stream, one per member - which is what a
//  `SYSTEM_INFO info = { 0 };` at this optimisation level becomes.
//============================================================================
#include "ostcp.h"

namespace OsNet
{


HANDLE __fastcall OsNetCreateIocp(unsigned int* workerCount)
{
    HANDLE port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    if (port == 0)
        return 0;

    SYSTEM_INFO info = { 0 };
    GetSystemInfo(&info);

    DWORD processors = info.dwNumberOfProcessors;
    if (processors == 0)
        processors = 1;

    *workerCount = processors * 2;
    return port;
}

}  // namespace OsNet

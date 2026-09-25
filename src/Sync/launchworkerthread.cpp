//============================================================================
//  0x6F6D8540 - see evtsched.h.
//============================================================================
#include "evtsched.h"
#include <process.h>

BOOL __fastcall LaunchWorkerThread(void* startAddress, void* argList,
                                    HANDLE* outHandle, void* unused)
{
    (void)unused;   // pushed by the one caller in this tree, never read here
    unsigned int threadId;
    uintptr_t handle = _beginthreadex(
        0, 0, (unsigned int(__stdcall*)(void*))startAddress,
        argList, 0, &threadId);
    *outHandle = (HANDLE)handle;
    return handle != 0;
}

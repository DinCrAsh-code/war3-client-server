//============================================================================
//  0x6F00B240 - GameMainInitSequence's (Game/gamemaininitsequence.cpp) way
//  of telling an already-running instance it should notice this one: open
//  the well-known "WARCRAFT_III_OK" event by name and, if it exists,
//  signal it then close the handle.  No return value is read by either
//  caller in this tree.
//============================================================================
#include <windows.h>

void __fastcall SignalRunningInstance()
{
    HANDLE hEvent = OpenEventA(EVENT_MODIFY_STATE, TRUE, "WARCRAFT_III_OK");
    if (hEvent)
    {
        SetEvent(hEvent);
        CloseHandle(hEvent);
    }
}

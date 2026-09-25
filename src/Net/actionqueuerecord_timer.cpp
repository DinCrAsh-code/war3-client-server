//============================================================================
//  0x6F6644B0 - set a deadline field (this+0x54) to GetTickCount()+delayMs,
//  clamped away from the two sentinel values (0 and 0xFFFFFFFF) up to 1.
//============================================================================
#include <windows.h>

void __fastcall SetTimerDeadline(void* self, int delayMs)
{
    unsigned int deadline = GetTickCount() + (unsigned int)delayMs;
    if (deadline == 0xFFFFFFFF)
        deadline = 1;
    else if (deadline == 0)
        deadline = 1;

    *(unsigned int*)((char*)self + 0x54) = deadline;
}

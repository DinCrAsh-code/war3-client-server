//============================================================================
//  0x6F6DA080 - a bare InterlockedIncrement, no null test.
//============================================================================
#include "os.h"

long __fastcall OsInterlockedIncrement(long* addend)
{
    return InterlockedIncrement(addend);
}

//============================================================================
//  0x6F6D8980 - a `jmp Event::Close` tail-forward, its own translation unit
//  so the tail call is not inlined into a full copy of Close's body.
//============================================================================
#include "event.h"

void Event::CloseForward()
{
    Close();
}

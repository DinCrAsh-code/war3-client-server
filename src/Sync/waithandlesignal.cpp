//============================================================================
//  0x6F6DAD30 - see waithandlesignal.h.
//============================================================================
#include "waithandlesignal.h"

//  Storm.dll ordinal 465 - same declaration shape as tls.cpp/refcnt.cpp's.
void __stdcall SErrSetLastError(unsigned int code);

void __fastcall SignalWaitHandle(IWaitHandleSignal* self)
{
    if (self == 0)
    {
        SErrSetLastError(0x57);
        return;
    }
    self->Signal();
}

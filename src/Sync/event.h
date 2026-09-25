//============================================================================
//  See event.cpp for the class-level notes.
//============================================================================
#ifndef EVENT_H
#define EVENT_H

#include <windows.h>

class Event
{
public:
    Event(BOOL bManualReset, BOOL bInitialState);   // 0x6F6D89B0
    ~Event();                                       // 0x6F6D8450 (via 0x6F6D8980/0x6F62D3D0)

    void  Reset();          // 0x6F6D8530
    void  Set();            // 0x6F6D8520
    DWORD Wait(DWORD dwMilliseconds);   // 0x6F6D8470
    void  Close();          // 0x6F6D8450

    //  0x6F6D8410 (`unknown_libname_949`) - its own translation unit
    //  (eventzero.cpp) so the constructor's call to it survives instead of
    //  being inlined away.
    void  ZeroHandle();

    //  0x6F6D8980 - a one-instruction `jmp Close` thunk (eventcloseforward.cpp,
    //  its own TU for the same inlining reason).
    void  CloseForward();

    HANDLE m_handle;   // +0
};

//  0x6F62D3D0 - `jmp` to Event::CloseForward, reached only from the
//  __finally trampoline of a function with a local Event (sub_6F661CD0's
//  stop path). Its own free function/TU (eventcloseforward2.cpp): nothing
//  else calls it, and it is not itself an Event member in the dump (IDA
//  never shows a `this` load before the jmp - it is reached with whatever
//  `ecx` the unwind already set up), so it is modelled as a plain forwarder
//  taking the object explicitly rather than a second member function.
void __fastcall Event_CloseForward2(Event* self);

#endif

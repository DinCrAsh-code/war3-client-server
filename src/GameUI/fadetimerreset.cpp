//============================================================================
//  0x6F30C510 - CFadeTimer::ResetToIdle: put the timer back to its
//  just-constructed idle state without touching the vtable, the step or
//  the two reserved tail fields CFadeTimer::CFadeTimer (fadetimer.cpp) also
//  sets - a partial re-init, not a full (re)construction.
//
//  CItem vtable-closure batch 2.  Own translation unit, separate from both
//  fadetimer.cpp (the constructor's own module) and cameracursormode.cpp's
//  own SetCursorMode/StartCursorModeTransition pair - a real out-of-line
//  call from 0x6F310690.
//============================================================================
#include "fadetimer.h"

class CFadeTimer : public CObserver
{
public:
    void ResetToIdle();

    CEventTimer  m_channel;     // +0x0C
    float        m_from;        // +0x28
    float        m_to;          // +0x2C
    float        m_step;        // +0x30
    int          m_state;       // +0x34
    int          m_field38;     // +0x38
    int          m_field3C;     // +0x3C
    int          m_field40;     // +0x40
};

//  0x6F62BAA0 - `retn 0` (verified: `pop esi / retn`) - a CEventTimer
//  method reached here and from CFadeTimer::~CFadeTimer (0x6F314750); not
//  the constructor (0x6F62BBD0, a separate address) and not in this batch.
//  A naked ADDR_THUNK-shaped redirect, not a bodyless extern: nothing on
//  the real link line defines a bare `sub_<addr>` symbol either.
__declspec(naked) void __fastcall ResetEventTimerChannel(CEventTimer*)
{
    __asm { mov eax, 0x6F62BAA0 }
    __asm { jmp eax }
}

void CFadeTimer::ResetToIdle()
{
    ResetEventTimerChannel(&m_channel);

    m_to = 0.0f;
    m_state = 2;
    m_from = 0.0f;
    m_field38 = 0;
}

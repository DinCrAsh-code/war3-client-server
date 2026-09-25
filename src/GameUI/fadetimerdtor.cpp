//============================================================================
//  0x6F314750 - CFadeTimer::~CFadeTimer.  `retn 0`.
//
//  Stamps the vftable, calls CFadeTimer::Method (0x6F310690, StopFading -
//  see fadetimerstop.cpp) with `false`, tears down the +0x0C CEventTimer
//  sub-object (0x6F62BBF0, out of this batch), then the CObserver base
//  (0x6F62A9D0, already reconstructed - observer.cpp).
//
//  Same unreproducible __except_handler4-shaped SEH frame as every other
//  destructor in this family (docs/msvc-vc8-idioms.md) - the +0x0C
//  sub-object's own destructor is what needs the unwind.  DIFFERS for that
//  reason; the body past the frame furniture matches instruction for
//  instruction.
//
//  Own translation unit, separate from fadetimer.cpp's own constructor
//  module: StopFading is a real out-of-line call.
//============================================================================
#include "fadetimer.h"
#include "observer.h"

class CFadeTimer : public CObserver
{
public:
    ~CFadeTimer();
    void StopFading(int recordLast);   // 0x6F310690, fadetimerstop.cpp

    CEventTimer  m_channel;     // +0x0C
    float        m_from;        // +0x28
    float        m_to;          // +0x2C
    float        m_step;        // +0x30
    int          m_state;       // +0x34
    int          m_field38;     // +0x38
    int          m_field3C;     // +0x3C
    int          m_field40;     // +0x40
};

//  0x6F62BBF0 - `retn 0` (verified: its own SEH-framed body ends
//  `... / pop esi / add esp,10h / retn`) - CEventTimer's own destructor;
//  reached here and from every other CEventTimer owner in this family, not
//  in this batch.  A naked ADDR_THUNK-shaped redirect, not a bodyless
//  extern: nothing on the real link line defines a bare `sub_<addr>`
//  symbol either.
__declspec(naked) void __fastcall DestroyEventTimerChannel(CEventTimer*)
{
    __asm { mov eax, 0x6F62BBF0 }
    __asm { jmp eax }
}

CFadeTimer::~CFadeTimer()
{
    *(const void**)this = &g_vftCFadeTimer;

    StopFading(0);
    DestroyEventTimerChannel(&m_channel);

    ((CObserver*)this)->~CObserver();
}

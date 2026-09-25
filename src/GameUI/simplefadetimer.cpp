//============================================================================
//  0x6F30C140 - CSimpleFadeTimer::CSimpleFadeTimer.
//
//  The class name and its *two* vtables are shipped facts: the constructor
//  stamps `??_7CSimpleFadeTimer@@6B@` (0x6F9380C4) at +0x00 and
//  `??_7CSimpleFadeTimer@@6B@_0` (0x6F9380A4) at +0x124.  Two vtable
//  pointers in one object is multiple inheritance, and the second one lands
//  exactly on the CObserver sub-object the shipped code constructs there -
//  so the class is "whatever the 0x124-byte base at +0x00 is, plus a
//  CObserver".
//
//  Both vtables are modelled as data symbols rather than as C++ base
//  classes with `virtual`s, for the reason floatboundarylistener.cpp gives:
//  declaring the slots would make this build emit its own short tables and
//  fail to link every body in them.  The CObserver member is real, though -
//  its constructor is a genuine call in the shipped stream.
//
//  Its two arguments are *bytes*: they land at +0x164 and +0x165 through
//  `al` and `cl`.
//
//  Same unreproducible __except_handler4-shaped frame as its sibling at
//  0x6F30C490 - two sub-objects with destructors have to be unwound if the
//  third one's constructor raises, and the shipped funclets say exactly
//  that (`jmp 0x6F609DD0` for the base, `add ecx,124h` / `jmp 0x6F62A9D0`
//  for the CObserver).
//
//  Own translation unit: three real calls, and CFadeTimer must not fold
//  into it.
//============================================================================
#include "observer.h"
#include "fadetimer.h"

CSimpleFadeTimer::CSimpleFadeTimer(unsigned char kind, unsigned char flags)
    : m_base(0)
{
    *(const void**)this = &g_vftCSimpleFadeTimer;
    *(const void**)&m_observer = &g_vftCSimpleFadeTimerObserver;

    m_from  = 0.0f;
    m_to    = 0.0f;
    m_state = 2;
    m_phase = 2;
    m_step  = g_fadeTimerStep;
    m_kind  = kind;
    m_field160 = 0;
    m_flags = flags;
    m_field168 = 0;
    m_field16C = 0;
}

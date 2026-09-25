//============================================================================
//  The two fade timers at 0x6F30C140 and 0x6F30C490 and the two sub-objects
//  they share.  Both class names are the shipped ones - each constructor
//  stamps its own `??_7...@@6B@` - and both vtables are modelled as data
//  symbols; see targetmode.cpp / floatboundarylistener.cpp for why.
//============================================================================
#ifndef FADETIMER_H
#define FADETIMER_H

#include "observer.h"
#include "frame.h"

extern void* const g_vftCFadeTimer;                  // ??_7CFadeTimer@@6B@
extern void* const g_vftCSimpleFadeTimer;            // ??_7CSimpleFadeTimer@@6B@
extern void* const g_vftCSimpleFadeTimerObserver;    // ...@@6B@_0

//  flt_6F92FE40 - one hundredth, the step both timers start with.
extern const float g_fadeTimerStep;

//----------------------------------------------------------------------------
//  The two sub-objects both timers own were reached from this side as
//  0x6F62BBD0 and 0x6F609C70 and modelled as opaque buffers with an Init.
//  The frame-library batch (src/Frame/) reconstructed both addresses, and
//  each stamps a vtable that names it: 0x6F62BBD0 is CEventTimer's
//  constructor and 0x6F609C70 is CSimpleFrame's.  They are spelled as those
//  classes here so that one shipped address keeps one C++ name; the layout is
//  unchanged - CSimpleFrame is 0x124 bytes and CEventTimer 0x1C, which is
//  exactly the two gaps this file already had.
//----------------------------------------------------------------------------

class CSimpleFadeTimer
{
public:
    CSimpleFadeTimer(unsigned char kind, unsigned char flags);

    CSimpleFrame   m_base;        // +0x000
    CObserver      m_observer;    // +0x124
    CEventTimer    m_channel;     // +0x130
    float          m_from;        // +0x14C
    float          m_to;          // +0x150
    float          m_step;        // +0x154 - 0.01
    int            m_state;       // +0x158 - 2
    int            m_phase;       // +0x15C - 2
    int            m_field160;    // +0x160
    unsigned char  m_kind;        // +0x164 - the first argument
    unsigned char  m_flags;       // +0x165 - the second
    char           m_reserved166[2];
    int            m_field168;    // +0x168
    int            m_field16C;    // +0x16C
};

#endif

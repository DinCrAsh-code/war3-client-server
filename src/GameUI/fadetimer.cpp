//============================================================================
//  0x6F30C490 - CFadeTimer::CFadeTimer.
//
//  The class name is the shipped one: `??_7CFadeTimer@@6B@` (0x6F938134).
//  A CObserver base, a sub-object at +0x0C, and then eight fields: two
//  zeroed floats, a 2, a 0.01 and four zeroed words.  0.01 is
//  flt_6F92FE40, which the SimpleFadeTimer next door writes into the same
//  relative slot.
//
//  Same unreproducible __except_handler4-shaped frame as every other
//  constructor in this family - the +0x0C sub-object's own constructor is
//  what needs the unwind.
//
//  Own translation unit: two real calls out of it, and its sibling at
//  0x6F30C140 must not fold into it.
//============================================================================
#include "fadetimer.h"

class CFadeTimer : public CObserver
{
public:
    CFadeTimer();

    CEventTimer  m_channel;     // +0x0C
    float        m_from;        // +0x28
    float        m_to;          // +0x2C
    float        m_step;        // +0x30 - 0.01
    int          m_state;       // +0x34 - 2
    int          m_field38;     // +0x38
    int          m_field3C;     // +0x3C
    int          m_field40;     // +0x40
};

CFadeTimer::CFadeTimer()
{
    *(const void**)this = &g_vftCFadeTimer;

    m_from = 0.0f;
    m_state = 2;
    m_to = 0.0f;
    m_field38 = 0;
    m_step = g_fadeTimerStep;
    m_field3C = 0;
    m_field40 = 0;
}

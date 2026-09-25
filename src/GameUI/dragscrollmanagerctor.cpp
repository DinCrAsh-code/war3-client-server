//============================================================================
//  0x6F3781D0 - CDragScrollManager::CDragScrollManager.
//
//  The class name is the shipped one: the body stamps
//  ??_7CDragScrollManager@@6B@ over the CObserver base's vtable pointer.
//
//  Thirty consecutive floats and one word, all zeroed and none of them read
//  anywhere in this call tree - so they are one named array rather than
//  thirty invented field names.  The shipped stream treats them as one run
//  too: a single `fldz` and thirty `fst`s down consecutive addresses, with
//  the last one an `fstp` that pops the register.  Written out rather than
//  looped because a loop compiles to a counted store loop, not to the run.
//
//  The `mov eax, esi` that materialises the constructor's `this` return sits
//  in the middle of that run in the shipped stream; it is the scheduler
//  moving it, not anything the source can say.
//============================================================================
#include "observer.h"

class CDragScrollManager : public CObserver
{
public:
    CDragScrollManager();

    float m_state[30];      // +0x0C .. +0x80
    int   m_field84;        // +0x84
};

CDragScrollManager::CDragScrollManager()
{
    m_state[0] = 0.0f;
    m_state[1] = 0.0f;
    m_state[2] = 0.0f;
    m_state[3] = 0.0f;
    m_state[4] = 0.0f;
    m_state[5] = 0.0f;
    m_state[6] = 0.0f;
    m_state[7] = 0.0f;
    m_state[8] = 0.0f;
    m_state[9] = 0.0f;
    m_state[10] = 0.0f;
    m_state[11] = 0.0f;
    m_state[12] = 0.0f;
    m_state[13] = 0.0f;
    m_state[14] = 0.0f;
    m_state[15] = 0.0f;
    m_state[16] = 0.0f;
    m_state[17] = 0.0f;
    m_state[18] = 0.0f;
    m_state[19] = 0.0f;
    m_state[20] = 0.0f;
    m_state[21] = 0.0f;
    m_state[22] = 0.0f;
    m_state[23] = 0.0f;
    m_state[24] = 0.0f;
    m_state[25] = 0.0f;
    m_state[26] = 0.0f;
    m_state[27] = 0.0f;
    m_state[28] = 0.0f;
    m_state[29] = 0.0f;
    m_field84 = 0;
}

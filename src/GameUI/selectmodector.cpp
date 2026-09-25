//============================================================================
//  0x6F37A200 - CSelectMode::CSelectMode.
//
//  The class name is the shipped one: the body stamps
//  ??_7CSelectMode@@6B@ over the CObserver base's vtable pointer.
//
//  Four things of its own: a word left at -1, a zeroed float, and two
//  embedded sub-objects that are constructed by real calls rather than
//  inline.  Both sub-object constructors are spelled as ordinary `Construct`
//  members and called from the body rather than as C++ member constructors,
//  for the reason GameUI/gameui.h gives about the chat bar's: neither is
//  reconstructed, both are still redirects, and MSVC will not take
//  __declspec(naked) on a constructor.  Calling them from the body also puts
//  them after the two stores, which is the shipped order.
//
//  Score capped by the __except_handler4 frame the shipped build puts round
//  a constructor with sub-objects to unwind; this repo's fixed /GS- /EHs-c-
//  emits none of it (docs/msvc-vc8-idioms.md).
//============================================================================
#include "observer.h"

//  +0x14 - one word, zeroed by its constructor before that constructor hands
//  its own argument on to 0x6F2C86C0.  CGameUI embeds one of these at its
//  +0x3B8 as well.  Nothing in this tree reads it afterwards, so the name
//  says where it is used and not what it holds.
struct SSelectModeRequestSlot
{
    //  0x6F2C9670 - `retn 4`.  Still a redirect.
    void Construct(int arg);
    void* m_field00;        // +0x00
};

//  +0x18 - six instructions, `retn 0`, and nothing in this tree reads the
//  result either.
struct SSelectModeEventSlot
{
    //  0x6F62BBD0 - `retn 0`.  Still a redirect.
    void Construct();
    void* m_field00;        // +0x00
};

class CSelectMode : public CObserver
{
public:
    CSelectMode();

    int                     m_field0C;      // +0x0C
    float                   m_float10;      // +0x10
    SSelectModeRequestSlot  m_request;      // +0x14
    SSelectModeEventSlot    m_events;       // +0x18
};

CSelectMode::CSelectMode()
{
    m_float10 = 0.0f;
    m_field0C = -1;
    m_request.Construct(0);
    m_events.Construct();
}

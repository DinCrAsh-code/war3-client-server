//============================================================================
//  0x6F4A91B0 - SHandleObjectRangeImpl::AnnounceRateChange, the tail of
//  rangeadjustrate.cpp's own AdjustRate.
//
//  A third sibling in the '^pro'-tagged "build an eleven-dword event
//  block, hand it to a named listener through vtable slot 8 (+0x20), or
//  broadcast it through SPublishedFieldOwner::BroadcastFieldChange when
//  the caller supplied none" family - rangenotifychanged.cpp's own
//  NotifyRangeChanged and Misc/handlefieldnotify.cpp's own
//  NotifyField78Changed already establish the shape; only the identity's
//  own tag2 and the payload are its own here (one word, not two - the
//  new rate, with the second payload slot left zero the way
//  NotifyField78Changed's own is).
//
//  Own translation unit: AdjustRate calls it for real.
//============================================================================
#include "handleobjectrange.h"

//----------------------------------------------------------------------------
//  The listener, when the caller names one: the block goes to vtable
//  slot 8 (+0x20) as its single argument - same minimal-shape convention
//  rangenotifychanged.cpp's own CRangeEventListener uses.
//----------------------------------------------------------------------------
class CRateEventListener
{
public:
    virtual void Slot00();
    virtual void Slot04();
    virtual void Slot08();
    virtual void Slot0C();
    virtual void Slot10();
    virtual void Slot14();
    virtual void Slot18();
    virtual void Slot1C();
    virtual void OnRateEvent(void* block);     // +0x20
};

//  0x6F497510 - publishedfieldbroadcast.cpp.  Re-declared here on the same
//  class the definition is on, with the same signature, so the two mangle
//  identically - same reasoning rangenotifychanged.cpp's own copy gives.
struct SPublishedFieldOwner
{
    void BroadcastFieldChange(void* block);
};

//  MSB first: '^pro', '`rel', '`rle' - one nibble off rangenotifychanged.cpp's
//  own '`rld' (tag2), which is what makes this a distinct message in the
//  same family rather than the same one misfiled.
const unsigned int kRateEventTag0 = 0x5E70726F;
const unsigned int kRateEventTag1 = 0x6072656C;
const unsigned int kRateEventTag2 = 0x60726C65;

struct SRateEvent
{
    unsigned int m_tag0;        // 0x00
    unsigned int m_tag1;        // 0x04
    unsigned int m_tag2;        // 0x08
    void*        m_subject;     // 0x0C
    int          m_value;       // 0x10
    int          m_reserved14;  // 0x14
    int          m_reserved18;  // 0x18
    int          m_reserved1C;  // 0x1C
    int          m_reserved20;  // 0x20
    int          m_reserved24;  // 0x24
    int          m_reserved28;  // 0x28
};

void SHandleObjectRangeImpl::AnnounceRateChange(const CFloat* value, void* listener)
{
    if ((m_suppressNotify & 1) == 0)
    {
        SRateEvent event;
        event.m_value = *(const int*)value;
        event.m_reserved28 = -1;
        event.m_reserved24 = -1;
        event.m_tag0 = kRateEventTag0;
        event.m_tag1 = kRateEventTag1;
        event.m_tag2 = kRateEventTag2;
        event.m_subject = this;
        event.m_reserved14 = 0;
        event.m_reserved18 = 0;
        event.m_reserved1C = 0;
        event.m_reserved20 = 0;

        if (listener)
            ((CRateEventListener*)listener)->OnRateEvent(&event);
        else
            ((SPublishedFieldOwner*)this)->BroadcastFieldChange(&event);
    }
}

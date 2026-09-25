//============================================================================
//  0x6F4A9230 - SHandleObjectRangeImpl::NotifyRangeChanged, the tail of
//  SHandleObjectRange::SetRange (handleobjectrange.cpp).  A naked redirect
//  there until now.
//
//  It is the *same function*, one field apart, as
//  SPublishedFieldOwner::NotifyField78Changed (handlefieldnotify.cpp):
//  build an eleven-dword event block on the stack, hand it to a named
//  listener through vtable slot 8 when the caller supplied one, and
//  broadcast it through 0x6F497510 when the caller did not.  Only the
//  identity words and the payload differ, so this file re-uses that one's
//  reading of the shape rather than re-deriving it.
//
//  Three differences worth stating:
//
//  * **the guard is a flag, not a comparison.**  Bit 0 of the byte at
//    +0x4F suppresses the notification outright; NotifyField78Changed's
//    equivalent is "did the value actually change".  It really is a byte
//    test (`test byte ptr [ecx+4Fh], 1`), so the field is `unsigned char`.
//  * **the payload is two words, not one** - the new value at +0x10 and
//    the previous one at +0x14, both dereferenced from the caller's
//    pointers.  NotifyField78Changed's +0x14 is one of its zeros.
//  * the identity triple is `^pro` / `` `rel `` / `` `rld ``, where the
//    field event's is `^pro` / `` `fflg `` / `` `fflc ``.  Same first word,
//    which is what makes them one message family.
//
//  Own translation unit: SetRange calls it for real.
//============================================================================
#include "handleobjectrange.h"

//----------------------------------------------------------------------------
//  The listener, when the caller names one: the block goes to vtable slot 8
//  (+0x20) as its single argument.  Only that slot is declared and the
//  eight ahead of it are held open so its index is right - the same
//  minimal shape handlefieldnotify.cpp uses, and safe for the same reason:
//  nothing here defines this class, so no vtable of it is emitted and
//  check_vtables.py has nothing to disagree with.
//----------------------------------------------------------------------------
class CRangeEventListener
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
    virtual void OnRangeEvent(void* block);     // +0x20
};

//  0x6F497510 - publishedfieldbroadcast.cpp.  Re-declared here on the same
//  class the definition is on, with the same signature, so the two mangle
//  identically; a fresh local name would be a second symbol and an
//  LNK2019 nothing but the gate could see.
struct SPublishedFieldOwner
{
    void BroadcastFieldChange(void* block);
};

//  MSB first: '^pro', '`rel', '`rld'.
const unsigned int kRangeEventTag0 = 0x5E70726F;
const unsigned int kRangeEventTag1 = 0x6072656C;
const unsigned int kRangeEventTag2 = 0x60726C64;

struct SRangeEvent
{
    unsigned int m_tag0;        // 0x00
    unsigned int m_tag1;        // 0x04
    unsigned int m_tag2;        // 0x08
    void*        m_subject;     // 0x0C
    int          m_value;       // 0x10
    int          m_previous;    // 0x14
    int          m_reserved18;  // 0x18
    int          m_reserved1C;  // 0x1C
    int          m_reserved20;  // 0x20
    int          m_reserved24;  // 0x24
    int          m_reserved28;  // 0x28
};

void SHandleObjectRangeImpl::NotifyRangeChanged(const CFloat* value,
                                                const CFloat* previous,
                                                void* listener)
{
    //  The only exit, shared by both arms: one `add esp, 2Ch` / `retn 0Ch`,
    //  which is what a plain `if` around the whole body gives and what an
    //  early `return` does not.
    if ((m_suppressNotify & 1) == 0)
    {
        SRangeEvent event;
        event.m_value = *(const int*)value;
        event.m_previous = *(const int*)previous;
        event.m_reserved28 = -1;
        event.m_reserved24 = -1;
        event.m_tag0 = kRangeEventTag0;
        event.m_tag1 = kRangeEventTag1;
        event.m_tag2 = kRangeEventTag2;
        event.m_subject = this;
        event.m_reserved18 = 0;
        event.m_reserved1C = 0;
        event.m_reserved20 = 0;

        if (listener)
            ((CRangeEventListener*)listener)->OnRangeEvent(&event);
        else
            ((SPublishedFieldOwner*)this)->BroadcastFieldChange(&event);
    }
}

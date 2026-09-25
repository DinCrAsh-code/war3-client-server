//============================================================================
//  0x6F497510 - SPublishedFieldOwner::BroadcastFieldChange: hand one event
//  block to every registered observer.
//
//  Both no-listener arms in this repo end here -
//  SPublishedFieldOwner::NotifyField78Changed (handlefieldnotify.cpp) and
//  SHandleObjectRangeImpl::NotifyRangeChanged (rangenotifychanged.cpp) -
//  which is why it is its own translation unit: each of them calls it for
//  real, so it may not be folded into either.  It was a naked redirect in
//  handlefieldnotify.cpp, where being naked was the only thing keeping the
//  compiler from inlining it.
//
//  **A `__thiscall` member, and getting that wrong was a live SIGSEGV** -
//  the whole argument is in handlefieldnotify.cpp's own note and is not
//  repeated here.  The body is what settles it: the list head comes off
//  `this+0x64`.
//
//  The walk is a `while` and not a `do`/`while`, even though the shipped
//  code has the test-and-skip ahead of a bottom-tested loop: that shape is
//  MSVC's own loop rotation, and writing it as a `do` in the source needs
//  a null check of its own that the rotation then cannot fold away.
//============================================================================

//----------------------------------------------------------------------------
//  One observer registration.  Two of its words are touched: the next link
//  at +0x04 and the object to dispatch to at +0x08.
//----------------------------------------------------------------------------
struct SFieldObserverNode
{
    char                m_reserved00[4];
    SFieldObserverNode* m_pNext;      // 0x04
    class CFieldEventTarget* m_pTarget;   // 0x08
};

//  The dispatch is vtable slot 8 (+0x20), one argument.  Declared with the
//  eight slots ahead of it held open so the index is right; nothing here
//  defines the class, so no vtable is emitted.
class CFieldEventTarget
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
    virtual void OnFieldEvent(void* block);     // +0x20
};

//  The same minimal declaration handlefieldnotify.cpp and
//  rangenotifychanged.cpp call through, plus the one field this body
//  reads.  The class name is what MSVC mangles, so all three agree.
struct SPublishedFieldOwner
{
    void BroadcastFieldChange(void* block);

    char                m_reserved00[0x64];
    SFieldObserverNode* m_pObservers;    // 0x64
};

void SPublishedFieldOwner::BroadcastFieldChange(void* block)
{
    SFieldObserverNode* node = m_pObservers;
    while (node != 0)
    {
        node->m_pTarget->OnFieldEvent(block);
        node = node->m_pNext;
    }
}

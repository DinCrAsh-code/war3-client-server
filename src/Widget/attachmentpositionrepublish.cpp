//============================================================================
//  0x6F0C6840 - reached from CUnit::Reposition's own slot-96 dispatch tree
//  (slot 96 / +0x180, 0x6F2A5D50 - one of the giant slot roots
//  docs/targets/CUnit__vtable.md gives up on as a whole).  This one leaf is
//  small and self-contained, so it is worth reconstructing on its own even
//  though the mega-function that calls it is not attempted.
//
//  `this` is not CUnit - it carries a cached owner pointer at +0x30, empty
//  on the first call and resolved once through
//  `SAttachmentOwnerQuery::QueryOwner` (0x6F472890, already real,
//  attachmentownerquery.cpp) exactly the way `SAttachmentTarget::Show`'s
//  own +0x30 cache does (attachmentcounters.cpp).  Whatever comes back is
//  a `CWidget`-family object: its own vtable slot 46 (+0xB8) is
//  `GetHandleObject()`, which both CWidget's and CUnit's own bodies at
//  that slot already establish, and `CPathRef::SmartPtrToWorldVec3`
//  (0x6F4743A0, already real, CPathRef.cpp) turns the handle it returns
//  into a world position - the exact
//  `((CPathRef*)GetHandleObject())->SmartPtrToWorldVec3()` shape
//  unit_clearpendingnotify.cpp's own CUnit method already uses on itself.
//
//  That position is then pushed out through two more embedded
//  vtable-having sub-objects at +0x2A8 and +0x2B0, each through slot 0 of
//  its own vtable with the same fixed `(1, &position)` argument pair -
//  neither sub-object's real type is established, so each is reached the
//  hand-written-dispatch way CLAUDE.md's own examples use, not through a
//  named virtual.
//
//  Own translation unit: no callee here shares a module with anything
//  else in src/ yet (0x6F0C6xxx has no other reconstructed neighbour).
//============================================================================
#include "game.h"
#include "widget.h"

//  Same class, same method, same address as attachmentcounters.cpp's own
//  declaration - a second textually-identical declaration of one already-
//  real function, so this call site reaches it with a real `call` instead
//  of inventing a second name for 0x6F472890.
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();
};

//  Slot 0 on whatever vtable-having object sits at +0x2A8/+0x2B0 below -
//  read straight off the call site (`this`, then the fixed `1` and
//  `&position` arguments), the same shape CLAUDE.md's own
//  `ApplyOffsetFn` example documents.  Neither sub-object's concrete type
//  is known.
typedef void (__thiscall *RepublishPositionFn)(void*, int, const CWorldVec3*);

struct SPositionRepublishTarget
{
    void RepublishOwnerPosition();

    char   m_reserved00[0x30];
    void*  m_pOwner;                   // +0x30 - resolved via QueryOwner if null
    char   m_reserved34[0x2A8 - 0x34];
    void*  m_sinkA;                    // +0x2A8 - vtable-having, slot 0 republishes position
    char   m_reserved2AC[0x2B0 - 0x2AC];
    void*  m_sinkB;                    // +0x2B0 - same shape
};

void SPositionRepublishTarget::RepublishOwnerPosition()
{
    CWidget* owner = (CWidget*)m_pOwner;
    if (!owner)
        owner = (CWidget*)((SAttachmentOwnerQuery*)this)->QueryOwner();

    CWorldVec3 pos = ((CPathRef*)owner->GetHandleObject())->SmartPtrToWorldVec3();

    ((RepublishPositionFn)(*(void***)&m_sinkA)[0])(&m_sinkA, 1, &pos);
    ((RepublishPositionFn)(*(void***)&m_sinkB)[0])(&m_sinkB, 1, &pos);
}

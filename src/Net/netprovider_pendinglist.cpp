//============================================================================
//  sub_6F65BC90 (0x6F65BC90) and sub_6F65C490 (0x6F65C490) - a
//  TSExplicitList<T>::UnlinkAll() instantiation (storm.h/tslist.inl, already
//  established by the earlier AGILE_TYPE_RELATIONSHIP target) and the
//  destructor of a local object that embeds one such list at offset +0xE8.
//
//  sub_6F65BC90's own loop uses the exact same TSLink<T>::Unlink() body as
//  the already-established TSExplicitList<T,LINKOFFSET> instantiations
//  (tslist.inl) - same `not`/`jg`/`sub` predecessor-pointer sign-recovery,
//  same final `m_next->m_prevlink = m_prevlink` and zeroing - but reads
//  `[this+8]` and uses it *directly* as the link address, with no
//  `m_linkoffset +` addition first the way `Link(tail)` would do. That
//  rules out TSExplicitList<T,LINKOFFSET> itself (its Link() always adds a
//  runtime m_linkoffset field, which this function never loads at all) -
//  this is the simpler sibling where a node's TSLink lives at the node's
//  own address (offset 0), so the tail field doubles as the link pointer
//  with nothing to add. The element type is not recoverable from this call
//  tree - TSLink<T>::Unlink() never touches T, only pointer arithmetic - so
//  it is left as an incomplete tag type rather than guessed at.
//
//  sub_6F65C490 is reached from NetProvider__Method_0x90's own SEH
//  __finally/fallthrough cleanup with `ecx` pointing at a stack-local
//  object, and does exactly `UnlinkAll(); m_terminator.Unlink();` on the
//  list embedded at that object's own +0xE8 - i.e. it is the compiler-
//  generated destructor for a local type whose only non-trivial member is
//  this list. Nothing before +0xE8 is ever touched by this function, so the
//  padding ahead of the list is left unnamed (its true layout belongs to
//  whatever local variable NetProvider__Method_0x90 itself declares, not to
//  this destructor).
//============================================================================
#include "storm.h"

struct UnknownNetQueueItem_6F65BC90;   // never dereferenced - see above

typedef TSLink<UnknownNetQueueItem_6F65BC90> DirectLink;

//  TSExplicitList-shaped (storm.h), but with an implicit LINKOFFSET of 0
//  baked in rather than a runtime m_linkoffset field: a node's own TSLink
//  lives at the node's own address, so the terminator's m_prevlink already
//  *is* the tail link's address, with nothing to add - see the header
//  comment above for how the disassembly rules out the general template.
struct DirectLinkedList
{
    void*        m_reserved0;   // +0
    DirectLink   m_terminator;  // +4 (m_next @ +4, m_prevlink @ +8)

    void UnlinkAll();
    ~DirectLinkedList() { UnlinkAll(); m_terminator.Unlink(); }
};

void DirectLinkedList::UnlinkAll()
{
    while (m_terminator.m_prevlink > 0)
        ((DirectLink*)m_terminator.m_prevlink)->Unlink();
}

struct NetProviderLocalWithPendingList
{
    char              pad_000[0xE8];
    DirectLinkedList  m_pendingList;   // +0xE8

    ~NetProviderLocalWithPendingList();
};

NetProviderLocalWithPendingList::~NetProviderLocalWithPendingList()
{
}

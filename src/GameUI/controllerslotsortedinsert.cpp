//============================================================================
//  The controller-slot / managed-property sorted-insertion helper cluster.
//
//  Every CDataMgr-derived object (CCamera, and controllerslot.h's own
//  controller/event-sink/fog/light hosts) embeds a
//  TSExplicitList<CBaseManaged,4> at +0x14 (Widget/cdatamgrbasemanagedlist.cpp
//  already reconstructs this same class's UnlinkAll, 0x6F4D57D0) that keeps
//  its members sorted ascending by a float key at +0x18 - the same offset
//  controllerslot.h's own SControllerSlot names m_field18 and
//  Misc/cameraangle.cpp's CAngle carries as m_rawValue.  Two real call
//  sites reach this insertion (both already-reconstructed naked thunks, so
//  neither is re-derived here): RegisterControllerSlotBinding (0x6F4E85F0,
//  GameUI/controllerslot_thunks.cpp) inserting a freshly-bound slot, and
//  CDataMgr::RegisterManagedProperty (0x6F4E8740, same file) inserting a
//  freshly-registered property when its own caller asked for it.  Both hand
//  this the host unchanged in ecx and the new member on the stack, which is
//  why LinkSorted below is a plain member of the *host*, not of the list
//  itself.
//
//  0x6F4E84B0 - LinkSorted(node): walk back from the tail while the
//  existing member's key is strictly greater than the new one's (the
//  two-operand `fcompp`/`test ah,41h` shape docs/msvc-vc8-idioms.md's
//  "Floating point" section already decodes - the loop body runs while the
//  *second*-loaded operand, the existing member, is strictly greater), then
//  unlink `node` from wherever it might currently be (always a no-op for a
//  freshly-constructed node - its own ctor already zeroed its link - but
//  the compiler cannot prove that statically, so the shipped code carries
//  the full defensive TSLink<T>::Unlink() body regardless) and splice it in
//  after whatever stopped the walk, or at the head if nothing did.
//
//  0x6F4E8490 - see controllerslotlistprev.cpp.
//============================================================================
#include "controllerslotlist.h"

//  Local stand-in for CBaseManaged - only the float sort key at +0x18 is
//  ever read through this type; the TSLink<CBaseManaged> membership every
//  real member of this list carries at +0x04 is touched only through
//  SLink's own pointer arithmetic above, never through a declared field
//  here (same minimal-stand-in convention Misc/ccamera.cpp and
//  Widget/cdatamgrbasemanagedlist.cpp already use, each tailored to what
//  its own file needs).
class CBaseManaged
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}
    ~CBaseManaged() {}

    char  m_reserved04[0x14];   // +0x04
    float m_sortKey;             // +0x18
};

//  storm.h's own TSLink<T>::Unlink(), copied here against SLink instead of
//  the shared template - see controllerslotlist.h for why this pair is not
//  the shared TSExplicitList<CBaseManaged,4> itself.
static void UnlinkNode(SLink* link)
{
    if (link->m_next)
    {
        SLink** prevlink;
        if (link->m_prevlink <= 0)
        {
            prevlink = (SLink**)~link->m_prevlink;
        }
        else
        {
            int linkoffset = (int)link - link->m_next->m_prevlink;
            prevlink = (SLink**)(link->m_prevlink + linkoffset);
        }
        *prevlink = link->m_next;
        link->m_next->m_prevlink = link->m_prevlink;
        link->m_next = 0;
        link->m_prevlink = 0;
    }
}

//  The CDataMgr-shaped host: only its own list member at +0x14 matters
//  here (see controllerslot.h / Misc/ccamera.cpp for the rest of this
//  family's own layout, which this file does not need).
struct SSortedPropertyListHost
{
    char         m_reserved00[0x14];
    SSortedList  m_list;   // +0x14

    void LinkSorted(CBaseManaged* node);   // 0x6F4E84B0
};

//  0x6F4E84B0.
void SSortedPropertyListHost::LinkSorted(CBaseManaged* node)
{
    int tail = m_list.m_terminator.m_prevlink;
    CBaseManaged* cur = tail > 0 ? (CBaseManaged*)tail : 0;

    while (cur && node->m_sortKey < cur->m_sortKey)
        cur = m_list.Prev(cur);

    if (cur)
    {
        //  Splice in right after `cur` - Link(node) needs no null check on
        //  this arm: the loop above already dereferenced `node` at least
        //  once to get here, so the optimiser can prove it non-null.
        SLink* link = (SLink*)((char*)node + m_list.m_linkoffset);
        UnlinkNode(link);
        SLink* at = (SLink*)((char*)cur + m_list.m_linkoffset);
        SLink* next = at->m_next;
        link->m_next = next;
        link->m_prevlink = next->m_prevlink;
        next->m_prevlink = (int)node;
        at->m_next = link;
    }
    else
    {
        //  The list was empty, or the walk reached the head without ever
        //  touching `node` - this arm's own Link(node) genuinely needs the
        //  null check (Link(0) folds to &m_terminator).
        SLink* link = node ? (SLink*)((char*)node + m_list.m_linkoffset)
                            : &m_list.m_terminator;
        UnlinkNode(link);
        SLink* head = m_list.m_terminator.m_next;
        link->m_next = head;
        link->m_prevlink = head->m_prevlink;
        head->m_prevlink = (int)node;
        m_list.m_terminator.m_next = link;
    }
}

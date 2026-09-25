//============================================================================
//  TSExplicitList<CBaseManaged,4>'s own layout (storm.h), duplicated here as
//  a plain, non-template struct rather than used directly: storm.h's own
//  Prev() is defined inline in-class, and every other real call site
//  already reconstructed in this build needs it to stay that way (see
//  storm.h's own header comment on TSExplicitList).  Reusing the shared
//  template here would make /Ob2 inline this one call site too, where the
//  shipped code leaves a real call (0x6F4E8490) - see
//  controllerslotlistprev.cpp.  A separate, declaration-only type - body in
//  a different translation unit - is what actually forces that.
//============================================================================
#ifndef CONTROLLERSLOTLIST_H
#define CONTROLLERSLOTLIST_H

class CBaseManaged;

//  TSLink<T>'s own two fields, T-independent (Unlink() never touches T -
//  see GameUI/controllerslotsortedinsert.cpp's own copy of its body).
struct SLink
{
    SLink* m_next;
    int    m_prevlink;
};

struct SSortedList
{
    int    m_linkoffset;    // +0x00 (4 for this instantiation)
    SLink  m_terminator;      // +0x04 (m_next @ +0x04, m_prevlink @ +0x08)

    //  0x6F4E8490 - defined in controllerslotlistprev.cpp, the only place
    //  its body is visible.
    CBaseManaged* Prev(CBaseManaged* ptr);
};

#endif

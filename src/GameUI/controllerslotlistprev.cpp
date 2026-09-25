//============================================================================
//  0x6F4E8490 - TSExplicitList<CBaseManaged,4>::Prev, compiled out of line.
//  See controllerslotlist.h for why this is its own type instead of the
//  shared template, and GameUI/controllerslotsortedinsert.cpp (0x6F4E84B0)
//  for the one real call site.
//
//  Byte for byte `Link(ptr)->m_prevlink` with `Link(0)` folded to
//  `&m_terminator` (storm.h's own TSExplicitList<T,LINKOFFSET>::Prev/Link):
//  `ecx` = the list itself (`&host->m_list`, i.e. `&m_linkoffset`), one
//  stack dword = the node to look up.
//============================================================================
#include "controllerslotlist.h"

CBaseManaged* SSortedList::Prev(CBaseManaged* ptr)
{
    SLink* link = ptr ? (SLink*)(m_linkoffset + (int)ptr) : &m_terminator;
    return (CBaseManaged*)link->m_prevlink;
}

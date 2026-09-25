//============================================================================
//  0x6F4D57D0 - TSExplicitList<CBaseManaged, 4>::UnlinkAll.
//
//  Called with `ecx` unchanged from its own caller's `this` (the loop's
//  head test reads `[ecx+8]` directly, matching TailLink()'s own
//  m_linkoffset-then-m_terminator layout: m_linkoffset at +0, m_terminator
//  (a TSLink<T>, m_next/m_prevlink) at +4, so m_terminator.m_prevlink lands
//  at +8) - so this instantiation's list is embedded at the front of
//  whatever owns it, not reached through a pointer.
//
//  Confirmed CBaseManaged as the element and 4 as the link offset by
//  following one of this function's own three call sites
//  (sub_6F4D7150/sub_6F4D5E70, both destructors two module-neighbourhoods
//  over): each stamps `??_7CBaseManaged@@6B@` into `[ecx]`, then unlinks
//  its own `TSLink<CBaseManaged>` at `[ecx+4]` right after calling this -
//  the same "vtable, then TSLink<CBaseManaged> at +4" sub-object
//  Misc/ccamera.cpp's own CBaseManaged stand-in already documents.  Neither
//  destructor is in this batch's own scope.
//
//  A local stand-in for CBaseManaged rather than a shared header: the real
//  class is unreconstructed and lives entirely in Game.dll, and this file
//  needs nothing about it beyond the mangled vtable-stamp name the generic
//  TSLink<T>::Unlink template never actually touches (T only appears in
//  pointer arithmetic here, never dereferenced as CBaseManaged itself).
//============================================================================
class CBaseManaged
{
public:
    virtual void ReleaseSelf() {}
    virtual void OnZeroRefCount() {}
    ~CBaseManaged() {}
};

#include "tslist.inl"

template void TSExplicitList<CBaseManaged, 4>::UnlinkAll();

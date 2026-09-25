//============================================================================
//  CUnitRefList's own two vtable slots (30, 31).  See unitreflist.h for
//  the class and the field evidence.
//
//  Own translation unit: neither calls the other, and both reach leaves
//  that already live in Agent/ and Widget/ - agenttimer.h/agent.h and
//  attachmentownerquery.cpp - rather than anything unitreflist.h itself
//  needs to see.
//============================================================================
#include "unitreflist.h"
#include "itemhandleresolve.h"

//  0x6F0419C0 - SOptionalHandleRefResolver::ResolveChained, already
//  reconstructed (handlereref_resolve.cpp); re-declared here the way
//  unitorder.h/unitordermake.cpp/agentregistrysingleton.cpp all
//  re-declare it, so the mangled name is the same symbol and not a
//  second one.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//  attachmentownerquery.cpp's own SAttachmentOwnerQuery is `static` there
//  (own translation unit); CUnitRefList's own +0x0C/+0x10 pair (CAgent's
//  m_handle/m_typeTag, agent.h) is laid out identically, so slot 31 below
//  reads it back through the same struct rather than re-declaring the
//  fields under a different name.
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();

    char         m_reserved00[0x0C];
    unsigned int m_handle;      // +0x0C
    int          m_typeTag;     // +0x10
};

//----------------------------------------------------------------------------
//  0x6F052170 - slot 30 (+0x78).  Point this agent's tracked-target link
//  at `a`, then resolve `b` through this object's own +0x24/+0x28
//  SOptionalHandleRef pair and hand back a pointer to it - the pair
//  itself, not whatever `b` named, so a lookup miss still leaves the
//  fields at their "not set" sentinel (SOptionalHandleRefResolver::
//  ResolveChained's own documented behaviour, handlereref_resolve.cpp).
//----------------------------------------------------------------------------
void* CUnitRefList::Method_0x78(int a, int b)
{
    SetTrackedTarget((void*)a);
    return &((SOptionalHandleRefResolver*)&m_field24)->ResolveChained(
        (const void*)b);
}

//----------------------------------------------------------------------------
//  0x6F0383A0 - slot 31 (+0x7C).  Resolve this agent's own owner and
//  tail-jump into *its* vtable slot 59 (+0xEC) with `this` set to the
//  owner and no arguments of its own.  The owner's real class is not
//  committed anywhere in this build, so the dispatch is a raw vtable
//  offset rather than a named call - the same "typedef from the call
//  site's own disassembly" shape CLAUDE.md's own vtable_dispatch_audit.py
//  section documents; it is a genuine tail jump in the shipped code
//  (`jmp eax`), reproduced here as an ordinary call since nothing this
//  session's own scope needs the trailing `retn` byte count for (this
//  slot is not itself wired into any hookable dispatch).
//----------------------------------------------------------------------------
typedef void (__thiscall *OwnerVtableSlot59Fn)(void*);

void CUnitRefList::Method_0x7C()
{
    void* owner = ((SAttachmentOwnerQuery*)this)->QueryOwner();
    ((OwnerVtableSlot59Fn)(*(void***)owner)[0xEC / 4])(owner);
}

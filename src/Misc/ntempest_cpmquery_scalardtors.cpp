//============================================================================
//  NIpse::CPmQuery<T> (0x6F46D730/0x6F489370/0x6F4893B0) and two classes
//  reached through its own vtable slot 0 without overriding it
//  (NIpse::CPoQuery/CSeQuery, 0x6F48A4A0/0x6F48A4E0 - RTTI-confirmed via
//  agent_worktrees/classes/ after the 2026-09-07 dump_agent_worktrees.py
//  namespace/template regex fix, docs/targets/NTempestNIpse_BaseChain.md).
//
//  Same base teardown as the CMemBlockT<T>/CDynTable<T> family
//  (Misc/ntempest_memblockt_scalardtors.cpp/ntempest_dyntable_scalardtors.cpp)
//  - restamp own vtable, ReleaseCMemBlockBase, conditional Storm free, no
//  `this != 0` guard on the free (the same shape
//  ntempest_priorityq_scalardtors.cpp already documents) - plus one more
//  step first: if `this+0x1C` (a live element count) is non-zero, clear
//  every element through the already-reconstructed
//  `AgileAgentQuery::RemoveEntries(0, count)` (Agent/agentquery.h,
//  0x6F46AC70) before the block itself is freed - the same generic
//  "clear all elements, then free the block" shape a `CDynTable<T>` with a
//  non-trivial cleanup step would need, reused here by raw address across
//  an unrelated class hierarchy exactly like this repo's other generic
//  container internals already are (see Pathfinding/crlagent.h's own
//  `CDynTable<CPrRelation*>` note).
//
//  0x6F48A4A0/0x6F48A4E0 are `NIpse::CPoQuery`/`NIpse::CSeQuery`'s own
//  vtable slot 0 - reached as their own distinct, separately-compiled
//  addresses (this image was linked without /OPT:ICF, so an unmodified
//  inherited virtual still gets its own copy at the derived class's own
//  vtable slot) - but each restamps `NIpse::CPmQuery<CPoQu>`/
//  `<CSeQu>`'s own vtable literal, not a `CPoQuery`/`CSeQuery`-specific
//  one: neither of those two classes overrides this destructor for real,
//  so the compiler emitted the identical body it already had for
//  `CPmQuery<T>` a second time, unchanged including the literal it
//  restamps - the same "no real vtable-stamping constructor for the
//  correct derived class yet, so the immediate is whatever the compiler
//  already had" shape `CAgentBaseAbs::RestampToPresenceVtable`
//  (Agent/cagentbaseabs_module_batch2.cpp) already documents.
//============================================================================
#include "cellbuffer.h"
#include "storm.h"
#include "agentquery.h"

namespace NIpse {

//  See Pathfinding/crlagent_dtor.cpp's own StampVtable for why this goes
//  through a volatile store rather than a plain literal assignment.
static __forceinline void StampVtable(void* pThis, void* vtbl)
{
    *(void* volatile*)pThis = vtbl;
}

struct CPmQuery_AgileAgentFilter
{
    //  0x6F46D730 - vtable slot 0 of NIpse::CPmQuery<AgileAgentFilter> (0x6F951844).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPmQuery_CPoQu
{
    //  0x6F489370 - vtable slot 0 of NIpse::CPmQuery<NIpse::CPoQu> (0x6F9525C4).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPmQuery_CSeQu
{
    //  0x6F4893B0 - vtable slot 0 of NIpse::CPmQuery<NIpse::CSeQu> (0x6F9525CC).
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CPoQuery
{
    //  0x6F48A4A0 - vtable slot 0 of NIpse::CPoQuery (0x6F952844) - restamps
    //  CPmQuery<CPoQu>'s own vtable literal, see this file's own header.
    void* ScalarDeletingDestructor(unsigned int flags);
};

struct CSeQuery
{
    //  0x6F48A4E0 - vtable slot 0 of NIpse::CSeQuery (0x6F95284C) - restamps
    //  CPmQuery<CSeQu>'s own vtable literal, see this file's own header.
    void* ScalarDeletingDestructor(unsigned int flags);
};

void* CPmQuery_AgileAgentFilter::ScalarDeletingDestructor(unsigned int flags)
{
    unsigned int count = *(unsigned int*)((char*)this + 0x1C);

    StampVtable(this, (void*)0x6F951844);

    if (count != 0)
        ((AgileAgentQuery*)this)->RemoveEntries(0, count);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPmQuery_CPoQu::ScalarDeletingDestructor(unsigned int flags)
{
    unsigned int count = *(unsigned int*)((char*)this + 0x1C);

    StampVtable(this, (void*)0x6F9525C4);

    if (count != 0)
        ((AgileAgentQuery*)this)->RemoveEntries(0, count);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPmQuery_CSeQu::ScalarDeletingDestructor(unsigned int flags)
{
    unsigned int count = *(unsigned int*)((char*)this + 0x1C);

    StampVtable(this, (void*)0x6F9525CC);

    if (count != 0)
        ((AgileAgentQuery*)this)->RemoveEntries(0, count);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CPoQuery::ScalarDeletingDestructor(unsigned int flags)
{
    unsigned int count = *(unsigned int*)((char*)this + 0x1C);

    //  Restamps CPmQuery<CPoQu>'s own vtable literal - see this file's own
    //  header comment for why.
    StampVtable(this, (void*)0x6F9525C4);

    if (count != 0)
        ((AgileAgentQuery*)this)->RemoveEntries(0, count);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

void* CSeQuery::ScalarDeletingDestructor(unsigned int flags)
{
    unsigned int count = *(unsigned int*)((char*)this + 0x1C);

    //  Restamps CPmQuery<CSeQu>'s own vtable literal - see this file's own
    //  header comment for why.
    StampVtable(this, (void*)0x6F9525CC);

    if (count != 0)
        ((AgileAgentQuery*)this)->RemoveEntries(0, count);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}

}  // namespace NIpse

//============================================================================
//  `AgentRef` - a stack-built, vtable-carrying reference to an agent.
//
//  The class name is not a reconstruction's guess: CAgent::Handler_lkl
//  (0x6F472A00) builds one of these as a local and stamps
//  `??_7AgentRef@@6B@` into it, so IDA's own symbol gives the name, the way
//  `AgentRefListener` next to it on the same frame does.
//
//  Layout is read off that construction site: the vtable pointer, one word
//  Handler_lkl clears and nothing here reads back, then the {handle,
//  typeTag} pair Resolve() puts through LookupHandle - the same pair every
//  other reader in this binary carries, just at +0x08/+0x0C rather than
//  CAgent's +0x0C/+0x10.
//
//  Declared without any `virtual`, deliberately: nothing in scope dispatches
//  through the slot, no vtable of this class has been committed under
//  docs/targets/vtables/, and emitting one from a guessed slot list is
//  exactly what check_vtables.py exists to catch.  The pointer is a reserved
//  word instead.
//============================================================================
#ifndef AGENTREF_H
#define AGENTREF_H

#include "game.h"

struct AgentRef
{
    //  0x6F478630 - resolve twice.  This reference's own pair names an
    //  object that carries a *second* pair at +0x78/+0x7C, and it is that
    //  inner object - gated on the same +0x20 pending flag every other
    //  reader in this family checks - that comes back.
    SItemHandleObject* Resolve();

    //  0x6F4786A0 - Resolve(), then one field off the answer.  The sibling
    //  of Item/itemhandleresolve.h's QueryHandleField0x54 (0x6F4786B0),
    //  0x10 bytes further on and reading the same +0x54 word off the same
    //  kind of object; this one goes through the two-step resolve above
    //  instead of a bare handle pair.
    void* QueryField0x54();

    void*           m_vtable;       // +0x00  ??_7AgentRef@@6B@
    void*           m_reserved04;   // +0x04  cleared by the builder
    unsigned int    m_handle;       // +0x08
    int             m_typeTag;      // +0x0C
};

//----------------------------------------------------------------------------
//  `AgentRefListener` - AgentRef's sibling on the same frame, and the other
//  half of what CAgent::Handler_lkl builds.  Same four words in the same
//  order, stamped with `??_7AgentRefListener@@6B@` (0x6F951B28) instead;
//  IDA's own symbol at that address gives this name too.
//
//  Its one use resolves the pair at +0x08/+0x0C and reads the resolved
//  object's +0x40 - which is 0x6F4806C0, already reconstructed as
//  `SHandleRefAt0x08Owner::ResolveAndGetField0x40`
//  (Misc/misc_handle_resolvers.cpp).  That declaration is what
//  Handler_lkl calls, and this struct only has to lay the fields out for
//  it; declaring a second member here at the same address would be the
//  two-symbols-for-one-address defect
//  docs/notes/verifier-gate-link-failures.md warns about.
//
//  No `virtual`, for the reason AgentRef's own comment gives.
//----------------------------------------------------------------------------
struct AgentRefListener
{
    void*           m_vtable;       // +0x00  ??_7AgentRefListener@@6B@
    void*           m_reserved04;   // +0x04  cleared by the builder
    unsigned int    m_handle;       // +0x08
    int             m_typeTag;      // +0x0C
};

//  The two shipped vtable symbols the builder stamps, reached by address
//  rather than emitted here: `off_6F951B0C` / `off_6F951B28` in
//  funcmap.DATA, the same way `off_6F8765F8`/`g_vftTRefCnt` is.  Binding
//  the real tables matters - AgentRef::Resolve's own callee chain runs on
//  objects the game built - and the cost is that the dump prints the
//  mangled vtable name, which no address-shaped DATA row can carry, so
//  each stamp canonicalises to SYM on the target side.
extern void* g_vftAgentRef;           // off_6F951B0C  ??_7AgentRef@@6B@
extern void* g_vftAgentRefListener;   // off_6F951B28  ??_7AgentRefListener@@6B@

#endif

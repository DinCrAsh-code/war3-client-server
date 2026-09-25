//============================================================================
//  0x6F4A4BE0 / 0x6F4A4C10 / 0x6F4A4C40 - two more relation lists this
//  agent keeps, at `this+0x9C` and `this+0xAC` (inside what crlagent.h
//  currently calls `m_ownReserved78[0x40]`), plus the function that
//  detaches all four of this agent's own relation lists at once (these two
//  and two more this session does not own - 0x6F4A4BB0/0x6F4A4B80, owned by
//  a parallel session on the same dump).
//
//  Neither list's node shape is established by this call tree: each
//  function only ever reads `this+0x9C`/`this+0xAC` itself (never a `next`
//  pointer inside a node), detaches whatever relation the head's own
//  `+8` names, and re-reads the head - relying entirely on
//  `DetachRelation`'s own side effects (through the detached relation's
//  `DetachEndpointA`/`DetachEndpointB`) to have changed what is there
//  before the next iteration. So this is not necessarily the same
//  `SRelationLink` shape the four `+0x78`/`+0x88`/`+0x98`/`+0xA8` lists use
//  (agentbaseabs_related.h) - transcribed here as the raw offsets the
//  dump actually shows, per CLAUDE.md's "only touched members get names".
//
//  `DetachRelationList9C`/`DetachRelationListAC` each get their own
//  translation unit (agentbaseabsrelationlist9c.cpp/agentbaseabsrelation-
//  listac.cpp) rather than living next to `DetachAllRelationLists` here -
//  CLAUDE.md's "put a callee in its own TU rather than reaching for
//  __declspec(noinline)": all three are small enough that /O2 inlined both
//  loops straight into `DetachAllRelationLists` when they shared a file,
//  which is not what the shipped binary does (three real `call`s).
//============================================================================
#ifndef AGENTBASEABSRELATIONLISTS_H
#define AGENTBASEABSRELATIONLISTS_H

//  0x6F4A4BB0 / 0x6F4A4B80 - the other two of this agent's four relation
//  lists' own detach-all functions, out of this session's batch (owned by
//  a parallel session on the same dump). Forward-declared here only so
//  this translation unit compiles under `verify.py`, which never links.
struct CAgentBaseAbsRelationListsExternal
{
    void DetachOther1_6F4A4BB0(int alsoEndpointB);
    void DetachOther2_6F4A4B80(int alsoEndpointB);
};

struct CAgentBaseAbsRelationLists
{
    //  0x6F4A4BE0 - the list at `this+0x9C`.
    void DetachRelationList9C(int alsoEndpointB);

    //  0x6F4A4C10 - the list at `this+0xAC`.
    void DetachRelationListAC(int alsoEndpointB);

    //  0x6F4A4C40 - all four of this agent's own relation lists.
    void DetachAllRelationLists(int alsoEndpointB);
};

#endif

//============================================================================
//  Element types for three of CFogMask's own TSGrowableArray members
//  (createunit-G4 cluster A). Each is named from its RTTI type-descriptor
//  string alone (".?AUSpan@@", ".?AUFogNode@@", ".?AUNodeMapDep@@" -
//  worktree_store.py resolve against agent_worktrees/names.json) - none of
//  the call tree this session actually reconstructed (the TSGrowableArray
//  SetAlloc/Assign instantiations only ever copy these elements as opaque
//  bytes) reads a named member of any of the three, so the fields stay
//  unnamed reserved blocks of the right total size rather than invented
//  members. CFogMask's own constructor and rebuild routine
//  (0x6F409120/0x6F409D00) are the functions that would establish the real
//  field layout, and neither is reconstructed in this session (budget -
//  see the session write-up).
//============================================================================
#ifndef FOGMASKTYPES_H
#define FOGMASKTYPES_H

struct Span
{
    char m_reserved[8];
};

struct FogNode
{
    char m_reserved[0x14];
};

struct NodeMapDep
{
    char m_reserved[8];
};

#endif

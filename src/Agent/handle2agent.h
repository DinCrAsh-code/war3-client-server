//============================================================================
//  A second TSHashTable<T,K> instantiation, sitting next to sub_6F430580 in
//  the unit/agent registration module.  IDA's two RTTI-descriptor labels for
//  the arrays this table grows - `aAuhandle2agent` for the node array itself
//  and `aAvTsexplicitli_12`/`aAvTsexplicitli_14` for its bucket array - both
//  decode from real string bytes (that is how IDA names an unnamed string
//  location), and both read as a class named `Handle2Agent`: the table maps
//  a handle to the CGameObject it names.
//
//  None of the functions reconstructed against this table from
//  sub_6F430C80's call tree (Initialize, CheckGrow, Rehash, NewNode,
//  UnlinkAll, ComputeChunk, SetAlloc of the bucket array) touch anything of
//  Handle2Agent's beyond its TSHashObject<T,K> base - Ptr()/New(), the two
//  members that would need the key's real shape and the node's own fields,
//  are not in this call tree - so nothing past that base is invented here.
//  Handle2Agent has no virtual function of its own, matching
//  AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP's HashLinkOffset() of 4 rather
//  than AGILE_TYPE_DATA's 8, and every constant this cluster's asm folds in
//  (the bucket link offset 4, the four-bucket initial SetCount) matches that.
//============================================================================
#ifndef HANDLE2AGENT_H
#define HANDLE2AGENT_H

#include "storm.h"

//: Not exercised by anything in this call tree (no Ptr()/New() instantiation
//: of this table is reachable from sub_6F430C80), so its shape is
//: unconstrained by anything observable.
typedef unsigned int HANDLE_ID;

//  sub_6F430C80 reaches two more fields past the TSHashObject base, both
//  written directly by the root function rather than through New(): the
//  agent-slot token (see kAgentSlotTokenBias in agentregistry.h - an index
//  into the agent-slot array biased by a fixed constant, not a counter that
//  advances call to call) and a second copy of the type tag that m_hashval
//  already carries, which the shipped code writes again rather than
//  reusing.  m_key itself (0x14) is never written in this call tree, so it
//  stays whatever TSHashObject<T,K>::m_key's default is.
struct HANDLE2AGENT : public TSHashObject<HANDLE2AGENT, HANDLE_ID>
{
    unsigned int    m_slotToken;    // 0x18
    unsigned int    m_typeTagCopy;  // 0x1C
};

//----------------------------------------------------------------------------
//  The concrete table sub_6F430C80 embeds: TSHashTable<T,K>::AllocNode is
//  pure virtual, so something has to override it to be instantiable, and
//  New()/Ptr() both take an extra 4 bytes for the vftable that pushes this
//  table's own fields out to +0x04.  The override itself is never reached
//  from this call tree (nothing here calls NewNode/New, only NewNode()'s
//  raw allocation through sub_6F430580, which the root function follows up
//  with its own field writes - see agentregistry.cpp), so nothing beyond
//  its existence is claimed here, matching the same not-in-this-call-tree
//  pattern as AGILE_TYPE_TABLE::AllocNode.
//----------------------------------------------------------------------------
class HANDLE2AGENT_TABLE : public TSHashTable<HANDLE2AGENT, HANDLE_ID>
{
public:
    virtual ~HANDLE2AGENT_TABLE();
    virtual HANDLE2AGENT* AllocNode(TSExplicitList<HANDLE2AGENT>* bucket,
                                    void* arg1, void* arg2);
};

#endif

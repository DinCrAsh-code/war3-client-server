//============================================================================
//  NIpse::CRlAgentDef (`??_7CRlAgentDef@NIpse@@6B@`, 0x6F952794) and
//  NTempest::CLinkedList<NIpse::CRlAgentDef>
//  (`??_7?$CLinkedList@VCRlAgentDef@NIpse@@@NTempest@@6B@`, 0x6F9527F4) -
//  the other of the two siblings this session's own dump adds beside
//  NIpse::CRlAgent (Pathfinding/crlagent.h) at the base of the
//  CAgentBaseAbs chain (NTempestNIpse_BaseChain cluster A, 2026-09-08).
//
//  Per RTTI, CRlAgentDef is CRlProperty's sibling, not its base or
//  derivative - each independently derives from the same
//  CPresenceTagged/CPrRelation-shaped chain (its own copy of that flat
//  base-chain layout occupies its own offsets 0x00-0x4F, entirely separate
//  storage from anything below), and additionally EMBEDS a real
//  `NIpse::CRlAgent` (Pathfinding/crlagent.h, 0xB8 bytes) as a plain member
//  at +0x50 - confirmed by this class's own destructor
//  (0x6F488DD0) calling the REAL, already-reconstructed
//  `NIpse::CRlAgent::~CRlAgent()` (crlagent_dtor.cpp) against `this+0x50`,
//  not `this` itself, and by +0x108 (0x50 + 0xB8, `CRlAgent`'s own total
//  constructed size) being exactly where this class's own array member
//  starts.
//
//  `NTempest::CLinkedList<NIpse::CRlAgentDef>` shares every one of
//  CRlAgentDef's own vtable slots verbatim except slot 2 (its own scalar
//  deleting destructor, a separately-compiled but byte-identical function -
//  see .cpp) - named `CLinkedList_CRlAgentDef` for the sanitized-template
//  convention Misc/ntempest_memblockt_scalardtors.cpp already establishes.
//
//  No real C++ layout is modelled here beyond what this call tree touches -
//  see Pathfinding/crlagent.h's own file header for why this whole chain
//  stays flat and address-only.
//============================================================================
#ifndef RLAGENTDEF_H
#define RLAGENTDEF_H

struct SHandleRegistrationRequest;

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F46ABE0 - the 0x2C-element-size sibling of AgileAgentQuery::RemoveEntries
//  (Agent/agentqueryentries.cpp, 0x6F46AC70, ninety bytes away in the same
//  module): the same CMemBlock-backed-array `RemoveEntries` shape, a
//  different element size and no header of its own yet for the owning
//  array type, so this stays a standalone struct addressed purely by the
//  two offsets this call tree actually touches (+0x0C the data pointer,
//  +0x1C the live count - the same pair AgileAgentQuery inherits off
//  CMemBlock/adds itself), rather than a real `CDynTable<T>` instantiation.
//----------------------------------------------------------------------------
struct CDynTable_SRelationEndpointRecord
{
    //  0x6F46ABE0 - drop `count` records starting at `start`, sliding what
    //  follows down over them.  Called with (0, m_count) - "forget
    //  everything" - by CRlAgentDef::Slot4 below.
    int RemoveEntries(unsigned int start, unsigned int count);

    char          m_reserved00[0x0C];
    void*         m_data;          // +0x0C
    char          m_reserved10[0x04]; // +0x10 - untouched by this call tree
    unsigned int  m_growChunk;     // +0x14 - untouched by this call tree
    unsigned int  m_capacity;      // +0x18 - untouched by this call tree
    unsigned int  m_count;         // +0x1C
};

class CRlAgentDef
{
public:
    //  0x6F488DD0 - ~CRlAgentDef().  Real SEH frame in the shipped body
    //  (`__try`/`__finally` around the array sub-object's own destructor
    //  call) - same unreproducible `__CxxFrameHandler3` frame mismatch as
    //  Pathfinding/crlagent_ctor.cpp/crlagent_dtor.cpp's own precedent
    //  (docs/msvc-vc8-idioms.md); the body below matches the dump
    //  instruction for instruction past the frame furniture.  Restamps
    //  this class's own vtable defensively, destroys the +0x108 array
    //  sub-object (ReleaseCMemBlockBase, Pathfinding/cellbuffer.h - the
    //  same real target, 0x6F4B3770, Pathfinding/crlagent_dtor.cpp's own
    //  call reaches), destroys the embedded `NIpse::CRlAgent` at +0x50 with
    //  a REAL call to the already-reconstructed `~CRlAgent()`
    //  (Pathfinding/crlagent_dtor.cpp - see this file's own header comment
    //  for why +0x50 is an embedded member, not a base), then restamps
    //  straight to `NTempest::CPresence`'s own vtable.
    ~CRlAgentDef();

    //  0x6F488D40 - vtable slot 0. The literal string "RlAgentDef"
    //  (aRlagentdef, 0x6F952898).
    const char* GetTypeName();

    //  0x6F488D90 - vtable slot 1. Same unlink-then-return-to-pool shape as
    //  every sibling in this chain, against this class's own static pool
    //  (CRlAgentDef_GetStaticPool, agentbasechain_staticpools.h).
    void ReturnToStaticPool();

    //  0x6F488E40 - vtable slot 2, scalar deleting destructor.  DOES call a
    //  real base destructor first (~CRlAgentDef(), see .cpp) and DOES carry
    //  the `this != 0` guard, the same shape
    //  Pathfinding/crlagent.h's own `CRlAgent::ScalarDeletingDestructor`
    //  uses whenever a real destructor call precedes the free.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  0x6F4A47A0 - vtable slot 3.  Resets this class's own embedded
    //  CAgentRelation's event binding, then registers `this` at the front
    //  of a global list rooted at `g_pTimeSync + 0x25C`
    //  (LinkNodeIntoRegistry, sub_6F4A4310 - see .cpp).
    void Apply(SHandleRegistrationRequest* arg);

    //  0x6F4A5680 - vtable slot 4.  Sets the 0x1000000 flags bit, unlinks
    //  `this` from its own +0x04/+0x08 sibling list, notifies every
    //  registered listener off +0x58 (NotifyListeners, sub_6F4A4750),
    //  drops every record from the +0x108 array if any are live
    //  (CDynTable_SRelationEndpointRecord::RemoveEntries above), then tears
    //  the underlying relation down.
    void Slot4(int arg);

    //  0x6F4A4F60 - vtable slot 9 (+0x24).  The base `CAgentRelation::Save`
    //  first, then this class's own +0x108 array (SaveRecordArray,
    //  sub_6F4A4850 - forward-declared only, a cross-batch naming seam:
    //  its own leaf callees are a different parallel session's own batch of
    //  this dump).
    void Save(void* writer);

    //  0x6F4A4F80 - vtable slot 10 (+0x28), Load's mirror image
    //  (LoadRecordArray, sub_6F4A4900 - same cross-batch seam as Save
    //  above), followed by the same global-registry link Apply performs.
    void Load(void* reader);

private:
    void NotifyListeners(void* arg);
};

//----------------------------------------------------------------------------
//  0x6F4A4310 - splice a node onto the front of a global doubly-linked
//  registry: `this` is the registry root/sentinel (fields at its own
//  +0x04/+0x08, the same pair every class in this chain uses for its own
//  sibling-list membership - see Pathfinding/crlagent.h's own
//  m_baseListPrev/Next), reached with a real `this`-in-ecx/one-stack-arg
//  calling convention (`retn 4`) - a plain thiscall, not `__fastcall` (no
//  `edx` argument at all), which is why this is a method of a minimal
//  registry-root type rather than a free function. Reached from both
//  CRlAgentDef::Apply and CRlAgentDef::Load, against the same global root
//  (`g_pTimeSync + 0x25C`) both times.
//----------------------------------------------------------------------------
struct SRegistryRoot
{
    void LinkNodeIntoRegistry(void* node);
};

}  // namespace NIpse

//----------------------------------------------------------------------------
//  0x6F9527F4 - NTempest::CLinkedList<NIpse::CRlAgentDef>.  Every slot but
//  its own scalar deleting destructor below is inherited from
//  NIpse::CRlAgentDef unchanged (same address); see rlagentdef.h's own file
//  header.
//----------------------------------------------------------------------------
namespace NTempest {

class CLinkedList_CRlAgentDef
{
public:
    //  0x6F4890D0 - vtable slot 2, the one slot this template instantiation
    //  overrides on its own: a byte-identical, separately-compiled twin of
    //  `NIpse::CRlAgentDef::ScalarDeletingDestructor` (same real base
    //  destructor call, same guard) - see rlagentdef_scalardtor.cpp.
    void* ScalarDeletingDestructor(unsigned int flags);
};

}  // namespace NTempest

#endif  // RLAGENTDEF_H

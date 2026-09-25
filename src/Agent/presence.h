//============================================================================
//  NTempest::CPresence (`??_7CPresence@NTempest@@6B@`, 0x6F951CA0) and
//  NTempest::CPresenceTagged (`??_7CPresenceTagged@NTempest@@6B@`,
//  0x6F951CD4) - two adjacent levels of the single-inheritance chain
//  Pathfinding/crlagent.h documents in full:
//
//      CAgentBaseAbs : public NIpse::CRlAgent : public NIpse::CPrRelation :
//        public NTempest::CPresenceTagged : public NTempest::CPresence :
//        public NTempest::CMemBlock : public NTempest::CEntity
//
//  Both classes get the same four-slot shape in this batch's own dump: a
//  `GetTypeName`-style class-name accessor at slot 0, a `ReturnToStaticPool`
//  at slot 1 (see agentbasechain_staticpools.h - each class's own free-list
//  pool lives at a fixed offset off a different global), and a scalar
//  deleting destructor at slot 2.
//
//  MERGE NOTE: a concurrent session, working in this same shared checkout,
//  independently created a separate `Agent/presencetagged.h` with its own,
//  different `namespace NTempest { class CPresenceTagged { ... }; }`
//  (one method, `FoldLinkFieldsIntoHash`, vtable slot 15/+0x3C) before this
//  file existed under that name - a genuine ODR collision once both were on
//  disk at once. Resolved here by merging that one method into this
//  (more complete) definition and deleting the now-redundant header;
//  Agent/agentbaseabscomputechecksum.cpp (the only includer) was
//  repointed at this file. No behaviour changed - same address, same body,
//  still defined in that same .cpp.
//
//  CPresence alone also introduces slot 4
//  (see below); CPresenceTagged instead introduces a debug-dump at slot 16.
//
//  ---------------------------------------------------------------------
//  A real oddity worth flagging rather than silently reconstructing past:
//  CPresence's own slot 2 (0x6F479910), CPresenceTagged's own slot 2
//  (0x6F479C60) and NIpse::CPrRelation's own slot 2
//  (Agent/agentrelation.h's `CAgentRelation::ScalarDeletingDestructor`,
//  0x6F47FFF0) are BYTE-IDENTICAL to each other (down to `raw_bytes` in
//  agent_worktrees/funcs/) apart from their own address and the resulting
//  `call` displacement - and every one of them restamps `this` straight to
//  `??_7CPresence@NTempest@@6B@` (0x6F951CA0), never to its own class's
//  vtable, and never calls any base destructor first (no `test esi,esi`
//  guard on the free either, unlike every sibling scalar deleting
//  destructor in this chain that DOES call a real base dtor -
//  Agent/cmemblock.h's own, Pathfinding/crlagent.h's own,
//  CAgentBaseAbs's own in Agent/cagentbaseabs_module_batch2.cpp).  That
//  disagrees with the ALREADY-reconstructed, differently-addressed
//  `NTempest::CPresence::ScalarDeletingDestructor` at 0x6F4A3760
//  (Agent/cpresence_dtor.cpp), which DOES call a real, SEH-framed
//  `~CPresence()` (0x6F4A1CF0) first and stamps no vtable of its own (the
//  real destructor stamps it on the way out instead). Reconstructed
//  byte-for-byte here regardless - the instruction stream is unambiguous
//  even though *why* three separate classes' own scalar deleting
//  destructors degenerate to an identical no-teardown restamp-and-free,
//  skipping the real ~CPresence() entirely, is left open for a future
//  session (most likely: per-TU duplicate compiled copies of one
//  compiler-synthesized special member, this binary's own linker not
//  folding them - the same kind of duplicate-address defect
//  docs/notes/thunk-removal-pass.md already documents one instance of, just
//  the mirror image: one C++ definition, several shipped addresses, rather
//  than one address under two names).
//
//  No real C++ layout is modelled here beyond what these functions touch;
//  see Pathfinding/crlagent.h's own file header for why this whole chain
//  stays flat and address-only rather than a real inheritance tree.
//============================================================================
#ifndef PRESENCE_H
#define PRESENCE_H

namespace NTempest {

class CPresence
{
public:
    //  0x6F4798A0 - vtable slot 0. `retn` with zero stack bytes, no `this`
    //  read - the literal string "Presence".
    const char* GetTypeName();

    //  0x6F479B20 - vtable slot 1. Unlink `this` from a doubly-linked list
    //  of live instances (own +0x04/+0x08 prev/next pair) and push `this`
    //  back onto CPresence's own static free-list pool
    //  (CPresence_GetStaticPool, agentbasechain_staticpools.h) - the
    //  intrusive link slot lives 4 bytes before the object itself
    //  (`this-4`), the same shape CAgentBaseAbs's own
    //  `ReturnToStaticPool` (Agent/cagentbaseabs_module_batch2.cpp) already
    //  establishes for a sibling pool.
    void ReturnToStaticPool();

    //  0x6F479910 - vtable slot 2, scalar deleting destructor. Named
    //  `ScalarDeletingDestructorTrivial` rather than plain
    //  `ScalarDeletingDestructor` because that name is already taken, in
    //  this exact class/namespace, by the DIFFERENT, already-reconstructed
    //  0x6F4A3760 (Agent/cpresence_dtor.cpp - real SEH-framed ~CPresence()
    //  call, no vtable self-stamp) - two distinct addresses cannot share
    //  one C++ member signature, and link_check.py caught the LNK2005 this
    //  would otherwise cause immediately. See this file's own header
    //  comment above for the byte-identical-triplet oddity this one shares
    //  with CPresenceTagged's and CPrRelation's own.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    //  0x6F4798C0 - vtable slot 4. A bare indirect tail-call through
    //  `this`'s OWN vtable slot 1 (`ReturnToStaticPool`), not a different
    //  object's - `ecx` is left untouched from entry and no argument is
    //  ever pushed for the inner call, so the incoming `flags` dword just
    //  sits above the return address the whole time and this function's
    //  own `retn 4` epilogue is what eventually pops it, whether or not
    //  the callee ever looks at it (it does not: every override of slot 1
    //  in this chain takes zero stack arguments).  Named `Slot4` rather
    //  than anything more specific, matching Pathfinding/crlagent.h's own
    //  precedent for the identical CAgentBaseAbs-vtable-slot-4 column
    //  (Agent/agentbaseabsrelatedcascade.cpp's own header: CPresenceTagged
    //  and CPrRelation both inherit this unchanged - hence neither has its
    //  own entry in this session's 15 roots - and NIpse::CRlAgent is the
    //  first class to actually override it with real behaviour).
    void* Slot4(unsigned int flags);
};

class CPresenceTagged
{
public:
    //  0x6F479BC0 - vtable slot 0. Same shape as CPresence::GetTypeName,
    //  literal string "PresenceTagged".
    const char* GetTypeName();

    //  0x6F479CD0 - vtable slot 1. Same shape as CPresence::ReturnToStaticPool,
    //  against CPresenceTagged's own pool (CPresenceTagged_GetStaticPool).
    void ReturnToStaticPool();

    //  0x6F479C60 - vtable slot 2, scalar deleting destructor. See this
    //  file's own header comment for the byte-identical-triplet oddity.
    void* ScalarDeletingDestructor(unsigned int flags);

    //  vtable slots 5-14 (including 9/10, 0x6F479BD0/0x6F479BE0 -
    //  nullsub_1477/nullsub_1478) are the shared do-nothing-virtual family
    //  Agent/cagentbaseabs_noop_slots.cpp documents in full (the same run
    //  as this class's own slot 8/11/13, already recorded there) - a
    //  genuine leftover leaf closed in that file, not a chain: nothing
    //  about the body is specific to CPresenceTagged. Slots 3/4
    //  (0x6F4AEFF0/0x6F4AEF00, SHandleTableEntry::RegisterOrReuse-family)
    //  are reconstructed elsewhere, cast through SHandleTableEntry rather
    //  than declared on this class - see Agent/agentbaseabseventbinding.cpp.

    //  0x6F479C30 - vtable slot 16. Prints an empty line (LogFormatted with
    //  the shared empty-string global GameUI/gameuiplayername.cpp's own
    //  header already names and spells as a plain `""` literal per that
    //  file's own reasoning - a shared constant that must not be given a
    //  DATA name), then this object's own tag pair at +0x14/+0x18 as
    //  "[PrTg] t[%u] b[%u]" - the same kind of birth-tag/presence-tag pair
    //  tools/funcmap.py's own BEHAVIOUR notes already describe elsewhere off
    //  g_unk6FAB778C, here read directly off `this` instead.
    void DumpDebugState(void* sink);

    //  0x6F4AEE60 - CPresenceTagged's own vtable slot 15 (+0x3C), merged in
    //  from the now-deleted Agent/presencetagged.h (a concurrent session's
    //  own file, sharing this exact class - see this header's own file
    //  comment): folds `this+0x14` then `this+0x18` (this class's own
    //  m_tagT/m_tagB, the same pair DumpDebugState above prints) into
    //  *hash. Defined in Agent/agentbaseabscomputechecksum.cpp, where the
    //  full checksum-fold reasoning lives.
    void FoldLinkFieldsIntoHash(unsigned int* hash);

    unsigned int m_reserved00_14[5];  // +0x00-+0x13, base-chain territory
    unsigned int m_tagT;               // +0x14 - "t" in "[PrTg] t[%u] b[%u]"
    unsigned int m_tagB;               // +0x18 - "b" in "[PrTg] t[%u] b[%u]"
};

}  // namespace NTempest

#endif  // PRESENCE_H

//============================================================================
//  0x6F4798D0 / 0x6F4798E0 / 0x6F4798F0 - IDA's nullsub_1473/1474/1475.
//  Reached as CAgentBaseAbs vtable slots 0x14/0x18/0x1C
//  (agent_worktrees/classes/0x6F9520D4.json), but nothing about the body
//  ties any of the three to that class specifically: each is a shared,
//  previously-unnamed do-nothing virtual, `this` (ecx) and one stack dword
//  both discarded, `retn 4` - the same empty-virtual shape src/Misc/
//  nullsub.cpp documents for the zero-argument `ret`-only case, just with
//  the thiscall-plus-one-stack-arg shape a `retn 4` needs.
//
//  `SVtableNoOpArg` exists only to get that exact calling convention: a
//  free function cannot be spelled `__thiscall` in this compiler (see
//  dataallocator.h's own note on the same restriction), so this is written
//  as an ordinary non-virtual method of a plain struct instead - the
//  compiler emits the identical `this` in ecx / one stack dword / `retn 4`
//  either way, and nothing about *this* file claims these three are really
//  members of one real class; they are just three otherwise-anonymous
//  addresses that happen to need the same shape.
//
//  Now confirmed by the full NTempest::CPresence -> ... -> CAgentBaseAbs
//  vtable chain (agent_worktrees/classes/, operator's re-dump after the
//  namespace/template regex fix) to be a *much* larger shared family than
//  this file's own name suggests: eight more addresses below are the same
//  "do-nothing virtual" shape at other slots this same chain never
//  overrides between whichever level first introduces them and
//  CAgentBaseAbs itself - four more `retn 4` (one discarded stack dword)
//  and two plain `retn` (no stack argument at all, the zero-argument shape
//  src/Misc/nullsub.cpp documents, just reached as a real thiscall member
//  here instead of a naked asm-call target). None of these belong to
//  CAgentBaseAbs specifically either - see each one's own comment below for
//  which class's vtable slot it actually first appears at.
//============================================================================

struct SVtableNoOpArg
{
    //  Declared (not defined) here so each body is a real, separately
    //  emitted, externally visible symbol rather than an implicitly-inline
    //  member definition the compiler is free to drop or fold together.
    void NoOp_6F4798D0(void* /*unused*/);
    void NoOp_6F4798E0(void* /*unused*/);
    void NoOp_6F4798F0(void* /*unused*/);

    //  0x6F4798B0 (nullsub_1472) - NTempest::CPresence's own vtable slot 3
    //  (agent_worktrees/classes/0x6F951CA0.json) - overridden by a real
    //  body at NTempest::CPresenceTagged (0x6F4AEFF0,
    //  SHandleTableEntry::RegisterOrReuse - already a real reconstruction
    //  from the earlier tempest-presence-table-slot-allocator investigation,
    //  docs/notes/checksum-provider-registry.md) and never a no-op again
    //  after that.
    void NoOp_6F4798B0(void* /*unused*/);

    //  0x6F479900 (nullsub_1476) - first appears at CPresenceTagged's own
    //  vtable slot 8 (agent_worktrees/classes/0x6F951CD4.json) and stays a
    //  no-op through CPrRelation and CRlAgent; CAgentBaseAbs overrides this
    //  same slot with a real body (SAgentEventRouter::DispatchByFourCC,
    //  Agent/agenteventrouter.cpp) - the one slot-8 case that really is
    //  CAgentBaseAbs's own override.
    void NoOp_6F479900(void* /*unused*/);

    //  0x6F479BF0 (nullsub_1479) - first appears at CPresenceTagged's own
    //  vtable slot 11, stays a no-op through CPrRelation and CRlAgent;
    //  CAgentBaseAbs overrides this slot for real
    //  (CAgentBaseAbs::ForwardSaveToDelegate,
    //  Agent/cagentbaseabs_module_batch2.cpp).
    void NoOp_6F479BF0(void* /*unused*/);

    //  0x6F479C10 (nullsub_1481) - first appears at CPresenceTagged's own
    //  vtable slot 13, stays a no-op through CPrRelation and CRlAgent;
    //  CAgentBaseAbs overrides this slot for real
    //  (CAgentBaseAbs::ForwardLoadToDelegate,
    //  Agent/cagentbaseabs_module_batch2.cpp).
    void NoOp_6F479C10(void* /*unused*/);

    //  0x6F47FFE0 (nullsub_1486) - NIpse::CPrRelation's own vtable slot 17
    //  (agent_worktrees/classes/0x6F952004.json) - a *newly introduced*
    //  slot at this level (CPresenceTagged's own vtable has only 17 slots,
    //  0-16; CPrRelation's has 19), overridden by a real body at NIpse::
    //  CRlAgent (0x6F4A5A30) and never a no-op again after that.
    void NoOp_6F47FFE0(void* /*unused*/);

    //  0x6F479D80 (nullsub_1483) - NIpse::CPrBehavior's own vtable slot 18
    //  (+0x48), agent_worktrees/classes/0x6F951D44.json
    //  (docs/targets/NTempestNIpse_CPrBehavior.md, cluster B, 2026-09-08).
    //  A genuinely new slot at this level: CPresenceTagged's own vtable has
    //  only 17 slots, and CPrBehavior branches off the chain at
    //  CPresenceTagged directly (not through CPrRelation/CRlAgent), so this
    //  is not an inherited no-op from any earlier class in this file - it is
    //  CPrBehavior's own, first introduced here.
    void NoOp_6F479D80(void* /*unused*/);

    //  0x6F479BD0 (nullsub_1477) / 0x6F479BE0 (nullsub_1478) -
    //  NTempest::CPresenceTagged's own vtable slots 9/10
    //  (agent_worktrees/classes/0x6F951CD4.json), sitting right between the
    //  already-documented nullsub_1476 (slot 8) and nullsub_1479 (slot 11)
    //  in this exact same run of shared do-nothing virtuals - a genuine
    //  leftover leaf, not a chain: `agent_worktrees/` had them as `TODO`
    //  even though the rest of CPresenceTagged (Agent/presence.h) was
    //  already fully reconstructed.
    void NoOp_6F479BD0(void* /*unused*/);
    void NoOp_6F479BE0(void* /*unused*/);
};

//  Plain, zero-stack-argument no-op virtuals (`retn` with no operand) -
//  same family, different calling shape: this compiler's own `void Foo()`
//  member (this in ecx, no stack args) already produces exactly this.
struct SVtableNoOp
{
    //  0x6F479C00 (nullsub_1480) - first appears at CPresenceTagged's own
    //  vtable slot 12, stays a no-op through CPrRelation and CRlAgent;
    //  CAgentBaseAbs overrides this slot for real
    //  (CAgentBaseAbs::Slot12_BindDelegate, Agent/cagentbaseabs_slot12.cpp).
    void NoOp_6F479C00();

    //  0x6F479C20 (nullsub_1482) - first appears at CPresenceTagged's own
    //  vtable slot 14, stays a no-op through CPrRelation and CRlAgent;
    //  CAgentBaseAbs overrides this slot for real
    //  (CAgentBaseAbs::RefreshDelegateOwningColor,
    //  Agent/cagentbaseabs_module_batch2.cpp).
    void NoOp_6F479C20();
};

void SVtableNoOpArg::NoOp_6F4798D0(void* /*unused*/)
{
}

void SVtableNoOpArg::NoOp_6F4798E0(void* /*unused*/)
{
}

void SVtableNoOpArg::NoOp_6F4798F0(void* /*unused*/)
{
}

void SVtableNoOpArg::NoOp_6F4798B0(void* /*unused*/)
{
}

void SVtableNoOpArg::NoOp_6F479900(void* /*unused*/)
{
}

void SVtableNoOpArg::NoOp_6F479BF0(void* /*unused*/)
{
}

void SVtableNoOpArg::NoOp_6F479C10(void* /*unused*/)
{
}

void SVtableNoOpArg::NoOp_6F47FFE0(void* /*unused*/)
{
}

void SVtableNoOp::NoOp_6F479C00()
{
}

void SVtableNoOp::NoOp_6F479C20()
{
}

void SVtableNoOpArg::NoOp_6F479D80(void* /*unused*/)
{
}

void SVtableNoOpArg::NoOp_6F479BD0(void* /*unused*/)
{
}

void SVtableNoOpArg::NoOp_6F479BE0(void* /*unused*/)
{
}

//============================================================================
//  CBhPoBehavior - `NIpse::CBhPoBehavior`, a sibling of CBhPoProjectile
//  (bhpoprojectile.h) in the same "cluster G" NTempest/NIpse vtable family
//  (agent_worktrees/classes/0x6F951DA4.json, 20 slots). Address-interleaved
//  with CBhPoProjectile's own constructor and vtable slots 0/1/2
//  (0x6F479EA0-0x6F479F5A sits directly between CBhPoProjectile's own
//  0x6F47A090-0x6F47A470 cluster and its ctor at 0x6F479F90), so almost
//  certainly the same original shipped module - not modelled as a real C++
//  base of CBhPoProjectile here (several of its own vtable slots are shared
//  verbatim with CBhPoProjectile's, per the task's own slot table, which
//  argues for a real inheritance relationship, but that has not been
//  independently confirmed this session and is left for whichever session
//  reconciles the two headers).
//
//  Own vtable/layout has not been enumerated beyond what this session's own
//  slots 0/1/2 touch - see bhpobehavior_typename_dtor.cpp.
//
//  Slots 3/4/8/9/10/15/16 (agent_worktrees/classes/0x6F951DA4.json) are the
//  SAME addresses as `NIpse::CPrBehavior`'s own (0x6F4AA340/0x6F4AA550/
//  0x6F4AA370/0x6F4AA450/0x6F4AA7C0/0x6F4AEE60/0x6F479C30 - Agent/prbehavior.h,
//  already real) - CBhPoBehavior does not override any of them, i.e. it
//  adds NO new fields of its own between +0x0C and CPrBehavior's own last
//  field at +0x38: every field this whole family's Save/Load pair
//  (prbehavior_save.cpp/prbehavior_load.cpp) touches already belongs to
//  the base. This is the real evidence for the "does the base carry real
//  fields your own new slots would collide with" question a later
//  session's own task list asked about `CBhPoProjectile:CBhPoBehavior` -
//  CBhPoBehavior itself is an EMPTY derived class field-wise; its only
//  purpose in the chain is declaring the pure virtual below and
//  overriding GetTypeName/dtor/ScalarDeletingDestructorTrivial.
//
//  Slot 17 (+0x44, 0x6F7E10C2) is `_purecall` - a REAL pure virtual this
//  class declares but does not implement (Agent/purecallstub.cpp, already
//  real), matching MSVC's own "call the pure-virtual-called handler"
//  stub. CBhPoBehavior does not override it either - the first concrete
//  body anywhere in this chain is `CBhPoProjectile::NotifyVelocityDelta`
//  (0x6F497340, Missile/bhpoprojectile.h), since a concrete class cannot
//  leave a pure virtual unfilled.
//
//  Slot 18 (+0x48, 0x6F479D80/nullsub_1483) is the shared
//  `SVtableNoOpArg::NoOp_6F479D80` (Agent/cagentbaseabs_noop_slots.cpp,
//  already real) - CBhPoBehavior's own copy of the same no-op every other
//  class in this family shares at that slot.
//
//  Slot 19 (+0x4C, 0x6F5067E0) is a MISATTRIBUTED bulk-dump entry, read
//  directly rather than guessed at: that address is a large, SEH-framed
//  `CModelComplex_` destructor (stamps `??_7CModelComplex_@@6B@`, tears
//  down geosets/particle emitters/ribbon emitters) with nothing to do
//  with this projectile/behavior family - the same class of finding
//  docs/targets/NTempestNIpse_clusterG.md's own "idx19" section already
//  made for the neighbouring `0x6F506EA0` (`CGeoset`). Left untouched.
//============================================================================
#ifndef BHPOBEHAVIOR_H
#define BHPOBEHAVIOR_H

class CBhPoBehavior
{
public:
    //  0x6F479EA0 - vtable slot 0. Same shape as every other class in this
    //  vtable family (Agent/presence.h, Missile/bhpoprojectile.h);
    //  "BhPoBehavior" is the untruncated string per agent_worktrees
    //  names.json.
    const char* GetTypeName();

    //  0x6F479F20 - vtable slot 1, the real (non-scalar) destructor. Same
    //  shape as `CBhPoProjectile::~CBhPoProjectile`
    //  (Missile/bhpoprojectile_typename_dtor.cpp): splice out of an
    //  intrusive doubly-linked list at +0x04/+0x08, then return `this` to
    //  this class's own pool (GetTimeSyncField838, Sync/
    //  ctimesyncfield838.cpp - the `+0x838` sibling of CBhPoProjectile's
    //  own `+0x858` pool) via the identical `raw = this - 4` / fast-free-
    //  list-push idiom.
    ~CBhPoBehavior();

    //  0x6F479EB0 - vtable slot 2, ScalarDeletingDestructor. Byte-identical
    //  to `CBhPoProjectile::ScalarDeletingDestructorTrivial`
    //  (Missile/bhpoprojectile.h's own file comment) and to
    //  `NTempest::CPresence::ScalarDeletingDestructorTrivial`
    //  (Agent/presence_dtor.cpp) - restamps to `??_7CPresence@NTempest@
    //  @6B@`, frees through Storm if bit 0 of `flags` is set, no base
    //  teardown call.
    void* ScalarDeletingDestructorTrivial(unsigned int flags);

    void* m_vtable;         // +0x00 - ??_7CBhPoBehavior@NIpse@@6B@
    char  m_reserved04[4];  // +0x04 - intrusive-list next (see ~CBhPoBehavior)
    char  m_reserved08[4];  // +0x08 - intrusive-list prev (see ~CBhPoBehavior)
};

#endif

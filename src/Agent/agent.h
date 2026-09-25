//============================================================================
//  CAgent: the polymorphic, reference-counted object a handle in
//  sub_6F430C80's call tree ultimately resolves to, and CAgentPtr, the
//  type-checked smart pointer that owns one.
//
//  CAgent derives from CObserver (observer.h), established by the
//  CWidget__Constructor dump's sub_6F001730: it builds a CObserver base
//  subobject at offset 0 first (a real call to CObserver::CObserver()),
//  then overwrites the vtable pointer with CAgent's own - the standard
//  base-then-derived constructor shape - and its refcount lives at +0x04,
//  which is exactly where TRefCnt::m_refcount (refcnt.h) already sits
//  through that same inheritance.  ReleaseSelf below overrides
//  TRefCnt::ReleaseSelf (vtable slot 0) for exactly that reason - see
//  refcnt.h.  Slot 1 (TRefCnt::OnZeroRefCount, inherited, not overridden)
//  takes over the "reserved, never called, only here to hold a slot"
//  job the old `Reserved04` placeholder had; nothing else about CAgent's
//  own six-classes-later vtable slots changes.
//============================================================================
#ifndef AGENT_H
#define AGENT_H

#include "observer.h"

//  Forward declarations for the slot signatures below - the vtable
//  block declares slots whose parameter types are defined further
//  down the include graph, and a pointer only needs the name.
class CDataStore;
class CDataStoreScratch;

class CAgent : public CObserver
{
public:
    //====================================================================
    //  CAgent's vtable contribution.  Declaration order *is* vtable order
    //  (C++'s own rule), so this block is sorted by slot index and has
    //  to stay that way - inserting one line in the wrong place renames
    //  every slot after it.  Slots 0-23; see
    //  docs/targets/CWidget_CSelectable_CItem__vtables.md.
    //====================================================================
    //  Deliberately *not* a virtual destructor: the shipped call
    //  (AssignChecked, 0x6F022AA0) is `call [ecx][0]` with no second
    //  argument, where a real `delete` through a virtual dtor would push a
    //  flags word for the deleting-destructor thunk MSVC generates for that.
    //  Whatever is really behind this slot is not in this call tree, so it
    //  is named for what the call site does with it and left unimplemented.
    //  slot 0 / +0x00 (0x6F001F70)
    virtual void ReleaseSelf();
    //  slot 1 / +0x04 (0x6F0018A0, sub_6F0018A0)
    virtual CAgent* DeleteSelf(int flags);
    //  placeholder, until this session's vtable sweep found its body: it
    //  resolves this agent's own m_handle/m_typeTag pair and dispatches on
    //  a FourCC the caller supplies.  See agent_slot18.cpp.
    //  slot 6 / +0x18 (0x6F472AE0)
    virtual void Method_0x18(unsigned int selector, void* arg);
    //  slot 7 / +0x1C (0x6F471BD0, sub_6F471BD0)
    virtual unsigned int GetAgileTypeId() const;
    //  slot 8 / +0x20 (0x6F471BE0)
    virtual int Method_0x20(int context);
    //  slot 9 / +0x24 (0x6F471BF0)
    virtual int Method_0x24(int context);
    //  slot 10 / +0x28 (0x6F471C10)
    virtual int GetRecordVersion(int context);
    //  slot 11 / +0x2C (0x6F471C30, nullsub_1471).  Empty, but *not*
    //  argument-less: the shipped body is `retn 8`, so it cleans two stack
    //  dwords and every caller pushes them.  Declared `void()` this slot
    //  would have the callee pop eight bytes the caller never pushed - the
    //  unbalanced-stack crash tools/abi_audit.py exists to catch, and one
    //  it could not see here because a __declspec(naked) thunk has no
    //  `ret` for it to compare against.
    virtual void Method_0x2C(int a, int b);
    //  slot 12 / +0x30 (0x6F471C20, nullsub_1470).  Same story, `retn 4`.
    virtual void Method_0x30(int a);
    //  slot 13 / +0x34 (0x6F471C40, nullsub_418)
    virtual void Deactivate();
    //  slot 14 / +0x38 (0x6F472420)
    virtual void Save(class CDataStoreScratch* store);
    //  slot 15 / +0x3C (0x6F472C70)
    virtual void Load(class CDataStore* store);
    //  slot 16 / +0x40 (0x6F001850, nullsub_46)
    virtual void RefreshOwningPlayerColor();
    //  slot 17 / +0x44 (0x6F001860, nullsub_47)
    virtual void DumpState(void* sink);
    //  slot 18 / +0x48 (0x6F001870, nullsub_48)
    virtual void SaveFlags(CDataStoreScratch* store);
    //  slot 19 / +0x4C (0x6F001880, nullsub_49)
    virtual void LoadFlags(CDataStore* store);
    //  slot 20 / +0x50 (0x6F472550)
    virtual void Method_0x50(CDataStoreScratch* store, unsigned int msgId, int count);
    //  slot 21 / +0x54 (0x6F472DE0)
    virtual void Method_0x54(CDataStore* store, unsigned int msgId, int count);
    //  slot 22 / +0x58 (0x6F001890, sub_6F001890)
    virtual const char* GetClassName();
    //  0x6F472990 - vtable slot 23 (+0x5C).  Resolve this agent's own
    //  handle, check the resolved object's finer-grained kind tag against
    //  '+agl', and if it passes, hand it to a rendering-side gate.  Not
    //  virtual, per this repo's convention for slots reached by raw offset
    //  (widget.h); CItem's own slot 23 is a bare tail jump straight back
    //  into this body (item_vtable.cpp).  See agent_slot5c.cpp.
    //  slot 23 / +0x5C (0x6F472990)
    virtual void Method_0x5C();

    //  0x6F001730 - see agent_ctor.cpp.  Builds the CObserver base
    //  subobject, two sentinel fields (+0x0C/+0x10, both -1) and one more
    //  embedded CObserver member (+0x14) whose own refcount this
    //  constructor immediately bumps to 1.
    CAgent();

    //  0x6F0017F0 - CItem__Constructor dump (0x6F2B7520): destructs the
    //  embedded CObserver at +0x14 first (member destruction order is
    //  reverse of construction; the base CObserver subobject at offset 0
    //  is what the compiler then destructs implicitly right after this
    //  body, matching the shipped tail call into CObserver::~CObserver()
    //  with ecx=this unchanged).  Only reachable so far as the target of
    //  an exception-unwind funclet (CAgentWar3::~CAgentWar3(), agentwar3.h,
    //  tail-calls straight into it) - not through any live `delete` in
    //  either dump, consistent with agent.h's own note above that
    //  ReleaseSelf(), not a destructor call, is this codebase's real
    //  teardown path for a CAgent.  Deliberately *not* virtual, for the
    //  same reason ReleaseSelf below is not: nothing in either call tree
    //  reaches it through the vtable, and adding one would shift every
    //  slot after it.
    ~CAgent();


    //  Slots 2-5 (Dispatch and three more) moved to CObserver this session
    //  - see observer.h for why; the numbering is unchanged.




    //====================================================================
    //  Slot 6's nine handlers (agent_slot18.cpp / agent_notify.cpp).  Each
    //  is a real member at its own address in the 0x6F472xxx module; slot 6
    //  picks between them on a FourCC selector.  Named for the selector
    //  that reaches them, since nothing else in these three vtables says
    //  what any of them is for.
    //====================================================================
    void Handler_lkl(void* payload, void* arg);   // 0x6F472A00, '+lkl`'
    void Handler_lcf(void* payload);              // 0x6F472010, '+lcf`'
    void Handler_oep(void* payload, void* arg);   // 0x6F4726A0, '+oep`'
    void Handler_ltr(void* payload);              // 0x6F472040, '+ltr`'
    void Handler_eip(void* payload, void* arg);   // 0x6F4725D0, '+eip`'
    //  0x6F495530 / 0x6F495600 / 0x6F495660 - the pathfinder's own senders
    //  for the three event codes NotifyMessage65/67/68 below answer; see
    //  pathagentpost.cpp.
    void PostMessage65();                         // 0x6F495530
    void PostMessage67();                         // 0x6F495600
    void PostMessage68();                         // 0x6F495660

    void NotifyMessage65();                       // 0x6F4720D0
    void NotifyMessage66();                       // 0x6F472100
    void NotifyMessage67();                       // 0x6F472130
    void NotifyMessage68();                       // 0x6F472160

    //  0x6F471A70 - a forwarder to Dispatch() with a fixed message id.  Out
    //  of line at its own address rather than inlined into Dispatch's
    //  callers, exactly as the shipped code has it.
    void NotifyRegistered(unsigned int arg1, void* arg2);

    //  0x6F4728D0 (`retn 4`) - point this agent's tracked-target link at
    //  `target`, or clear it when `target` is null.  Both sides go through
    //  LookupHandle on their own +0x0C/+0x10 pair with the same 'lga+'
    //  branchless tag check every other reader in this family uses, and the
    //  link itself is threaded through the target's own +0x78 chain; a null
    //  target takes the detach path through this agent's +0x2C instead.
    //  Read out of the live image with `ida_query bytes` rather than out of
    //  a dump - it is one level below JASS_IssueTargetOrderById's own depth
    //  cut - and redirected rather than reconstructed for that reason
    //  (Unit/unitorderpaththunks.cpp).  Its one call site here is
    //  CTaskActionFields::Init.
    void SetTrackedTarget(void* target);

    //  0x6F471A40 (`retn 8`) - `add ecx,14h / jmp 0x6F62A570`, four bytes
    //  plus a tail jump: post one event to this agent's *own* embedded
    //  CObserver at +0x14 (CObserver::PostEvent, observerpostevent.cpp).
    //  The same adjustor-forward shape its immediate neighbour
    //  SModifierSubject::AddValueListener (0x6F471A30) has, and the reason
    //  both exist out of line is that +0x14 is a member sub-object, not a
    //  base.  Not in any dump - the address and its `retn 8` were read out
    //  of the live image with `ida_query bytes` - so it is not scored; its
    //  one call site is CAgent::Handler_oep (agent_bighandlers.cpp), which
    //  posts the value it just notified with a null target.
    //  See agent_selfobserverpost.cpp.
    void PostEventToSelfObserver(unsigned int event, void* target);

    //  +0x0C / +0x10 - both set to -1 ("not yet bound") by the constructor.
    //  Named from a *different*, already-matched target
    //  (sub_6F430C80/agentregistry.h): CUnitAgent - a CAgent-derived class
    //  that dump's own evidence placed a "handle" and a "type tag" at
    //  these exact absolute offsets - previously declared as CUnitAgent's
    //  own fields, on the assumption that nothing sat between CAgent's
    //  vtable+refcount and them.  This dump's sub_6F001730 shows there is
    //  something there (the CObserver base's own +0x08 field) but it ends
    //  at +0x0C either way, so the absolute offsets - and every already-
    //  matched instruction that reads them - land unchanged; only which
    //  class declares the fields moves.  See agentregistry.h.
    unsigned int    m_handle;
    unsigned int    m_typeTag;

    //  +0x14 - a second, embedded CObserver (distinct from the base
    //  subobject at offset 0), placement-constructed inside CAgent::CAgent
    //  itself (agent_ctor.cpp) rather than declared as a plain typed
    //  member: an ordinary member's constructor runs implicitly before the
    //  owning constructor's body starts, ahead of the +0x0C/+0x10 stores -
    //  not the order sub_6F001730 has it in (base ctor, vtable store,
    //  +0x0C/+0x10 stores, *then* this member's ctor call, then its
    //  refcount bump).  The constructor immediately bumps its refcount to
    //  1 once placement-new returns, i.e. CAgent starts out holding one
    //  reference to its own embedded observer.  Nothing in either call
    //  tree reads it back beyond that.
    unsigned char   m_selfObserverStorage[sizeof(CObserver)];
};

//----------------------------------------------------------------------------
//  A type-checked, reference-counted CAgent*.  AssignChecked is the general
//  shape a whole family of these pointer types share in this binary - only
//  the "what type must this be" leaf differs per instantiation (compare
//  GetAgileRootTypeId, 0x6F471BC0, reached from this exact assignment).
//----------------------------------------------------------------------------
class CAgentPtr
{
public:
    //  Reached only as part of AGENT_SLOT's implicit copy/destroy when the
    //  agent-slot array moves (sub_6F42B060 - see the release-if-last loops
    //  there): the same test-decrement-call and test-increment this file's
    //  AssignChecked already has, so it is written the same way rather than
    //  guessed at from a different angle.
    CAgentPtr() : m_ptr(0) {}
    CAgentPtr(const CAgentPtr& other) : m_ptr(other.m_ptr)
    {
        if (m_ptr)
            m_ptr->m_refcount++;
    }

    //  0x6F02F780 - the converting constructor sub_6F42CB50 placement-
    //  constructs a fresh AGENT_SLOT's CAgentPtr with.  See agentregistry.cpp
    //  for why its body is wrapped in a __try/__finally that no other
    //  constructor here needs.
    CAgentPtr(CAgent* obj);

    ~CAgentPtr()
    {
        Release();
    }

    //  0x6F022AA0 - assign obj if it is null or derives from the agile root
    //  type, otherwise clear the pointer.  Returns `this` (an MSVC ABI
    //  quirk of some assignment-operator-shaped member functions, not a
    //  reference), matching every call site touched by this call tree.
    CAgentPtr* AssignChecked(CAgent* obj);

    CAgent*    m_ptr;    // 0x00

private:
    //  The release-if-last test-decrement-call three instructions shared by
    //  the destructor, AssignChecked's old-pointer drop, and the converting
    //  constructor's __finally (0x6F022820, appended right after
    //  sub_6F02F780's own body in the dump - the same shape, standing on
    //  its own because a __finally block cannot inline a member the way
    //  ordinary control flow can).  Named and factored out for that last
    //  use; the destructor and AssignChecked keep their own inline copies
    //  since nothing needs them out of line.
    void Release()
    {
        if (m_ptr)
        {
            if (--m_ptr->m_refcount == 0)
                m_ptr->ReleaseSelf();
        }
    }
};

//----------------------------------------------------------------------------
//  One entry of sub_6F430C80's flat agent-slot array (0x6F42CB50's growable
//  array): a 12-byte record with the CAgentPtr in the middle, flanked by two
//  plain, un-refcounted dwords - sub_6F42B060's element move copies the
//  first and third fields verbatim and only add-refs/releases the middle
//  one.  Neither flanking field is read by anything reconstructed this pass
//  (sub_6F42B060/sub_6F42CB50 are not - see docs/msvc-vc8-idioms.md), so
//  they stay unnamed; only the size and the smart pointer's position are
//  established, which is what TSGrowableArray<AGENT_SLOT>::ComputeChunk
//  (0x6F427550) needs.
//----------------------------------------------------------------------------
struct AGENT_SLOT
{
    unsigned int    m_reserved00;
    CAgentPtr       m_agent;         // 0x04
    unsigned int    m_reserved08;
};

#endif

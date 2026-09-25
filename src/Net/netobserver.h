//============================================================================
//  CNetObserver - the class this repo's own CNetPlayerRecord::CNetPlayerRecord
//  (0x6F54FAB0, netplayerrecordctor.cpp) actually constructs at its own
//  offset 0 (not the plain `CObserver m_pauseObserverList`/`m_uiObserverList`
//  Net/netdata.h's own CNetPlayerRecord declares at +0x04/+0x10) - found by
//  the cnd-ctor-agent-2 session while chasing CNetData::CNetData's own
//  prerequisite member subobjects (docs/notes/checksum-provider-registry.md,
//  "cnd-ctor-agent session").
//
//  Stamps its own one-slot `??_7CNetObserver@@6B@` at its own offset 0,
//  then reuses the already-reconstructed CObserver::CObserver (0x6F629A70,
//  Agent/observer.h) twice as a plain field-initializer for two embedded
//  CObserver members - *not* a base-class construction (see the
//  destructor/vtable note further down for how a follow-up session
//  resolved the ambiguity a first read of this constructor's disassembly
//  left open).  Net/netdata.h's own `CNetPlayerRecord` already types
//  its +0x00..+0x134 span as `m_reserved00`/`m_pauseObserverList`/
//  `m_uiObserverList`/`m_reserved1C`, and several already-`EXACT`/
//  `IDENTICAL`-scored functions (FireToObserverLists, DispatchToUiObservers)
//  read those two members by name at those exact offsets - retyping
//  CNetPlayerRecord's own declared layout to embed a `CNetObserver` risks
//  exactly nothing for those two functions (the final bytes are identical
//  either way) but is real, separate churn this session's own scope does
//  not need to take on.  Instead, CNetPlayerRecord::CNetPlayerRecord()
//  placement-constructs a `CNetObserver` directly over its own `this` (see
//  netplayerrecordctor.cpp) - same final memory, no header change to
//  netdata.h's own CNetPlayerRecord class.
//
//  Full real layout, confirmed field-for-field off 0x6F548EA0's own body
//  (0x134 bytes total - matches CNetPlayerRecord's own gap from +0x00 to
//  +0x134 exactly):
//    +0x00  vtable (??_7CNetObserver@@6B@)
//    +0x04  CObserver m_observer1      (0xC bytes - the "pause" list)
//    +0x10  CObserver m_observer2      (0xC bytes - the "UI" list)
//    +0x1C  SFixedPoolHeader m_pool    (0x14 bytes - constructed with
//                                        elementSize=0x1C, capacity=8;
//                                        Agent/fixedpoolheader.h)
//    +0x30  unsigned int m_slotCapacity  (=0x40, a redundant count cache -
//                                          the same value pushed as the
//                                          array's own element count below)
//    +0x34  unsigned int m_slots[0x40]  (0x100 bytes, ending +0x134) - a
//           64-slot array, each slot either null or the address of a
//           self-linked sentinel `CNetObserver::CallbackObserver` node
//           heading that slot's own singly-linked (next-only, its own
//           +0x0C) list; default-constructed to null (CNetObserverSlotElemCtor,
//           0x6F538390: "zero this dword, return").  The destructor
//           (below) fully drains every slot's list by hand before this
//           array's own `eh vector destructor iterator` pass runs, so
//           `CNetObserverSlotElemDtor` (0x6F53F900) - still a naked thunk
//           to the real per-element destructor, since by the time it runs
//           every slot is already back to null and its own body is a
//           no-op - never actually does anything live in this closure.
//
//  Both the constructor (0x6F548EA0) and the destructor (0x6F545040) are
//  real reconstructions now - see netobserver.cpp.  The destructor's own
//  per-slot doubly-linked unlink loop drains each of the 64 slots, posting
//  an event and deleting every node through the object's own vtable, then
//  releases the pool and tears down both embedded CObserver members.  Each
//  node is a `CNetObserver::CallbackObserver` (declared as a nested class
//  just above) -
//  confirmed by the drain's own `ReleaseAllBlocks()` call, which tags the
//  release with that exact RTTI type-descriptor-name string
//  (".?AVCallbackObserver@CNetObserver@@", 0x6FA89D6C) - allocated out of
//  `m_pool` below (CDataAllocator-shaped, elementSize=0x1C matching that
//  class's own size exactly).
//
//  CNetObserver's own vtable (`??_7CNetObserver@@6B@`, 0x6F958814) has
//  exactly one slot - confirmed off `agent_worktrees/classes/0x6F958814.json`
//  (`informer.methods: 1`, and no unrelated symbol sits within 4 bytes of
//  it in `agent_worktrees/names.json`) - the scalar deleting destructor,
//  0x6F548F40, `CNetObserver::DeleteSelf()` below.  This is *not* the
//  "public CObserver" base the constructor's own reasoning once assumed:
//  the constructor's own raw bytes stamp this one-slot vtable at `this+0`
//  and only *afterward* placement-construct two independent CObserver
//  members at `this+4`/`this+0x10` (reusing CObserver::CObserver as a
//  plain field-initializer, not as a base-class constructor call) - which
//  is exactly what the members below already declare.
//============================================================================
#ifndef NETOBSERVER_H
#define NETOBSERVER_H

#include "observer.h"

class CNetObserver
{
public:
    //  CNetObserver::CallbackObserver - the node type ~CNetObserver()'s own
    //  per-slot drain loop (netobserver.cpp) walks and frees.  Confirmed via
    //  RTTI: the type-descriptor-name string that same drain's own
    //  ReleaseAllBlocks() call tags its pool release with
    //  (".?AVCallbackObserver@CNetObserver@@", 0x6FA89D6C) and Class
    //  Informer's own hierarchy row for the vtable at 0x6F9582A8
    //  (agent_worktrees/classes/0x6F9582A8.json:
    //  "CNetObserver::CallbackObserver: CObserver, TRefCnt").  Size 0x1C
    //  matches the drain loop's own pool element size exactly (m_pool
    //  constructed with elementSize=0x1C below).  Declared as a real nested
    //  class (rather than forward-declared here and defined out-of-line
    //  with `class CNetObserver::CallbackObserver { ... }`) so
    //  `tools/class_hierarchy_audit.py`'s own single-line `class NAME : ...`
    //  regex does not mistake the qualified name's second `::` colon for a
    //  base-list separator and misattribute this class's own `CObserver`
    //  base to CNetObserver instead.
    //
    //  Only the two slots the drain chain actually reaches are overridden
    //  here, confirmed off that same vtable's real slot targets (index 1
    //  and index 3): CObserver's own Dispatch/Method_0x10/Method_0x14
    //  (slots 2/4/5) are inherited unmodified.  Nothing in this closure
    //  constructs one (the registration path that fills m_slots below lives
    //  elsewhere, out of this session's own CNetObserver-drain scope), so
    //  no constructor is declared.
    //
    //    +0x00  CObserver base (0xC bytes: vtable, TRefCnt::m_refcount,
    //           CObserver::m_resource - unused by anything in this closure)
    //    +0x0C  m_next       - intrusive next-pointer; the drain loop's own
    //                          per-slot list is threaded through this
    //                          field, each slot header being a self-linked
    //                          sentinel node of this same type
    //    +0x10  m_eventId    - posted to m_observer1 via CObserver::PostEvent()
    //                          as this node is drained
    //    +0x14  m_callback   - __fastcall(msg, userdata) callback, invoked
    //                          by Method_0x0C below
    //    +0x18  m_userdata   - opaque second argument to m_callback
    class CallbackObserver : public CObserver
    {
    public:
        //  0x6F53DF50 - own translation unit
        //  (netobservercallback_deleteself.cpp) for the same reason
        //  observer_deleteself.cpp is: the base ~CObserver() call below
        //  must stay a real `call`, not inline.
        virtual CObserver* DeleteSelf(int flags);

        //  0x6F5375B0 - tail-invoke the stored callback with (msg,
        //  m_userdata); `this` itself is otherwise unused once the two
        //  fields are read.
        virtual int Method_0x0C(const struct SAgentMessage* msg);

        void*                                                m_next;      // +0x0C
        unsigned int                                         m_eventId;   // +0x10
        int (__fastcall *m_callback)(const struct SAgentMessage*, void*); // +0x14
        void*                                                m_userdata;  // +0x18
    };

    //  0x6F548EA0 - real reconstruction, netobserver.cpp.
    CNetObserver();

    //  0x6F545040 - real reconstruction, netobserver.cpp.
    ~CNetObserver();

    //  0x6F548F40 - CNetObserver's own (and only) vtable slot: the scalar
    //  deleting destructor.  Own translation unit
    //  (netobserverdeleteself.cpp) so the `~CNetObserver()` call above
    //  stays a real out-of-line call rather than inlining - same reasoning
    //  as `observer_deleteself.cpp`'s own header comment.
    virtual CNetObserver* DeleteSelf(int flags);

    char         m_observer1[0xC];  // +0x04 - CObserver, embedded raw
                                     //   (Agent/observer.h's own CObserver
                                     //   is constructed in place by the
                                     //   .cpp; kept as raw bytes here so
                                     //   this header does not have to
                                     //   #include observer.h just to name
                                     //   the type of a member nothing
                                     //   outside the .cpp reads by field).
    char         m_observer2[0xC];  // +0x10 - CObserver, embedded raw
    char         m_pool[0x14];      // +0x1C - SFixedPoolHeader, embedded
                                     //   raw for the same reason
    unsigned int m_slotCapacity;    // +0x30
    unsigned int m_slots[0x40];     // +0x34, ending +0x134
};

#endif

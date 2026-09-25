//============================================================================
//  The unit-order layer: what an order object is, how one is made, and the
//  handful of CUnit members that take one.
//
//  Everything here below MakeOrderAgent sits under JASS_SetUnitPosition's
//  own dump depth cut - the only evidence for each signature is its call
//  site in CUnit::SubmitOrder (unitordersubmit.cpp) plus the `retn <n>`
//  read out of the shipped image, which is what fixes each argument count.
//  Every one of them is a `this`-in-ecx member: getting that wrong is the
//  callee-cleanup crash class CLAUDE.md's thunk-ABI table records, not a
//  matter of taste.
//============================================================================
#ifndef UNITORDER_H
#define UNITORDER_H

#include "itemhandleresolve.h"

class CAgent;

//  851972.  The JASS order id "stop", and the only one this tree makes.
const int kUnitOrderStop = 0xD0004;

//  0x6F294A40 - unitordermake.cpp.
CAgent* __fastcall MakeOrderAgent(int orderId, int target,
                                  const void* sourceRef);

//  The three CUnit fields the order path reads, at their exact offsets.
//  Declared as a view rather than grown onto unit.h's own CUnit, which is
//  fixed up to a different extent and must not sprout a second,
//  contradicting tail.
struct SUnitOrderState
{
    char m_reserved00[0x5C];
    //  +0x5C - bit 8 refuses every order outright.
    int  m_orderFlags;            // +0x5C
    char m_reserved60[0x138];
    //  +0x198 - how many orders are already queued.
    int  m_queuedOrderCount;      // +0x198
    //  +0x19C - the {handle, typeTag} pair naming whatever the current
    //  order is aimed at, with itemhandleresolve.h's own all-ones "not
    //  set" sentinel.
    SOptionalHandleRef m_orderTargetRef;   // +0x19C
};


//----------------------------------------------------------------------------
//  The order classes.  The binary names them itself: `COrder`,
//  `COrderPoint`, `COrderPoint2`, `COrderTarget`, `COrderTarget2` are RTTI
//  strings in the image (0x6F92F788 ff.), and each one has an agile type id
//  in one block of `mov eax,<imm32>` / `retn` leaves 0x80 apart starting at
//  GetOrderAgileTypeId:
//
//      0x6F2712B0  '+ord'  0x2B6F7264   COrder        the base agile type
//      0x6F271330  'ord.'  0x6F72642E   COrderPoint
//      0x6F2713B0  'or.2'  0x6F722E32   COrderPoint2
//      0x6F271430  'ordt'  0x6F726474   COrderTarget
//      0x6F2714B0  'ort2'  0x6F727432   COrderTarget2
//
//  Read most significant byte first, the same way every other agile type id
//  in this binary is (misc_rawcode_getters.cpp).  The fourth character is
//  the variant - '.' for a point, 't' for a target - and it moves one place
//  left in the "2" form to make room for the digit, which is why a '.'
//  turns up where a letter would.  What fixes each tag to its class is what
//  builds it: MakeOrderPointAgent (0x6F294B30) makes an 'ord.' and fills in
//  a point, MakeOrderTargetAgent (0x6F294D40) makes an 'ordt' and fills in
//  a point *and* a target widget.
//
//  Two further tags in the same block belong to the parallel CTask* family
//  (CTask, CTaskParam, CTaskRotate, CTaskAction, ... - RTTI at 0x6F92F7F4):
//  0x6F271940 is 'tskA', CTaskAction's, and CUnit::FinishCurrentOrder is
//  the only thing here that makes one.
//----------------------------------------------------------------------------

//  Only touched members are named; everything else is a sized gap, and a
//  member with no established meaning keeps unit.h's own `m_intNN` /
//  `m_refNN` convention rather than a guessed name.

//  What sits at COrder+0x44 and +0x4C: an eight-byte object with a vtable
//  word of its own and one CFloat-shaped word behind it, written through
//  slot 0 and read back straight out of the field.  Spelled as a plain
//  vtable word and a hand-written dispatch rather than as a `virtual`,
//  for the reason agentdaynight.cpp's FloatListener gives: the class's own
//  slots are all outside this tree, so declaring them would make this
//  build emit a vtable it cannot fill.
struct SOrderCoord
{
    const void* m_vtable;   // +0x00
    int         m_value;    // +0x04
};

//  slot 0 - store a value, with a "tell whoever is watching" flag.  Both
//  call sites push the value and a literal 1 and clean nothing, so it is a
//  two-argument __thiscall.
typedef void (__thiscall *OrderCoordSetFn)(SOrderCoord*, const void* value,
                                           int notify);

inline void SetOrderCoord(SOrderCoord* coord, const void* value)
{
    ((OrderCoordSetFn)(*(void***)coord)[0x00 / 4])(coord, value, 1);
}

//  COrder - the base.  MakeOrderAgent (unitordermake.cpp) already names
//  +0x24, +0x28 and +0x38; the rest arrived with COrder::Init (0x6F2860B0).
struct COrder
{
    char m_reserved00[0x20];
    //  +0x20 - COrder's own flag byte.  Bit 2 is cleared when the order
    //  stops being the one its target is tracking (CUnit::NotifyOrderTarget)
    //  and bit 7 when it starts running (CUnit::BeginOrder).
    unsigned int       m_flags20;      // +0x20
    int                m_orderId;      // +0x24
    int                m_owningPlayer; // +0x28
    //  +0x2C - the next order in the unit's queue, as a {handle, typeTag}
    //  pair.  The queue is intrusive: CUnit+0x19C is the head and
    //  CUnit+0x1A8 the tail.
    SOptionalHandleRef m_next;         // +0x2C
    char m_reserved34[0x04];
    //  +0x38 - what issued the order; matched against each candidate
    //  ability's own source when one is picked.
    SOptionalHandleRef m_sourceRef;    // +0x38
    char m_reserved40[0x04];
    SOrderCoord        m_x;            // +0x44
    SOrderCoord        m_y;            // +0x4C
    //  +0x54 - a refcounted object the order holds for as long as it lives.
    CAgent*            m_pOwner;       // +0x54

    //  0x6F2860B0 (`retn 18h`) - unitorderinit.cpp.
    void Init(int orderId, int player, const void* sourceRef, const void* x,
              const void* y, CAgent* owner);
};

//  COrderTarget - COrder plus the widget the order is aimed at.
struct COrderTarget
{
    COrder             m_base;         // +0x00
    SOptionalHandleRef m_target;       // +0x58
    char m_reserved60[0x04];
    CAgent*            m_pRef64;       // +0x64 - refcounted, same shape as
                                       //   COrder's own +0x54

    //  0x6F286180 (`retn 20h`) - unitordertargetinit.cpp.
    void Init(int orderId, int player, const void* sourceRef,
              const void* target, const void* x, const void* y,
              CAgent* owner, CAgent* tracker);
};

//----------------------------------------------------------------------------
//  CTaskActionFields - a raw field-overlay view onto a CTaskAction object
//  (the real class now lives in Task/taskaction.h, reconstructed in a
//  later session; this predates it and is kept as a lightweight overlay
//  rather than pulled apart, since every user here only ever reaches a
//  CTaskAction through CUnit's own +0x174 intrusive chain, never through
//  a typed pointer the real class's own header would need to be pulled
//  in for). A unit's tasks hang off CUnit+0x174 in the same intrusive
//  shape its orders hang off +0x19C, each one linked through its own
//  +0x24 pair. Named distinctly from `class CTaskAction` (Task/taskaction.h)
//  so the two never collide in a tool that indexes declarations by name
//  (e.g. class_hierarchy_audit.py) - they describe the same shipped object
//  from two different vantage points, not two different classes.
//----------------------------------------------------------------------------
struct CTaskActionFields
{
    char m_reserved00[0x24];
    SOptionalHandleRef m_next;     // +0x24
    char m_reserved2C[0x04];
    int  m_orderId;                // +0x30
    void* m_pOwner;                // +0x34

    //  0x6F28E3C0 (`retn 8`) - unittaskactioninit.cpp.
    void Init(void* trackedTarget, void* owner);
};

//  852322.  The order id CUnit::FinishCurrentOrder stamps into the
//  CTaskAction it makes, and the id of the event it announces with it.
const int kUnitOrderFinishTask = 0xD0162;

//----------------------------------------------------------------------------
//  What an ability looks like from the order path.  Every unit keeps its
//  abilities as an intrusive list: CUnit+0x1DC is the head and each
//  ability's own +0x24 is the next link, both as {handle, typeTag} pairs.
//  Nothing here reconstructs an ability; the five vtable slots the order
//  path dispatches through are declared for their argument counts and
//  nothing else.
//----------------------------------------------------------------------------
struct SUnitAbility
{
    char m_reserved00[0x24];
    SOptionalHandleRef m_next;   // +0x24
    char m_reserved2C[0x10];
    //  +0x3C / +0x40 / +0x44 - three counters that all have to be
    //  non-positive before the ability will take an order.  A positive
    //  +0x40 answers 0xDD and a positive +0x3C or +0x44 answers 0xB9; no
    //  dump here says what any of the three counts, so they keep unit.h's
    //  own honest `m_intNN` convention.
    int m_int3C;                 // +0x3C
    int m_int40;                 // +0x40
    int m_int44;                 // +0x44
};

//----------------------------------------------------------------------------
//  The event record the order path builds on the stack and hands to its own
//  observers.  Four words: the vtable, an always-zero word, the message id
//  and the order the message is about.  Same shape agentdaynight.cpp's
//  SDayNightEvent has, with the fourth word used.
//----------------------------------------------------------------------------
struct SOrderEvent
{
    const void*  m_vtable;      // +0x00
    int          m_reserved04;  // +0x04
    unsigned int m_msgId;       // +0x08
    void*        m_pOrder;      // +0x0C
};

//: ??_7CEvent@@6B@ at 0x6F877928 - the vtable the record is stamped with,
//: as a data symbol rather than a C++ class for the reason
//: agentdaynight.cpp gives: the class's own slots are outside this tree and
//: a `virtual` declaration would make this build emit a vtable it cannot
//: fill.
extern void* const g_vftCEvent;
//: ??_7TRefCnt@@6B@ at 0x6F8765F8 - stamped back over the same word when
//: the record goes out of scope.  That store is the shipped ~CEvent's own
//: inlined base-class destructor and it is in the instruction stream, so it
//: is written out here rather than left to a destructor this build cannot
//: reproduce.
extern void* const g_vftTRefCnt;

//  The three message ids the order path announces.
const unsigned int kEventOrderCancelled = 0xD02A6;
const unsigned int kEventOrderIssued    = 0xD02A5;
const unsigned int kEventOrderFinished  = 0xD0144;

//  Slot 4 (+0x10) on a CAgent - hand an event record to whatever is
//  observing this object.  One argument, nothing cleaned by the caller.
typedef void (__thiscall *PostAgentEventFn)(void*, void*);

//  Slot 23 (+0x5C) on an order object - "you are finished, let go".  No
//  arguments: every call site is a bare `call eax`/`jmp eax` with nothing
//  pushed and nothing cleaned.
typedef void (__thiscall *OrderReleaseFn)(void*);

//  slot 0x22C - is this ability already carrying `order` out?
typedef int (__thiscall *AbilityIsRunningOrderFn)(void*, void*);
//  slot 0x248 - how good a match this ability is for `orderId`.  The
//  candidate with the *lowest* answer wins, and the first one wins a tie.
typedef int (__thiscall *AbilityOrderPriorityFn)(void*, int orderId);
//  slot 0xA0 - what the order's own +0x38 source ref is matched against:
//  null for an ability the unit owns outright, the granting object
//  otherwise.
typedef void* (__thiscall *AbilityGetSourceAgentFn)(void*);
//  slot 0x25C - can this ability take `orderId` at a point?  0 means yes.
typedef int (__thiscall *AbilityCheckPointOrderFn)(void*, int orderId,
                                                   int player, const void* x,
                                                   const void* y, int flags);
//  slot 0x260 - ... and at a widget target.
typedef int (__thiscall *AbilityCheckTargetOrderFn)(void*, int orderId,
                                                    int player, void* target,
                                                    int flags);

//----------------------------------------------------------------------------
//  "Resolve this {handle, typeTag} pair, or null if it is not set."  The
//  shipped shape is a `cmp`/`jnz` over a `xor eax,eax` and the call, and it
//  is inlined at every one of its many sites in this path rather than being
//  a function of its own - so it lives in the header.  The type tag is
//  loaded first and the handle second, which is the order unithandleref.h
//  already records for the same test.
//----------------------------------------------------------------------------
inline void* ResolveHandleRefOrNull(SOptionalHandleRef* ref)
{
    if ((ref->m_typeTag & (int)ref->m_handle) == -1)
        return 0;
    return QueryHandleField0x54(ref);
}

//  0x6F0419C0 - SOptionalHandleRefResolver::ResolveChained, already
//  reconstructed (handlereref_resolve.cpp); re-declared here the way
//  unitordermake.cpp and agentregistrysingleton.cpp both re-declare it, so
//  the mangled name is the same symbol and not a second one.
struct SOptionalHandleRefResolver : SOptionalHandleRef
{
    SOptionalHandleRef& ResolveChained(const void* ref);
};

//  The order queue refuses to grow past this.
const int kOrderQueueLimit = 0x1F4;   // 500

//  0x6F294B30 - unitorderpointmake.cpp.
CAgent* __fastcall MakeOrderPointAgent(int orderId, int player,
                                       const void* sourceRef, const void* x,
                                       const void* y, void* owner);

//  0x6F294D40 - unitordertargetmake.cpp.
CAgent* __fastcall MakeOrderTargetAgent(int orderId, int player,
                                        const void* sourceRef, void* target,
                                        const void* x, const void* y,
                                        void* owner, void* ref64);

#endif

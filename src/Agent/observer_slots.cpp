//============================================================================
//  CObserver's vtable slots 2, 4 and 5.  See observer.h.
//
//  All three sit in the 0x6F629xxx/0x6F62Axxx module with
//  CObserver::CObserver (0x6F629A70) and ~CObserver (0x6F62A9D0), and none
//  of them calls another, so one translation unit reproduces all three.
//  Slot 3's own implementation is CAgentWar3's, over in the 0x6F001xxx
//  module (agentwar3_slots.cpp).
//============================================================================
#include "observer.h"
#include "observereventreg.h"
#include "dataallocator.h"

//----------------------------------------------------------------------------
//  The registration record CObserver::m_resource points at.  Both methods
//  below are real methods of CObserverEventList (observereventreg.h) -
//  EnsureResource's `m_resource` is placement-constructed as exactly that
//  type (observer_resource.cpp), and both the bucket-table fields these two
//  read/write and the `retn 0Ch`/`retn 8` shapes line up with it exactly.
//  `SObserverRecord` is kept as the caller-facing name (observer_slots.cpp
//  is CObserver's own file, and neither slot 2 nor slot 5 needs to know the
//  registration table's real type) but is otherwise the same object.
//
//  Real thiscall members and not __fastcall free functions: both take every
//  argument past `this` on the stack (`retn 0Ch` and `retn 8`), where
//  __fastcall would have taken the first of them in edx and cleaned four
//  bytes less.  Getting that wrong is exactly the unbalanced-stack crash
//  tools/abi_audit.py exists to catch, and here it also costs Dispatch its
//  tail jump.
//----------------------------------------------------------------------------
struct SObserverRecord
{
    //  0x6F62A820
    void Broadcast(unsigned int msg, unsigned int arg1, void* arg2);
    //  0x6F62A5D0
    int Invoke(unsigned int msgId, SAgentMessage* msg);
};

//  The pool individual registration nodes come from - the same
//  g_observerEventNodePool observereventreg.cpp already declares extern
//  over unk_6FACE9EC.  Declared here too rather than pulled through a shared
//  header: every TU that reaches this DATA symbol extern-declares it
//  locally in this codebase (observer.cpp and observer_resource.cpp do the
//  same with g_observerResourcePool) so that build_mix.py's gen_data.asm
//  binding - keyed on the symbol coming back EXTRN - never has a second,
//  competing definition to contend with (see observer.cpp's own comment on
//  why a *definition* here would be a live-crash regression).
extern CDataAllocator g_observerEventNodePool;   // unk_6FACE9EC

//  The refcount-then-vtable[0]-call idiom this module shares with
//  observereventreg.cpp's own ReleaseResourceRef, CObserver::~CObserver's
//  resource release and CAgentPtr::Release (agent.h): drop the reference,
//  and once it reaches zero, let the object react through its own vtable
//  slot 0.  A local `static` copy rather than a call to the one in
//  observereventreg.cpp - that one is a different translation unit's
//  internal helper, and the shipped code repeats these five instructions at
//  every site instead of factoring them out, so this file does too.
typedef void (__thiscall *ReleaseResourceFn)(void*);

static void ReleaseResourceRef(void* res)
{
    if (res)
    {
        if (--*(int*)((char*)res + 4) == 0)
        {
            void** vtbl = *(void***)res;
            ((ReleaseResourceFn)vtbl[0])(res);
        }
    }
}

//----------------------------------------------------------------------------
//  0x6F62A820.  Register (or refresh) `resource` under `msgId` in this
//  observer's CObserverEventList-shaped bucket table.
//
//  Walks `msgId`'s bucket looking for a node that already carries this exact
//  (key, resource) pair.  On the way, any dead node it passes (m_resource
//  already null - a previous owner having released its own reference) is
//  garbage-collected right there, unlinked from the chain and handed back to
//  `g_observerEventNodePool`, unless `m_staticStorage` says this table's
//  storage is borrowed and not ours to shrink.
//
//  Three outcomes, converging on one tail:
//    - exact (key, resource) match found: `skipInsert` is set and no new
//      node is linked in or counted;
//    - a *different* resource under the same key: `keyFound` is set (so the
//      final NoteInsert() below is skipped - the shipped code does not grow
//      the table's live count for this case) but a fresh node is still
//      linked in behind it, so the same key can carry more than one
//      registration;
//    - neither: a fresh node is allocated and spliced onto the bucket's
//      circular chain (or becomes a new one-element chain when the bucket
//      was empty), and NoteInsert() runs.
//
//  The shared tail always overwrites `node->m_value`, always bumps
//  `resource`'s own refcount (even on the exact-match path, where the
//  immediately following release of the *old* m_resource - the same
//  object - cancels it back out; the shipped code does not special-case
//  that), releases whatever the node's previous resource was, and stores
//  the new one.
//----------------------------------------------------------------------------
void SObserverRecord::Broadcast(unsigned int msgId, unsigned int value,
                                 void* resource)
{
    CObserverEventList* self = (CObserverEventList*)this;
    unsigned int mask = (unsigned int)self->m_bucketCount - 1;
    ObserverEventReg** slot =
        (ObserverEventReg**)self->m_buckets + (mask & msgId);

    ObserverEventReg* prev = *slot;                    // the bucket's tail
    ObserverEventReg* cur  = prev ? prev->m_next : 0;   // its head
    ObserverEventReg* node = cur;

    int keyFound   = 0;
    int skipInsert = 0;

    if (cur == 0)
        goto insertNew;

    for (;;)
    {
        void* curResource = cur->m_resource;
        if (curResource != 0)
        {
            if (cur->m_key == msgId)
            {
                keyFound = 1;
                if (curResource == resource)
                {
                    skipInsert = 1;
                    node = cur;
                    goto deliver;
                }
            }
        }
        else if (self->m_staticStorage == 0)
        {
            //  a dead node found mid-walk: unlink it and hand it back to
            //  the node pool before moving on.
            ObserverEventReg* next = cur->m_next;
            prev->m_next = next;
            ObserverEventReg* bucketTail = *slot;
            ObserverEventReg* advanced;
            if (cur == bucketTail)
            {
                if (prev == bucketTail)
                {
                    *slot = 0;
                    prev = 0;
                }
                else
                {
                    *slot = prev;
                }
                advanced = 0;
            }
            else
            {
                advanced = next;
            }
            cur->m_next = 0;

            ReleaseResourceRef(cur->m_resource);
            g_observerEventNodePool.Free(cur, 0, 0);

            cur = advanced;
            node = cur;
            if (cur != 0)
                continue;
            break;
        }

        if (cur == *slot)
        {
            cur = 0;
        }
        else
        {
            prev = cur;
            cur = cur->m_next;
        }
        node = cur;
        if (cur == 0)
            break;
    }

insertNew:
    {
        void* raw = g_observerEventNodePool.Alloc(0, ".?AUObserverEventReg@@",
                                                   -2);
        if (raw != 0)
        {
            ((ObserverEventReg*)raw)->m_next     = 0;
            ((ObserverEventReg*)raw)->m_resource = 0;
        }
        node = (ObserverEventReg*)raw;

        ObserverEventReg* bucketTail = *slot;
        if (bucketTail != 0)
        {
            node->m_next = bucketTail->m_next;
            bucketTail->m_next = node;
        }
        else
        {
            node->m_next = node;
        }
        *slot = node;
        node->m_key = msgId;
    }

deliver:
    node->m_value = (void*)value;
    if (resource != 0)
        ((int*)resource)[1] += 1;

    ReleaseResourceRef(node->m_resource);
    node->m_resource = resource;

    if (!skipInsert && !keyFound)
        self->NoteInsert();
}

//----------------------------------------------------------------------------
//  0x6F62A5D0.  Deliver `msg` to every live registration under `msgId` in
//  this bucket, and combine each handler's own answer into this call's
//  return value.
//
//  Bumps `m_staticStorage` for the duration through the same
//  SByteCounterOwner::IncrementByte0/DecrementByte0 one-instruction helpers
//  Broadcast()'s own dead-node reclaim already reads - not "storage is
//  borrowed" here but a plain re-entrancy guard: a handler this walk calls
//  is free to re-enter Dispatch()/Broadcast() on the very table being
//  walked (nothing stops it, and the walk holds raw node pointers into it),
//  so only the *outermost* Invoke (the byte reading back exactly 1, not
//  merely nonzero) is allowed to unlink and free a dead node it passes -
//  a nested one leaves it in place rather than risk freeing a node the
//  outer walk still holds a pointer to.
//
//  The walk itself needs a fixed stopping point rather than Broadcast()'s
//  "reached the recorded tail again": a stack-local `sentinel` node is
//  spliced in as the bucket's new tail before the walk starts (its own
//  address becomes both the last real node's `m_next` and `*slot`), so the
//  loop simply runs until it reaches `&sentinel`, and one
//  TSLiteList<ObserverEventReg>::Unlink(&cursor) call at the very end
//  (0x6F629830 in the call tree) takes the sentinel back out and restores
//  the bucket's real tail - `cursor.m_prev` by then holding whatever the
//  last real node turned out to be. `ObserverEventCursor` (observereventreg.h)
//  is exactly this stack frame - Invoke is the "0x6F62A5D0" caller its own
//  comment already names.
//
//  A matched node's `m_value` is written into `msg`'s own +8 word (the same
//  slot SAgentMessage::m_id lives at) immediately before the delivery call -
//  safe because Method_0x10 has already read `m_id` out for its own use by
//  the time it calls down to this - and the delivery itself is a real
//  thiscall through the matched resource's own vtable slot 3, `this` =
//  the resource, one stack argument = `msg`.
//
//  On the way out, once the outermost frame is back down to 0, a
//  deferred Grow() runs if `m_entryCount` grew *during* this call relative
//  to the count this frame itself observed on entry - which only happens
//  when a re-entrant Broadcast() linked a new node in while this walk had
//  Grow() itself guarded off (Broadcast() checks the same `m_staticStorage`
//  byte and skips its own Grow() call while nonzero).
//----------------------------------------------------------------------------
int SObserverRecord::Invoke(unsigned int msgId, SAgentMessage* msg)
{
    CObserverEventList* self = (CObserverEventList*)this;
    int result = 0;

    self->m_staticStorage += 1;               // SByteCounterOwner::IncrementByte0
    unsigned short entryCountOnEntry = self->m_entryCount;

    unsigned int mask = (unsigned int)self->m_bucketCount - 1;
    ObserverEventReg** slot =
        (ObserverEventReg**)self->m_buckets + (mask & msgId);

    //  Splice a stack sentinel in as the new tail so the walk below has a
    //  fixed end.
    ObserverEventReg sentinel;
    ObserverEventReg* sentinelNode = (ObserverEventReg*)&sentinel;
    ObserverEventReg* oldTail = *slot;
    ObserverEventReg* head = oldTail ? oldTail->m_next : sentinelNode;
    if (oldTail != 0)
        oldTail->m_next = sentinelNode;
    *slot = sentinelNode;

    ObserverEventCursor cursor;
    cursor.m_reserved00 = slot;
    cursor.m_prev = sentinelNode;
    cursor.m_cur  = head;

    ObserverEventReg* cur = head;
    if (cur != sentinelNode)
    {
        for (;;)
        {
            void* curResource = cur->m_resource;
            if (curResource != 0)
            {
                if (cur->m_key == msgId)
                {
                    void* value = cur->m_value;
                    *(unsigned int*)&msg->m_id = (unsigned int)value;

                    void** vtbl = *(void***)curResource;
                    typedef int (__thiscall *DeliverFn)(void*, SAgentMessage*);
                    if (((DeliverFn)vtbl[3])(curResource, msg))
                        result = 1;
                }
            }
            else if (self->m_staticStorage == 1)
            {
                //  a dead node, and this is the outermost call: unlink it
                //  (TSLiteList<ObserverEventReg>::Unlink's own logic,
                //  inlined here exactly as Broadcast() inlines its own
                //  copy) and hand the (already-null) resource release
                //  through the same helper Broadcast() uses, for parity.
                ObserverEventReg* dead = cursor.m_cur;
                ObserverEventReg* next = dead->m_next;
                cursor.m_prev->m_next = next;
                dead->m_next = 0;
                cursor.m_cur = next;

                ReleaseResourceRef(dead->m_resource);

                cur = next;
                if (cur == sentinelNode)
                    break;
                continue;
            }

            cursor.m_prev = cur;
            cur = cur->m_next;
            cursor.m_cur = cur;
            if (cur == sentinelNode)
                break;
        }
    }

    //  Take the sentinel back out and restore the bucket's real tail.
    ((TSLiteList<ObserverEventReg>*)slot)->Unlink(&cursor);

    self->m_staticStorage -= 1;               // SByteCounterOwner::DecrementByte0
    if (self->m_staticStorage == 0 && self->m_entryCount > entryCountOnEntry)
        self->Grow();

    return result;
}

//----------------------------------------------------------------------------
//  0x6F62A9A0 - slot 2.  A `push 1` / `call` / `mov ecx,eax` / `jmp`: the
//  resource is created on demand and the broadcast is a tail call, which is
//  only possible because this function's own three stack arguments and
//  ObserverBroadcast's are the same three.
//----------------------------------------------------------------------------
void CObserver::Dispatch(unsigned int msg, unsigned int arg1, void* arg2)
{
    //  The record in its own local first: written as one expression, MSVC
    //  pushes the three arguments *before* calling EnsureResource and can
    //  no longer tail-jump, because their slots are no longer the ones this
    //  function was called with.  Resolved first, the three arguments never
    //  move and the whole call becomes the shipped `jmp`.
    SObserverRecord* record = (SObserverRecord*)EnsureResource(1);
    record->Broadcast(msg, arg1, arg2);
}

//----------------------------------------------------------------------------
//  0x6F629A90 - slot 4.  Unpack the message id and hand both it and the
//  message itself to slot 5, through the vtable rather than by name: this
//  is the base implementation and a derived class is free to override 5
//  without overriding 4.
//
//  This body was already reconstructed once, as `TaskDispatchOwner::
//  ForwardToSlot5` in widget_taskdispatch.cpp, from a call site in a
//  different target that could only see it as a raw slot-5 forward.  The
//  vtable sweep gives it its real owner and its real address, so that
//  placeholder is gone.
//----------------------------------------------------------------------------
typedef int (__thiscall *Slot0x14Fn)(void* self, unsigned int msgId,
                                     SAgentMessage* msg);

int CObserver::Method_0x10(SAgentMessage* msg)
{
    //  The id is read into a named local *before* the dispatch is spelled.
    //  Read inline as the second argument, MSVC keeps only two registers
    //  live and never touches esi; the shipped code loads the id first,
    //  which leaves `msg` live across the vtable load and forces the third,
    //  callee-saved register the `push esi` / `pop esi` pair is for.
    unsigned int msgId = msg->m_id;
    return ((Slot0x14Fn)(*(void***)this)[0x14 / 4])(this, msgId, msg);
}

//----------------------------------------------------------------------------
//  0x6F62A7B0 - slot 5.  Pin this observer with a temporary reference
//  across the delivery (the shipped `add [esi+4],1` / `add [esi+4],-1` pair
//  around the call), and if that reference turns out to have been the last
//  one, let the object destroy itself through vtable slot 0 before
//  returning the delivery's own answer.
//
//  The refcount is bumped and dropped inline rather than through
//  TRefCnt::AddRef/Release (refcnt.h): those are real out-of-line functions
//  at their own addresses and the shipped code does not call either.
//----------------------------------------------------------------------------
int CObserver::Method_0x14(unsigned int msgId, SAgentMessage* msg)
{
    //  One exit, reached from both paths - the shipped code zeroes eax
    //  before the null test and jumps forward to the epilogue, where an
    //  early `return 0` would give two epilogues with different pop counts
    //  (docs/msvc-vc8-idioms.md, "Shared exits need goto").
    int result = 0;
    if (m_resource != 0)
    {
        m_refcount++;
        result = ((SObserverRecord*)m_resource)->Invoke(msgId, msg);
        if (--m_refcount == 0)
            ReleaseSelf();
    }
    return result;
}

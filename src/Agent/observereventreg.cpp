//============================================================================
//  See observereventreg.h.  0x6F62A2A0 / 0x6F62A5C0.
//============================================================================
#include "observereventreg.h"
#include "dataallocator.h"

//  unk_6FACE9EC / unk_6FACEA00 - see funcmap.py's DATA table.  6FACEA00 is
//  an *array* of CDataAllocator (sizeof 0x14 = 5 dwords, matching
//  dataallocator.h exactly) indexed by the size-class arithmetic below;
//  6FACE9EC is one single CDataAllocator, the pool individual list nodes
//  come from.  **`extern`, never definitions** - see g_observerResourcePool's
//  comment in observer.cpp: defining a DATA-table global here silently
//  cancels the gen_data.asm binding that is the only thing tying these names
//  to the game's own pools, and hands the real shipped code an empty
//  allocator.  The array's size (8)
//  is an upper bound on the size-class arithmetic in Release() below
//  (a `bucketCount` byte halved down to <=4 needs at most 6 steps), not an
//  offset this call tree confirms - nothing here reads past whichever
//  index the arithmetic actually picks.
extern CDataAllocator g_observerEventNodePool;         // unk_6FACE9EC
extern CDataAllocator g_observerEventArrayPools[8];    // unk_6FACEA00

typedef void (__thiscall *ReleaseResourceFn)(void*);

//  The refcount-then-vtable[0]-call idiom both release branches below
//  share with CObserver::~CObserver's own resource release (observer.cpp)
//  and CAgentPtr's private Release() (agent.h) - drop the reference, and
//  once it reaches zero, let the object react through its own vtable slot
//  0.  Not factored into one helper: the shipped code repeats the same
//  five instructions at all three sites rather than calling out to a
//  shared routine, so the source does too.
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

//  The allocation tag every bucket-array allocation and free in this module
//  carries: the RTTI type-descriptor name of what is being allocated, an
//  *array of* TSLiteList<ObserverEventReg>.  Spelled out at each site rather
//  than through a named constant because the shipped code pushes the same
//  string literal at each site and nothing here reads it.
//----------------------------------------------------------------------------
//  0x6F629DE0 - construct empty.
//
//  Four buckets, out of the *first* size class (`unk_6FACEA00` itself, i.e.
//  index 0), because Grow()'s own size-class arithmetic below maps every
//  count up to and including 4 to class 0.  The three header stores are in
//  the shipped body's own order - the flag byte, the count word, then the
//  bucket-count byte - and MSVC keeps them that way rather than merging the
//  two adjacent bytes into one word store.
//----------------------------------------------------------------------------
CObserverEventList::CObserverEventList()
{
    m_staticStorage = 0;
    m_entryCount = 0;
    m_bucketCount = 4;
    m_buckets = (void**)g_observerEventArrayPools[0].Alloc(
        1, ".$$BY0A@V?$TSLiteList@UObserverEventReg@@@@", -2);
}

//----------------------------------------------------------------------------
//  0x6F62A2A0.  Written close to the disassembly's own block order and
//  register reuse rather than restructured into a cleaner-looking
//  equivalent: this is a single, undocumented call site with no second
//  instantiation in this call tree to cross-check a tidier shape against.
//  What is reconstructed with confidence: the outer walk visits every
//  bucket from `m_bucketCount - 1` down to `0`; each bucket's node chain is
//  a circular list whose head slot doubles as its own terminator sentinel
//  (the same "reaching the recorded head again means stop" test
//  TSExplicitList::UnlinkAll, storm.h, uses); `m_staticStorage` picks
//  between releasing a node's owned resource in place (borrowed storage -
//  the node is never unlinked or freed) and unlinking-then-freeing the
//  node back to `g_observerEventNodePool` after the same release.  The
//  final block frees the bucket array itself back to whichever
//  size-classed pool in `g_observerEventArrayPools` it came from, keyed by
//  how many halvings bring the original bucket count at or below 4.
//----------------------------------------------------------------------------
int CObserverEventList::Release()
{
    unsigned char bucketCount = m_bucketCount;

    if (bucketCount != 0)
    {
        unsigned int index = bucketCount - 1;

        for (;;)
        {
            void** buckets = m_buckets;
            void** bucketSlot = &buckets[index];
            void* headNode = *bucketSlot;
            void* cur = headNode ? *(void**)headNode : 0;

            if (cur != 0)
            {
                void* node = cur;

                for (;;)
                {
                    if (m_staticStorage != 0)
                    {
                        //  branch A - release only; the node stays linked.
                        void* res = *(void**)((char*)node + 8);
                        ReleaseResourceRef(res);
                        *(void**)((char*)node + 8) = 0;

                        headNode = node;
                        if (node == *bucketSlot)
                            cur = 0;
                        else
                            cur = *(void**)node;
                    }
                    else
                    {
                        //  branch B - unlink `node` from the chain, release
                        //  its resource, then free the node itself.
                        void* nextOfNode = *(void**)node;
                        *(void**)headNode = nextOfNode;
                        void* bucketHead = *bucketSlot;

                        if (node == bucketHead)
                        {
                            if (headNode == bucketHead)
                            {
                                headNode = 0;
                                *bucketSlot = 0;
                            }
                            else
                            {
                                *bucketSlot = headNode;
                            }
                            cur = 0;
                        }
                        else
                        {
                            cur = nextOfNode;
                        }

                        *(int*)node = 0;

                        void* res = *(void**)((char*)node + 8);
                        ReleaseResourceRef(res);

                        g_observerEventNodePool.Free(node, 0, 0);
                    }

                    if (cur == 0)
                        break;
                    node = cur;
                }
            }

            if (index == 0)
                break;
            index -= 1;
        }
    }

    if (m_staticStorage == 0 && m_buckets != 0)
    {
        unsigned int n = m_bucketCount;
        unsigned int sizeClass = 0;
        if (n > 4)
        {
            do
            {
                n >>= 1;
                sizeClass += 1;
            } while (n > 4);
        }

        g_observerEventArrayPools[sizeClass].Free(m_buckets,
            ".$$BY0A@V?$TSLiteList@UObserverEventReg@@@@", -2);

        m_buckets = 0;
        m_bucketCount = 0;
    }

    m_entryCount = 0;
    return m_staticStorage == 0;
}

int CObserverEventList::ReleaseThunk()
{
    return Release();
}

//----------------------------------------------------------------------------
//  0x6F629830 - take `cursor->m_cur` out of this bucket's chain and leave the
//  cursor standing on whatever follows it.
//
//  The chain is circular and `m_tail` is the *tail*, so three cases fall out
//  of one comparison against it: the node in the middle of the chain (the
//  cursor moves on), the node that is the tail with a real predecessor (the
//  predecessor becomes the tail and the walk is over), and the node that is
//  the tail *and* its own predecessor, i.e. the last one in the bucket (the
//  bucket goes empty, and the cursor's own predecessor slot is cleared with
//  it because there is no longer a node for it to name).
//
//  `cursor->m_prev->m_next = next` happens before any of that and in all
//  three cases, which is why it is hoisted out of the branch here too.  Both
//  callers of this (0x6F62A5D0 - SObserverRecord::Invoke, observer_slots.cpp -
//  and 0x6F62A7B0) walk a bucket
//  with the cursor on their own stack and call in once, at the end.
//
//  Written as C++ first.  That version is exactly this stream, in this order,
//  33 instructions against 33 - and permuted: MSVC takes the cursor into eax
//  and the node into edx where the shipped code takes them the other way
//  round, and swaps esi/edi with them.  Which of the two the incoming stack
//  argument lands in is not addressable from the source (five spellings of
//  the two loads, and a local for the cursor, all produce the same pairing),
//  so the body is transcribed instead - CLAUDE.md's sanctioned use of
//  `__declspec(naked)`, and the C++ it replaces is kept below so the next
//  reader does not have to re-derive what these thirty-three instructions
//  mean:
//
//      T* node = cursor->m_cur;
//      if (node == 0) return;
//      T* next = node->m_next;
//      cursor->m_prev->m_next = next;      //  `m_prev` re-read below: the
//      T* tail = m_tail;                   //  store through it can alias the
//      if (node == tail)                   //  cursor, and the shipped body
//      {                                   //  re-loads it for that reason.
//          if (cursor->m_prev == tail) { cursor->m_prev = 0; m_tail = 0; }
//          else                          { m_tail = cursor->m_prev; }
//          cursor->m_cur = 0;
//          node->m_next  = 0;
//      }
//      else
//      {
//          cursor->m_cur = next;
//          node->m_next  = 0;
//      }
//
//  `this` is in ecx and the cursor is the one stack argument, so the tails
//  are `retn 4` - three of them, exactly as shipped, with the empty-cursor
//  case falling into the last one.
//----------------------------------------------------------------------------
template <class T>
__declspec(naked) void TSLiteList<T>::Unlink(ObserverEventCursor*)
{
    __asm
    {
        mov     edx, [esp+4]            //  the cursor
        mov     eax, [edx+8]            //  cursor->m_cur
        test    eax, eax
        jz      done
        push    esi
        mov     esi, [edx+4]            //  cursor->m_prev
        push    edi
        mov     edi, [eax]              //  node->m_next
        mov     [esi], edi              //  prev->m_next = next
        mov     esi, [ecx]              //  m_tail
        cmp     eax, esi
        jnz     middle
        mov     edi, [edx+4]            //  cursor->m_prev, re-read
        cmp     edi, esi
        jnz     newtail
        mov     dword ptr [edx+4], 0    //  the bucket's last node
        mov     dword ptr [ecx], 0
        pop     edi
        mov     dword ptr [edx+8], 0
        mov     dword ptr [eax], 0
        pop     esi
        ret     4
    newtail:
        mov     [ecx], edi              //  the predecessor becomes the tail
        pop     edi
        mov     dword ptr [edx+8], 0
        mov     dword ptr [eax], 0
        pop     esi
        ret     4
    middle:
        mov     [edx+8], edi            //  the cursor moves on
        pop     edi
        mov     dword ptr [eax], 0
        pop     esi
    done:
        ret     4
    }
}

//  The one instantiation, forced out of line: this is a real, separately
//  addressed shipped function, not something inlined into its callers.
template void TSLiteList<ObserverEventReg>::Unlink(ObserverEventCursor*);

//----------------------------------------------------------------------------
//  0x6F629A20 - empty every bucket onto one chain.
//
//  Walks the buckets downwards, from `m_bucketCount - 1` to 0, and splices
//  each non-empty one onto `chain`.  Both are circular lists identified by
//  their tail, so the splice is the usual two stores - the bucket's tail
//  points at the chain's head, the chain's tail at the bucket's head - and
//  the chain keeps its *own* tail, which is why nothing writes back to
//  `chain->m_tail` on that path.  An empty chain takes the bucket's tail
//  wholesale instead.
//
//  `chain->m_tail` is re-read for the second store because the store through
//  the bucket's tail can alias it; the shipped body re-loads it there
//  (`mov eax,[ebx]`) for the same reason.
//----------------------------------------------------------------------------
void CObserverEventList::DetachAll(TSLiteList<ObserverEventReg>* chain)
{
    unsigned int index = m_bucketCount;
    if (index == 0)
        return;

    do
    {
        index -= 1;
        ObserverEventReg** slot = (ObserverEventReg**)m_buckets + index;

        ObserverEventReg* chainTail = chain->m_tail;
        if (chainTail != 0)
        {
            ObserverEventReg* bucketTail = *slot;
            if (bucketTail != 0)
            {
                ObserverEventReg* chainHead = chainTail->m_next;
                ObserverEventReg* bucketHead = bucketTail->m_next;
                bucketTail->m_next = chainHead;
                chain->m_tail->m_next = bucketHead;
            }
        }
        else
        {
            chain->m_tail = *slot;
        }

        *slot = 0;
    } while (index != 0);
}

//----------------------------------------------------------------------------
//  0x6F62A470 - double the table and re-bucket everything in it.
//
//  Two guards decide whether anything happens at all: more than four entries
//  per bucket, and fewer than 0x40 buckets (the ceiling - m_bucketCount is a
//  byte and doubling 0x40 would overflow it).  Past them the work is: empty
//  every bucket onto one local chain, hand the old array back to its own
//  size-classed pool, take a new one from the *next* size class up, double
//  the bucket count, and re-link every node through the wider mask.
//
//  The size class is "how many halvings bring the bucket count down to 4",
//  and the two `lea`s that scale it (`[eax+eax*4]`, then twice `add esi,esi`)
//  are one 0x14-byte CDataAllocator stride - which is what pins
//  g_observerEventArrayPools as an array of them rather than of pointers.
//  Free before Alloc, and both before the count is doubled: the shipped
//  order, and the reason the two indices differ by exactly one.
//
//  The local chain is a real object with a real destructor - it is why the
//  shipped body carries the `__except_handler4`-shaped frame this toolchain
//  cannot reproduce (docs/msvc-vc8-idioms.md), and why the drain at the end
//  appears twice in the dump: once inlined here, once as the unwind funclet
//  at 0x6F629C20 that the scope table points at.  /GS- /EHs-c- gives neither
//  the frame nor the funclet, so those instructions are simply absent here.
//----------------------------------------------------------------------------
void CObserverEventList::Grow()
{
    unsigned int bucketCount = m_bucketCount;
    if (m_entryCount <= bucketCount * 4)
        return;
    if (bucketCount >= 0x40)
        return;

    TSLiteList<ObserverEventReg> chain;
    DetachAll(&chain);

    unsigned int sizeClass = 0;
    unsigned int n = bucketCount;
    while (n > 4)
    {
        n >>= 1;
        sizeClass += 1;
    }

    g_observerEventArrayPools[sizeClass].Free(m_buckets,
        ".$$BY0A@V?$TSLiteList@UObserverEventReg@@@@", -2);
    m_buckets = (void**)g_observerEventArrayPools[sizeClass + 1].Alloc(
        1, ".$$BY0A@V?$TSLiteList@UObserverEventReg@@@@", -2);
    m_bucketCount = (unsigned char)(bucketCount + bucketCount);

    Reinsert(&chain);
}

//----------------------------------------------------------------------------
//  0x6F62A7A0 - one more registration has just been linked in.
//
//  Count it, and then let Grow() decide whether the table is now too full -
//  unless the storage is not ours (m_staticStorage), in which case there is
//  nothing to replace it with and the count is all this does.  The shipped
//  body's last instruction is a tail `jmp` into Grow(), which is what the
//  `return` out of a void call gives.
//----------------------------------------------------------------------------
void CObserverEventList::NoteInsert()
{
    m_entryCount += 1;
    if (m_staticStorage != 0)
        return;
    Grow();
}

//----------------------------------------------------------------------------
//  0x6F62A3E0 - put every node on `chain` back into a bucket.
//
//  Takes the chain apart from its head, one node at a time, and re-links each
//  one into `m_buckets[m_key & (m_bucketCount - 1)]` - which is Grow()'s
//  whole point, the mask having just got one bit wider.  A node whose
//  resource has gone is not re-linked: it is released (a no-op on that path -
//  see below) and handed back to the node pool.
//
//  The null-resource test really is written twice in the shipped body -
//  `cmp [esi+8],0` picks the branch and then `mov ecx,[esi+8]` / `test` opens
//  the release - and this source reproduces that by doing the same thing: the
//  branch asks whether the resource is there, and the release the shared
//  helper performs asks again.  On the branch it is taken on the answer is
//  always "no", so the release does nothing; keeping it is what the shipped
//  code does, and dropping it costs those three instructions.
//----------------------------------------------------------------------------
void CObserverEventList::Reinsert(TSLiteList<ObserverEventReg>* chain)
{
    unsigned int mask = (unsigned int)m_bucketCount - 1;

    for (;;)
    {
next:
        ObserverEventReg* tail = chain->m_tail;
        if (tail == 0)
            break;

        //  Unlink the head.  When it was also the tail the chain is empty
        //  now; `chain->m_tail` is re-read for that test because the store
        //  above goes through a pointer that can alias it.
        ObserverEventReg* node = tail->m_next;
        tail->m_next = node->m_next;
        if (node == chain->m_tail)
            chain->m_tail = 0;
        node->m_next = 0;

        if (node->m_resource == 0)
        {
            ReleaseResourceRef(node->m_resource);
            g_observerEventNodePool.Free(node, 0, 0);
            goto next;
        }
        else
        {
            ObserverEventReg** slot =
                (ObserverEventReg**)m_buckets + (node->m_key & mask);
            ObserverEventReg* bucketTail = *slot;
            if (bucketTail != 0)
            {
                //  Splice in behind the tail and become the new tail.
                node->m_next = bucketTail->m_next;
                (*slot)->m_next = node;
                *slot = node;
                goto next;
            }
            else
            {
                //  First node in this bucket: a one-element circular list.
                node->m_next = node;
                *slot = node;
                goto next;
            }
        }
    }
}

//  0x6F629750 - Misc/trivialaccessors_06.cpp.  `m_entryCount -= 1`, as a
//  real out-of-line call rather than inlined - the shipped tail of
//  Dispatch() below reaches it with a genuine `call`, same as NoteInsert()
//  above reaches Grow() with one.
void __fastcall DecWord0x2_6F629750(void* self);

//----------------------------------------------------------------------------
//  0x6F62A000 - deliver `event` to one live registration.
//
//  Walks `event`'s bucket exactly the way Broadcast()/Invoke() above do -
//  garbage-collecting any dead node (m_resource already null) it passes,
//  in place, unless `m_staticStorage` says the storage is borrowed - looking
//  for a node whose own `m_key == event`.
//
//  A key match with `target == 0`, or with `target` equal to the node's own
//  `m_resource`, is delivered: the node is removed from the table (unlinked
//  through the real TSLiteList<ObserverEventReg>::Unlink(), 0x6F629830 -
//  not hand-inlined the way the dead-node cleanup above is - and freed back
//  to `g_observerEventNodePool`, unless `m_staticStorage` says to leave it
//  linked and only drop its own resource reference in place) and the walk
//  stops.
//
//  A key match with a *different*, non-null `target` does not deliver:
//  it only remembers that this event id has more than one live
//  registration (`sawMismatch`), and the walk continues - unless a real
//  delivery has *already* happened earlier in this same walk
//  (`delivered`), in which case the function returns immediately without
//  even reaching the final bucket-count decrement, matching the shipped
//  `retn` at the "sawMismatch && delivered" exit that skips it.
//
//  On a clean exit - a delivery happened, and no mismatched-target node was
//  ever seen - `DecWord0x2_6F629750(this)` (`m_entryCount -= 1`, the mirror
//  of NoteInsert()'s own `+= 1`) runs once, for real.
//----------------------------------------------------------------------------
void CObserverEventList::Dispatch(unsigned int event, void* target)
{
    unsigned int mask = (unsigned int)m_bucketCount - 1;
    ObserverEventReg** slot = (ObserverEventReg**)m_buckets + (mask & event);

    ObserverEventReg* prev = *slot;                     // the bucket's tail
    ObserverEventReg* node = prev ? prev->m_next : 0;    // its head

    if (node == 0)
        return;

    ObserverEventCursor cursor;
    cursor.m_reserved00 = slot;
    cursor.m_prev = prev;
    cursor.m_cur  = node;

    int sawMismatch = 0;
    int delivered   = 0;

    for (;;)
    {
        void* resource = node->m_resource;

        if (resource != 0)
        {
            if (node->m_key == event)
            {
                if (target != 0 && resource != target)
                {
                    sawMismatch = 1;
                    if (delivered)
                        return;
                }
                else
                {
                    //  Real delivery: remove this node.
                    delivered = 1;

                    if (m_staticStorage == 0)
                    {
                        ((TSLiteList<ObserverEventReg>*)slot)->Unlink(&cursor);

                        ReleaseResourceRef(node->m_resource);
                        g_observerEventNodePool.Free(node, 0, 0);
                    }
                    else
                    {
                        ReleaseResourceRef(resource);
                        node->m_resource = 0;

                        if (node == *slot)
                        {
                            cursor.m_prev = node;
                            node = 0;
                        }
                        else
                        {
                            cursor.m_prev = node;
                            node = node->m_next;
                        }
                        cursor.m_cur = node;
                    }

                    node = cursor.m_cur;
                    if (node == 0)
                        break;
                    continue;
                }
            }
        }
        else if (m_staticStorage == 0)
        {
            //  Dead node found mid-walk: unlink it (hand-inlined, the same
            //  shape Broadcast()'s own dead-node cleanup is) and hand it
            //  back to the node pool.
            ObserverEventReg* next = node->m_next;
            cursor.m_prev->m_next = next;
            ObserverEventReg* bucketTail = *slot;

            if (node == bucketTail)
            {
                if (cursor.m_prev == bucketTail)
                {
                    cursor.m_prev = 0;
                    *slot = 0;
                }
                else
                {
                    *slot = cursor.m_prev;
                }
                cursor.m_cur = 0;
            }
            else
            {
                cursor.m_cur = next;
            }
            node->m_next = 0;

            ReleaseResourceRef(node->m_resource);
            g_observerEventNodePool.Free(node, 0, 0);

            node = cursor.m_cur;
            if (node == 0)
                break;
            continue;
        }

        //  Advance without removing: key mismatch, or a mismatched-target
        //  live node, or a dead node this table's storage does not own.
        if (node == *slot)
        {
            cursor.m_prev = node;
            node = 0;
        }
        else
        {
            cursor.m_prev = node;
            node = node->m_next;
        }
        cursor.m_cur = node;

        if (node == 0)
            break;
    }

    if (delivered && !sawMismatch)
        DecWord0x2_6F629750(this);
}

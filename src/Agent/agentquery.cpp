//============================================================================
//  0x6F629FA0 / 0x6F629AB0 / 0x6F629770 - the stack-built query object
//  CAgentWar3's slot 20 (agent_slot20.cpp) hands to the message writer:
//  its constructor, its one method and its destructor.  All three were
//  `mov eax,<address> / jmp eax` redirects; none of them is a subsystem.
//
//  What the object is is readable straight off the three bodies.  The
//  constructor asks its owner for the observer resource
//  (CObserver::EnsureResource, 0x6F629F70, already reconstructed) *without*
//  creating one, parks it at +0x00, zeroes the other four words and takes a
//  counted reference to it; Add hashes a message id into that resource's
//  own bucket table and remembers where it landed; the destructor drops the
//  reference again.  So the object is a *cursor* into the owner's observer
//  registration table, not a container - which is why the destructor has
//  nothing to free.
//
//  The reference count is the one-byte counter at the resource's own +0x00
//  that misc_trivial_getters.cpp already reconstructs as
//  SByteCounterOwner::IncrementByte0 / DecrementByte0 - a byte, not a
//  dword, so this is not the +0x04 dword refcount refcnt.h's counted
//  objects use.
//
//  The owner parameter stays declared as `CAgent*` rather than the
//  `CObserver*` EnsureResource is really a member of: the shipped call site
//  passes CAgent's own `this`, agent_slot20.cpp is compiled against that
//  signature, and MSVC mangles the parameter type into the constructor's
//  name - changing it here breaks the link for a comment's worth of
//  accuracy.  The cast to CObserver* is where the relationship is stated.
//
//  Own translation unit: agent_slot20.cpp's slot 20 reaches all three with
//  real calls, and all three reach three more.
//============================================================================
#include "agent.h"
#include "observer.h"
#include "observereventreg.h"

//----------------------------------------------------------------------------
//  The observer resource the cursor walks.  Only what these three bodies
//  reach through is named: the reference byte at +0x00, the bucket count's
//  log2 at +0x01, and the bucket array at +0x04.
//----------------------------------------------------------------------------
struct SObserverResource
{
    unsigned char m_refByte;      // +0x00
    unsigned char m_bucketBits;   // +0x01
    char          m_reserved02[2];
    void**        m_buckets;      // +0x04
};

//  0x6F629730 / 0x6F629740 - the byte increment/decrement, already
//  reconstructed in misc_trivial_getters.cpp.  Declared, not re-thunked.
struct SByteCounterOwner
{
    void __thiscall IncrementByte0();
    void __thiscall DecrementByte0();
};

//----------------------------------------------------------------------------
//  The cursor itself.  The 0x1C-byte size is agent_slot20.cpp's, read off
//  the shipped frame there (var_20..var_4); only the first 0x14 bytes are
//  ever touched, and the tail is kept so slot 20's own frame does not move.
//----------------------------------------------------------------------------
struct SAgentQuery
{
    SAgentQuery(CAgent* owner);
    ~SAgentQuery();
    void Add(unsigned int msgId);

    //  0x6F629AF0 - hand back the next registration in the bucket the cursor
    //  is standing in whose key is the one being asked for, and note the
    //  smallest key above it on the way past.  See below.
    int NextEntry(void** value, void** resource);

    //  0x6F629B60 - move on to the next key there is anything registered
    //  under, walking into later buckets until one has something.
    int NextKey(unsigned int* key);

    SObserverResource* m_pResource;   // +0x00
    void**             m_ppBucket;    // +0x04 - where in m_buckets we landed
    void*              m_pFirst;      // +0x08 - the entry the walk is on
    //  +0x0C - the key being asked for, and +0x10 the smallest key seen so
    //  far that is strictly above it.  NextEntry() maintains the second
    //  while it walks; NextKey() promotes it to the first and starts again,
    //  which is what makes the pair an in-order walk over the whole table.
    //  Equal is the "nothing found yet" state, which is why Add() sets both
    //  to the same value.
    unsigned int       m_key;         // +0x0C
    unsigned int       m_nextKey;     // +0x10
    char               m_reserved14[0x1C - 0x14];
};

//----------------------------------------------------------------------------
//  0x6F629FA0.
//
//  `create` is 0: the cursor never builds a registration table, it only
//  walks one that is already there - which is why every field below has to
//  survive a null resource.
//----------------------------------------------------------------------------
SAgentQuery::SAgentQuery(CAgent* owner)
{
    SObserverResource* resource =
        (SObserverResource*)((CObserver*)owner)->EnsureResource(0);

    m_pResource = resource;
    m_ppBucket = 0;
    m_pFirst = 0;
    m_key = 0;
    m_nextKey = 0;

    if (resource != 0)
        ((SByteCounterOwner*)resource)->IncrementByte0();
}

//----------------------------------------------------------------------------
//  0x6F629770 - drop the reference.  A tail jump in the shipped code, which
//  is what `return`ing straight out of the guarded call gives.
//----------------------------------------------------------------------------
SAgentQuery::~SAgentQuery()
{
    SObserverResource* resource = m_pResource;
    if (resource != 0)
        ((SByteCounterOwner*)resource)->DecrementByte0();
}

//----------------------------------------------------------------------------
//  0x6F629AB0 - point the cursor at the bucket a message id hashes to.
//
//  The mask is `(bucketBits - 1)`, read as a *byte* and widened
//  (`movzx esi, byte ptr [edx+1]`), so the field really is one byte and not
//  a dword the compiler happened to narrow.
//
//  `m_key` and `m_nextKey` both get the raw id, in that order and before
//  anything is computed from it - two stores of the same register, not one
//  field read twice.
//----------------------------------------------------------------------------
void SAgentQuery::Add(unsigned int msgId)
{
    SObserverResource* resource = m_pResource;
    if (resource == 0)
        return;

    m_key = msgId;
    m_nextKey = msgId;

    void** bucket = resource->m_buckets
                  + ((((unsigned int)resource->m_bucketBits) - 1) & msgId);
    m_ppBucket = bucket;

    //  The conditional assigned *back into* `head` rather than straight into
    //  the field, and written as a `?:` rather than as an if/else.  Both
    //  halves are load-bearing:
    //
    //   * assigning into the field from two branches (`if (...) m_pFirst =
    //     *head; else m_pFirst = 0;`) makes MSVC store the constant
    //     directly (`mov [ecx+8],0`), where the shipped code materialises
    //     the zero into the register the merged value lives in
    //     (`xor eax,eax` / `mov [ecx+8],eax`);
    //   * dropping the else (`if (head) head = *head;`) merges the two
    //     stores into one, where the shipped code duplicates the store and
    //     the `retn` into both arms.
    //
    //  Only this spelling gives both.
    void* head = *bucket;
    head = (head != 0) ? *(void**)head : 0;
    m_pFirst = head;
}

//----------------------------------------------------------------------------
//  0x6F629AF0 - walk what is left of the current bucket looking for the key
//  the cursor is asking for, and note the next key up on the way past.
//
//  Two jobs in one walk, which is what makes the pair of walks below an
//  in-order enumeration of a *hash* table.  The first is the obvious one:
//  a node whose key is the one being asked for hands its value and its
//  resource back and stops the walk (`found` is also the loop's own second
//  exit condition, which is why the shipped code tests it at the bottom).
//  The second is the bookkeeping: any key strictly *above* the current one
//  is a candidate for "the key to ask for next", and m_nextKey keeps the
//  smallest such - with `m_nextKey == m_key` standing for "nothing found
//  yet", the state Add() leaves the cursor in.
//
//  A node with no resource is skipped entirely: it is a registration on its
//  way out, not a live one.
//
//  The advance is the same `?:`-assigned-back shape Add() documents: reaching
//  the bucket's own tail ends the walk, anything else steps to m_next, and
//  the merged value is stored to m_pFirst once.
//----------------------------------------------------------------------------
int SAgentQuery::NextEntry(void** value, void** resource)
{
    int found = 0;

    //  Both exits are written at the *bottom* on purpose.  Spelled as one
    //  `while (m_pFirst != 0 && found == 0)`, MSVC hoists the `found` half to
    //  the top of the loop and the shipped code tests it after the advance,
    //  next to the end-of-bucket test it belongs with.
    //  The entry test wraps the loop rather than returning early: the shipped
    //  body branches straight to its one epilogue, which an early `return`
    //  turns into a second, duplicated `retn 8`.
    if (m_pFirst != 0)
    {
      for (;;)
      {
        ObserverEventReg* node = (ObserverEventReg*)m_pFirst;

        void* res = node->m_resource;
        if (res != 0)
        {
            unsigned int key = node->m_key;
            if (key == m_key)
            {
                *value = node->m_value;
                *resource = res;
                found = 1;
            }
            else if (key > m_key)
            {
                if (m_nextKey == m_key || key < m_nextKey)
                    m_nextKey = key;
            }
        }

        node = (node == *(ObserverEventReg**)m_ppBucket) ? 0 : node->m_next;
        m_pFirst = node;

        if (node == 0)
            break;
        if (found != 0)
            break;
      }
    }

    return found;
}

//----------------------------------------------------------------------------
//  0x6F629B60 - move the cursor on to the next key anything is registered
//  under, and hand that key back.  Returns 0 once the table is exhausted.
//
//  This is the outer half of the in-order walk NextEntry() feeds: whenever
//  the two key words are still equal - i.e. the bucket the cursor is in had
//  nothing above the current key - it steps to the next bucket, scans that
//  bucket's whole chain for its smallest key, and repeats until it finds one
//  or runs off the end of the array.  Then it promotes m_nextKey to m_key,
//  points m_pFirst at the bucket's head so NextEntry() can walk it, and
//  writes the key out.
//
//  `m_ppBucket` being null means the walk has not started: it is primed to
//  *one before* the first bucket (`m_buckets - 1`) so that the first step
//  lands on bucket 0, which is why the shipped code's `sub eax,4` is there.
//
//  Two things about the shipped body are load-bearing and are reproduced
//  here rather than tidied:
//
//   * the outer test reads m_key and m_nextKey **once**, into registers, and
//     the bucket-advance path jumps back to that same test without
//     re-reading them - so it is the *entry* state of the pair that keeps
//     the loop going, not the fields as the inner scan leaves them;
//   * both success paths return 1 and both write *key; the shipped code
//     duplicates the epilogue rather than merging them, with the
//     empty-bucket arm materialising its 1 out of the zero it just stored
//     (`lea eax,[edx+1]`).
//
//  **Not scored.** No dump in `asm/` has a `### sub_6F629B60` heading - only
//  the two `call sub_6F629B60` sites in 0x6F472420's body - and the IDA
//  worker's own script set could not answer a fresh request for one this
//  session, so `verify.py` has no target stream to compare this against.  It
//  is written from the disassembly and reviewed by hand; nothing in this
//  build calls it yet either, because its one caller (CAgent::Save,
//  0x6F472420) is still a redirect.
//----------------------------------------------------------------------------
int SAgentQuery::NextKey(unsigned int* key)
{
    SObserverResource* resource = m_pResource;
    if (resource == 0)
        return 0;

    if (m_ppBucket == 0)
        m_ppBucket = resource->m_buckets - 1;

    unsigned int curKey = m_key;
    unsigned int nextKey = m_nextKey;
    int exhausted = 0;

    while (curKey == nextKey)
    {
        //  One past the last bucket is the end of the walk.
        void** last = resource->m_buckets
                    + (unsigned int)resource->m_bucketBits - 1;
        if (m_ppBucket == last)
            return exhausted;

        void** slot = m_ppBucket + 1;
        m_ppBucket = slot;

        ObserverEventReg* tail = *(ObserverEventReg**)slot;
        if (tail == 0)
            continue;
        ObserverEventReg* node = tail->m_next;
        if (node == 0)
            continue;

        //  The smallest key in this bucket, kept in the field itself the way
        //  the shipped code does: every improvement is stored and the scan
        //  restarts from the node that produced it.
        unsigned int best = node->m_key;
        for (;;)
        {
            m_nextKey = best;
            for (;;)
            {
                if (node == *(ObserverEventReg**)slot)
                    goto found;
                node = node->m_next;
                if (node == 0)
                    goto found;
                best = node->m_key;
                if (best < m_nextKey)
                    break;
            }
        }
    }

found:
    {
        unsigned int answer = m_nextKey;
        ObserverEventReg* tail = *(ObserverEventReg**)m_ppBucket;
        m_key = answer;
        if (tail != 0)
        {
            m_pFirst = tail->m_next;
            *key = answer;
            return 1;
        }
        m_pFirst = 0;
        *key = answer;
        return 1;
    }
}

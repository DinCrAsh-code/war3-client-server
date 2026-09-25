//============================================================================
//  CObserverEventList: the RTTI string 0x6F62A2A0 passes to
//  CDataAllocator::Free (".$$BY0A@V?$TSLiteList@UObserverEventReg...",
//  i.e. an *array of* `TSLiteList<ObserverEventReg>`) is the only naming
//  evidence in this call tree for the type this cleans up.  Only the four
//  header fields Release() itself touches are recovered; nothing here
//  establishes what a single `ObserverEventReg`/list node actually looks
//  like beyond the two offsets Release() reads on it (a "next" link at +0,
//  a TRefCnt-shaped owned resource pointer at +8).
//============================================================================
#ifndef OBSERVEREVENTREG_H
#define OBSERVEREVENTREG_H

//----------------------------------------------------------------------------
//  One registration.  Four words, every one of them established by the
//  bucket-table maintenance below (0x6F629A20 / 0x6F62A3E0 / 0x6F629830) and
//  by the cursor in agentquery.cpp, which walks the same chains:
//
//    +0x00  m_next      - the next registration in this bucket's chain.  The
//                         chain is *circular* and the bucket slot holds its
//                         **tail**, so the head is always `tail->m_next`;
//                         every walk in this module ends by reaching the
//                         recorded tail again, and a node that is out of a
//                         list carries 0 here.
//    +0x04  m_key       - the message id.  Reinsert() re-buckets a node by
//                         `m_key & (m_bucketCount - 1)`, which is what makes
//                         m_bucketCount a power of two.
//    +0x08  m_resource  - the TRefCnt-shaped thing this registration owns a
//                         counted reference to.  A null one is not a live
//                         registration: Reinsert() drops such a node back
//                         into the node pool instead of re-linking it.
//    +0x0C  m_value     - handed back to the caller by the cursor's own
//                         lookup (agentquery.cpp).  Nothing in this module
//                         reads it.
//----------------------------------------------------------------------------
struct ObserverEventReg
{
    ObserverEventReg* m_next;       // +0x00
    unsigned int      m_key;        // +0x04
    void*             m_resource;   // +0x08
    void*             m_value;      // +0x0C
};

//----------------------------------------------------------------------------
//  One bucket.  The RTTI string the array allocation carries is
//  ".$$BY0A@V?$TSLiteList@UObserverEventReg@@@@" - an *array of*
//  `TSLiteList<ObserverEventReg>` - so both the template's name and its
//  element's are the shipped ones, and a bucket is exactly one pointer wide:
//  the circular chain's tail, or null when the bucket is empty.
//
//  Not Storm's TSExplicitList (storm.h): that one is doubly linked through a
//  separate TSLink and carries a link offset.  This is the "lite" flavour -
//  one word, singly linked, circular.
//
//  The destructor is deliberately written in the class body (i.e. inline):
//  the only thing that has one of these as a *local* is
//  CObserverEventList::Grow(), and the shipped Grow() has this drain inlined
//  into its own body twice over - once in line at the end and once in the
//  unwind funclet its SEH frame points at.
//----------------------------------------------------------------------------
struct ObserverEventCursor;

template <class T>
class TSLiteList
{
public:
    TSLiteList() { m_tail = 0; }

    ~TSLiteList()
    {
        T* tail = m_tail;
        if (tail)
        {
            //  Break every node's link, tail first, so that nothing left in
            //  the chain still claims to be in a list.  The nodes themselves
            //  are not freed - whoever drained the chain owns them by now.
            T* node = tail->m_next;
            tail->m_next = 0;
            while (node != tail)
            {
                T* dead = node;
                node = node->m_next;
                dead->m_next = 0;
            }
        }
    }

    //  0x6F629830 - unlink the node `cursor` is standing on and advance the
    //  cursor past it.  See observereventreg.cpp.
    void Unlink(ObserverEventCursor* cursor);

    T* m_tail;      // +0x00
};

//----------------------------------------------------------------------------
//  What a walk over one bucket's chain keeps while it is walking, and what
//  TSLiteList::Unlink() is handed: the node it is standing on and the node
//  before it (whose m_next has to be re-pointed when the current one goes).
//  Its callers (0x6F62A5D0 - SObserverRecord::Invoke, observer_slots.cpp -
//  and 0x6F62A7B0) build one on
//  their own stack; only the last two words are read here, so the first is
//  named for its offset alone.
//----------------------------------------------------------------------------
struct ObserverEventCursor
{
    void*             m_reserved00;   // +0x00
    ObserverEventReg* m_prev;         // +0x04
    ObserverEventReg* m_cur;          // +0x08
};

class CObserverEventList
{
public:
    //  0x6F629DE0 - construct empty: four buckets, freshly allocated from
    //  the first of the size-classed array pools, and no entries.  The
    //  shipped caller is SObserverRecordPool::Alloc (0x6F629E90), which
    //  placement-constructs the record it has just allocated.
    CObserverEventList();

    //  0x6F62A7A0 - one more entry has just been linked in: count it, and
    //  grow the table if the storage is ours to replace.
    void NoteInsert();

    //  0x6F62A470 - double the bucket count and re-bucket every
    //  registration, when the table has more than four entries per bucket
    //  and has not already reached the 0x40-bucket ceiling.
    void Grow();

    //  0x6F629A20 - splice every bucket's chain onto `chain` and leave every
    //  bucket empty.
    void DetachAll(TSLiteList<ObserverEventReg>* chain);

    //  0x6F62A3E0 - the other half of Grow(): take `chain` apart a node at a
    //  time and re-link each one into the bucket its key now hashes to,
    //  dropping the ones that no longer own a resource.
    void Reinsert(TSLiteList<ObserverEventReg>* chain);

    //  0x6F62A2A0 - walk every bucket's node chain releasing each node's
    //  owned resource (and, unless m_staticStorage says the storage isn't
    //  ours, unlinking and freeing the node itself back to its pool), then
    //  free the bucket array back to whichever size-classed
    //  CDataAllocator it came from.  Returns whether the header's own
    //  small-storage byte reads back zero.  See observereventreg.cpp for
    //  why this is written close to the disassembly's own control flow
    //  rather than a cleaner-looking equivalent - the shape (which of two
    //  release paths a node takes, and the size-class arithmetic below)
    //  was reverse engineered from this one call tree with no second call
    //  site to cross-check it against.
    int Release();

    //  0x6F62A000 - deliver `event` to the one live registration under it
    //  that also matches `target` (or the first one, if `target` is null),
    //  removing it from the table in the process.  See observereventreg.cpp
    //  for the full shape - CObserver::PostEvent (observerpostevent.cpp)
    //  is the caller, through `m_resource`.
    void Dispatch(unsigned int event, void* target);

    //  0x6F62A5C0 - a one-instruction `jmp` tail-thunk to Release() with no
    //  adjustor.  A real member function (not a free function): this
    //  compiler rejects an explicit `__thiscall` on a free function
    //  (C3865), and a plain `__cdecl` free-function wrapper would pass
    //  `this` on the stack instead of in ecx - the wrong ABI for the one
    //  caller in this call tree (CObserver::~CObserver, observer.cpp)
    //  that expects `call sub_6F62A5C0` to behave exactly like a direct
    //  `call Release` with `this` already in ecx.  IDA gives it its own
    //  address only because something elsewhere takes its address rather
    //  than calling Release() directly (not visible in this call tree).
    int ReleaseThunk();

    unsigned char   m_staticStorage;   // +0x00 - nonzero: release resources only, never unlink/free node or array storage
    unsigned char   m_bucketCount;     // +0x01 - 0: no bucket array to walk
    //  +0x02 - how many registrations are linked into the table.  Named by
    //  NoteInsert() (0x6F62A7A0), which increments it, and by Grow(), which
    //  compares it against `m_bucketCount * 4`; Release() clears it.
    unsigned short  m_entryCount;      // +0x02
    void**          m_buckets;         // +0x04 - the bucket array itself
};

#endif

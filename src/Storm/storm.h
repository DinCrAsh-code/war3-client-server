//============================================================================
//  Storm's generic container library: the intrusive list, the growable array
//  and the chained hash table that sit under AGILE_TYPE_DATA.
//
//  The class names are not guesses.  The shipped code hands the allocator the
//  RTTI type-descriptor name of the array element it is resizing:
//
//      ".?AV?$TSExplicitList@UAGILE_TYPE_RELATIONSHIP@AGILE_TYPE_DATA@@$0?CCCCCCCD@@@"
//
//  which demangles to
//
//      class TSExplicitList<struct AGILE_TYPE_DATA::AGILE_TYPE_RELATIONSHIP,
//                           -572662307>
//
//  (`$0?CCCCCCCD@` is MSVC's encoding for an int template argument: `?` is the
//  sign and `CCCCCCCD` the nibbles 2,2,2,2,2,2,2,3, i.e. -0x22222223, which is
//  0xDDDDDDDD read as unsigned - the poison value the list's link offset is
//  born with and which the code writes into every freshly constructed
//  element).  So the element type is a list, the list is a template over its
//  element and over a link offset, and the container names below are Storm's.
//============================================================================
#ifndef STORM_H
#define STORM_H

//----------------------------------------------------------------------------
//  Placement new.  <new> is not usable in this build (/Zl, no CRT headers
//  wanted), and the shipped code depends on the null test MSVC emits in front
//  of a placement-new constructor call, so the operator is declared here
//  rather than the construction being spelled out by hand.
//----------------------------------------------------------------------------
inline void* operator new(unsigned int, void* where) { return where; }
inline void  operator delete(void*, void*) {}

//----------------------------------------------------------------------------
//  Storm.dll's allocator, imported by ordinal (401 = SMemAlloc,
//  403 = SMemFree, 405 = SMemReAlloc; the dump names them Storm_401/403/405
//  because the import library exports them ordinal-only).  The `logfilename`
//  slot is where Storm expects __FILE__; this call tree passes the element
//  type's RTTI descriptor name instead, and -2 for the line number.
//
//  SMemReAlloc is called with flags 0x10 and its result checked against null,
//  with a full alloc/copy/free fallback behind it - i.e. 0x10 asks Storm to
//  resize in place and to fail rather than move the block.
//----------------------------------------------------------------------------
void* __stdcall SMemAlloc(unsigned int amount, const char* logfilename,
                          int logline, unsigned int flags);
void  __stdcall SMemFree(void* ptr, const char* logfilename,
                         int logline, unsigned int flags);
void* __stdcall SMemReAlloc(void* ptr, unsigned int amount,
                            const char* logfilename, int logline,
                            unsigned int flags);

//: The link offset a TSExplicitList is born with; see the header comment.
const int TSLIST_LINKOFFSET_POISON = -572662307;    // 0xDDDDDDDD

//----------------------------------------------------------------------------
//  One node's membership of one list.
//
//  `m_next` points at the *link* of the next node (the terminator's link when
//  this is the last one), and is null while the node is not in any list.
//
//  `m_prevlink` identifies the predecessor, and is where all the cleverness
//  is: when the predecessor is a node it holds that node's *object* pointer,
//  and when it is the list's own terminator - which is not inside a node and
//  therefore has no object pointer - it holds the complement of the
//  terminator's address.  Object pointers are positive and complemented
//  addresses are negative, so one sign test tells the two apart, and that
//  same test is what ends a walk.  Unlink() needs the list's link offset to
//  turn the predecessor's object pointer back into a link address and does
//  not have the list to hand, so it recovers the offset from the successor,
//  which is holding *this* node's object pointer: this - m_next->m_prevlink.
//----------------------------------------------------------------------------
template <class T>
class TSLink
{
public:
    TSLink() { m_next = 0; m_prevlink = 0; }

    void Unlink()
    {
        if (m_next)
        {
            TSLink<T>** prevlink;
            if (m_prevlink <= 0)
            {
                prevlink = (TSLink<T>**)~m_prevlink;
            }
            else
            {
                // The successor is holding this node's object pointer, so the
                // difference is the list's link offset.
                int linkoffset = (int)this - m_next->m_prevlink;
                prevlink = (TSLink<T>**)(m_prevlink + linkoffset);
            }

            // Both stores go through pointers that may alias this link (the
            // predecessor of the only node in a list is the terminator), so
            // the reloads below are the shipped code's, not an oversight.
            *prevlink = m_next;
            m_next->m_prevlink = m_prevlink;
            m_next = 0;
            m_prevlink = 0;
        }
    }

    TSLink<T>*  m_next;         // 0x00
    int         m_prevlink;     // 0x04
};

//----------------------------------------------------------------------------
//  A list whose link offset is a template argument, defaulted to the poison
//  value and overwritten at run time by SetLinkOffset().
//----------------------------------------------------------------------------
template <class T, int LINKOFFSET = TSLIST_LINKOFFSET_POISON>
class TSExplicitList
{
public:
    TSExplicitList()
    {
        m_linkoffset = LINKOFFSET;
        Reset();
    }

    // Copying an element of the bucket array carries the link offset across
    // and starts the copy empty; the nodes stay where they are and are
    // relinked by the caller.
    TSExplicitList(const TSExplicitList<T, LINKOFFSET>& other)
    {
        m_linkoffset = other.m_linkoffset;
        Reset();
    }

    ~TSExplicitList()
    {
        UnlinkAll();
        m_terminator.Unlink();
    }

    void Reset()
    {
        m_terminator.m_next = &m_terminator;
        m_terminator.m_prevlink = ~(int)&m_terminator;
    }

    void SetLinkOffset(int linkoffset)
    {
        if (m_linkoffset != linkoffset)
        {
            UnlinkAll();
            m_linkoffset = linkoffset;
            Reset();
        }
    }

    TSLink<T>* Link(T* ptr)
    {
        if (ptr)
            return (TSLink<T>*)(m_linkoffset + (int)ptr);
        return &m_terminator;
    }

    //  The tail as the field stands: positive is the last node's object
    //  pointer, negative the terminator, i.e. the list is empty.  Every loop
    //  that drains a list tests this sign directly rather than going through
    //  the masked pointer Tail() hands back.
    //
    //  Going through Link(0) rather than reading m_terminator directly is
    //  load-bearing, not decoration: it is what puts the shipped code's
    //  separate `lea` of the terminator address in the stream, and with the
    //  test that much bigger MSVC stops duplicating it at the bottom of the
    //  loop, which is how the shipped code enters all of these loops.  Worth
    //  ~30 instructions across Rehash, UnlinkAll and their callers.
    int TailLink()
    {
        return Link(0)->m_prevlink;
    }

    T* Tail()
    {
        int prevlink = TailLink();
        return prevlink > 0 ? (T*)prevlink : 0;
    }

    //  Raw, like TailLink() and for the same reason: inside a walk the value
    //  is only ever used after its sign has been tested, and the shipped code
    //  does not mask it there.  Tail() is the masked one, for entering a walk.
    T* Prev(T* ptr)
    {
        return (T*)Link(ptr)->m_prevlink;
    }

    //  The head, in the same raw/masked pair as TailLink()/Tail().
    //
    //  There is no field holding the head's *object* pointer - m_terminator
    //  only knows the head's *link* address, and turning that back into an
    //  object needs the link offset the caller may not have.  So the shipped
    //  code takes the long way round: the head's successor is holding the
    //  head's object pointer in its own m_prevlink, and the head's successor
    //  is one m_next hop past the terminator.  On an empty list every hop
    //  lands back on the terminator, whose m_prevlink is the negative
    //  complement - so the same sign test that ends a walk also says "empty",
    //  with no null check anywhere.
    int HeadLink()
    {
        return Link(0)->m_next->m_next->m_prevlink;
    }

    T* Head()
    {
        int headlink = HeadLink();
        return headlink > 0 ? (T*)headlink : 0;
    }

    //  Forced: the shipped code has this inlined at every one of its call
    //  sites, and MSVC's cost model declines it at exactly one of them
    //  (TSHashTable::NewNode, where it is not inside a loop).
    __forceinline void LinkToHead(T* ptr)
    {
        TSLink<T>* link = Link(ptr);
        link->Unlink();

        TSLink<T>* head = m_terminator.m_next;
        link->m_next = head;
        link->m_prevlink = head->m_prevlink;
        head->m_prevlink = (int)ptr;
        m_terminator.m_next = link;
    }

    //  Splice `ptr` in immediately after `after`, or at the head when
    //  `after` is null - Link(0) being the terminator is what makes those
    //  one code path and not two.  Forced for the same reason LinkToHead is:
    //  CEventBucketList::InsertHandler (0x6F631DC0) has it written out.
    __forceinline void LinkAfter(T* after, T* ptr)
    {
        TSLink<T>* link = Link(ptr);
        link->Unlink();

        TSLink<T>* at = Link(after);
        TSLink<T>* next = at->m_next;
        link->m_next = next;
        link->m_prevlink = next->m_prevlink;
        next->m_prevlink = (int)ptr;
        at->m_next = link;
    }

    //  The other end.  Splicing at the tail needs the *predecessor's* link
    //  address, which the terminator only knows as an object pointer, so
    //  this is where the run-time link offset is used for real: the field
    //  when it is non-negative, and the head's own m_prevlink to recover it
    //  when it is still the poison value.  Forced for the same reason
    //  LinkToHead is - CMessageFrame::AddMessage (0x6F623830) has it
    //  written out twice.  The TSList twin below is the identical body with
    //  the offset folded.
    __forceinline void LinkToTail(T* ptr)
    {
        TSLink<T>* link = Link(ptr);
        link->Unlink();

        link->m_next = &m_terminator;
        link->m_prevlink = m_terminator.m_prevlink;

        TSLink<T>** prevlink;
        int tail = m_terminator.m_prevlink;
        if (tail <= 0)
        {
            prevlink = (TSLink<T>**)~tail;
        }
        else
        {
            int linkoffset = m_linkoffset;
            if (linkoffset < 0)
                linkoffset = (int)&m_terminator
                             - m_terminator.m_next->m_prevlink;
            prevlink = (TSLink<T>**)(tail + linkoffset);
        }

        *prevlink = link;
        m_terminator.m_prevlink = (int)ptr;
    }

    void UnlinkAll();           // out of line: 0x6F46B560

    int         m_linkoffset;   // 0x00
    TSLink<T>   m_terminator;   // 0x04

private:
    TSExplicitList<T, LINKOFFSET>& operator=(
        const TSExplicitList<T, LINKOFFSET>&);
};

//----------------------------------------------------------------------------
//  The same list with the link offset known at compile time.
//
//  Storm ships both shapes and the disassembly tells them apart in one
//  instruction: `TSExplicitList` loads m_linkoffset out of the object before
//  every Link() (`mov eax,[list]` / `add eax,node`), while this one folds the
//  constant away - so a walk whose tail value is dereferenced *directly* as a
//  node (`mov eax,[ecx+8]` / `mov edi,[eax]`, no add) is this template with
//  LINKOFFSET 0, i.e. a T whose TSLink is its first member.  Both keep the
//  m_linkoffset field, and both constructors store it, which is why the
//  layout is identical and only the accessors differ.
//
//  Deliberately not derived from TSExplicitList: the two have separate
//  out-of-line UnlinkAll() bodies in the shipped binary (0x6F46B560 for the
//  explicit one, 0x6F6DE1B0 for this one), and a derived class would either
//  hide the base's accessors while still inheriting its destructor - which
//  would drain the list twice - or drag the runtime offset load back in.
//----------------------------------------------------------------------------
template <class T, int LINKOFFSET>
class TSList
{
public:
    //  The stored offset is **zero**, not LINKOFFSET.  Two shipped
    //  constructors say so directly - CLayer::CLayer (0x6F600720) writes 0
    //  into its children list's m_linkoffset at +0x14 and
    //  CLayoutFrame::CLayoutFrame (0x6F6065C0) writes 0 into its dependents
    //  list's at +0x38 - while both lists really are walked at a link offset
    //  of 4.  The field exists only so this template and TSExplicitList have
    //  the identical layout; TSList's own accessors fold the constant and
    //  never read it.  (TSExplicitList's constructor really does store its
    //  own poison value, which 0x6F600430's `mov dword ptr [edi],
    //  0DDDDDDDDh` confirms, so only this one changed.)
    TSList()
    {
        m_linkoffset = 0;
        Reset();
    }

    ~TSList()
    {
        UnlinkAll();
        m_terminator.Unlink();
    }

    void Reset()
    {
        m_terminator.m_next = &m_terminator;
        m_terminator.m_prevlink = ~(int)&m_terminator;
    }

    TSLink<T>* Link(T* ptr)
    {
        if (ptr)
            return (TSLink<T>*)(LINKOFFSET + (int)ptr);
        return &m_terminator;
    }

    //  Raw and masked accessors, same pairing and same reasoning as
    //  TSExplicitList's - see the comments there.
    int TailLink()  { return Link(0)->m_prevlink; }
    T*  Tail()      { int t = TailLink(); return t > 0 ? (T*)t : 0; }
    T*  Prev(T* ptr){ return (T*)Link(ptr)->m_prevlink; }
    int HeadLink()  { return Link(0)->m_next->m_next->m_prevlink; }
    T*  Head()      { int h = HeadLink(); return h > 0 ? (T*)h : 0; }

    //  Forced for the same reason as TSExplicitList::LinkToHead: the shipped
    //  code has it inlined at its one call site here (TCPCONN::Queue).
    __forceinline void LinkToHead(T* ptr)
    {
        TSLink<T>* link = Link(ptr);
        link->Unlink();

        TSLink<T>* head = m_terminator.m_next;
        link->m_next = head;
        link->m_prevlink = head->m_prevlink;
        head->m_prevlink = (int)ptr;
        m_terminator.m_next = link;
    }

    //  LinkToHead with an arbitrary predecessor link instead of the
    //  terminator: the same five stores, and the only thing that makes
    //  CLayer::AddChild's second splice a different block from its first.
    __forceinline void LinkAfter(TSLink<T>* where, T* ptr)
    {
        TSLink<T>* link = Link(ptr);
        link->Unlink();

        TSLink<T>* next = where->m_next;
        link->m_next = next;
        link->m_prevlink = next->m_prevlink;
        next->m_prevlink = (int)ptr;
        where->m_next = link;
    }

    //  The other end.  Splicing at the tail needs the *predecessor's* link
    //  address, which the terminator only knows as an object pointer, so
    //  this is where the link offset is needed for real: the field is used
    //  when it is non-negative and recovered from the head's own m_prevlink
    //  when it is still the poison value.  Forced for the same reason
    //  LinkToHead is - CFrame::AddLight (0x6F607E80) has it written out.
    __forceinline void LinkToTail(T* ptr)
    {
        TSLink<T>* link = Link(ptr);
        link->Unlink();

        link->m_next = &m_terminator;
        link->m_prevlink = m_terminator.m_prevlink;

        TSLink<T>** prevlink;
        int tail = m_terminator.m_prevlink;
        if (tail <= 0)
        {
            prevlink = (TSLink<T>**)~tail;
        }
        else
        {
            int linkoffset = m_linkoffset;
            if (linkoffset < 0)
                linkoffset = (int)&m_terminator
                             - m_terminator.m_next->m_prevlink;
            prevlink = (TSLink<T>**)(tail + linkoffset);
        }

        *prevlink = link;
        m_terminator.m_prevlink = (int)ptr;
    }

    void UnlinkAll();           // out of line: 0x6F6DE1B0

    //  Same walk as UnlinkAll, but each node is SMemFree'd as it comes off
    //  the list instead of merely unlinked - only one shipped caller so far
    //  (0x6F66F230, w3gsaction_tier5.cpp), see tslist_destroy.inl.
    void DestroyAll();

    int         m_linkoffset;   // 0x00
    TSLink<T>   m_terminator;   // 0x04

private:
    TSList(const TSList<T, LINKOFFSET>&);
    TSList<T, LINKOFFSET>& operator=(const TSList<T, LINKOFFSET>&);
};

//----------------------------------------------------------------------------
//  A vector that grows in chunks and reallocates through Storm.
//----------------------------------------------------------------------------
template <class T>
class TSGrowableArray
{
public:
    T& operator[](unsigned int index) { return m_data[index]; }

    //  Out of line (0x6F00AAA0 and its sibling instantiations below) even
    //  though every caller in these call trees reaches it only through a
    //  containing object's own destructor: releasing m_data through Storm is
    //  not something MSVC inlines into a caller that never touches T's own
    //  members, so it stays a real call just like SetAlloc/SetCount do.
    ~TSGrowableArray();

    //  Append one element.  Inline, and it has to be: the shipped body of
    //  BuildAndRegisterEventBinding (agenteventbinding.cpp) carries this
    //  sequence fifteen times with no call between the copies, which is
    //  what /Ob2 does to it.  It is not SetCount(m_count+1) followed by a
    //  store - that would set m_count inside the grow and read it back
    //  afterwards, where the shipped stream reads m_count once, forms the
    //  element address from it, increments, and only then stores the
    //  value.
    //
    //  The grow is SetCount's own grow with its `count > m_count` test
    //  dropped: for count = m_count + 1 it is always true, and the shipped
    //  code has no branch for it.
    //  __forceinline for the reason storm.h's own LinkToHead and
    //  position.h's constructor are: /Ob2's cost model declines a body
    //  this size on its own (measured - 484 generated instructions
    //  against the shipped 829, with fifteen `call ...::Add` where the
    //  shipped stream has the sequence written out), and the shipping
    //  build plainly inlined it.
    __forceinline void Add(const T& value)
    {
        unsigned int count = m_count + 1;
        if (count > m_alloc)
        {
            unsigned int chunk = m_chunk;
            if (chunk == 0)
                chunk = ComputeChunk(count);

            unsigned int alloc = count;
            unsigned int over = count % chunk;
            if (over != 0)
                alloc = count + (chunk - over);

            SetAlloc(alloc);
        }

        T* slot = &m_data[m_count];
        m_count++;
        *slot = value;
    }

    //  Grow by one and default-construct the new element in place, handing
    //  back its address (or null when the allocation failed - the shipped
    //  code's own placement-new null test is what produces that answer).
    //  Out of line at 0x6F0A4730 for the CAgentPtr<CUnit> instantiation;
    //  unlike Add() above it is a real call at its one call site, which is
    //  why it is declared here and defined in tsarray.inl.
    T* New();                                   // out of line: 0x6F0A4730

    //  Grow by one and hand back the new slot *uninitialised*.  New()
    //  without the placement `new`, and the shipped bodies say so directly:
    //  0x6F2867E0 and 0x6F286700 are New()'s stream with neither the
    //  element store nor the null test MSVC puts in front of a placement
    //  `new`, which even a POD `new (slot) T` still emits.  Out of line at
    //  both of those addresses, and a real call at every site.
    T* Grow();                                  // out of line: 0x6F2867E0

    void SetCount(unsigned int count);          // out of line: 0x6F46F0C0
    void SetAlloc(unsigned int alloc);          // out of line: 0x6F46DFB0

    //  Two more members, reached only by the CAgentPtr instantiation (the
    //  hero list, src/Agent/agentptrarray_*.cpp).  Neither is SetAlloc or
    //  SetCount respelled:
    //
    //    * ReleaseAndReserve (0x6F379640) destroys *every* live element and
    //      then resizes the block outright, with Storm's plain flags rather
    //      than SetAlloc's "resize in place or fail" 0x10 and with no
    //      alloc/copy/free fallback behind it - it has nothing to move,
    //      because it has just destroyed everything;
    //    * Assign (0x6F37B6D0) is that followed by a placement-copy of
    //      `count` elements out of another array and a store of the new
    //      count.  Its per-element null test is placement new's, not the
    //      source's.
    void ReleaseAndReserve(unsigned int alloc); // out of line: 0x6F379640
    void Assign(unsigned int count, const T* src);   // out of line: 0x6F37B6D0
    unsigned int ComputeChunk(unsigned int count);  // out of line: 0x6F467B00

    unsigned int    m_alloc;    // 0x00
    unsigned int    m_count;    // 0x04
    T*              m_data;     // 0x08
    unsigned int    m_chunk;    // 0x0C
};

//----------------------------------------------------------------------------
//  The same vector without the growth chunk: three fields, and a SetCount
//  that sizes the allocation to exactly the count it is given (and releases
//  it outright for a count of zero) instead of rounding up to a chunk.
//  TSGrowableArray is this plus m_chunk, which is why the two share
//  SetAlloc's body instruction for instruction across their instantiations.
//
//  Reached through CGameData::m_queryPool (game.h), the stack of
//  AgileAgentQuery scratch objects the agent enumerators borrow from -
//  0x6F469880 SetAlloc, 0x6F46A950 SetCount, both in
//  src/Containers/agentquerypool.cpp.
//----------------------------------------------------------------------------
template <class T>
class TSFixedArray
{
public:
    T& operator[](unsigned int index) { return m_data[index]; }

    void SetCount(unsigned int count);
    void SetAlloc(unsigned int alloc);

    unsigned int    m_alloc;    // 0x00
    unsigned int    m_count;    // 0x04
    T*              m_data;     // 0x08
};

//----------------------------------------------------------------------------
//  What a hash-table node has to start with.  The bucket list threads through
//  m_hashlink; the table's own all-nodes list threads through a second link
//  somewhere in the eight bytes after it, but this call tree only ever
//  reaches that one through the run-time link offset in the list itself, so
//  its position is not recoverable and it is left unnamed.
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  Storm's own "this table has no key" policy.  An empty class whose compare
//  is a constant, which is exactly why a table keyed on it never reads the
//  key argument it is still handed (the solid-colour texture table's lookup,
//  0x6F4E1150, is one; Config/preferences.h's CPreferencesWar3 is another).
//  Shared here rather than declared per-file so two headers that both derive
//  from TSHashTable<T,HASHKEY_NONE> don't collide over two separate
//  definitions of the same name.
//----------------------------------------------------------------------------
class HASHKEY_NONE
{
public:
    int operator==(const HASHKEY_NONE&) const { return 1; }
};

template <class T, class K>
class TSHashObject
{
public:
    unsigned int    m_hashval;      // 0x00
    TSLink<T>       m_hashlink;     // 0x04
    char            m_reserved0C[8];// 0x0C  the table-wide list link
    K               m_key;          // 0x14
};

//----------------------------------------------------------------------------
//  Chained hash table.  The bucket count is always a power of two and only
//  the mask is stored; -1 means "no buckets yet".  Nodes are allocated by the
//  derived class through the virtual below, so the table itself never sees T's
//  storage.
//----------------------------------------------------------------------------
template <class T, class K>
class TSHashTable
{
public:
    virtual ~TSHashTable();
    //  Declared, never defined, and deliberately *not* pure: a pure virtual
    //  makes the class abstract, and the font cache at 0x6FAB7D44 is a real
    //  global of this type that has to be `extern`-declarable (Render/text.h).
    //  Nothing constructs a TSHashTable here, so no vtable is emitted and the
    //  missing definition never reaches the linker; the generated code for
    //  every member is unchanged either way.
    virtual T* AllocNode(TSExplicitList<T>* bucket, void* arg1, void* arg2);

    T* Ptr(unsigned int hashval, const K& key);         // 0x6F001EC0/0x6F46E720
    // A third lookup shape, for the instantiations whose K is one of
    // Storm's *string* key policies (HASHKEY_STR / HASHKEY_STRI, see
    // Containers/allochashtable.h): the key is a `const char*` in the node
    // and the table hashes it itself rather than being handed a hash value.
    // Defined in Containers/tshashstr.inl, which only the string-keyed
    // instantiations include.  0x6F4CD050 is the font cache's; 0x6F5C7920
    // and two more are the three frame registries'.
    T* Ptr(const char* key);                            // 0x6F4CD050
    // 0x6F4CCA70 - take a node out of both of the table's lists.  The guard
    // is the bucket link's m_prevlink: the two links are always spliced and
    // unspliced together, so one test covers both.
    void Unlink(T* ptr);                                // 0x6F4CCA70
    // A second lookup shape, reached only from the handle-to-agent table
    // (0x6F42CCF0): the walk is identical to Ptr()'s but the match test only
    // ever reads ptr->m_hashval, never the key argument - see tshash.inl for
    // the reasoning.  Kept separate from Ptr() itself so the AGILE_TYPE_DATA
    // instantiations, which do need the key compared, are untouched.
    T* PtrHashOnly(unsigned int hashval, const K& key); // 0x6F42CCF0
    T* New(unsigned int hashval, const K& key,
           void* arg1, void* arg2);                     // 0x6F4713D0
    T* NewNode(unsigned int hashval, void* arg1, void* arg2);   // 0x6F4710A0
    void Initialize();                                  // 0x6F4700B0
    int CheckGrow(unsigned int index);                  // 0x6F470CC0
    void Rehash(unsigned int newcount);                 // 0x6F4701D0
    //  Empty every bucket and the all-nodes list - 0x6F367470/0x6F3675B0,
    //  the CommandReg/OrderTypeReg instantiations (Containers/
    //  commandregtable.cpp, ordertyperegtable.cpp).  `eachInPlace` true
    //  unlinks every node in place (TSLink<T>::Unlink() on each, the same
    //  small body TSExplicitList<T>::UnlinkAll() inlines - see tshash.inl);
    //  false instead calls through this table's own vtable slot 0 per node,
    //  which both known call sites always pass true and never exercise.
    void UnlinkAll(bool eachInPlace);                   // 0x6F367470/0x6F3675B0

    // Where the bucket list's link sits inside a node.  A constant the
    // compiler folds, but one that has to be recomputed per instantiation:
    // it is 4 for a plain node and 8 for one that carries a vftable.
    static int HashLinkOffset()
    {
        return (int)(unsigned int)&((T*)0)->m_hashlink;
    }

    TSExplicitList<T>                       m_list;     // 0x04 every node
    unsigned int                            m_probes;   // 0x10 chain-length run
    TSGrowableArray<TSExplicitList<T> >     m_buckets;  // 0x14
    unsigned int                            m_mask;     // 0x24
};

#endif

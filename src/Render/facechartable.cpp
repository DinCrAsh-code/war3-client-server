//============================================================================
//  FACEDATA's character-code-descriptor bucket table
//  (`TSHashTable<CHARCODEDESC,HASHKEY_NONE>`-shaped, Render/facedata.h) -
//  its constructor, destructor and Clear.
//
//  Clear(freeNodes) walks every bucket and, per node still in it, either
//  physically unlinks it (freeNodes != 0 - the same TailLink()/Link()/
//  Unlink() walk TSExplicitList<T>::UnlinkAll() itself does, one node at a
//  time) or hands it to the table's own vtable slot 0 (freeNodes == 0) - a
//  virtual FreeNode(T*) storm.h's TSHashTable<T,K> never declares.  Read as
//  a raw indirect call rather than through a `virtual` declaration for the
//  same reason facedata.h's own header comment gives for the whole table
//  shape: declaring it would force this build to emit a vtable of its own,
//  which needs a real address for the AllocNode slot this repo has no body
//  for.  The call site itself is unambiguous - `this` in ecx, the node
//  pointer as the one stack argument, straight off the dump.
//============================================================================
#include "facedata.h"
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UCHARCODEDESC@@$0?CCCCCCCD@@@"
#include "tsarray.inl"
#include "tslist.inl"

//  0x6F7C2960 - m_list.SetLinkOffset(0x0C) is the whole point of the body,
//  same shape as Frame/shortcutregtable.cpp's SHORTCUTREGTABLE ctor: the
//  list is default-constructed with Storm's poison offset and this replaces
//  it, draining the (empty) list on the way past.
CHARCODEDESC_TABLE::CHARCODEDESC_TABLE()
    : m_vtable((void*)kCharCodeDescTableVftable)
{
    m_buckets.m_alloc = 0;
    m_buckets.m_count = 0;
    m_buckets.m_data = 0;
    m_buckets.m_chunk = 0;
    m_probes = 0;
    m_list.SetLinkOffset(0x0C);
    m_mask = 0xFFFFFFFFu;
}

//  0x6F7C2B80 - Clear(1) first, then the two members' own destructors:
//  m_buckets (TSGrowableArray<TSExplicitList<CHARCODEDESC>>, out of line at
//  0x6F7C0CD0) and m_list (TSExplicitList<CHARCODEDESC>::UnlinkAll at
//  0x6F7BECC0 plus its terminator's own inlined Unlink()), run automatically
//  in reverse declaration order.  The `__CxxFrameHandler3`/security-cookie
//  frame the dump wraps this in is the shape
//  docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" already documents and gives up on: this
//  build's fixed /GS- /EHs-c- cannot reach it with the bundled compiler, so
//  the frame furniture never matches however the body is spelled.
CHARCODEDESC_TABLE::~CHARCODEDESC_TABLE()
{
    m_vtable = (void*)kCharCodeDescTableVftable;
    Clear(1);
}

//  0x6F7BED70 - `retn 4`, thiscall + one stack bool.
void CHARCODEDESC_TABLE::Clear(unsigned int freeNodes)
{
    m_probes = 0;
    m_list.UnlinkAll();

    typedef void (__thiscall *FreeNodeFn)(void*, void*);

    for (unsigned int i = 0; i < m_buckets.m_count; i++)
    {
        TSExplicitList<CHARCODEDESC>& bucket = m_buckets[i];
        for (;;)
        {
            int tail = bucket.TailLink();
            if (tail <= 0)
                break;
            if (freeNodes)
                bucket.Link((CHARCODEDESC*)tail)->Unlink();
            else
                ((FreeNodeFn)(*(void***)this)[0])(this, (void*)tail);
        }
    }
}

//  0x6F7BECC0 - the parked list's own UnlinkAll, and the bucket array's
//  UnlinkAll (called once per live bucket from Clear() above).
template void TSExplicitList<CHARCODEDESC>::UnlinkAll();

//  0x6F7C0CD0 - the bucket array's own destructor: every live bucket is a
//  TSExplicitList<CHARCODEDESC>, whose destructor (UnlinkAll() plus the
//  terminator's own inlined Unlink()) is non-trivial, so ~TSGrowableArray's
//  generic loop in tsarray.inl actually runs it per element here.
template TSGrowableArray<TSExplicitList<CHARCODEDESC> >::~TSGrowableArray();

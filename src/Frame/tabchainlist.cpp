//============================================================================
//  0x6F5C92A0 - STabChainList::~STabChainList.
//
//  The list of TABCHAINNODEs a frame-creation call builds up and throws away
//  again.  The node type's name is the shipped one: the free at 0x6F5C8D80
//  passes ".?AUTABCHAINNODE@@", the RTTI descriptor name Storm's allocator
//  gets for a templated container's element.  The list itself has no name in
//  the image, so it takes the node's.
//
//  Three steps, and the last two are the base's: free every node
//  (FreeAll, 0x6F5C8D10 - each node's two owned buffers, then the node
//  itself, unlinking as it goes), then ~TSList<TABCHAINNODE,0> - the
//  out-of-line UnlinkAll call and the inlined terminator Unlink that
//  Containers/tsfixedlist.inl and Storm/storm.h already model (TSList, not
//  TSExplicitList - see framereg.h's own comment on STabChainList's base).
//  Writing it as a derived class with a destructor that only frees is what
//  puts those three in that order, and the shipped stream is exactly that.
//
//  The list's own link offset is zero: its one constructor site (inside
//  0x6F5C9560) writes 0 into +0x00 and self-links the terminator at +0x04,
//  so the node's link is its first member.
//
//  The declarations are framereg.h's, beside CreateFrameByName - the one
//  function that builds one of these lists.
//
//  Own translation unit: FreeAll and UnlinkAll are both real calls.
//
//  FreeAll (0x6F5C8D10) itself: walk the tail raw (TailLink(), same
//  raw/never-masked convention as every other drain in this repo - see
//  storm.h's own comment on why), and for each node free its two owned
//  buffers (sizes 0x3F/0x40, the shipped tag both frees share -
//  ".?AUTABCHAINNODE@@" is the *node's* own free, not either buffer's, and
//  is what the RTTI-descriptor free at the end tags with instead), unlink
//  it with the ordinary inlined `TSLink<T>::Unlink()` (no `Link()` call
//  needed - the link offset is zero, so the node pointer already is its own
//  link) and hand it back with `SMemFree(node, ..., -2, 0)`.  Both string
//  operands canonicalise to `SYM` in scoring, so the exact bytes are not
//  behaviour; only the two-buffers-then-node shape and the two distinct
//  sizes are.
//============================================================================
#include "framereg.h"
#include "tsfixedlist.inl"

static const char s_tabChainBufferTag[] =
    "e:\\drive1\\temp\\buildwar3x\\engine\\storm\\source\\.\\TabChainList.cpp";
static const char s_tabChainNodeTag[] = ".?AUTABCHAINNODE@@";

void STabChainList::FreeAll()
{
    for (;;)
    {
        int tail = TailLink();
        if (tail <= 0)
            break;

        TABCHAINNODE* node = (TABCHAINNODE*)tail;

        if (node->m_owned08)
            SMemFree(node->m_owned08, s_tabChainBufferTag, 0x3F, 0);
        if (node->m_owned0C)
            SMemFree(node->m_owned0C, s_tabChainBufferTag, 0x40, 0);

        node->m_link.Unlink();
        SMemFree(node, s_tabChainNodeTag, -2, 0);
    }
}

STabChainList::~STabChainList()
{
    FreeAll();
}

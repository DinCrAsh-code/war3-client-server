//============================================================================
//  Two more small hash-table-node bodies in the JASS VM module's own
//  address neighbourhood, the same "two embedded TSLink members, unlink
//  most-recently-declared first" and "TSList<T,0> freelist drain" shapes
//  jasstablenodethunks.h/jasstablenodethunks.cpp already document for the
//  nine record types there - these two addresses are not among that
//  documented nine, so the real element type is left unconfirmed rather
//  than guessed at.
//============================================================================
#include "tslist.inl"

//----------------------------------------------------------------------------
//  0x6F44CB40 - `retn 0`, real `this`-in-ecx.  Two embedded TSLink members
//  at +0x0C and +0x04, unlinked most-recently-declared-first - the same
//  base-Dtor() shape as jasstablenodethunks.cpp's own nine, but at an
//  address none of those nine claim.  Nothing in this closure's own reach
//  (no allocator, no type-descriptor string) identifies which table this
//  node belongs to.
//----------------------------------------------------------------------------
struct STrayTwoLinkNode
{
    char                      m_reserved00[4];
    TSLink<STrayTwoLinkNode>  m_reserved04;
    TSLink<STrayTwoLinkNode>  m_reserved0C;

    void Dtor();
};

//  0x6F44CB40
void STrayTwoLinkNode::Dtor()
{
    m_reserved0C.Unlink();
    m_reserved04.Unlink();
}

//----------------------------------------------------------------------------
//  0x6F44F410 - `retn 0`, real `this`-in-ecx.  Drains a `TSList<T,0>`-shaped
//  freelist at +0x08 (LINKOFFSET 0 - the node dereferences directly, no
//  linkoffset add, jasstablenodethunks.h's own TSList-vs-TSExplicitList
//  tell): repeatedly takes the tail, unlinks it in place and hands it to
//  Storm ordinal 403 tagged "AUInitFuncAddr" - the shipped type-descriptor
//  string this call site pushes, so the real element name is
//  `InitFuncAddr` (the same "AU" allocation-tag-vs-C++-name relationship
//  jassscriptframepool.h's own "AUFrame"-tagged `JassScriptFrame` uses).
//----------------------------------------------------------------------------
struct InitFuncAddr
{
    TSLink<InitFuncAddr> m_link;   // +0x00 - LINKOFFSET 0, the node is its own link
};

struct InitFuncAddrFreeList
{
    char m_reserved00[8];
    TSLink<InitFuncAddr>* m_tail;   // +0x08 - TailLink(), raw (LINKOFFSET 0)

    void FreeAll();
};

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

//  0x6F44F410
void InitFuncAddrFreeList::FreeAll()
{
    InitFuncAddr* node = (InitFuncAddr*)m_tail;
    while (node != 0)
    {
        node->m_link.Unlink();
        SMemFree(node, ".?AUInitFuncAddr@@", -2, 0);
        node = (InitFuncAddr*)m_tail;
    }
}

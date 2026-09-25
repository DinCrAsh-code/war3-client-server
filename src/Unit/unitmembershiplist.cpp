//============================================================================
//  0x6F4219B0 - SUnitMembershipList::Contains.  Reached only through
//  SUnitMembershipSet::Contains (unitmembershipset.cpp), which was a naked
//  redirect in unit_playertable.cpp until now.
//
//  Four gates, then a list walk:
//
//    * a null candidate is not a member;
//    * its {handle, typeTag} pair at +0x0C/+0x10 has to resolve;
//    * what resolves has to carry the 'lga+' tag at its own +0x0C - tested
//      branchlessly, `setnz`/`sub`/`and`, exactly as
//      SAttachmentOwnerQuery::QueryOwner does (attachmentownerquery.cpp);
//    * and that object's +0x20 has to be clear.
//
//  Only then is the list walked, and the walk compares the *candidate*
//  pointer against each node's +0x08 - not the resolved object.
//
//  Both `<= 0` tests are signed and neither is a null check: the head is
//  sanitised with the same `setle`/`sub`/`and` mask the Storm list walks
//  use, and the loop ends on `jg`.
//
//  Own translation unit: SUnitMembershipSet::Contains tail-calls it.
//============================================================================
#include "unitmembership.h"

class CHandleObject;
CHandleObject* __fastcall LookupHandle(unsigned int handle, int typeTag);

//  The candidate, as this function reads it.
struct SMembershipCandidate
{
    char         m_reserved00[0x0C];
    unsigned int m_handle;      // +0x0C
    int          m_typeTag;     // +0x10
};

//  What the handle resolves to: the same 'lga+'-tagged block
//  attachmentownerquery.cpp names.
struct SMembershipOwnerBlock
{
    char         m_reserved00[0x0C];
    unsigned int m_tag;         // +0x0C - 'lga+'
    char         m_reserved10[0x10];
    int          m_gate;        // +0x20
};

int __thiscall SUnitMembershipList::Contains(void* unit)
{
    SMembershipCandidate* candidate = (SMembershipCandidate*)unit;
    if (candidate == 0)
        return 0;

    SMembershipOwnerBlock* block = (SMembershipOwnerBlock*)
        LookupHandle(candidate->m_handle, candidate->m_typeTag);
    if (block == 0)
        return 0;

    //  A mask, not a branch - see attachmentownerquery.cpp.
    block = (SMembershipOwnerBlock*)((unsigned int)block &
                (unsigned int)((block->m_tag != 0x2B61676Cu) - 1));
    if (block == 0)
        return 0;
    if (block->m_gate != 0)
        return 0;

    //  The head goes through the same sanitising mask the list terminator
    //  needs: a value at or below zero becomes zero, and the walk then
    //  tests it as signed.
    SUnitMembershipNode* node = m_pHead;
    node = (SUnitMembershipNode*)((unsigned int)node &
               (unsigned int)(((int)(unsigned int)node <= 0) - 1));
    if ((int)(unsigned int)node <= 0)
        return 0;

    do
    {
        if (node->m_pUnit == unit)
            return 1;
        node = node->m_pNext;
    } while ((int)(unsigned int)node > 0);

    return 0;
}

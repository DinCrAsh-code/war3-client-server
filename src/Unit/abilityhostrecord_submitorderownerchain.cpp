//============================================================================
//  0x6F2D8190 - SAbilityHostRecord::SubmitOrderAcrossOwnerChain
//  (abilityhostrecord.h).  Guarded by TestOrArmFlagBit20(); walks the
//  owner chain starting at m_ref28 (resolve, then
//  SAttachmentOwnerQuery::QueryOwner - attachmentownerquery.cpp - on the
//  raw handle it just resolved) and, for every owner reached whose own
//  +0x20 bit 0 is clear, +0x248 bit 4 is set and +0x194 is zero, issues
//  CUnit::SubmitOrder_0xD0006(1).  The next link is the SAME raw handle's
//  own +0x24 pair, resolved and queried the same way.
//
//  The shipped body carries a second loop register (`edi`) this
//  reconstruction's own trace shows can never leave zero on this call
//  path - every write to it is either its own initial `xor edi,edi` or an
//  AND-mask against a boolean that only clears it further - so it never
//  changes the loop's outcome here; see the file's own commit message for
//  the full register trace.  Written without that dead carrier.
//============================================================================
#include "abilityhostrecord.h"
#include "unit.h"

//  0x6F472890 - SAttachmentOwnerQuery::QueryOwner, already reconstructed
//  (Widget/attachmentownerquery.cpp); re-declared here so the mangled name
//  binds to that one real symbol.
struct SAttachmentOwnerQuery
{
    void* __thiscall QueryOwner();

    char         m_reserved00[0x0C];
    unsigned int m_handle;      // +0x0C
    int          m_typeTag;     // +0x10
};

void SAbilityHostRecord::SubmitOrderAcrossOwnerChain()
{
    if (!TestOrArmFlagBit20())
        return;

    void* current = 0;
    void* nextRaw = 0;

    if ((m_ref28.m_typeTag & (int)m_ref28.m_handle) != -1)
    {
        nextRaw = QueryHandleField0x54(&m_ref28);
        if (nextRaw != 0)
            current = ((SAttachmentOwnerQuery*)nextRaw)->QueryOwner();
    }

    while (current != 0)
    {
        if (!(*(unsigned char*)((char*)current + 0x20) & 1) &&
            (*(unsigned int*)((char*)current + 0x248) & 0x10) &&
            *(unsigned int*)((char*)current + 0x194) == 0)
        {
            ((CUnit*)current)->SubmitOrder_0xD0006(1);
        }

        void* newOwner = 0;
        if (nextRaw != 0)
        {
            SOptionalHandleRef* ref = (SOptionalHandleRef*)((char*)nextRaw + 0x24);
            if ((ref->m_typeTag & (int)ref->m_handle) != -1)
            {
                nextRaw = QueryHandleField0x54(ref);
                if (nextRaw != 0)
                    newOwner = ((SAttachmentOwnerQuery*)nextRaw)->QueryOwner();
            }
            else
            {
                nextRaw = 0;
            }
        }
        current = newOwner;
    }
}

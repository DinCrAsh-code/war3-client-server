//============================================================================
//  0x6F3726D0 / 0x6F372C30 - looking a four-character order-type code up in
//  CCommandBar's own registry (m_orderTypes @ +0x188, GameUI/commandbar.cpp),
//  reached from CUnit's own order dispatch (sub_6F3B3090) as
//  GetGameUI()->m_pCommandCard->LookupOrderTypeHandlerToken(orderId).
//
//  0x6F3726D0 is the table's own Ptr(): walk the bucket for `hashval`,
//  matching a node's own hashval field and (once matched) its 4CC key
//  against the caller's, returning the node or null.  Its own bucket-table
//  field reads (this+0x1C the array, this+0x24 the mask) are the identical
//  offsets sub_6F001EC0 (TSHashTable<AGILE_TYPE_DATA,AGILE_TYPE_ID>::Ptr)
//  reads - same TSHashTable<T,K> base, this is a second instantiation of
//  the same template member.
//
//  Its NODE offsets disagree with Containers/hashtablectors.h's own note
//  that OrderTypeReg's link offset is 0x0C ("nodes carry a vtable ahead of
//  the hash link"): this walk compares a candidate's own +0x00 against the
//  hash and, once matched, +0x14 against the caller's key - the *unshifted*
//  TSHashObject<T,K> layout (hashval@0x00, key@0x14), not the +0x04/+0x18
//  shift a node-owned vtable would put in front of it (which is exactly
//  what sub_6F001EC0 itself reads, for AGILE_TYPE_DATA, which does have one -
//  see agiletype.h).  Transcribed from the bytes as found; the discrepancy
//  with that header's note is left for whoever next has IDA access to the
//  real OrderTypeReg node RTTI, not resolved by guessing here.
//
//  0x6F372C30 is CCommandBar's own `retn 4` member wrapping it (`this` in
//  ecx, one stack word cleaned by the callee - declaring it as a plain free
//  function pushed 0 bytes where the real callee cleans 4, exactly the
//  FATAL ABI class tools/abi_audit.py exists to catch, so it is a real
//  __thiscall member here instead): hash the order id the same way every
//  other four-character code in this binary is hashed (AGILE_TYPE_ID::Hash,
//  0x6F4C8520, reused verbatim - every AGILE_TYPE_ID sibling table already
//  does this for a rawcode that is not itself an AGILE_TYPE_ID), look the
//  node up, and return the field right after its key (+0x18) - a
//  per-order-type token or handler id, never read further than that value
//  by anything in this call tree.
//============================================================================
#include "storm.h"
#include "agiletype.h"

//  The node this table's own +0x1C/+0x24 walk expects: TSHashObject<T,K>'s
//  own unshifted layout (see the file header for why this is not the
//  vtable-shifted shape hashtablectors.h's own note for OrderTypeReg
//  describes).  Kept local and untyped past what this call tree touches,
//  the same convention as HANDLE2AGENT / CommandReg.
struct SOrderTypeRegNode
{
    unsigned int m_hashval;      // +0x00
    char         m_reserved04[0x10];
    unsigned int m_key4cc;       // +0x14
    unsigned int m_handlerToken; // +0x18
};

//  The table itself: TSHashTable<T,K>'s fixed shape (Storm/storm.h) - the
//  bucket array pointer at +0x1C and the mask at +0x24, identical to every
//  other TSHashTable<T,K>::Ptr in this repo regardless of T.
struct SOrderTypeRegTableView
{
    char          m_reserved00[0x1C];
    unsigned int* m_bucketData;   // +0x1C
    char          m_reserved20[0x04];
    unsigned int  m_mask;         // +0x24

    //  0x6F3726D0, `retn 8`.
    SOrderTypeRegNode* Ptr(unsigned int hashval, const unsigned int* key);
};

SOrderTypeRegNode* SOrderTypeRegTableView::Ptr(unsigned int hashval,
                                               const unsigned int* key)
{
    if (m_mask == 0xFFFFFFFFu)
        return 0;

    unsigned int index = m_mask & hashval;
    //  TSExplicitList<T> is 12 bytes; +4 lands on its own m_next field, and
    //  reading +4 again off that gets to m_prevlink - the "tail" the same
    //  way TSExplicitList<T>::TailLink() does, without the call.
    char* bucket = (char*)m_bucketData + index * 12;
    int tail = *(int*)(bucket + 8);

    SOrderTypeRegNode* node = (tail > 0) ? (SOrderTypeRegNode*)tail : 0;
    while (node)
    {
        if (node->m_hashval == hashval && node->m_key4cc == *key)
            return node;

        char* bucket2 = (char*)m_bucketData + (m_mask & hashval) * 12;
        int linkoffset = *(int*)bucket2;
        int prevlink = *(int*)(linkoffset + (char*)node + 4);
        node = (prevlink > 0) ? (SOrderTypeRegNode*)prevlink : 0;
    }
    return 0;
}

//  CCommandBar itself, sliced to the one field this call tree reaches -
//  m_orderTypes at +0x188 (GameUI/commandbar.cpp's own CCommandBar already
//  names it the same way; not re-included from here to avoid dragging that
//  TU's own CSimpleGrid base into this one, the same "declare a local view"
//  convention HANDLE2AGENT_TABLE and its siblings already use).
struct SOrderTypeRegTableOwner
{
    char                    m_reserved000[0x188];
    SOrderTypeRegTableView  m_orderTypes;   // +0x188

    //  0x6F372C30, `retn 4`.
    unsigned int LookupOrderTypeHandlerToken(unsigned int orderId);
};

unsigned int SOrderTypeRegTableOwner::LookupOrderTypeHandlerToken(unsigned int orderId)
{
    unsigned int key = orderId;
    unsigned int hashval = AGILE_TYPE_ID(key).Hash();

    SOrderTypeRegNode* node = m_orderTypes.Ptr(hashval, &key);
    if (!node)
        return 0;
    return node->m_handlerToken;
}

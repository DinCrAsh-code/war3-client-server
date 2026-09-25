//============================================================================
//  0x6F53E640 - the session's four-character type tag, 'NONE' when there is
//  no session record yet.
//
//  The two values this call tree shows are 'NONE' (the default answered
//  here) and 'BNET' (what CGameUI's constructor compares against before it
//  registers one more UI message).  Both are spelled the way the shipped
//  constants are, with the first character in the low byte.
//
//  Own translation unit: the thread-local chain is a real call.
//============================================================================
#include "netdata.h"

unsigned int __fastcall GetNetSessionTag()
{
    SNetDataSlotRoot* root = (SNetDataSlotRoot*)GetThreadLocalSlot(0x0D);
    SNetSessionInfo* info = (SNetSessionInfo*)root->m_pCache->m_pRecords;

    //  Tested for *having* a record rather than for not having one: the
    //  shipped code falls through to the field read and branches away to
    //  the default, which is what this polarity gives and the other does
    //  not.
    if (info != 0)
        return info->m_sessionTag;

    return kNetSessionTagNone;
}

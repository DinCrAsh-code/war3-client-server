//============================================================================
//  0x6F54B080 - ship one record's accumulated game results.
//
//  Builds the per-player summary blob first (0x6F549790, netgameresultblob
//  .cpp), then hands the twenty-byte result array and two empty strings to
//  the network client's own submit gateway, then frees the array and clears
//  the three words that described it.
//
//  Both string arguments are the *same* empty string in the shipped code -
//  IDA calls the address `ValueName` and its first byte is a zero.  Written
//  as a literal here rather than named in funcmap.DATA: an empty string
//  constant is shared with everything else in the image that passes one.
//
//  The record is addressed off the *CNetData* base, not off the +8 session
//  view: the three words are at +0x2FC/+0x300/+0x304 here where
//  CNetSession::Reset (netsessionclear.cpp) zeroes them at +0x2F4/+0x2F8/
//  +0x2FC.  A separate view struct rather than a member, which is the same
//  "one object, one view per use" split netdata.h's own header explains.
//
//  The shipped prologue carries a /GS buffer cookie for the summary buffer;
//  this repo builds at a fixed /GS- and cannot emit it, which is where most
//  of this function's gap is.
//============================================================================
#include "netdata.h"
#include "netclient.h"
#include "storm.h"

//  0x6F549790 - Net/netgameresultblob.cpp.
int __fastcall BuildGameResultBlob(unsigned char* out);

//----------------------------------------------------------------------------
//  The record's own game-result array, at CNetData offsets.
//----------------------------------------------------------------------------
struct SNetResultArray
{
    char          m_reserved000[0x2FC];
    unsigned int  m_alloc;      // +0x2FC
    unsigned int  m_count;      // +0x300
    void*         m_pResults;   // +0x304
};

void SNetSessionInfo::SubmitGameResults(int index)
{
    SNetResultArray* record =
        (SNetResultArray*)((char*)this + index * 0x304);

    if (record->m_count != 0)
    {
        unsigned char summary[0x84];
        summary[0] = 0;
        BuildGameResultBlob(summary);

        ::SubmitGameResults(record->m_count, record->m_pResults, "", "",
                            summary);

        if (record->m_pResults != 0)
            SMemFree(record->m_pResults, ".?AUGAMERESULT@Net@@", -2, 0);

        record->m_alloc = 0;
        record->m_count = 0;
        record->m_pResults = 0;
    }
}

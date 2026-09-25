//============================================================================
//  0x6F5545F0 - the replay-payload twin of ReadLengthPrefixedPayload
//  (netcommand_readlengthprefixed.cpp) - cap 0x3FD rather than 0x400 (the
//  only difference the dump shows).  Shared by both replay-data Attach
//  hooks (CNetCommandReplayDataUncompressed_Attach and
//  CNetCommandReplayDataCompressed_Attach, netcommand_dispatch.cpp).
//
//  Own translation unit for the same /Ob2-folding reason as its twin -
//  two call sites in the same file were still enough for the inliner to
//  fold both copies in, where the dump has one shared out-of-line
//  function reached through a real `call` from each.
//============================================================================
#include "cdatastore.h"

CDataStore* __fastcall ReadReplayPayloadHeader(CDataStore* self, unsigned char* dst)
{
    unsigned short len = 0;
    self->ReadWord(&len);

    if (len <= 0x3FD)
    {
        self->ReadRawAlias(dst + 2, len);
        *(unsigned short*)dst = len;
    }
    else
    {
        //  Not `m_readPos += 1` - see ReadLengthPrefixedPayload's own
        //  comment (netcommand_readlengthprefixed.cpp): this is
        //  `m_field10 + 1`, not the current `m_readPos`.
        self->m_readPos = self->m_field10 + 1;
    }

    if ((unsigned int)self->m_readPos > (unsigned int)self->m_field10)
        *(unsigned short*)dst = 0;

    return self;
}

//============================================================================
//  0x6F651AB0 - CNetData_ReadSenderKeyAndLength's own helper
//  (netcommand_dispatch.cpp): read a 2-byte length prefix through
//  CDataStore::ReadWord, and - only when it is within cap (0x400 here) -
//  copy that many raw bytes into `dst+2` through CDataStore::ReadRawAlias
//  and store the length itself at `dst[0..1]`; over cap, just nudge
//  `m_readPos` by one instead (the shipped body's own `[esi+10h]+1 ->
//  [esi+14h]` store, reproduced verbatim rather than guessed at).  Either
//  way, the shared EnsureReadable overflow guard at the end zeroes the
//  stored length if the stream ran out.  `retn` bare -
//  __fastcall(self, dst).
//
//  Own translation unit rather than a `static` helper alongside its one
//  caller: with a single call site in the same TU, /Ob2 folded this back
//  into CNetData_ReadSenderKeyAndLength's own body every time, which
//  reproduces the bytes read but not the shape the dump actually has (one
//  shared out-of-line function reached through a real `call`).  Its own
//  twin, ReadReplayPayloadHeader (0x6F5545F0, cap 0x3FD - the only
//  difference from this body), gets the same treatment in its own file
//  for the identical reason, even though it has two call sites.
//============================================================================
#include "cdatastore.h"

CDataStore* __fastcall ReadLengthPrefixedPayload(CDataStore* self, unsigned char* dst)
{
    unsigned short len = 0;
    self->ReadWord(&len);

    if (len <= 0x400)
    {
        self->ReadRawAlias(dst + 2, len);
        *(unsigned short*)dst = len;
    }
    else
    {
        //  Not `m_readPos += 1` - the shipped body forms this from
        //  `m_field10 + 1`, not from the current `m_readPos` (the same
        //  "declared length overrun" value EnsureReadable's own failure
        //  path stores).
        self->m_readPos = self->m_field10 + 1;
    }

    if ((unsigned int)self->m_readPos > (unsigned int)self->m_field10)
        *(unsigned short*)dst = 0;

    return self;
}

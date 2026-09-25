//============================================================================
//  SSyncValueTail - the tail every CNetCommandSyncStoreInteger/Real/Boolean
//  builder embeds after its three wire-key string buffers
//  (netcommand_dispatch.cpp): two raw TSLink<T> nodes (storm.h's own
//  template - the terminator-complement convention there is the exact
//  shape this destructor walks) plus one Storm-tagged owned pointer,
//  released through Storm_403 (SMemFree) if non-null.
//
//  0x6F3EBE80/0x6F3EBF10/0x6F3EBFA0 (Integer/Real/Boolean's own copies) are
//  three of eight call sites this same body has (`ida_query xrefs` on
//  0x6F3EBE80 alone finds it called from six more classes elsewhere in the
//  image, all outside this batch's own worklist) - this image having been
//  linked without /OPT:ICF (docs/msvc-vc8-idioms.md), so one shared
//  definition plus a byte-identical duplicate per real address is the
//  correct shape, not a bug to dedupe.  `retn` bare on all three (confirmed
//  against each one's own dump body) - thiscall(this), no stack cleanup.
//
//  Field +0x00 (the discriminator ahead of the two links) is never read or
//  written here - CLAUDE.md's own "only touched members get names" rule -
//  and its consumer (whatever fills it in, presumably one of the
//  sub_6F53C4xx "fire" helpers netcommand_dispatch.cpp already reconstructs
//  as an CNetData_FireCommand duplicate) is out of this batch's scope.
//  Bytes-for-address: the tag string is the full, non-truncated literal at
//  0x6F875C68 (`ida_query strings buildwar3x`), IDA's own display of it
//  truncated to "e:\\Drive1\\temp\\buildwar3x\\Storm\\H"...; line 0xA29,
//  flags 0 - all three exact operand values, confirmed identical across
//  all three dumps.
//============================================================================
#include "storm.h"

void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

struct SSyncValueTail
{
    int          m_discriminator;   // +0x00 - untouched here
    TSLink<void> m_link1;           // +0x04
    TSLink<void> m_link2;           // +0x0C
    void*        m_ownedPtr;        // +0x14
};

__forceinline static void SSyncValueTail_Release(SSyncValueTail* self)
{
    if (self->m_ownedPtr)
        SMemFree(self->m_ownedPtr, "e:\\Drive1\\temp\\buildwar3x\\Storm\\H\\stpl.h", 0xA29, 0);
    //  Shipped order: the second link (+0x0C) is unlinked before the first
    //  (+0x04) - matches all three dumps' own instruction order exactly.
    self->m_link2.Unlink();
    self->m_link1.Unlink();
}

//  0x6F3EBE80 - CNetCommandSyncStoreInteger's own copy.
void __fastcall SyncValueTail_Release_6F3EBE80(void* self)
{
    SSyncValueTail_Release((SSyncValueTail*)self);
}

//  0x6F3EBF10 - CNetCommandSyncStoreReal's own copy.
void __fastcall SyncValueTail_Release_6F3EBF10(void* self)
{
    SSyncValueTail_Release((SSyncValueTail*)self);
}

//  0x6F3EBFA0 - CNetCommandSyncStoreBoolean's own copy.
void __fastcall SyncValueTail_Release_6F3EBFA0(void* self)
{
    SSyncValueTail_Release((SSyncValueTail*)self);
}

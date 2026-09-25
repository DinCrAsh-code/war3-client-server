//============================================================================
//  0x6F685050 - WriteLenPrefixedFromPtr_0x25 (networking wave 7,
//  agent-networking-wave3-2026-08-29's claim).
//
//  The family's own per-member payload writer for tag 0x25 (see
//  netclient_notifyevent_0x25.cpp's own header for the full trace):
//  WriteWord(the u16 length already sitting at the front of the buffer
//  CopyLengthPrefixedBlock built) followed by TailCallWriteRaw(the `len`
//  bytes right after it).
//
//  Its own translation unit, not `static` inside the caller's file: the
//  shipped body is a genuine out-of-line call (`call sub_6F685050`, not
//  inlined) from 0x6F677C70, and this repo's own /O2 will happily inline
//  a `static` single-call-site function of this size back into its only
//  caller - which cost the first pass its own call instruction and both
//  `mov [esp+N], eax` argument spills the shipped code keeps.  A separate
//  TU is what CLAUDE.md's own "one translation unit per original module"
//  rule is for: no LTCG in this toolchain, so a cross-TU call can never be
//  inlined away regardless of size.
//============================================================================
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteLenPrefixedFromPtr_0x25(CDataStoreScratch* writer, void* passthrough)
{
    //  Read the u16 length twice rather than caching it in a local - the
    //  shipped body re-reads it from `passthrough` for the second call
    //  rather than keeping it live across WriteWord, and matching that
    //  spelling (not a cached variable) is what gets the register
    //  allocation to agree.
    writer->WriteWord(*(unsigned short*)passthrough);
    writer->TailCallWriteRaw((char*)passthrough + 2, *(unsigned short*)passthrough);
    return writer;
}

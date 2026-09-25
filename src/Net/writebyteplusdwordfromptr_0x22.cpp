//============================================================================
//  0x6F684360 - WriteBytePlusDwordFromPtr_0x22 (networking wave 7,
//  agent-networking-wave3-2026-08-29's claim).
//
//  The family's own per-member payload writer for tag 0x22 (see
//  netclient_notifyevent_0x22.cpp's own header for the full trace):
//  WriteByte(the packed byte) then WriteDword(the packed dword) - the
//  family's only two-field payload.
//
//  Its own translation unit, not `static` inside the caller's file, for
//  the same reason as writelenprefixedfromptr_0x25.cpp's own header
//  explains: the shipped body is a genuine out-of-line call from
//  0x6F677F70, and a `static` copy here gets inlined back into its only
//  call site by this repo's own /O2.
//============================================================================
#include "cdatastorescratch.h"

//  The {byte, dword} pair the slot packs - a 5-byte, unaligned struct (no
//  padding the shipped code respects between the two fields). Redeclared
//  here rather than shared through a header, matching this repo's own
//  rule against inventing a shared type from one call site's own view.
#pragma pack(push, 1)
struct BytePlusDword
{
    unsigned char byteValue;
    unsigned int  dwordValue;
};
#pragma pack(pop)

CDataStoreScratch* __fastcall WriteBytePlusDwordFromPtr_0x22(CDataStoreScratch* writer, BytePlusDword* passthrough)
{
    writer->WriteByte(passthrough->byteValue);
    writer->WriteDword(passthrough->dwordValue);
    return writer;
}

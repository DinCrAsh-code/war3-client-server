//============================================================================
//  0x6F675920 - one of a family of ~23 near-identical action-payload
//  parsers (0x6F675030-0x6F675F50) Probably_W3GS_ActionHandler_
//  MaybeServerVersion's opcode handlers each call exactly one of. Every
//  member of the family does the same three things: build a local
//  `CDataStore` view over the caller's raw buffer with the forwarded
//  opcode-dword reused whole as the declared length, call one specific
//  "field reader" to fill the caller's own local record (a different reader
//  per family member - see w3gsaction2_readdword.cpp for this one's), then
//  report success only if the reader consumed *exactly* the declared
//  length (no leftover, no overread). This is the member `W3GSAction2_6F676DA0`
//  (opcode 0x13) uses, reading a single dword field.
//
//  Local `CDataStore` construction is field assignment, not a formal
//  constructor - same pattern ContainsW3GSBytePacketCheck's own local
//  CDataStore uses (w3gs_packetcheck.cpp): m_field8=0 (no backing capacity
//  of its own) and m_field0xC=-1 (does not own pool-recycled storage) mark
//  this as a plain, non-owning view over the caller-supplied buffer.
//============================================================================
#include "cdatastore.h"

CDataStore* __fastcall ReadSingleDword(CDataStore* self, unsigned int* out);

int __fastcall ParseSingleDwordAction(void* src, unsigned int declaredLength, unsigned int* out)
{
    CDataStore local;
    local.m_field4 = src;
    local.m_field8 = 0;
    local.m_field0xC = -1;
    local.m_field10 = declaredLength;
    local.m_readPos = 0;

    ReadSingleDword(&local, out);
    return local.m_readPos == local.m_field10;
}

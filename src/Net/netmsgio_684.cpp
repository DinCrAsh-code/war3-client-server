//============================================================================
//  0x6F684xxx - single-field message serialisers.
//
//  One family of per-message serialisers from the W3GS / net-message layer:
//  each one takes the stream in ecx and a pointer to one small message
//  struct in edx, moves that struct's fields through CDataStore /
//  CDataStoreScratch one primitive at a time, and returns the stream so the
//  caller can chain.  All of them are __fastcall two-register signatures
//  (`retn`, no stack arguments) - the same shape ReadHeaderDword
//  (gameaction0x26_readheaderdword.cpp) and WriteHeaderFrom
//  (cdatastorescratch_writers.cpp) already establish for this neighbourhood.
//
//  Only the fields each function actually touches are asserted; the message
//  structs themselves are deliberately not modelled, so the parameter is the
//  primitive pointer the code really reads through.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

//  0x6F684040 - write the leading byte of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteByte_6F684040(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteByte(*src);
    return self;
}

//  0x6F684050 - read the leading byte of the message struct.
CDataStore* __fastcall NetMsgReadByte_6F684050(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    return self;
}

//  0x6F684120 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F684120(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F684130 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F684130(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F684140 - write the leading byte of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteByte_6F684140(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteByte(*src);
    return self;
}

//  0x6F684150 - read the leading byte of the message struct.
CDataStore* __fastcall NetMsgReadByte_6F684150(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    return self;
}

//  0x6F684160 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F684160(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F684170 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F684170(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F684330 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F684330(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F684350 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F684350(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F6843B0 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F6843B0(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F684420 - write the leading byte of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteByte_6F684420(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteByte(*src);
    return self;
}

//  0x6F684430 - read the leading byte of the message struct.
CDataStore* __fastcall NetMsgReadByte_6F684430(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    return self;
}

//  0x6F6844A0 - write the leading word of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteWord_6F6844A0(CDataStoreScratch* self, const unsigned short* src)
{
    self->WriteWord(*src);
    return self;
}

//  0x6F6844B0 - read the leading word of the message struct.
CDataStore* __fastcall NetMsgReadWord_6F6844B0(CDataStore* self, unsigned short* dst)
{
    self->ReadWord(dst);
    return self;
}

//  0x6F6844C0 - write the leading byte of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteByte_6F6844C0(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteByte(*src);
    return self;
}

//  0x6F6847F0 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F6847F0(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F684870 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F684870(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F684910 - write the leading byte of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteByte_6F684910(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteByte(*src);
    return self;
}

//  0x6F684920 - read the leading byte of the message struct.
CDataStore* __fastcall NetMsgReadByte_6F684920(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    return self;
}

//  0x6F684930 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F684930(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F684940 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F684940(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F684830 - three consecutive dwords.
CDataStore* __fastcall NetMsgReadDwordDwordDword_6F684830(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    self->ReadDword(dst + 1);
    self->ReadDword(dst + 2);
    return self;
}


//----------------------------------------------------------------------------
//  0x6F6841E0 - dword then the byte immediately after it (offset 4).
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall NetMsgWriteDwordByte_6F6841E0(CDataStoreScratch* self,
                                                            const unsigned char* src)
{
    self->WriteDword(*(const unsigned int*)src);
    self->WriteByte(src[4]);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F684200 - the read-side mirror of 0x6F6841E0.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadDwordByte_6F684200(CDataStore* self, unsigned char* dst)
{
    self->ReadDword((unsigned int*)dst);
    self->ReadByte(dst + 4);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F684280 - two adjacent dwords.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadDwordPair_6F684280(CDataStore* self, unsigned char* dst)
{
    self->ReadDword((unsigned int*)dst);
    self->ReadDword((unsigned int*)(dst + 4));
    return self;
}

//----------------------------------------------------------------------------
//  0x6F684380 - byte then unaligned dword, the read-side shape 0x6F683F40 and
//  0x6F683FE0 also carry at their own addresses (no /OPT:ICF - see
//  agentdefaults.cpp).
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadByteDword_6F684380(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    self->ReadDword((unsigned int*)(dst + 1));
    return self;
}

//----------------------------------------------------------------------------
//  0x6F684010 - a bare wrapper that forwards `this` and edx untouched to
//  NetMsgReadByteDword_6F683FE0 (netmsgio_683.cpp - a different translation
//  unit on purpose, so the callee stays a real call) and returns the stream
//  itself rather than the callee's own return value.  Same shape as
//  WriteHeaderFromWrapper (0x6F684000, EXACT).
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadByteDword_6F683FE0(CDataStore* self, unsigned char* dst);

CDataStore* __fastcall NetMsgReadByteDwordWrapper_6F684010(CDataStore* self, unsigned char* dst)
{
    NetMsgReadByteDword_6F683FE0(self, dst);
    return self;
}

//----------------------------------------------------------------------------
//  networking wave (batch-B-w3gsaction_thunks) - five more members of this
//  same "stream in ecx, dest struct in edx" family, reached directly from the
//  W3GSAction_* handlers (src/Net/w3gsaction_*.cpp) rather than through any
//  ParseXAction wrapper - those handlers build their own local, non-owning
//  CDataStore inline and call straight into one of these.
//----------------------------------------------------------------------------

//  0x6F6508C0 - NetMsgReadRaw16_6F6508C0 (netmsgio_6508c0.cpp) - a different
//  translation unit on purpose, so the call below stays a real call the way
//  the shipped body's own does (matches NetMsgReadByteDwordWrapper_6F684010's
//  own same-file precedent).
CDataStore* __fastcall NetMsgReadRaw16_6F6508C0(CDataStore* self, void* dst);

//  0x6F6843F0 - a 0x10-byte raw run, then dword at +0x10, then unaligned
//  dword at +0x14.
CDataStore* __fastcall NetMsgReadRawDwordDword_6F6843F0(CDataStore* self, unsigned char* dst)
{
    NetMsgReadRaw16_6F6508C0(self, dst);
    self->ReadDword((unsigned int*)(dst + 0x10));
    self->ReadDword((unsigned int*)(dst + 0x14));
    return self;
}

//  0x6F684510 - dword, then byte immediately after it (+4), then unaligned
//  dword at +5.
CDataStore* __fastcall NetMsgReadDwordByteDword_6F684510(CDataStore* self, unsigned char* dst)
{
    self->ReadDword((unsigned int*)dst);
    self->ReadByte(dst + 4);
    self->ReadDword((unsigned int*)(dst + 5));
    return self;
}

//  0x6F684A20 - two leading bytes, then an unaligned dword at +2, then a
//  fourth field (dword) at +6.
CDataStore* __fastcall NetMsgReadByteByteDwordDword_6F684A20(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    self->ReadByte(dst + 1);
    self->ReadDword((unsigned int*)(dst + 2));
    self->ReadDword((unsigned int*)(dst + 6));
    return self;
}

//  0x6F684A90 - two leading bytes, then an unaligned dword at +2.
CDataStore* __fastcall NetMsgReadByteByteDword_6F684A90(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    self->ReadByte(dst + 1);
    self->ReadDword((unsigned int*)(dst + 2));
    return self;
}

//  0x6F650770 - already reconstructed as NetMsgReadBlock20
//  (netmsgio_650_readblock.cpp); declared here only for callers in this file.
void __fastcall NetMsgReadBlock20(CDataStore* self, void* dest);

//----------------------------------------------------------------------------
//  0x6F684C30 - a length-prefixed name field, up to 16 bytes: read one
//  length byte; if it fits (<= 0x10), read that many raw bytes right after
//  the length byte and write the length back into byte 0; otherwise force
//  the stream's own read position past the end (the same "reject" outcome
//  EnsureReadable's own too-long branch produces). Either way, if the stream
//  ended up past its declared length, the name is forced back to an empty
//  string (byte 0 = 0) - the same "overran, so blank" idiom Reader_6F6851B0
//  below also applies to its own 0x104-byte string field.
//
//  Real `__thiscall` shape: `this` (ecx) is the destination buffer, not a
//  CDataStore - the *store* is the one explicit stack argument (`retn 4`).
//  Modelled as a member of a throwaway wrapper class over the raw buffer,
//  the same technique ActionQueueTable/FixedPatternKey already use for a
//  real `this` this session did not otherwise name.
//----------------------------------------------------------------------------
struct NameField16
{
    void ReadFrom(CDataStore* store);
};

void NameField16::ReadFrom(CDataStore* store)
{
    unsigned char* dest = (unsigned char*)this;
    unsigned char len = 0;
    store->ReadByte(&len);
    if (len <= 0x10)
    {
        store->ReadRaw(dest + 1, len);
        dest[0] = len;
    }
    else
    {
        store->m_readPos = store->m_field10 + 1;
    }
    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        dest[0] = 0;
}

//----------------------------------------------------------------------------
//  0x6F685230 - a NameField16 (see 0x6F684C30 just above) immediately
//  followed by one more raw byte at +0x11 (0x11 = 1 length byte + up to 16
//  raw bytes, the field's own full width). Returns `self` unconditionally,
//  the same stream-style `>>` return every other reader in this file uses,
//  regardless of what ReadByte itself returned.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadPrefixedField_6F685230(CDataStore* self, unsigned char* dst)
{
    ((NameField16*)dst)->ReadFrom(self);
    self->ReadByte(dst + 0x11);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F6851B0 - a bigger record: dword, a 0x104-byte string field
//  (ReadString already null-terminates on its own; this call site
//  additionally forces byte 0 to 0 when the stream overran), then three more
//  dwords and a trailing 0x14-byte raw block.
//----------------------------------------------------------------------------
CDataStore* __fastcall Reader_6F6851B0(CDataStore* self, unsigned char* dst)
{
    self->ReadDword((unsigned int*)dst);
    self->ReadString((char*)(dst + 4), 0x104);
    if ((unsigned int)self->m_readPos > (unsigned int)self->m_field10)
        dst[4] = 0;
    self->ReadDword((unsigned int*)(dst + 0x108));
    self->ReadDword((unsigned int*)(dst + 0x10C));
    self->ReadDword((unsigned int*)(dst + 0x110));
    NetMsgReadBlock20(self, dst + 0x114);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F6849A0 - two leading bytes, then three dwords (14 bytes total).
//  Reached from W3GSAction_6F670170 (w3gsaction_tier30.cpp), which reads
//  only the first byte (a target id for its own list search) and the
//  second byte (a class-id check against self+0xB4) and the first dword
//  (a lookup key into FindProgressRecordById) individually - the trailing
//  two dwords are forwarded as part of the raw input buffer rather than
//  read back out of this struct.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadByteByteDwordDwordDword_6F6849A0(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    self->ReadByte(dst + 1);
    self->ReadDword((unsigned int*)(dst + 2));
    self->ReadDword((unsigned int*)(dst + 6));
    self->ReadDword((unsigned int*)(dst + 10));
    return self;
}

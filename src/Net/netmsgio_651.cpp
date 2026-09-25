//============================================================================
//  0x6F651xxx - single-field message serialisers.
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

//  0x6F651350 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F651350(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F651370 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F651370(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F6513B0 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F6513B0(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F651450 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F651450(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F6515F0 - write the leading byte of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteByte_6F6515F0(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteByte(*src);
    return self;
}

//  0x6F651610 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F651610(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F651630 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F651630(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F651650 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F651650(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F6516B0 - write the leading word of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteWord_6F6516B0(CDataStoreScratch* self, const unsigned short* src)
{
    self->WriteWord(*src);
    return self;
}

//  0x6F6516C0 - read the leading word of the message struct.
CDataStore* __fastcall NetMsgReadWord_6F6516C0(CDataStore* self, unsigned short* dst)
{
    self->ReadWord(dst);
    return self;
}


//----------------------------------------------------------------------------
//  0x6F651290 - a byte at offset 0 then a dword at offset 4 (the struct pads
//  between them; the write side skips the padding).
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall NetMsgWriteByteDword_6F651290(CDataStoreScratch* self,
                                                            const unsigned char* src)
{
    self->WriteByte(src[0]);
    self->WriteDword(*(const unsigned int*)(src + 4));
    return self;
}

//----------------------------------------------------------------------------
//  0x6F651310 - two adjacent bytes.
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall NetMsgWriteBytePair_6F651310(CDataStoreScratch* self,
                                                           const unsigned char* src)
{
    self->WriteByte(src[0]);
    self->WriteByte(src[1]);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F651330 - the read-side mirror of 0x6F651310.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadBytePair_6F651330(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    self->ReadByte(dst + 1);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F6516F0 - two adjacent bytes again at its own address (no /OPT:ICF -
//  see agentdefaults.cpp).
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadBytePair_6F6516F0(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    self->ReadByte(dst + 1);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F651770 - two adjacent dwords.
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall NetMsgWriteDwordPair_6F651770(CDataStoreScratch* self,
                                                            const unsigned int* src)
{
    self->WriteDword(src[0]);
    self->WriteDword(src[1]);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F651790 - the read-side mirror of 0x6F651770.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadDwordPair_6F651790(CDataStore* self, unsigned char* dst)
{
    self->ReadDword((unsigned int*)dst);
    self->ReadDword((unsigned int*)(dst + 4));
    return self;
}

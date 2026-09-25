//============================================================================
//  0x6F683Dxx-0x6F683Fxx - single- and two-field message serialisers.
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

//  0x6F683D30 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F683D30(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F683DA0 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F683DA0(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F683DD0 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F683DD0(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F683DE0 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F683DE0(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F683DF0 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F683DF0(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F683E80 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F683E80(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F683E90 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F683E90(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}

//  0x6F683F60 - write the leading byte of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteByte_6F683F60(CDataStoreScratch* self, const unsigned char* src)
{
    self->WriteByte(*src);
    return self;
}

//  0x6F683F70 - read the leading byte of the message struct.
CDataStore* __fastcall NetMsgReadByte_6F683F70(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    return self;
}

//  0x6F683F90 - read the leading word of the message struct.
CDataStore* __fastcall NetMsgReadWord_6F683F90(CDataStore* self, unsigned short* dst)
{
    self->ReadWord(dst);
    return self;
}

//  0x6F683FB0 - read the leading word of the message struct.
CDataStore* __fastcall NetMsgReadWord_6F683FB0(CDataStore* self, unsigned short* dst)
{
    self->ReadWord(dst);
    return self;
}

//----------------------------------------------------------------------------
//  0x6F683F20 - byte then dword, the same two-field body WriteHeaderFrom
//  (0x6F683FC0, cdatastorescratch_writers.cpp) already carries at its own
//  address: this image was linked without /OPT:ICF (agentdefaults.cpp), so
//  identical bodies at different addresses stay separate functions.
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall NetMsgWriteByteDword_6F683F20(CDataStoreScratch* self,
                                                            const unsigned char* src)
{
    self->WriteByte(src[0]);
    self->WriteDword(*(const unsigned int*)(src + 1));
    return self;
}

//----------------------------------------------------------------------------
//  0x6F683F40 - the read-side mirror: one byte then one unaligned dword
//  straight after it.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadByteDword_6F683F40(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    self->ReadDword((unsigned int*)(dst + 1));
    return self;
}

//----------------------------------------------------------------------------
//  0x6F683FE0 - byte-then-dword again at its own address (see 0x6F683F20's
//  note on /OPT:ICF).  Called through the 0x6F684010 wrapper as well as
//  directly.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadByteDword_6F683FE0(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst);
    self->ReadDword((unsigned int*)(dst + 1));
    return self;
}

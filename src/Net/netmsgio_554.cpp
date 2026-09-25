//============================================================================
//  0x6F554xxx - two empty message serialisers.
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

//  0x6F5542D0 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F5542D0(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F554570 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F554570(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}


//----------------------------------------------------------------------------
//  0x6F554BF0 - read one byte at struct offset 0x18 and one dword at 0x1C.
//  The first 0x18 bytes are not touched by this reader at all.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadByteDwordAt18_6F554BF0(CDataStore* self, unsigned char* dst)
{
    self->ReadByte(dst + 0x18);
    self->ReadDword((unsigned int*)(dst + 0x1C));
    return self;
}

//============================================================================
//  0x6F537xxx - two single-field message serialisers.
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

//  0x6F537820 - write the leading dword of the message struct.
CDataStoreScratch* __fastcall NetMsgWriteDword_6F537820(CDataStoreScratch* self, const unsigned int* src)
{
    self->WriteDword(*src);
    return self;
}

//  0x6F537830 - read the leading dword of the message struct.
CDataStore* __fastcall NetMsgReadDword_6F537830(CDataStore* self, unsigned int* dst)
{
    self->ReadDword(dst);
    return self;
}


//============================================================================
//  0x6F650xxx - four empty message serialisers.
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

//  0x6F650780 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F650780(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F6507A0 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F6507A0(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F6507C0 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F6507C0(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F650800 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F650800(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}


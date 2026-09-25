//============================================================================
//  0x6F683Bxx-0x6F683C80 - fifteen empty message serialisers.
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

//  0x6F683B10 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B10(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683B30 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B30(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683B40 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B40(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683B50 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B50(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683B60 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B60(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683B70 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B70(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683B80 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B80(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683B90 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683B90(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683BA0 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683BA0(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683BB0 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683BB0(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683C00 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C00(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683C20 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C20(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683C40 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C40(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683C50 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C50(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}

//  0x6F683C80 - the message struct has no serialised fields; the whole body
//  is the fluent `return this`, the same as NoOpReadHeader (0x6F683B00).
CDataStoreScratch* __fastcall NetMsgIoNoFields_6F683C80(CDataStoreScratch* self, const void* /*src*/)
{
    return self;
}


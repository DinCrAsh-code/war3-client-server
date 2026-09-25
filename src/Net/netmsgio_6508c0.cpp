//============================================================================
//  0x6F6508C0 - ReadRawAlias(dst, 0x10), re-returning the stream (matches
//  ReadRawAlias's own stream-chain convention). Its own callers
//  (NetMsgReadRawDwordDword_6F6843F0, netmsgio_684.cpp) reach it through a
//  real call, so it lives in its own translation unit rather than getting
//  inlined - the same reasoning netmsgio_684.cpp's own
//  NetMsgReadByteDwordWrapper_6F684010 documents for its own cross-TU call.
//============================================================================
#include "cdatastore.h"

CDataStore* __fastcall NetMsgReadRaw16_6F6508C0(CDataStore* self, void* dst)
{
    self->ReadRawAlias(dst, 0x10);
    return self;
}

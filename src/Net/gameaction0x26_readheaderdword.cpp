//============================================================================
//  0x6F683DB0 - the same one-dword-reader shape as ReadSingleDword
//  (w3gsaction2_readdword.cpp, the sibling W3GS dispatcher's own copy of
//  this identical pattern) - `self->ReadDword(out); return self;` - reached
//  from GameAction_0x26_PacketHandler to pull the declared checksum off the
//  front of an incoming action block. Its own TU: same shape, different
//  module (address neighbourhood 0x6F683xxx here vs 0x6F684xxx there).
//============================================================================
#include "cdatastore.h"

CDataStore* __fastcall ReadHeaderDword(CDataStore* self, unsigned int* out)
{
    self->ReadDword(out);
    return self;
}

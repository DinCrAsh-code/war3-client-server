//============================================================================
//  0x6F684030 - one of a family of ~23 near-identical "field reader"
//  functions (0x6F683Bxx-0x6F685xxx) each of Probably_W3GS_ActionHandler_
//  MaybeServerVersion's action parsers (sub_6F675xxx, see
//  w3gsaction2_parsesingle.cpp) calls exactly one of, to fill its own
//  action-specific local record. This is the plainest one: read a single
//  dword and nothing else - the shape `W3GSAction2_6F676DA0` (opcode 0x13) uses.
//  The other ~22 siblings (multi-field records, length-prefixed strings,
//  fixed-size raw copies) are not reconstructed this session.
//============================================================================
#include "cdatastore.h"

CDataStore* __fastcall ReadSingleDword(CDataStore* self, unsigned int* out)
{
    self->ReadDword(out);
    return self;
}

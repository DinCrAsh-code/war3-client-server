//============================================================================
//  0x6F6EED50 - read one CFloat back out of a CDataStore, by address.
//
//  WriteCFloat's exact counterpart (cdatastore_writecfloat.cpp), sixteen
//  bytes below it in the same software-float module and with the same
//  `const CFloat*`-in-edx shape.  It reads into a local first and copies
//  that into the caller's CFloat rather than handing the reader the
//  caller's address directly - which is what the shipped `push ecx` frame
//  slot is for.
//
//  Own translation unit: the call into ReadDwordAlt is real, and its four
//  callers reach this with real calls too.
//============================================================================
#include "cdatastore.h"
#include "CFloat.h"

CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out)
{
    unsigned int bits;
    store->ReadDwordAlt(&bits);
    out->m_bits = (int)bits;
    return store;
}

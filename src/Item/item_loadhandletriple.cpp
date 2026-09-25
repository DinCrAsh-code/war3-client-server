//============================================================================
//  0x6F02F110 - ReadHandleTriple: read three dwords out of the save stream
//  and store them together.
//
//  All three destination words are staged in locals first - the first two
//  pre-set to -1 and the third to 0, the same "not set" sentinel every
//  {handle, typeTag} pair in this codebase uses - and only copied into the
//  caller's record once all three reads are done.  That is what makes a
//  short stream leave the record wholly at its sentinel rather than
//  half-written.
//
//  Three *different* four-byte readers again (0x6F4C2D30 twice and
//  0x6F4C2D00 once); see cdatastore.h.
//
//  Own translation unit: three real calls out of it.
//============================================================================
#include "cdatastore.h"
#include "handletriple.h"

CDataStore* __fastcall ReadHandleTriple(CDataStore* store, SHandleTriple* out)
{
    //  typeTag pre-set before handle: the shipped code stores the shared
    //  -1 into the higher slot first.
    unsigned int handle;
    unsigned int typeTag;
    unsigned int extra = 0;
    typeTag = 0xFFFFFFFF;
    handle = 0xFFFFFFFF;

    store->ReadDword(&handle);
    store->ReadDword(&typeTag);
    store->ReadDwordAlt3(&extra);

    out->m_handle = handle;
    out->m_typeTag = typeTag;
    out->m_extra = extra;
    return store;
}

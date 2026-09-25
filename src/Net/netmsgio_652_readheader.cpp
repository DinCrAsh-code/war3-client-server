//============================================================================
//  0x6F6521B0 - read one saved-game header block: three NUL-terminated
//  strings of 0x20, 0x10 and 0x80 bytes at +0x00, +0x20 and +0x30, then two
//  dwords at +0xB0 and +0xB4.
//
//  Each string is followed by the same overrun test the stream has - a read
//  that ran past the declared end leaves a NUL in the first byte rather than
//  whatever the buffer happened to hold.
//============================================================================
#include "cdatastore.h"
#include "cgamedescstore.h"    // SSaveGameHeaderBlock

CDataStore* __fastcall DataStoreReadHeaderBlock(CDataStore* store,
                                                SSaveGameHeaderBlock* out)
{
    store->ReadString(out->m_name.m_text, 0x20);
    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out->m_name.m_text[0] = 0;

    store->ReadString(out->m_short.m_text, 0x10);
    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out->m_short.m_text[0] = 0;

    store->ReadString(out->m_long, 0x80);
    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out->m_long[0] = 0;

    store->ReadDword(&out->m_wordB0);
    store->ReadDword(&out->m_wordB4);

    return store;
}

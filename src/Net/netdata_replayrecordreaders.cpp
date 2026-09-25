//============================================================================
//  Two of the three record shapes CNetData::LoadReplayHeader's own record
//  loop reads (netdata_loadreplayheader.cpp), each its own small reader over
//  a CDataStore - same address neighbourhood, same shape as
//  DataStoreReadHeaderBlock (netmsgio_652_readheader.cpp), which both of
//  these call for their own header-block field.
//
//  sub_6F6522A0 was carried as a naked thunk under the placeholder name
//  "ReadWordDirect" (guessed - two bytes of alignment room after it, so it
//  looked like a plain `unsigned short` field) because the walk that
//  reached it had stopped one level short and it had no dump body of its
//  own. batch-I-playertable-and-bnet pulled its dump via
//  `ida_query calltree_asm`: it is NOT a two-byte word read at all - it is
//  CDataStore::ReadString into a fixed 16-byte buffer (the same
//  zero-on-overrun guard ReadClampedField_6F651C60/ReadClampedField60h_
//  6F651CF0 already establish for this file's sibling record family),
//  immediately followed by a second, clamped (max 8) length-prefixed field
//  at the string's own +0x10 - genuinely up to 25 bytes written, not 2.
//  Renamed to ReadNameAndClampedField8_6F6522A0 and its own `out` parameter
//  widened from `unsigned short*` to `void*` to say so; both records'
//  reserved padding after this field already has the room (0x652560's
//  record leaves 0x1B bytes before the header block at +0x20, 0x652780's
//  leaves 0x1B before +0x1C), so nothing else moves.
//============================================================================
#include "netdata_replayrecords.h"

//  0x6F6521B0 - Net/netmsgio_652_readheader.cpp.
CDataStore* __fastcall DataStoreReadHeaderBlock(CDataStore* store,
                                                SSaveGameHeaderBlock* out);

//  0x6F651A30 - the clamp-8 twin of ReadClampedField60h_6F651CF0/
//  ReadClampedField_6F651C60 (netevent_replaystream_records.cpp): a byte
//  length probe via CDataStore::ReadByte, and - only if it comes back
//  `<= 8` - that many raw bytes via CDataStore::ReadRawAlias into `dest+1`
//  with the length itself stored at `dest[0]`; over 8 (the sentinel
//  surviving unread, i.e. EOF) instead bumps the store's own read-position
//  counter.  Either way, a store left past its own declared length zeroes
//  `dest[0]` again.  `this` = store, `edx` = dest, retn bare.
CDataStore* __fastcall ReadClampedField8_6F651A30(CDataStore* store, void* dest)
{
    unsigned char* out = (unsigned char*)dest;
    unsigned char len = 0;
    store->ReadByte(&len);

    if (len <= 8)
    {
        store->ReadRawAlias(out + 1, len);
        out[0] = len;
    }
    else
    {
        store->m_readPos = store->m_field10 + 1;
    }

    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out[0] = 0;

    return store;
}

//  0x6F6522A0 - see the file header above: a 16-byte CDataStore::ReadString
//  read (with the shared zero-on-overrun guard applied to just its own
//  first byte, matching the shipped body's own single `mov byte ptr
//  [edi], 0`) followed immediately by ReadClampedField8_6F651A30 at the
//  string's own +0x10.  `this` = store, `edx` = dest, retn bare.
CDataStore* __fastcall ReadNameAndClampedField8_6F6522A0(CDataStore* store, void* dest)
{
    char* out = (char*)dest;
    store->ReadString(out, 0x10);

    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out[0] = 0;

    ReadClampedField8_6F651A30(store, out + 0x10);
    return store;
}

//----------------------------------------------------------------------------
//  0x6F652560 - the record type-0x10 shape: a dword, a byte, a 16-byte name
//  plus clamped field (ReadNameAndClampedField8_6F6522A0 above - NOT a
//  plain word, see this file's own header note), a full save-game header
//  block, then a trailing dword.  0xDC bytes in total; the caller only
//  ever uses the header block and the leading dword/byte/name slice that
//  sits in front of it (netdata_loadreplayheader.cpp).
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadReplayType0x10Record(CDataStore* store,
                                                SReplayType0x10Record* out)
{
    store->ReadDword(&out->m_field0);
    store->ReadByte((unsigned char*)&out->m_field4);
    ReadNameAndClampedField8_6F6522A0(store, &out->m_field5);
    DataStoreReadHeaderBlock(store, &out->m_header);
    store->ReadDword(&out->m_fieldD8);
    return store;
}

//----------------------------------------------------------------------------
//  0x6F652780 - the record type-0x16 shape: a byte, a 16-byte name plus
//  clamped field (again ReadNameAndClampedField8_6F6522A0, not a plain
//  word), then a dword at +0x1C - the bytes in between are never named
//  because nothing here reads a field of them, only that the store's own
//  read position has to land past them.
//----------------------------------------------------------------------------
CDataStore* __fastcall ReadReplayType0x16Record(CDataStore* store,
                                                SReplayType0x16Record* out)
{
    store->ReadByte(&out->m_field0);
    ReadNameAndClampedField8_6F6522A0(store, &out->m_field1);
    store->ReadDword(&out->m_field1C);
    return store;
}

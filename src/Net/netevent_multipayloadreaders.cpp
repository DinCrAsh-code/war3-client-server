//============================================================================
//  Six of the twelve per-event deserializers netevent_dispatch.cpp's own
//  NET_EVENT_CTOR_MULTI table2 events call - each event that carries more
//  than the uniform group's single dword (a save-game header block, a
//  string, a nested dword-block read) gets its own address here.
//
//  Deliberately its own translation unit, NOT inlined into
//  netevent_dispatch.cpp alongside their NET_EVENT_CTOR_MULTI callers: see
//  the comment left in that file at the forward-declaration site for why -
//  in short, same-TU /Ob2 inlining silently rewrote the calling
//  constructor's own instruction stream and cost that function's score,
//  not this one's.  These addresses (0x6F6525C0-0x6F652966) also sit well
//  outside the dispatcher's own 0x6F551D80 neighbourhood, so the split is
//  a real module boundary anyway, not a workaround.
//
//  The other six deserializers in this same address range each reach a
//  callee with no dump yet (sub_6F651250, sub_6F6511E0/sub_6F653760,
//  sub_6F5382E0/sub_6F6512B0, sub_6F6537F0, sub_6F653860) and stay
//  NET_EVENT_DESERIALIZER_THUNK naked thunks in netevent_dispatch.cpp.
//============================================================================
#include "cdatastore.h"
#include "netdata_replayrecords.h"   // SSaveGameHeaderBlock (via cgamedescstore.h)

//  0x6F6521B0 - Net/netmsgio_652_readheader.cpp.
CDataStore* __fastcall DataStoreReadHeaderBlock(CDataStore* store, SSaveGameHeaderBlock* out);
//  0x6F6522A0 - Net/netdata_replayrecordreaders.cpp.
CDataStore* __fastcall ReadNameAndClampedField8_6F6522A0(CDataStore* store, void* dest);
//  0x6F650770 - Net/netmsgio_650_readblock.cpp.
void __fastcall NetMsgReadBlock20(CDataStore* self, void* dest);

//  0x6F6525C0 - a single dword (a lobby/game "type" or "flags" field) then
//  a save-game header block at +0x04.
void* __fastcall ReadGameAdPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    DataStoreReadHeaderBlock(store, (SSaveGameHeaderBlock*)(out + 4));
    return store;
}

//  0x6F652690 - two dwords, then a header block at +0x08.
void* __fastcall ReadGameFindPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    store->ReadDword((unsigned int*)(out + 4));
    DataStoreReadHeaderBlock(store, (SSaveGameHeaderBlock*)(out + 8));
    return store;
}

//  0x6F652710 - a dword, a byte, the name+clamped-8 field at +0x05, a
//  header block at +0x20, then one more dword at +0xD8.
void* __fastcall ReadGameJoinPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    store->ReadByte((unsigned char*)(out + 4));
    ReadNameAndClampedField8_6F6522A0(store, out + 5);
    DataStoreReadHeaderBlock(store, (SSaveGameHeaderBlock*)(out + 0x20));
    store->ReadDword((unsigned int*)(out + 0xD8));
    return store;
}

//  0x6F652920 - a dword, a dword, a 0x104-byte string at +0x08 (with the
//  same zero-on-overrun guard DataStoreReadHeaderBlock's own strings get),
//  then a dword at +0x10C.
void* __fastcall ReadDistFileStartPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    store->ReadDword((unsigned int*)(out + 4));
    store->ReadString(out + 8, 0x104);
    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out[8] = 0;
    store->ReadDword((unsigned int*)(out + 0x10C));
    return store;
}

//  0x6F6529F0 - a dword, a dword, two 0x104-byte strings at +0x08 and
//  +0x10C (each with its own zero-on-overrun guard), a dword at +0x210,
//  then a dword-block read at +0x214.
void* __fastcall ReadDistFileCompletePayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    store->ReadDword((unsigned int*)(out + 4));
    store->ReadString(out + 8, 0x104);
    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out[8] = 0;
    store->ReadString(out + 0x10C, 0x104);
    if ((unsigned int)store->m_readPos > (unsigned int)store->m_field10)
        out[0x10C] = 0;
    store->ReadDword((unsigned int*)(out + 0x210));
    NetMsgReadBlock20(store, out + 0x214);
    return store;
}

//  0x6F652450 - four dwords, then a header block at +0x10.
void* __fastcall ReadGameListAddPayload(void* storeArg, void* dst)
{
    CDataStore* store = (CDataStore*)storeArg;
    char* out = (char*)dst;
    store->ReadDword((unsigned int*)out);
    store->ReadDword((unsigned int*)(out + 4));
    store->ReadDword((unsigned int*)(out + 8));
    store->ReadDword((unsigned int*)(out + 0xC));
    DataStoreReadHeaderBlock(store, (SSaveGameHeaderBlock*)(out + 0x10));
    return store;
}

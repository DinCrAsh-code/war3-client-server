//============================================================================
//  0x6F653D70, 0x6F653D90, 0x6F653E20 and 0x6F653E40 - four one-field
//  extensions of the same message, two on the write side and two on the
//  read side, each chaining into the shorter form before adding its own
//  byte at +7 or +8.
//
//  Four separate functions rather than one with a count: that is what the
//  image has, and each is a real call from the next.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

struct SNetMsg653
{
    char m_reserved0[7];
    unsigned char m_byte7;      // +0x07
    unsigned char m_byte8;      // +0x08
};

//  0x6F653C40 / 0x6F653CA0 - the six-byte forms this pair extends.
CDataStoreScratch* __fastcall NetMsgWrite653Base(CDataStoreScratch* store,
                                                 SNetMsg653* msg);
CDataStore*        __fastcall NetMsgRead653Base(CDataStore* store,
                                                SNetMsg653* msg);

CDataStoreScratch* __fastcall NetMsgWrite653Byte7(CDataStoreScratch* store,
                                                  SNetMsg653* msg)
{
    NetMsgWrite653Base(store, msg);
    store->WriteByte(msg->m_byte7);
    return store;
}

CDataStore* __fastcall NetMsgRead653Byte7(CDataStore* store, SNetMsg653* msg)
{
    NetMsgRead653Base(store, msg);
    store->ReadByte(&msg->m_byte7);
    return store;
}

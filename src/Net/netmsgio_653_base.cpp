//============================================================================
//  0x6F653C40 / 0x6F653CA0 - the base of the four-deep 653 message pair
//  (netmsgio_653_writepair.cpp and _writepair8.cpp are the two extensions
//  above it).  Seven individual bytes at +0x00 .. +0x06, written and read
//  one at a time rather than as a block: that is what the image has, and it
//  is what makes the message's own element seven bytes wide before the
//  0002 and 0003 layouts add their +0x07 and +0x08.
//
//  Its own translation unit for the same reason the two extensions have
//  theirs - in one TU /Ob2 folds the shorter form into the longer one and
//  the call the image really makes disappears.
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"

struct SNetMsg653
{
    char m_reserved0[7];
    unsigned char m_byte7;      // +0x07
    unsigned char m_byte8;      // +0x08
};

CDataStoreScratch* __fastcall NetMsgWrite653Base(CDataStoreScratch* store,
                                                 SNetMsg653* msg)
{
    store->WriteByte(msg->m_reserved0[0]);
    store->WriteByte(msg->m_reserved0[1]);
    store->WriteByte(msg->m_reserved0[2]);
    store->WriteByte(msg->m_reserved0[3]);
    store->WriteByte(msg->m_reserved0[4]);
    store->WriteByte(msg->m_reserved0[5]);
    store->WriteByte(msg->m_reserved0[6]);
    return store;
}

CDataStore* __fastcall NetMsgRead653Base(CDataStore* store, SNetMsg653* msg)
{
    store->ReadByte((unsigned char*)&msg->m_reserved0[0]);
    store->ReadByte((unsigned char*)&msg->m_reserved0[1]);
    store->ReadByte((unsigned char*)&msg->m_reserved0[2]);
    store->ReadByte((unsigned char*)&msg->m_reserved0[3]);
    store->ReadByte((unsigned char*)&msg->m_reserved0[4]);
    store->ReadByte((unsigned char*)&msg->m_reserved0[5]);
    store->ReadByte((unsigned char*)&msg->m_reserved0[6]);
    return store;
}

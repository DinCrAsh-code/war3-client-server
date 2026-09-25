//============================================================================
//  0x6F655880 / 0x6F655910 / 0x6F6559A0 - the three save-game slot-table
//  readers, one per GAMESETUP_WAR3 layout (seven, eight and nine bytes an
//  element), and 0x6F653E60, the nine-byte layout's writer.
//
//  All four have the same shape: a one-byte element count, refused outright
//  at 0x10 or more, then that many elements through the 653 message pair,
//  then a dword and two bytes of trailing state.  The refusal is the
//  stream's own "this read ran off the end" marking - `m_readPos` is set one
//  past `m_field10`, which is what every later overrun test compares - and
//  not a separate error return: all four return the stream either way.
//
//  Which of the three a save carries is decided by CSaveGame::ReadHeader's
//  version gates; the two older ones are read into a local and widened
//  there (savegamereadheader.cpp).
//============================================================================
#include "cdatastore.h"
#include "cdatastorescratch.h"
#include "savegame.h"

struct SNetMsg653;

//  netmsgio_653_base.cpp / _writepair.cpp / _writepair8.cpp - the element
//  serialisers, one per element size.
CDataStore*        __fastcall NetMsgRead653Base(CDataStore*, SNetMsg653*);
CDataStore*        __fastcall NetMsgRead653Byte7(CDataStore*, SNetMsg653*);
CDataStore*        __fastcall NetMsgRead653Byte8(CDataStore*, SNetMsg653*);
CDataStoreScratch* __fastcall NetMsgWrite653Byte8(CDataStoreScratch*, SNetMsg653*);

//----------------------------------------------------------------------------
//  0x6F655880 - the seven-byte (GAMESETUP_WAR3_0001) layout.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadSlotArray0001(CDataStore* store,
                                               Net::GAMESETUP_SLOTS_0001* out)
{
    unsigned char count = 0;
    store->ReadByte(&count);

    //  The success arm is the fall-through, and the refusal is the block
    //  branched to past the tail - see docs/msvc-vc8-idioms.md, "Put the arm
    //  the shipped code falls through into first".  Written the other way
    //  round this is sixteen instructions in the wrong order.
    if (count < 0x10)
    {
        out->m_array.SetCount(count);
        for (unsigned char i = 0; i < count; i++)
            NetMsgRead653Base(store, (SNetMsg653*)&out->m_array.m_data[i]);

        store->ReadDword(&out->m_field0C);
        store->ReadByte(&out->m_field10);
        store->ReadByte(&out->m_field11);
        return store;
    }

    store->m_readPos = store->m_field10 + 1;
    return store;
}

//----------------------------------------------------------------------------
//  0x6F655910 - the eight-byte (GAMESETUP_WAR3_0002) layout.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadSlotArray0002(CDataStore* store,
                                               Net::GAMESETUP_SLOTS_0002* out)
{
    unsigned char count = 0;
    store->ReadByte(&count);

    //  The success arm is the fall-through, and the refusal is the block
    //  branched to past the tail - see docs/msvc-vc8-idioms.md, "Put the arm
    //  the shipped code falls through into first".  Written the other way
    //  round this is sixteen instructions in the wrong order.
    if (count < 0x10)
    {
        out->m_array.SetCount(count);
        for (unsigned char i = 0; i < count; i++)
            NetMsgRead653Byte7(store, (SNetMsg653*)&out->m_array.m_data[i]);

        store->ReadDword(&out->m_field0C);
        store->ReadByte(&out->m_field10);
        store->ReadByte(&out->m_field11);
        return store;
    }

    store->m_readPos = store->m_field10 + 1;
    return store;
}

//----------------------------------------------------------------------------
//  0x6F6559A0 - the nine-byte (GAMESETUP_WAR3_0003) layout, the one a save
//  game keeps.
//----------------------------------------------------------------------------
CDataStore* __fastcall NetMsgReadSlotArray0003(CDataStore* store,
                                               Net::GAMESETUP_SLOTS* out)
{
    unsigned char count = 0;
    store->ReadByte(&count);

    //  The success arm is the fall-through, and the refusal is the block
    //  branched to past the tail - see docs/msvc-vc8-idioms.md, "Put the arm
    //  the shipped code falls through into first".  Written the other way
    //  round this is sixteen instructions in the wrong order.
    if (count < 0x10)
    {
        out->m_array.SetCount(count);
        for (unsigned char i = 0; i < count; i++)
            NetMsgRead653Byte8(store, (SNetMsg653*)&out->m_array.m_data[i]);

        store->ReadDword(&out->m_field0C);
        store->ReadByte(&out->m_field10);
        store->ReadByte(&out->m_field11);
        return store;
    }

    store->m_readPos = store->m_field10 + 1;
    return store;
}

//----------------------------------------------------------------------------
//  0x6F653E60 - the nine-byte layout's writer.  The count goes out as one
//  byte and is what the loop counts down, so the array's own dword count is
//  truncated first rather than compared as a dword.
//----------------------------------------------------------------------------
CDataStoreScratch* __fastcall NetMsgWriteSlotArray0003(CDataStoreScratch* store,
                                                       Net::GAMESETUP_SLOTS* in)
{
    unsigned char count = (unsigned char)in->m_array.m_count;
    store->WriteByte(count);

    for (unsigned char i = 0; i < count; i++)
        NetMsgWrite653Byte8(store, (SNetMsg653*)&in->m_array.m_data[i]);

    store->WriteDword(in->m_field0C);
    store->WriteByte(in->m_field10);
    store->WriteByte(in->m_field11);
    return store;
}

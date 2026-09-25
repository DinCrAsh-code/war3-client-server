//============================================================================
//  0x6F655580 and 0x6F655C40 - the save game's slot table to and from its
//  wire form, plus 0x6F5BE710 / 0x6F5BE740, the two validators
//  CSaveGame::ReadHeader ends in, and 0x6F5BD1F0, the one-instruction alias
//  the restart path reaches the encoder through.
//
//  The encoder builds the record in a CDataStoreCache<128>, which is where
//  the 0x80 bound comes from: a table that will not fit in that buffer is
//  refused outright rather than the buffer being grown.  The decoder reads
//  it back through a bare CDataStore pointed straight at the record's own
//  bytes - m_field0xC is -1, which is this class's "the buffer is not mine
//  to recycle" marker, and is why the inlined ~CDataStore below does
//  nothing at run time.
//============================================================================
#include "savegame.h"
#include "cdatastore.h"
#include "cdatastorescratch.h"
#include "cdatastorecache128.h"

extern "C" void* __cdecl memcpy(void* dest, const void* src, unsigned int count);

extern "C" void(__stdcall* g_pfnRecycleBuffer)(void** pField4, int* pField8, int* pField0xC);
typedef void(__thiscall* RecycleThisFn)(void* self, void** pField4, int* pField8, int* pField0xC);

//  gamesetupslotio.cpp
CDataStore*        __fastcall NetMsgReadSlotArray0003(CDataStore*, Net::GAMESETUP_SLOTS*);
CDataStoreScratch* __fastcall NetMsgWriteSlotArray0003(CDataStoreScratch*, Net::GAMESETUP_SLOTS*);

//----------------------------------------------------------------------------
//  0x6F655580 - encode `in` into `out`.  The buffer and its length come back
//  through the cache's own vtable slot 9 (0x6F4C1BB0,
//  CDataStoreScratch::QueryBufferState) rather than a direct call: the
//  shipped code loads the slot out of the object.
//----------------------------------------------------------------------------
typedef void(__thiscall* QueryBufferStateFn)(void* self, void** outBuffer,
                                             unsigned int* outLength, int* outCapacity);

int __fastcall GameSetupSlotsEncode(Net::GAMESETUP_SLOTRECORD* out,
                                    Net::GAMESETUP_SLOTS* in)
{
    CDataStoreCache128 cache;

    NetMsgWriteSlotArray0003((CDataStoreScratch*)&cache, in);

    //  The success arm is the fall-through and the refusal is branched past
    //  it - written the other way round the two destructor calls swap over.
    if ((unsigned int)cache.m_field10 <= 0x80)
    {
        void* buffer;
        unsigned int length;
        ((QueryBufferStateFn)(*(void***)&cache)[0x24 / 4])(&cache, &buffer, &length, 0);

        out->m_length = (unsigned short)length;
        memcpy(out->m_data, buffer, (unsigned short)length);
        return 1;
    }

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F5BD1F0 - a bare `jmp` at its own address, which is how the restart
//  path (gamerestartrequest.cpp) reaches the encoder.
//----------------------------------------------------------------------------
int __fastcall SlotRecordEncode(Net::GAMESETUP_SLOTRECORD* out,
                                Net::GAMESETUP_SLOTS* in)
{
    return GameSetupSlotsEncode(out, in);
}

//----------------------------------------------------------------------------
//  0x6F655C40 - decode `in` back into `out`, and say whether the record was
//  consumed exactly.  A record that is short, long, or malformed leaves
//  m_readPos somewhere other than m_field10 and fails here.
//----------------------------------------------------------------------------
int __fastcall GameSetupSlotsDecode(Net::GAMESETUP_SLOTS* out,
                                    const Net::GAMESETUP_SLOTRECORD* in)
{
    CDataStore store;
    store.m_field4 = (void*)in->m_data;
    store.m_field8 = 0;
    store.m_field0xC = -1;
    store.m_field10 = in->m_length;
    store.m_readPos = 0;

    NetMsgReadSlotArray0003(&store, out);

    return store.m_readPos == store.m_field10;
}

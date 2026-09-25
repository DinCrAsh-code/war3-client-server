//============================================================================
//  0x6F443860 - CSaveGame::ReadHeader.  Open `path`, load the whole stream
//  out of it, and pull the save game's own fields off the front of it.
//
//  Everything past the probe is version-gated, and the five gates are the
//  shipped constants:
//
//      0x17AB  the newest format this build will open at all (the probe)
//      0x178B  ... and above carries a second counted string at +0x424
//      0x1770  ... and above carries the slot record in its final form; an
//              older save carries one of the two older slot-array layouts,
//              which is read into a local pair of arrays, widened into the
//              nine-byte one and re-encoded back into the record
//      0x11DC  which of those two older layouts (seven bytes below, eight
//              bytes at or above)
//      0x17AA  ... and above carries its own twenty-byte block; an older
//              one gets the five defaults at dword_6F950A6C
//
//  The save game *is* the data store: every read below takes `this` as a
//  CDataStore (savegame.h).
//
//  The two local slot tables are released by hand rather than by a
//  destructor, under the element type's own Storm tag - the same shape
//  savegamedtor.cpp already uses for the member one, and the reason
//  TSFixedArray has no destructor in this repo.
//============================================================================
#include "savegame.h"
#include "netgameload.h"
#include "cdatastore.h"
#include "cgamedescstore.h"
#include "storm.h"

//  0x6F309A00 / 0x6F6521B0 / 0x6F650770 - the three composite readers.
CDataStore* __fastcall DataStoreReadStringField(CDataStore* store, STStringField* field);
CDataStore* __fastcall DataStoreReadHeaderBlock(CDataStore* store, SSaveGameHeaderBlock* out);
void        __fastcall NetMsgReadBlock20(CDataStore* self, void* dest);

//  0x6F443720 / 0x6F5BE710 / 0x6F5BE740 / 0x6F5BD1F0 - Game/.
CDataStore* __fastcall SaveGameReadSlotRecord(CDataStore* store, Net::GAMESETUP_SLOTRECORD* out);
int __fastcall SaveGameHeaderUsable(Net::GAMEDESC* desc, const SSaveGameHeaderBlock* block);
int __fastcall SaveGameSlotsUsable(Net::GAMESETUP_SLOTS* slots, const Net::GAMESETUP_SLOTRECORD* record);
int __fastcall SlotRecordEncode(Net::GAMESETUP_SLOTRECORD* out, Net::GAMESETUP_SLOTS* in);

//  0x6F655880 / 0x6F655910 - Net/gamesetupslotio.cpp.
CDataStore* __fastcall NetMsgReadSlotArray0001(CDataStore*, Net::GAMESETUP_SLOTS_0001*);
CDataStore* __fastcall NetMsgReadSlotArray0002(CDataStore*, Net::GAMESETUP_SLOTS_0002*);

//  0x6F950A6C - the five dwords a save older than 0x17AA gets instead of a
//  block of its own.
extern "C" const unsigned int g_defaultSaveGameBlock[5];

int CSaveGame::ReadHeader(const char* path)
{
    CSaveGame* self = this;

    if (!SaveGameHeaderProbe(path, 0x17AB))
        return 0;

    if (!((SGameDataLoad*)self)->LoadFile(path))
        return 0;

    CDataStore* stream = (CDataStore*)self;
    unsigned int version = (unsigned int)stream->GetVersion();

    SSaveGameHeaderBlock block;
    Net::GAMESETUP_SLOTRECORD record;

    block.m_name.m_text[0] = 0;
    block.m_short.m_text[0] = 0;
    block.m_long[0] = 0;
    record.m_length = 0;

    DataStoreReadStringField(stream, &self->m_string418);

    if (version >= 0x178B)
        DataStoreReadStringField(stream, &self->m_string424);

    DataStoreReadHeaderBlock(stream, &block);
    SaveGameReadSlotRecord(stream, &record);

    if (version < 0x1770)
    {
        //  An older save carries the slot table in one of the two earlier
        //  layouts.  Read it out of the record, widen it into the nine-byte
        //  one and write the record back, so everything below this point
        //  sees the current form.
        CDataStore reader;
        reader.m_field4 = record.m_data;
        reader.m_field8 = 0;
        reader.m_field0xC = -1;
        reader.m_field10 = record.m_length;
        reader.m_readPos = 0;

        Net::GAMESETUP_SLOTS wide;
        wide.m_array.m_alloc = 0;
        wide.m_array.m_count = 0;
        wide.m_array.m_data = 0;

        if (version < 0x11DC)
        {
            Net::GAMESETUP_SLOTS_0001 old;
            old.m_array.m_alloc = 0;
            old.m_array.m_count = 0;
            old.m_array.m_data = 0;

            NetMsgReadSlotArray0001(&reader, &old);

            unsigned int count = old.m_array.m_count;
            wide.m_array.SetCount(count);

            for (unsigned int i = 0; i < count; i++)
            {
                //  A nine-byte copy out of a seven-byte element: the two
                //  bytes past the end are read and then overwritten by the
                //  two stores below, which is what the shipped loop does.
                wide.m_array.m_data[i] = *(const Net::GAMESETUP_SLOT*)&old.m_array.m_data[i];
                wide.m_array.m_data[i].m_reserved4[3] = 1;
                wide.m_array.m_data[i].m_byte8 = 'd';
            }

            wide.m_field0C = old.m_field0C;
            wide.m_field10 = old.m_field10;
            wide.m_field11 = old.m_field11;

            if (old.m_array.m_data != 0)
                SMemFree(old.m_array.m_data, ".?AUSLOT@GAMESETUP_WAR3_0001@Net@@", -2, 0);
        }
        else
        {
            Net::GAMESETUP_SLOTS_0002 old;
            old.m_array.m_alloc = 0;
            old.m_array.m_count = 0;
            old.m_array.m_data = 0;

            NetMsgReadSlotArray0002(&reader, &old);

            unsigned int count = old.m_array.m_count;
            wide.m_array.SetCount(count);

            for (unsigned int i = 0; i < count; i++)
            {
                wide.m_array.m_data[i] = *(const Net::GAMESETUP_SLOT*)&old.m_array.m_data[i];
                wide.m_array.m_data[i].m_byte8 = 'd';
            }

            wide.m_field0C = old.m_field0C;
            wide.m_field10 = old.m_field10;
            wide.m_field11 = old.m_field11;

            if (old.m_array.m_data != 0)
                SMemFree(old.m_array.m_data, ".?AUSLOT@GAMESETUP_WAR3_0002@Net@@", -2, 0);
        }

        SlotRecordEncode(&record, &wide);

        if (wide.m_array.m_data != 0)
            SMemFree(wide.m_array.m_data, ".?AUSLOT@GAMESETUP_WAR3_0003@Net@@", -2, 0);
    }

    stream->ReadDword((unsigned int*)&self->m_word430);
    stream->ReadDword((unsigned int*)&self->m_word434);

    if (version >= 0x17AA)
    {
        NetMsgReadBlock20(stream, self->m_block438);
    }
    else
    {
        self->m_block438[0] = g_defaultSaveGameBlock[0];
        self->m_block438[1] = g_defaultSaveGameBlock[1];
        self->m_block438[2] = g_defaultSaveGameBlock[2];
        self->m_block438[3] = g_defaultSaveGameBlock[3];
        self->m_block438[4] = g_defaultSaveGameBlock[4];
    }

    stream->ReadDword((unsigned int*)&self->m_word404);
    stream->ReadDword((unsigned int*)&self->m_word408);
    DataStoreReadStringField(stream, &self->m_mapPath);

    int headerOk = SaveGameHeaderUsable(&self->m_header, &block);
    int slotsOk  = SaveGameSlotsUsable(&self->m_slots, &record);

    if (headerOk != 0 && slotsOk != 0)
    {
        self->m_valid = 1;
        return 1;
    }

    return 0;
}

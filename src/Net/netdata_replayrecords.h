//============================================================================
//  The two record shapes CNetData::LoadReplayHeader's own record loop reads
//  through CDataStore - declared here, rather than in
//  netdata_replayrecordreaders.cpp alone, so that file's readers and
//  netdata_loadreplayheader.cpp's own caller agree on one type each.  See
//  netdata_replayrecordreaders.cpp for the field-by-field derivation.
//============================================================================
#ifndef NETDATA_REPLAYRECORDS_H
#define NETDATA_REPLAYRECORDS_H

#include "cdatastore.h"
#include "cgamedescstore.h"    // SSaveGameHeaderBlock

//  Both records are the stream's own unaligned wire layout - a 2-byte field
//  sitting at an odd offset in each - so both are packed to stop the
//  compiler inserting alignment padding no read here ever accounts for.
#pragma pack(push, 1)

//  0x6F652560 - record type 0x10.
struct SReplayType0x10Record
{
    unsigned int         m_field0;      // +0x00
    unsigned char        m_field4;      // +0x04
    unsigned short       m_field5;      // +0x05
    char                 m_reserved07[0x20 - 0x07];
    SSaveGameHeaderBlock m_header;      // +0x20, 0xB8 bytes
    unsigned int         m_fieldD8;     // +0xD8
};

//  0x6F652780 - record type 0x16.
struct SReplayType0x16Record
{
    unsigned char  m_field0;             // +0x00
    unsigned short m_field1;             // +0x01
    char           m_reserved03[0x1C - 0x03];
    unsigned int   m_field1C;            // +0x1C
};

#pragma pack(pop)

CDataStore* __fastcall ReadReplayType0x10Record(CDataStore* store,
                                                SReplayType0x10Record* out);
CDataStore* __fastcall ReadReplayType0x16Record(CDataStore* store,
                                                SReplayType0x16Record* out);

#endif

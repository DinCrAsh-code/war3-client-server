//============================================================================
//  0x6F3A4460 - CGameWar3::Save's own nested count/pointer table walk at
//  m_field98..m_fieldA4.  `header` is &this->m_field98: its own +0x00
//  (m_field98 itself) is never read here, +0x04 (m_field9C) is the outer
//  row count and +0x08 (m_fieldA0) the row array, each row 0x10 bytes with
//  the same count/array shape one level down (+0x04 inner count, +0x08 an
//  array of SerializePlayerSlotRef's own 4-byte refs).  Its own
//  translation unit for the same reason gamewar3serializegameslotref.cpp
//  is - a call this many instructions long, called from exactly one site,
//  still risks /Ob2 inlining it back into CGameWar3::Save.
//============================================================================
#include "cdatastorescratch.h"
#include "agent.h"

CDataStoreScratch* __fastcall SerializePlayerSlotRef(CDataStoreScratch* stream,
                                                       CAgent* const* ref);

struct SGameWar3RefTableRow
{
    char          m_reserved00[4];
    unsigned int  m_count;      // +0x04
    CAgent**      m_refs;       // +0x08
    char          m_reserved0C[4];
};

struct SGameWar3RefTable
{
    char                    m_reserved00[4];
    unsigned int            m_rowCount;   // +0x04
    SGameWar3RefTableRow*   m_rows;       // +0x08
};

CDataStoreScratch* __fastcall SerializeNestedRefTable(CDataStoreScratch* stream,
                                                        SGameWar3RefTable* header)
{
    stream->WriteDword(header->m_rowCount);

    for (unsigned int row = 0; row < header->m_rowCount; ++row)
    {
        SGameWar3RefTableRow* r = &header->m_rows[row];

        stream->WriteDword(r->m_count);

        for (unsigned int i = 0; i < r->m_count; ++i)
            SerializePlayerSlotRef(stream, &r->m_refs[i]);
    }

    return stream;
}

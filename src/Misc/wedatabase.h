//============================================================================
//  CWEDatabase - the "World Editor Database" generic .slk-backed table this
//  closure's HDATABASE/HDBFIELD Storm_401 allocation tags (see
//  wedatabase_thunks.cpp) and the WEDBEntryHash string table
//  (wedbentrylookup.cpp) both belong to.  Only the two fields this closure
//  actually reads are named; everything else is an unread gap.
//============================================================================
#ifndef WEDATABASE_H
#define WEDATABASE_H

class CWEDatabase
{
public:
    //  0x6F6F8D50 - the record at a one-based index, or the default record
    //  (+0x04) for index 0.  `retn 4`.
    void* RecordAt(int index);

    char  m_reserved00[0x04];
    void* m_defaultRecord;          // +0x04
    char  m_reserved08[0x114 - 0x08];
    void* m_records;                // +0x114, stride 0x108 per entry
};

#endif

//============================================================================
//  0x6F25CDD0 / 0x6F25CCD0 / 0x6F25CC50 - three column reads.
//
//  Each is the same six instructions: push the caller's row and key between
//  the two words of the table's own {column, fallback} pair and forward to
//  one shared cell reader.  The pair is the whole difference between them -
//  +0x78/+0x7C, +0x38/+0x3C and +0x18/+0x1C - and it is why three
//  addresses exist instead of one function with a column argument.
//
//  Two different readers, not one: the first two forward to 0x6F25C9C0 and
//  the third to 0x6F25CBA0, which is what says the third column's cells are
//  strings.  All three are `retn 8` and clean nothing of the four words
//  they push, so both readers clean sixteen (`retn 10h`).
//
//  The second word of each pair is loaded *first* in the shipped stream,
//  which is what an argument list evaluated right to left gives for free.
//============================================================================
#include "objectdatatable.h"

int CObjectDataTable::QueryColumn78(void* row, int key)
{
    return DataCellQueryInt(row, m_column78, key, m_column7C);
}

int CObjectDataTable::QueryColumn38(void* row, int key)
{
    return DataCellQueryInt(row, m_column38, key, m_column3C);
}

const char* CObjectDataTable::QueryColumn18(void* row, int key)
{
    return DataCellQueryString(row, m_column18, key,
                               (const char*)m_column1C);
}

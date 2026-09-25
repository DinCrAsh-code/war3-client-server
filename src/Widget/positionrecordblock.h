//============================================================================
//  The 0x2C-byte "position record" request block: two FourCC-tag-shaped
//  words, a caller value, a global, five zeroed words and two -1 sentinels
//  in positionrecordbuild.cpp's own builder (0x6F473980's forward, 0x6F478830);
//  the same 11-field layout reused with different field values (no tags at
//  all) by positionrecordarray.cpp's second entry point (0x6F4A54A0).  Only
//  the field *positions* are established here - what a receiver reads out of
//  them is not, so members keep the generic names both builders' own
//  comments explain.
//============================================================================
#ifndef POSITIONRECORDBLOCK_H
#define POSITIONRECORDBLOCK_H

struct SPositionRecordBlock
{
    unsigned int m_tag0;         // +0x00
    unsigned int m_tag1;         // +0x04
    int          m_argC;         // +0x08
    unsigned int m_global;       // +0x0C
    unsigned int m_zero10;       // +0x10
    unsigned int m_zero14;       // +0x14
    unsigned int m_zero18;       // +0x18
    unsigned int m_zero1C;       // +0x1C
    unsigned int m_zero20;       // +0x20
    int          m_sentinel24;   // +0x24
    int          m_sentinel28;   // +0x28
};

#endif

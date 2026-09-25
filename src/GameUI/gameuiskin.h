//============================================================================
//  The skin store: a hash table of named sections, each of which is itself a
//  hash table of named values.  Nothing in this call tree builds either -
//  only the two lookups and the three-step fallback on top of them - so the
//  layout is exactly the fields those touch.
//============================================================================
#ifndef GAMEUISKIN_H
#define GAMEUISKIN_H

#include "storm.h"

struct SSkinValue;
struct SSkinSection;

//  One node of either table: the intrusive link is at +0x0C (the tables'
//  own run-time link offset), the hash at +0x04, the key at +0x18 and the
//  payload at +0x1C.
struct SSkinValue
{
    char         m_reserved00[0x04];
    unsigned int m_hash;            // +0x04
    char         m_reserved08[0x10];
    const char*  m_name;            // +0x18
    const char*  m_value;           // +0x1C
};

struct SSkinSection
{
    char         m_reserved00[0x04];
    unsigned int m_hash;            // +0x04
    char         m_reserved08[0x10];
    const char*  m_name;            // +0x18
    char         m_values[0x28];    // +0x1C - a CSkinValueTable
};

class CSkinValueTable
{
public:
    char                          m_reserved00[0x1C];
    TSExplicitList<SSkinValue>*   m_pBuckets;   // +0x1C
    char                          m_reserved20[0x04];
    unsigned int                  m_mask;       // +0x24, -1 when unbuilt

    SSkinValue* Find(const char* name);         // 0x6F319890
};

class CSkinSectionTable
{
public:
    char                          m_reserved00[0x1C];
    TSExplicitList<SSkinSection>* m_pBuckets;   // +0x1C
    char                          m_reserved20[0x04];
    unsigned int                  m_mask;       // +0x24, -1 when unbuilt

    SSkinSection* Find(const char* name);       // 0x6F319920
};

//: unk_6FAB5564 - the section table itself, not a pointer to one: the
//: shipped code takes its address with `mov ecx, offset ...`.
extern CSkinSectionTable g_skinSectionTable;
//: dword_6FAB5100 - the name of the skin currently selected, null when none.
extern const char* g_pActiveSkinName;

#endif

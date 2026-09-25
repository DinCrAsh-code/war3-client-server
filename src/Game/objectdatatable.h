//============================================================================
//  CObjectDataTable - one of the game's row/column data tables, loaded from
//  a `Units\...` sheet and reached through a lazily created singleton kept
//  on the game data cache.
//
//  The table's own name is not recoverable from this call tree; what *is*
//  recoverable is the path it loads ("Units\\UpgradeData", the string
//  0x6F261CC0 hands its loader) and the shape of the accessors, which is
//  uniform: a row is found by a four-character key, and every column
//  accessor is a two-word {column, default} pair kept on the table itself
//  and forwarded to one shared cell reader.  Only the three column pairs
//  this closure reads are named.
//
//  Reached from JASS's EndGame: the per-player score's "upgrade ratio"
//  (Player/playerroster.cpp) asks this table how many upgrade types belong
//  to a race and which of them a given type is.
//============================================================================
#ifndef OBJECTDATATABLE_H
#define OBJECTDATATABLE_H

class CObjectDataTable
{
public:
    //  0x6F25C820 - is there a row under this key?  Consults the table's own
    //  index first and falls back on the shared row store, re-keying through
    //  PackFourCCString/UnpackFourCCString on the way.  `retn 4`.
    int  HasRow(const char* key);
    //  0x6F25C8D0 - the row object itself, reference-counted (the caller
    //  releases it through 0x6F4C8870).  Zero when there is none.  `retn 4`.
    void* AcquireRow(const char* key);

    //  The three column reads.  Each pushes its own {column, default} pair
    //  around the caller's row and key and forwards to one shared cell
    //  reader; the pair is what makes them three functions instead of one.
    //  All three are `retn 8`.
    int         QueryColumn78(void* row, int key);   // 0x6F25CDD0
    int         QueryColumn38(void* row, int key);   // 0x6F25CCD0
    const char* QueryColumn18(void* row, int key);   // 0x6F25CC50

    //  0x6F261D00 - how many of the table's rows belong to `race`.
    //  `retn 4`, so `race` is on the stack and `this` in ecx - a member,
    //  not a two-register __fastcall free function.
    int CountForRace(int race);

    //  Not reconstructed - naked redirects in objectdatatable_thunks.cpp.
    //  Every argument count is the shipped call site's own push count.
    int   RowCount();                                     // 0x6F25C910 retn 0
    int   QueryColumn88(void* row, int key);              // 0x6F25CCB0 retn 8

    char  m_reserved00[0x18];
    int   m_column18;      // +0x18  QueryColumn18's pair
    int   m_column1C;      // +0x1C
    char  m_reserved20[0x38 - 0x20];
    int   m_column38;      // +0x38  QueryColumn38's pair
    int   m_column3C;      // +0x3C
    char  m_reserved40[0x78 - 0x40];
    int   m_column78;      // +0x78  QueryColumn78's pair
    int   m_column7C;      // +0x7C
};

//  0x6F25C9C0 / 0x6F25CBA0 - the two shared cell readers the three column
//  accessors forward to.  `retn 10h` on both: each of the three pushes four
//  words and cleans none of them itself.
int         __stdcall DataCellQueryInt(void* row, int column, int key,
                                       int fallback);
const char* __stdcall DataCellQueryString(void* row, int column, int key,
                                          const char* fallback);

//  0x6F261CC0 - the singleton, created on first use out of
//  "Units\\UpgradeData".
CObjectDataTable* GetUpgradeDataTable();

//  0x6F26E210 - a race name to its id (Game/racename.cpp).
int __fastcall RaceNameToId(const char* name);

//  0x6F4C3F80 - the inverse of PackFourCCString (Net/packfourcc.cpp).
int __fastcall UnpackFourCCString(unsigned int fourCC, char* out, int size);

//  0x6F4C3F50 - and the packer itself, declared exactly as that file
//  defines it so the two mangled names are one symbol.
unsigned int __fastcall PackFourCCString(const char* text);

//  0x6F4C8870 - Agent/refcnt.h's release forward, under the name this repo
//  already binds to that address.  Declared exactly as
//  Agent/refcnt_release_forward.cpp defines it - the parameter type is part
//  of the mangled name (tools/link_check.py, cause 1).
class TRefCnt;
void __fastcall ReleaseForward_6F4C8870(TRefCnt* self);

//  0x6F0098F0 - Pathfinding/rowtable.cpp.
int __fastcall RowTableLookup(int column);

//  0x6F2B01A0 / 0x6F2B00C0 - Game/upgradeaffects.cpp.
int __fastcall UpgradeAffectedTypeCount(unsigned int race);
int __fastcall UpgradeAffectsType(unsigned int typeId, unsigned int race);

#endif

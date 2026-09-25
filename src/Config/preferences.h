//============================================================================
//  0x6F005720 / 0x6F0059C0 - see preferences.cpp.
//============================================================================
#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "storm.h"

//  The key->value cache living at CPreferencesWar3+0x28.  Its lookup is a
//  thiscall member for the same reason SPlayerSlotTable::Find is - the
//  shipped call site pushes the key rather than putting it in edx, and the
//  callee cleans it.
//
//  The node is a plain Storm hash node (storm.h's TSHashObject, offset for
//  offset): the hash value the bucket chain is keyed on at +0x00, the
//  bucket link at +0x04, the table-wide list link in the eight bytes after
//  it, the key at +0x14 and the stored preference at +0x18.  0x6F004390's
//  walk reads +0x00 and steps through +0x04 exactly as
//  TSHashTable<T,K>::PtrHashOnly does; the preference id *is* the hash
//  value, so nothing ever reads +0x14.
struct SPreferenceEntry
{
    unsigned int             m_hashval;      // +0x00 - the preference id
    TSLink<SPreferenceEntry> m_hashlink;     // +0x04
    char                     m_reserved0C[8];// +0x0C - the all-nodes link
    int                      m_key;          // +0x14 - never read here
    int                      m_value;        // +0x18
};

//  The table itself.  Confirmed to be TSHashTable<SPreferenceEntry, int>'s
//  own layout, field for field: the vtable ConstructPreferenceCache
//  (0x6F004AB0) stamps occupies +0x00, m_list is the 12 bytes after it,
//  m_probes is the word after that, and m_buckets (a
//  TSGrowableArray<TSExplicitList<SPreferenceEntry>>) starts at +0x14 -
//  its own m_data lands at +0x1C, which is the only field of it this
//  call tree's callers ever name directly.  See
//  Config/preferencehashtable.cpp for New/Rehash/Initialize/CheckGrow and
//  their own UnlinkAll/SetCount/ComputeChunk/SetAlloc callees, all
//  Containers/tshash.inl's and Containers/tsarray.inl's shared bodies
//  rather than anything preference-specific.
class SPreferenceCache : public TSHashTable<SPreferenceEntry, int>
{
public:
    //  0x6F004390.
    SPreferenceEntry* Find(int key, void* bucket);
};

//  0x6F0047B0's own element type: CPreferencesWar3 is itself a second Storm
//  hash table (name -> preference id, ConstructPreferenceTable's own vtable),
//  not just the id->value cache at +0x28.  Nothing in this call tree ever
//  reaches a live node of it (Rehash/Initialize/UnlinkAll/SetCount never
//  dereference past +0x00), so only the TSHashObject-shaped header every
//  node has to start with is named; the key/value fields past +0x14 are
//  still Game.dll's.
struct PrefString
{
    unsigned int      m_hashval;       // +0x00
    TSLink<PrefString> m_hashlink;     // +0x04
    char              m_reserved0C[8]; // +0x0C - the table-wide list link
};

class CPreferencesWar3 : public TSHashTable<PrefString, HASHKEY_NONE>
{
public:
    //  0x6F0057F0 - remember a freshly parsed default.  Two stack arguments,
    //  not one register and one stack.  Answers 1 when it stored and 0 for
    //  an out-of-range id; preferences.cpp's one caller ignores that, but
    //  the shipped body plainly computes it (`mov eax, 1` on one tail,
    //  `xor eax, eax` on the other).
    int StoreValue(int key, int value);

    //  0x6F0028C0 - push one preference id/value pair at whatever subsystem
    //  owns it.  See preferenceapply.cpp.
    int ApplyPreference(int key, int value);

    //  0x6F005BB0 - read a preference back out of the store and apply it.
    //  See preferenceapplystored.cpp.
    int ApplyStoredPreference(int key);

    //  0x6F005A30 - the string sibling of GetIntValue: three stack
    //  arguments (`retn 0Ch`), the buffer and its size.
    int GetStringValue(int key, char* buffer, unsigned int size);

    //  0x6F0059C0.  A thiscall member: the key and the out-pointer are both
    //  stack arguments and it cleans eight bytes - its one caller pushes
    //  `7` rather than loading edx with it.
    int GetIntValue(int key, int* out);

    //  0x6F0058A0 - gamemain-batch-1's own display-mode/resolution-list
    //  init; see Game/gamemaininitresolutionlist.cpp.
    int InitResolutionList(int index);

    //  TSHashTable<PrefString,HASHKEY_NONE>'s own base occupies exactly
    //  0x28 bytes (vtable + m_list + m_probes + m_buckets), which is why
    //  the cache lands at +0x28 - 0x6F005640 sizes this table's own bucket
    //  array before it sizes m_cache's.
    SPreferenceCache m_cache;          // +0x28 - the key->value map
};

//  0x6F005720 - the singleton, created on first use.
CPreferencesWar3* GetPreferences();

//  dword_6F8755B0 - CPreferencesWar3's own defaults array, one 16-byte
//  record per preference (0x4F of them): the kind word (0 = integer,
//  1 = string) at +0x00, the default value/string the rest of the record.
//  preferencesctor.cpp's own 0x6F8755B4 is the same array's second field,
//  addressed from its own name rather than as an offset of this one -
//  see that file's header comment for why.  Declared once, here, so every
//  TU that reaches it (preferencesctor.cpp, preferenceapplystored.cpp,
//  gamemaininitresolutionlist.cpp) mangles the same symbol; two of those
//  three used to carry their own local copy of this declaration and one,
//  briefly, a same-address `int[]` alias under a different name
//  (g_displayModeKindTable) - link_check.py's own declaration-mismatch
//  check is what catches a drift here, not verify.py.
struct SPreferenceKind
{
    int  m_kind;              // +0x00 - 0 = integer, 1 = string
    char m_reserved04[0x0C];
};
extern SPreferenceKind g_preferenceKinds[];      // dword_6F8755B0

#endif

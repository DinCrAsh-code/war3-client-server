//============================================================================
//  0x6F005640 - CPreferencesWar3's constructor proper.
//
//  Two Storm hash tables are built here, not one: *this* object is a table
//  in its own right (mask at +0x24) and it carries a second one, the
//  id->value cache, at +0x28.  The two are pre-sized from the shipped
//  defaults array by counting how many of its 0x4F records are type 0 and
//  how many are type 1 - one table per kind - so neither has to rehash on
//  the first pass of ApplyStoredPreference.
//
//  The record base is 0x6F8755B0 and *not* preferences.cpp's
//  g_preferenceDefaults (0x6F8755B4): the type word sits four bytes ahead
//  of the default string, so the two files address the same array from two
//  different fields.  Both are declared here at the field they use rather
//  than one being expressed as an offset of the other, because the operand
//  the canonicaliser compares is the symbol.
//
//  The power-of-two rounding is written out twice in the shipped stream,
//  once per table, so the helper is __forceinline: MSVC declines a body
//  this size at /Ob2 on its own and emits two calls where the shipped code
//  has two copies.
//
//  Own translation unit: four real calls out of it.
//============================================================================
#include "preferences.h"

//  0x6F0047B0 / 0x6F004AB0 - the two tables' own constructors, `this` in
//  ecx, both `retn 0`.  Below this dump's depth cut.
void __fastcall ConstructPreferenceTable(void* table);
void __fastcall ConstructPreferenceCache(void* cache);

//  SPreferenceKind / g_preferenceKinds - preferences.h (one 16-byte record
//  per preference; preferences.cpp declares the same array from its
//  second field, see this file's own header comment).

//  Storm's tables are always a power of two buckets and never more than
//  0x2000 of them.  `n` is a count of live entries, so this is "the
//  smallest power of two that holds them all".
static __forceinline unsigned int PreferenceBucketCount(unsigned int n)
{
    if (n > 0x2000)
        return 0x2000;

    unsigned int shift = 0;
    unsigned int scan = n;
    while (scan > 1)
    {
        scan >>= 1;
        shift++;
    }

    unsigned int buckets = 1u << shift;
    if (n > buckets)
        buckets += buckets;
    return buckets;
}

CPreferencesWar3* __fastcall ConstructPreferences(void* raw)
{
    CPreferencesWar3* self = (CPreferencesWar3*)raw;

    ConstructPreferenceTable(self);

    //  The cache is reached through this pointer and not as `self->m_cache`
    //  for the rest of the function: the shipped code keeps &m_cache in a
    //  register and reads the mask at +0x24 *of it*, where the member
    //  spelling folds the two offsets into one [self+0x4C] load.  Declared
    //  here rather than above the first call because MSVC issues the `lea`
    //  where the pointer is formed, and the shipped stream has it after
    //  ConstructPreferenceTable.
    SPreferenceCache* cache = &self->m_cache;

    ConstructPreferenceCache(cache);

    unsigned int ints = 0;
    unsigned int strings = 0;
    for (unsigned int i = 0; i < 0x4F; i++)
    {
        int kind = g_preferenceKinds[i].m_kind;
        if (kind == 0)
            ints++;
        else if (kind == 1)
            strings += kind;
    }

    if (ints > cache->m_mask + 1)
        cache->Rehash(PreferenceBucketCount(ints));

    if (strings > self->m_mask + 1)
        self->Rehash(PreferenceBucketCount(strings));

    return self;
}

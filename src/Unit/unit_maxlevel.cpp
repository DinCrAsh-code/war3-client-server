//============================================================================
//  0x6F29DCA0 - the level cap that applies to this unit, clamped against
//  whatever its own type declares.
//
//  Three steps.  The type's own base level comes out of the unit-type probe
//  at 0x6F29BB80; if the unit carries the ability 0x6F0787D0 finds and that
//  ability's vtable slot 0xDB answers non-zero, that ability's own +0xE4
//  replaces it.  Then the configured cap is read from `Misc` - and *which*
//  key is asked for is the whole point of the function:
//
//      MaxHeroLevel   the type id starts with an upper-case letter and
//                     bit 30 of the unit's flags word is clear
//      MaxUnitLevel   otherwise
//
//  An upper-case first character in a Warcraft III type id is the hero
//  convention, and this is where the score cluster's own reading of
//  `m_footprintType`'s top byte (Player/playerheroselect.cpp uses exactly
//  the same `- 'A'` / `> 25` range test) is confirmed against something
//  that names the two cases.
//
//  The `MaxHeroLevel` string is loaded *between* the flag test and its
//  branch, which is the "default first, overwritten in the else" shape an
//  if/else with a value in each arm gives.
//============================================================================
#include "gamecontext.h"

//  0x6F009F90 - Config/configint.cpp.
int __fastcall GetConfigInt(const char* section, const char* key, int index);

//  0x6F113CC0 - Misc/misc_trivial_getters.cpp's 'Bply'.
unsigned int GetFourCC_6F113CC0();

//  0x6F29BB80 - the unit type's own declared level, and 0x6F0787D0 - find
//  an ability on a unit.  Both are naked redirects in
//  Unit/unit_herothunks.cpp; the argument counts are the shipped call
//  sites' own push counts.
int __fastcall UnitTypeDeclaredLevel(unsigned int typeId);

struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

struct SUnitLevelSource
{
    char          m_reserved000[0x30];
    unsigned int  m_typeId;      // +0x30 - the type FourCC; its top byte is
    char          m_reserved034[0x5C - 0x34];  //   the character tested below
    unsigned int  m_flags5C;     // +0x5C
};

typedef int (__thiscall *AbilityQueryFn)(void* self);

int __fastcall UnitConfiguredMaxLevel(void* unit)
{
    SUnitLevelSource* self = (SUnitLevelSource*)unit;

    int level = UnitTypeDeclaredLevel(self->m_typeId);

    void* ability = ((SUnitAbilityHost*)unit)
                        ->FindAbility(GetFourCC_6F113CC0(), 1, 0, 1, 1);
    if (ability != 0)
    {
        if (((AbilityQueryFn)(*(void***)ability)[0x36C / 4])(ability) != 0)
            level = *(int*)((char*)ability + 0xE4);
    }

    const char* key = "MaxHeroLevel";
    if ((unsigned int)((self->m_typeId >> 24) - 'A') > 25
        || (self->m_flags5C & 0x40000000) != 0)
    {
        key = "MaxUnitLevel";
    }

    int cap = GetConfigInt("Misc", key, 0);

    if (level <= 0)
        return 0;

    if (level > cap)
        level = cap;

    return level;
}

//============================================================================
//  0x6F2A0210 - a unit's level.
//
//  The record at +0x1F0 carries a published counter at its own +0x6C, and
//  the level is that counter plus one.  With no record the answer falls
//  back on the configured maximum for the unit's kind, reached as a tail
//  jump with `this` untouched in ecx.
//
//  Own translation unit: HeroLevel is a real call from two places in the
//  hero cluster, and the fallback has to stay a real `jmp`.
//============================================================================
#include "playerscore.h"       // HandleRefFieldOwner_6F473170

//  0x6F29DCA0 - Unit/unit_maxlevel.cpp.
int __fastcall UnitConfiguredMaxLevel(void* unit);

struct SUnitTypeHolderB
{
    char m_reserved000[0x1F0];
    char* m_pRecord;      // +0x1F0
};

int __fastcall HeroLevel(void* unit)
{
    //  The *found* arm first: the shipped `je` jumps forward onto the tail
    //  jump, so the counter read is the fall-through.  Written the other way
    //  round the two blocks swap and the tail call lands at the top.
    char* record = ((SUnitTypeHolderB*)unit)->m_pRecord;
    if (record != 0)
        return ((HandleRefFieldOwner_6F473170*)(record + 0x6C))
                   ->QueryField78() + 1;

    return UnitConfiguredMaxLevel(unit);
}

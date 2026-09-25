//============================================================================
//  0x6F0787D0 - SUnitAbilityHost::FindAbility, `this` (a CUnit) in ecx, five
//  stack arguments, `retn 14h`.  Every caller that already declares
//  SUnitAbilityHost locally (unit_flagbit8.cpp, unit_maxlevel.cpp,
//  unit_abilityoverridefloat.cpp, and CUnit's own vtable slot 113 below)
//  reaches this real body now instead of the naked redirect
//  Unit/unit_herothunks.cpp used to carry - that file's own third and last
//  thunk is the only one still standing.
//
//  A fast path first: five known ability rawcodes are cached on the unit
//  itself the moment they're granted (RefreshAbilityShortcuts,
//  0x6F076380 - unit_abilityshortcuts.cpp - is the writer, walking the same
//  list below and filling these same five fields), so a caller asking for
//  exactly one of them skips the walk entirely and returns the cached
//  pointer (or null) straight off `this`:
//
//      'ANbu' 'AHbu' 'AEbu' 'AGbu' 'AObu' 'AUbu'   ->  +0x1F4
//      'AHer'                                       ->  +0x1F0
//      'AInv'                                       ->  +0x1F8 (the
//                                                        inventory ability -
//                                                        unit.h's own
//                                                        "ability object at
//                                                        +0x1F8" note)
//      'Aatk'                                       ->  +0x1E8
//      'Amov'                                       ->  +0x1EC
//
//  Anything else falls into the general walk: follow the {handle,typeTag}
//  pair at +0x1DC (unit.h's own `m_ref1DC`) through
//  Item/itemhandleresolve.h's QueryHandleField0x54 to the first ability
//  object, then each object's own +0x24/+0x28 pair to the next.  Per
//  candidate, `a`/`b`/`c`/`d` pick which of two independent checks the
//  caller wants:
//
//    - bit 0x20 of the ability's own +0x20 selects which of `d` or `c`
//      gates the match check below at all (skip this candidate if the
//      selected one is 0, exactly like a per-candidate "don't bother
//      unless the caller asked for this kind" flag);
//    - the match check itself is vtable+0x1C(candidate) - the candidate's
//      own reported code - compared against `rawcode`, unless `b` is set,
//      in which case the candidate's own +0x34 is compared instead;
//    - a miss on that check is still a match if `a` is set and
//      Agent/agiletype.h's AgileTypeIsDerivedFrom(vtable+0x1C(candidate),
//      rawcode) says the reported code derives from the one asked for.
//
//  First candidate that matches (by either check) is the return value;
//  running off the end returns null.
//============================================================================
#include "itemhandleresolve.h"   // QueryHandleField0x54 (0x6F4786B0)
#include "agiletype.h"           // AgileTypeIsDerivedFrom (0x6F471910)

struct SUnitAbility
{
    char          m_reserved000[0x20];
    unsigned char m_flags20;    // +0x20 - bit 0x20: match against `c`, not `d`
    char          m_reserved021[3];
    SOptionalHandleRef m_next;  // +0x24/+0x28 - next ability in the list
    char          m_reserved02C[0x34 - 0x2C];
    int           m_code34;     // +0x34 - alternate match code (the `b` path)
};

typedef int (__thiscall *AbilityGetOwnCodeFn)(SUnitAbility* self);

struct SUnitAbilityHost
{
    char           m_reserved000[0x1DC];
    SOptionalHandleRef m_ref1DC;  // +0x1DC/+0x1E0 - first ability in the list
    char           m_reserved1E4[0x1E8 - 0x1E4];
    void*          m_atk1E8;      // +0x1E8 - cached 'Aatk'
    void*          m_vmo1EC;      // +0x1EC - cached 'Amov'
    void*          m_her1F0;      // +0x1F0 - cached 'AHer'
    void*          m_bonus1F4;    // +0x1F4 - cached attribute-bonus ability
    void*          m_inv1F8;      // +0x1F8 - cached 'AInv' (the inventory
                                   //   ability, unit.h's own +0x1F8 note)

    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

void* SUnitAbilityHost::FindAbility(int rawcode, int a, int b, int c, int d)
{
    switch (rawcode)
    {
    case 'ANbu': case 'AHbu': case 'AEbu': case 'AGbu':
    case 'AObu': case 'AUbu':
        return m_bonus1F4;
    case 'AHer':
        return m_her1F0;
    case 'AInv':
        return m_inv1F8;
    case 'Aatk':
        return m_atk1E8;
    case 'Amov':
        return m_vmo1EC;
    default:
        break;
    }

    SUnitAbility* candidate = (m_ref1DC.m_typeTag & (int)m_ref1DC.m_handle) != -1
                                   ? (SUnitAbility*)QueryHandleField0x54(&m_ref1DC)
                                   : 0;

    while (candidate)
    {
        SUnitAbility* next = (candidate->m_next.m_typeTag & (int)candidate->m_next.m_handle) != -1
                                  ? (SUnitAbility*)QueryHandleField0x54(&candidate->m_next)
                                  : 0;

        int wanted = (candidate->m_flags20 & 0x20) ? d : c;
        if (wanted != 0)
        {
            int code = ((AbilityGetOwnCodeFn)(*(void***)candidate)[0x1C / 4])(candidate);
            int compareAgainst = b ? candidate->m_code34 : code;
            if (compareAgainst == rawcode)
                return candidate;
            if (a && AgileTypeIsDerivedFrom(code, rawcode))
                return candidate;
        }

        candidate = next;
    }

    return 0;
}

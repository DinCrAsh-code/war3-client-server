//============================================================================
//  0x6F0765E0 - SUnitAbilityHost::HasChanneledAbility, `this` (a CUnit) in
//  ecx, no stack arguments, `retn 0`.
//
//  Walks the same ability list Unit/unit_findability.cpp's FindAbility and
//  Unit/unit_abilityshortcuts.cpp's RefreshAbilityShortcuts both walk (the
//  {handle,typeTag} chain rooted at +0x1DC, each candidate's own +0x24/+0x28
//  pair for "next"), asking each candidate's own vtable+0x198 (slot 0x66,
//  no arguments past `this`) whether it applies; the first candidate that
//  answers non-zero makes the whole call return true.  CUnit's own vtable
//  slot 113 (0x6F27A620) calls this when asked for the unit's own default
//  footprint scale, to decide whether to look up a channeled ability's own
//  footprint override instead.
//============================================================================
#include "itemhandleresolve.h"   // QueryHandleField0x54 (0x6F4786B0)

struct SUnitAbility
{
    char          m_reserved000[0x24];
    SOptionalHandleRef m_next;  // +0x24/+0x28 - next ability in the list
};

typedef int (__thiscall *AbilityIsChanneledFn)(SUnitAbility* self);

struct SUnitAbilityHost
{
    char           m_reserved000[0x1DC];
    SOptionalHandleRef m_ref1DC;  // +0x1DC/+0x1E0 - first ability in the list

    int HasChanneledAbility();
};

int SUnitAbilityHost::HasChanneledAbility()
{
    SUnitAbility* candidate = (m_ref1DC.m_typeTag & (int)m_ref1DC.m_handle) != -1
                                   ? (SUnitAbility*)QueryHandleField0x54(&m_ref1DC)
                                   : 0;

    while (candidate)
    {
        SUnitAbility* next = (candidate->m_next.m_typeTag & (int)candidate->m_next.m_handle) != -1
                                  ? (SUnitAbility*)QueryHandleField0x54(&candidate->m_next)
                                  : 0;

        if (((AbilityIsChanneledFn)(*(void***)candidate)[0x198 / 4])(candidate))
            return 1;

        candidate = next;
    }

    return 0;
}

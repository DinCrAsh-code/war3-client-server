//============================================================================
//  0x6F076380 - SUnitAbilityHost::RefreshAbilityShortcuts, `this` (a CUnit)
//  in ecx, no stack arguments, `retn 0`.
//
//  The writer for the five cached-ability fields
//  Unit/unit_findability.cpp's own FindAbility (0x6F0787D0) reads as its
//  fast path: walks the whole ability list once (the same {handle,typeTag}
//  chain at +0x1DC, through the same per-candidate vtable+0x1C "own code"
//  call) and, for each of five known rawcodes, stamps the matching
//  candidate straight onto `this` - last one wins if more than one
//  candidate reports the same code, since nothing here stops the walk on a
//  match.  See unit_findability.cpp for the full rawcode -> field table;
//  it is the exact same five groups this function's own cmp chain sorts
//  candidates into.
//============================================================================
#include "itemhandleresolve.h"   // QueryHandleField0x54 (0x6F4786B0)

struct SUnitAbility
{
    char          m_reserved000[0x20];
    unsigned char m_flags20;    // +0x20 - unused by this walk
    char          m_reserved021[3];
    SOptionalHandleRef m_next;  // +0x24/+0x28 - next ability in the list
};

typedef int (__thiscall *AbilityGetOwnCodeFn)(SUnitAbility* self);

struct SUnitAbilityHost
{
    char           m_reserved000[0x1DC];
    SOptionalHandleRef m_ref1DC;  // +0x1DC/+0x1E0 - first ability in the list
    char           m_reserved1E4[0x1E8 - 0x1E4];
    void*          m_atk1E8;      // +0x1E8 - 'Aatk'
    void*          m_vmo1EC;      // +0x1EC - 'Amov'
    void*          m_her1F0;      // +0x1F0 - 'AHer'
    void*          m_bonus1F4;    // +0x1F4 - attribute-bonus ability
    void*          m_inv1F8;      // +0x1F8 - 'AInv' (the inventory ability)

    void RefreshAbilityShortcuts();
};

void SUnitAbilityHost::RefreshAbilityShortcuts()
{
    if ((m_ref1DC.m_typeTag & (int)m_ref1DC.m_handle) == -1)
        return;

    SUnitAbility* candidate = (SUnitAbility*)QueryHandleField0x54(&m_ref1DC);
    if (!candidate)
        return;

    for (;;)
    {
        SUnitAbility* next = (candidate->m_next.m_typeTag & (int)candidate->m_next.m_handle) != -1
                                  ? (SUnitAbility*)QueryHandleField0x54(&candidate->m_next)
                                  : 0;

        switch (((AbilityGetOwnCodeFn)(*(void***)candidate)[0x1C / 4])(candidate))
        {
        case 'ANbu': case 'AHbu': case 'AEbu': case 'AGbu':
        case 'AObu': case 'AUbu':
            m_bonus1F4 = candidate;
            break;
        case 'AHer':
            m_her1F0 = candidate;
            break;
        case 'AInv':
            m_inv1F8 = candidate;
            break;
        case 'Aatk':
            m_atk1E8 = candidate;
            break;
        case 'Amov':
            m_vmo1EC = candidate;
            break;
        default:
            break;
        }

        if (!next)
            break;
        candidate = next;
    }
}

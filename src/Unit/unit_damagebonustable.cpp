//============================================================================
//  0x6F26D300 - `retn 4`.  CUnit vtable slot 72's own depth-1 closure: a
//  lazily-filled cross table of "Misc"/"DamageBonusXxx" config values (one
//  of seven keys - Spells/Normal/Pierce/Siege/Magic/Chaos/Hero - times eight
//  sub-indices each) plus a separate seven-entry "EtherealDamageBonus"
//  table, both filled once and then just indexed.
//
//  g_damageBonusTablePending reads backwards from the usual "already
//  initialised" sense - nonzero means "still needs filling" (the
//  disassembly's own `jz` skips the whole fill block when the flag is
//  already zero).
//
//  The ethereal table's fill order is not index order: index 5's own
//  config read (`GetConfigFloat(..., 5)`) is the one stored into the
//  table's first slot (dword_6FAB42E4), and indices 0-4 land one slot
//  further along each (dword_6FAB42E8..dword_6FAB42F8) - i.e.
//  table[(i+1)%7] = GetConfigFloat(key, i) for i in 0..6, confirmed
//  address by address rather than assumed from a clean loop shape, so it
//  is spelled out here the same way rather than written as a modulo that
//  might not compile to the same seven stores.
//============================================================================
#include "gamecontext.h"
#include "CFloat.h"

//  0x6F009EE0 - Config/configfloat.cpp.
CFloat __fastcall GetConfigFloat(const char* section, const char* key, int index);

extern int   g_damageBonusTablePending;        // dword_6FA70E90
extern CFloat g_damageBonusTable[7][8];        // dword_6FAB4358
extern CFloat g_etherealDamageBonusTable[7];   // dword_6FAB42E4

CFloat __fastcall GetDamageBonusMultiplier(int damageType, int attackType)
{
    if (g_damageBonusTablePending)
    {
        for (int type = 0; type < 7; ++type)
        {
            for (int idx = 0; idx < 8; ++idx)
            {
                switch (type)
                {
                case 0:
                    g_damageBonusTable[0][idx] = GetConfigFloat("Misc", "DamageBonusSpells", idx);
                    break;
                case 1:
                    g_damageBonusTable[1][idx] = GetConfigFloat("Misc", "DamageBonusNormal", idx);
                    break;
                case 2:
                    g_damageBonusTable[2][idx] = GetConfigFloat("Misc", "DamageBonusPierce", idx);
                    break;
                case 3:
                    g_damageBonusTable[3][idx] = GetConfigFloat("Misc", "DamageBonusSiege", idx);
                    break;
                case 4:
                    g_damageBonusTable[4][idx] = GetConfigFloat("Misc", "DamageBonusMagic", idx);
                    break;
                case 5:
                    g_damageBonusTable[5][idx] = GetConfigFloat("Misc", "DamageBonusChaos", idx);
                    break;
                case 6:
                    g_damageBonusTable[6][idx] = GetConfigFloat("Misc", "DamageBonusHero", idx);
                    break;
                }
            }
        }

        g_etherealDamageBonusTable[1] = GetConfigFloat("Misc", "EtherealDamageBonus", 0);
        g_etherealDamageBonusTable[2] = GetConfigFloat("Misc", "EtherealDamageBonus", 1);
        g_etherealDamageBonusTable[3] = GetConfigFloat("Misc", "EtherealDamageBonus", 2);
        g_etherealDamageBonusTable[4] = GetConfigFloat("Misc", "EtherealDamageBonus", 3);
        g_etherealDamageBonusTable[5] = GetConfigFloat("Misc", "EtherealDamageBonus", 4);
        g_etherealDamageBonusTable[0] = GetConfigFloat("Misc", "EtherealDamageBonus", 5);
        g_etherealDamageBonusTable[6] = GetConfigFloat("Misc", "EtherealDamageBonus", 6);

        g_damageBonusTablePending = 0;
    }

    return g_damageBonusTable[damageType][attackType];
}

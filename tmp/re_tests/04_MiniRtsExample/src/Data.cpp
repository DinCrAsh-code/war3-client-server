#include "Data.h"

#define N 0xFF

#define ST  UTF_STRUCTURE
#define HR  UTF_HERO
#define WK  UTF_WORKER
#define FL  UTF_FLYING
#define SM  UTF_SUMMONED
#define MC  UTF_MECHANICAL
#define TH  UTF_TOWNHALL
#define GM  UTF_GOLDMINE
#define LD  UTF_LUMBERDROP
#define CR  UTF_CREEP
#define AA  UTF_ATTACKS_AIR
#define AG  UTF_ATTACKS_GND
#define CS  UTF_CASTER
#define AL  UTF_ALTAR
#define RG  UTF_RANGED

// flags, race, level, life, mana, armorX10, def, atk, weapon, dice, sides, footprint,
// dmgBase, cooldownMs, dmgPointMs, backswingMs, range, missileSpeed, splash,
// move, turn, collision, sightDay, sightNight, gold, lumber, buildTime, food, bounty,
// lifeRegenX100, manaRegenX100, abilities[4], prereqs[2], producedBy, upgradeClass
const UnitTypeData g_unitTypes[UT_COUNT] =
{
    // ---- race A ----
    { WK|AG,       RACE_A, 1,  220,   0,   0, DEF_MEDIUM,    ATK_NORMAL, WPN_INSTANT,   1,  2, 0,   4, 2000, 433, 567,   90,    0,   0, 190, 500, 16,  800, 600,  75,   0, 15,   1,  5,  25,   0, { AB_HARVEST, AB_BUILD, N, N }, { N, N }, UT_A_TOWNHALL, 0 },
    { AG,          RACE_A, 2,  420,   0,  20, DEF_HEAVY,     ATK_NORMAL, WPN_INSTANT,   1,  2, 0,  11, 1350, 500, 500,   90,    0,   0, 270, 600, 16, 1400, 800, 135,   0, 20,   2, 15,  25,   0, { N, N, N, N }, { N, N }, UT_A_BARRACKS, 1 },
    { AG|AA|RG,    RACE_A, 3,  505,   0,   0, DEF_MEDIUM,    ATK_PIERCE, WPN_MISSILE,   2,  4, 0,  16, 1500, 170, 700,  500, 1800,   0, 270, 600, 16, 1400, 800, 205,  30, 26,   3, 20,  25,   0, { N, N, N, N }, { UT_A_BLACKSMITH, N }, UT_A_BARRACKS, 2 },
    { AG,          RACE_A, 4,  835,   0,  50, DEF_HEAVY,     ATK_NORMAL, WPN_INSTANT,   2,  6, 0,  26, 1400, 660, 340,  100,    0,   0, 350, 600, 24, 1400, 800, 245,  60, 45,   4, 30,  25,   0, { N, N, N, N }, { UT_A_LUMBERMILL, UT_A_BLACKSMITH }, UT_A_BARRACKS, 1 },
    { AG|AA|RG|CS, RACE_A, 2,  290, 200,   0, DEF_UNARMORED, ATK_MAGIC,  WPN_MISSILE,   1,  2, 0,   7, 2000, 590, 410,  600,  900,   0, 270, 600, 16, 1400, 800, 135,  10, 28,   2, 12,  25,  67, { AB_HEAL, N, N, N }, { N, N }, UT_A_SANCTUM, 3 },
    { AG|AA|RG|CS, RACE_A, 2,  325, 200,   0, DEF_UNARMORED, ATK_MAGIC,  WPN_MISSILE,   1,  2, 0,   9, 1750, 750, 780,  600,  900,   0, 270, 600, 16, 1400, 800, 155,  20, 30,   2, 12,  25,  67, { AB_SLOW, N, N, N }, { N, N }, UT_A_SANCTUM, 3 },
    { AG|RG,       RACE_A, 3,  360,   0,   0, DEF_HEAVY,     ATK_SIEGE,  WPN_ARTILLERY, 1, 10, 0,  51, 3500,1000, 500, 1150,  900, 150, 270, 400, 24, 1400, 800, 180,  70, 40,   3, 20,  25,   0, { N, N, N, N }, { N, N }, UT_A_WORKSHOP, 2 },
    { FL|AG|AA|RG, RACE_A, 4,  875,   0,   0, DEF_LIGHT,     ATK_MAGIC,  WPN_MISSILE,   2,  8, 0,  42, 2400, 500, 500,  450, 1100,   0, 350, 700, 32, 1400, 800, 280,  70, 45,   4, 30,  25,   0, { N, N, N, N }, { UT_A_LUMBERMILL, N }, UT_A_WORKSHOP, 2 },
    { HR|AG,       RACE_A, 1,  100,   0,  20, DEF_HERO,      ATK_HERO,   WPN_INSTANT,   2,  6, 0,   2, 2200, 433, 567,  100,    0,   0, 270, 600, 32, 1800, 800, 425, 100, 55,   5,  0,  25,   0, { N, N, N, N }, { N, N }, UT_A_ALTAR, 0 },
    { HR|AG|AA|RG, RACE_A, 1,  100,   0,   0, DEF_HERO,      ATK_HERO,   WPN_MISSILE,   1,  8, 0,   1, 2130, 550, 450,  600,  900,   0, 320, 600, 32, 1800, 800, 425, 100, 55,   5,  0,  25,   0, { N, N, N, N }, { N, N }, UT_A_ALTAR, 0 },
    { HR|AG,       RACE_A, 1,  100,   0,  20, DEF_HERO,      ATK_HERO,   WPN_INSTANT,   2,  6, 0,   1, 2220, 350, 650,  100,    0,   0, 270, 600, 32, 1800, 800, 425, 100, 55,   5,  0,  25,   0, { N, N, N, N }, { N, N }, UT_A_ALTAR, 0 },
    { ST|TH|LD,    RACE_A, 1, 1500,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 4,   0,    0,   0,   0,    0,    0,   0,   0,   0, 64,  900, 900, 385, 185, 90, -12,  0,   0,   0, { N, N, N, N }, { N, N }, UT_A_WORKER, 0 },
    { ST,          RACE_A, 1,  500,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 2,   0,    0,   0,   0,    0,    0,   0,   0,   0, 32,  900, 900,  80,  20, 25,  -6,  0,   0,   0, { N, N, N, N }, { N, N }, UT_A_WORKER, 0 },
    { ST,          RACE_A, 1, 1500,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 160,  60, 40,   0,  0,   0,   0, { N, N, N, N }, { UT_A_TOWNHALL, N }, UT_A_WORKER, 0 },
    { ST|AL,       RACE_A, 1,  900,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 180,  50, 40,   0,  0,   0,   0, { N, N, N, N }, { N, N }, UT_A_WORKER, 0 },
    { ST|LD,       RACE_A, 1,  900,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 120,   0, 40,   0,  0,   0,   0, { N, N, N, N }, { UT_A_TOWNHALL, N }, UT_A_WORKER, 0 },
    { ST,          RACE_A, 1, 1200,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 140,  60, 45,   0,  0,   0,   0, { N, N, N, N }, { UT_A_TOWNHALL, N }, UT_A_WORKER, 0 },
    { ST,          RACE_A, 1, 1050,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 150, 140, 50,   0,  0,   0,   0, { N, N, N, N }, { UT_A_BARRACKS, UT_A_LUMBERMILL }, UT_A_WORKER, 0 },
    { ST,          RACE_A, 1, 1200,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 140, 140, 50,   0,  0,   0,   0, { N, N, N, N }, { UT_A_BLACKSMITH, N }, UT_A_WORKER, 0 },
    { ST|AG|AA|RG, RACE_A, 1,  500,   0,  50, DEF_FORTIFIED, ATK_PIERCE, WPN_MISSILE,   1, 10, 2,  24,  900, 300, 300,  700, 1800,   0,   0,   0, 32, 1200, 1200,100,  50, 35,   0,  0,   0,   0, { N, N, N, N }, { UT_A_LUMBERMILL, N }, UT_A_WORKER, 0 },

    // ---- race B ----
    { WK|AG,       RACE_B, 1,  250,   0,   0, DEF_MEDIUM,    ATK_NORMAL, WPN_INSTANT,   1,  2, 0,   7, 2000, 433, 567,   90,    0,   0, 190, 500, 16,  800, 600,  75,   0, 15,   1,  5,  25,   0, { AB_HARVEST, AB_BUILD, N, N }, { N, N }, UT_B_GREATHALL, 0 },
    { AG,          RACE_B, 3,  700,   0,  10, DEF_HEAVY,     ATK_NORMAL, WPN_INSTANT,   1,  3, 0,  18, 1600, 330, 670,  100,    0,   0, 270, 600, 24, 1400, 800, 200,   0, 30,   3, 20,  25,   0, { N, N, N, N }, { N, N }, UT_B_BARRACKS, 1 },
    { AG|AA|RG,    RACE_B, 2,  350,   0,   0, DEF_MEDIUM,    ATK_PIERCE, WPN_MISSILE,   1,  4, 0,  22, 2310, 310, 690,  450, 1500,   0, 270, 600, 16, 1400, 800, 135,  20, 20,   2, 15,  25,   0, { N, N, N, N }, { UT_B_WARMILL, N }, UT_B_BARRACKS, 2 },
    { AG,          RACE_B, 3,  610,   0,  10, DEF_MEDIUM,    ATK_SIEGE,  WPN_INSTANT,   1,  5, 0,  19, 1850, 500, 500,  100,    0,   0, 350, 700, 24, 1400, 800, 180,  40, 28,   3, 20,  25,   0, { N, N, N, N }, { N, N }, UT_B_BESTIARY, 1 },
    { AG|AA|RG|CS, RACE_B, 2,  335, 200,   0, DEF_UNARMORED, ATK_MAGIC,  WPN_MISSILE,   1,  2, 0,   7, 1800, 700, 510,  600,  900,   0, 270, 600, 16, 1400, 800, 130,  20, 30,   2, 12,  25,  67, { AB_BLOODLUST, N, N, N }, { N, N }, UT_B_LODGE, 3 },
    { AG|AA|RG|CS, RACE_B, 2,  315, 200,   0, DEF_UNARMORED, ATK_MAGIC,  WPN_MISSILE,   1,  2, 0,   8, 1750, 530, 470,  600,  900,   0, 270, 600, 16, 1400, 800, 145,  25, 30,   2, 12,  25,  67, { AB_REJUV, N, N, N }, { N, N }, UT_B_LODGE, 3 },
    { AG|RG|MC,    RACE_B, 3,  700,   0,  20, DEF_HEAVY,     ATK_SIEGE,  WPN_ARTILLERY, 1, 15, 0,  83, 4500,1000, 500, 1150,  900, 125, 220, 300, 32, 1400, 800, 220,  50, 45,   3, 20,   0,   0, { N, N, N, N }, { UT_B_WARMILL, N }, UT_B_BARRACKS, 2 },
    { FL|AG|AA|RG, RACE_B, 4,  570,   0,  10, DEF_LIGHT,     ATK_PIERCE, WPN_MISSILE,   1,  8, 0,  34, 1900, 530, 470,  300, 1200,   0, 320, 700, 32, 1400, 800, 265,  40, 38,   4, 25,  25,   0, { AB_ENVENOM, N, N, N }, { N, N }, UT_B_BESTIARY, 2 },
    { HR|AG,       RACE_B, 1,  100,   0,  10, DEF_HERO,      ATK_HERO,   WPN_INSTANT,   2,  6, 0,   1, 1770, 330, 670,  100,    0,   0, 320, 600, 32, 1800, 800, 425, 100, 55,   5,  0,  25,   0, { N, N, N, N }, { N, N }, UT_B_ALTAR, 0 },
    { HR|AG|AA|RG, RACE_B, 1,  100,   0,   0, DEF_HERO,      ATK_HERO,   WPN_MISSILE,   2,  4, 0,   2, 2130, 300, 700,  600, 1200,   0, 320, 600, 32, 1800, 800, 425, 100, 55,   5,  0,  25,   0, { N, N, N, N }, { N, N }, UT_B_ALTAR, 0 },
    { HR|AG,       RACE_B, 1,  100,   0,  10, DEF_HERO,      ATK_HERO,   WPN_INSTANT,   2,  8, 0,   2, 2200, 360, 640,  100,    0,   0, 270, 500, 40, 1800, 800, 425, 100, 55,   5,  0,  25,   0, { N, N, N, N }, { N, N }, UT_B_ALTAR, 0 },
    { ST|TH|LD,    RACE_B, 1, 1500,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 4,   0,    0,   0,   0,    0,    0,   0,   0,   0, 64,  900, 900, 385, 185, 90, -11,  0,   0,   0, { N, N, N, N }, { N, N }, UT_B_WORKER, 0 },
    { ST|AG|AA|RG, RACE_B, 1,  600,   0,  50, DEF_FORTIFIED, ATK_PIERCE, WPN_MISSILE,   1,  4, 2,  12, 1500, 300, 300,  650, 1500,   0,   0,   0, 32,  900, 900, 160,  40, 30, -10,  0,   0,   0, { N, N, N, N }, { N, N }, UT_B_WORKER, 0 },
    { ST,          RACE_B, 1, 1200,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 180,  50, 40,   0,  0,   0,   0, { N, N, N, N }, { UT_B_GREATHALL, N }, UT_B_WORKER, 0 },
    { ST|AL,       RACE_B, 1,  900,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 180,  50, 40,   0,  0,   0,   0, { N, N, N, N }, { N, N }, UT_B_WORKER, 0 },
    { ST|LD,       RACE_B, 1, 1000,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 205,   0, 45,   0,  0,   0,   0, { N, N, N, N }, { UT_B_GREATHALL, N }, UT_B_WORKER, 0 },
    { ST,          RACE_B, 1,  800,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 150, 150, 50,   0,  0,   0,   0, { N, N, N, N }, { UT_B_BARRACKS, UT_B_WARMILL }, UT_B_WORKER, 0 },
    { ST,          RACE_B, 1, 1100,   0,  50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,      0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,  900, 900, 145, 140, 50,   0,  0,   0,   0, { N, N, N, N }, { UT_B_WARMILL, N }, UT_B_WORKER, 0 },
    { ST|AG|AA|RG, RACE_B, 1,  500,   0,  50, DEF_FORTIFIED, ATK_PIERCE, WPN_MISSILE,   1, 10, 2,  22, 1000, 300, 300,  700, 1800,   0,   0,   0, 32, 1200, 1200,110,  80, 35,   0,  0,   0,   0, { N, N, N, N }, { UT_B_WARMILL, N }, UT_B_WORKER, 0 },

    // ---- neutral ----
    { ST|GM,       RACE_NEUTRAL, 1, 2000, 0, 50, DEF_FORTIFIED, ATK_NORMAL, WPN_NONE,   0,  0, 3,   0,    0,   0,   0,    0,    0,   0,   0,   0, 48,    0,   0,   0,   0,  0,   0,  0,   0,   0, { N, N, N, N }, { N, N }, N, 0 },
    { CR|AG,       RACE_NEUTRAL, 1,  240, 0,  0, DEF_MEDIUM,    ATK_NORMAL, WPN_INSTANT, 1,  2, 0,   8, 1350, 400, 600,   90,    0,   0, 270, 600, 16, 1000, 600,   0,   0,  0,   0, 15,  25,   0, { N, N, N, N }, { N, N }, N, 0 },
    { CR|AG|AA|RG, RACE_NEUTRAL, 1,  200, 0,  0, DEF_MEDIUM,    ATK_PIERCE, WPN_MISSILE, 1,  3, 0,  10, 1800, 400, 600,  500, 1500,   0, 270, 600, 16, 1000, 600,   0,   0,  0,   0, 15,  25,   0, { N, N, N, N }, { N, N }, N, 0 },
    { CR|AG|AA|RG, RACE_NEUTRAL, 2,  350, 0,  0, DEF_MEDIUM,    ATK_PIERCE, WPN_MISSILE, 1,  4, 0,  14, 1800, 400, 600,  500, 1500,   0, 270, 600, 16, 1000, 600,   0,   0,  0,   0, 20,  25,   0, { N, N, N, N }, { N, N }, N, 0 },
    { CR|AG,       RACE_NEUTRAL, 3,  550, 0, 30, DEF_HEAVY,     ATK_NORMAL, WPN_INSTANT, 1,  6, 0,  20, 1600, 400, 600,  100,    0,   0, 270, 600, 24, 1000, 600,   0,   0,  0,   0, 25,  25,   0, { N, N, N, N }, { N, N }, N, 0 },
    { CR|AG,       RACE_NEUTRAL, 4,  800, 0, 30, DEF_HEAVY,     ATK_NORMAL, WPN_INSTANT, 2,  5, 0,  28, 1800, 500, 500,  100,    0,   0, 270, 500, 32, 1000, 600,   0,   0,  0,   0, 35,  25,   0, { AB_BASH, N, N, N }, { N, N }, N, 0 },
    { CR|AG|AA|RG|CS, RACE_NEUTRAL, 3, 400, 250, 0, DEF_UNARMORED, ATK_MAGIC, WPN_MISSILE, 1, 3, 0, 12, 2000, 500, 500,  500,  900,   0, 270, 600, 16, 1000, 600,   0,   0,  0,   0, 25,  25,  80, { AB_HEAL, N, N, N }, { N, N }, N, 0 },
    { CR|FL|AG|AA|RG, RACE_NEUTRAL, 6, 1300, 0, 40, DEF_LIGHT, ATK_MAGIC, WPN_MISSILE_SPLASH, 2, 10, 0, 50, 2500, 600, 400, 500, 1000, 125, 300, 500, 40, 1200, 800, 0, 0, 0,   0, 60, 100,   0, { N, N, N, N }, { N, N }, N, 0 },
    { SM|AG|AA|RG, RACE_NEUTRAL, 2,  525, 0, 10, DEF_HEAVY,     ATK_PIERCE, WPN_MISSILE, 1,  9, 0,  19, 1500, 300, 700,  300, 1000,   0, 220, 500, 24,  800, 600,   0,   0,  0,   0, 10,   0,   0, { N, N, N, N }, { N, N }, N, 0 },
    { SM|AG,       RACE_NEUTRAL, 1,  250, 0,  0, DEF_MEDIUM,    ATK_NORMAL, WPN_INSTANT, 1,  3, 0,  10, 1350, 330, 670,   90,    0,   0, 350, 700, 16, 1200, 800,   0,   0,  0,   0, 10,   0,   0, { N, N, N, N }, { N, N }, N, 0 },
};

// unitType, primary, abilities, base str/agi/int, per-level str/agi/int
const HeroTypeData g_heroTypes[6] =
{
    { UT_A_HERO_PALADIN,     ATTR_STR, { AB_HOLYLIGHT, AB_DIVINESHIELD, AB_DEVOTION },  22.0f, 13.0f, 17.0f, 2.7f, 1.5f, 1.8f },
    { UT_A_HERO_ARCHMAGE,    ATTR_INT, { AB_BLIZZARD, AB_ELEMENTAL, AB_BRILLIANCE },    14.0f, 17.0f, 19.0f, 1.8f, 1.0f, 3.2f },
    { UT_A_HERO_MOUNTAIN,    ATTR_STR, { AB_STORMBOLT, AB_THUNDERCLAP, AB_BASH },       24.0f, 11.0f, 15.0f, 3.0f, 1.5f, 1.5f },
    { UT_B_HERO_BLADEMASTER, ATTR_AGI, { AB_WINDWALK, AB_CRITICAL, AB_EVASION },        18.0f, 23.0f, 16.0f, 2.0f, 1.75f, 1.5f },
    { UT_B_HERO_FARSEER,     ATTR_INT, { AB_CHAINLIGHTNING, AB_FERALSPIRIT, AB_BRILLIANCE }, 15.0f, 18.0f, 19.0f, 2.0f, 1.0f, 3.0f },
    { UT_B_HERO_CHIEFTAIN,   ATTR_STR, { AB_SHOCKWAVE, AB_WARSTOMP, AB_ENDURANCE },     25.0f, 10.0f, 14.0f, 3.2f, 1.5f, 1.3f },
};

#define HF  ABF_HERO
#define PS  ABF_PASSIVE
#define AC  ABF_AUTOCAST
#define TU  ABF_TARGET_UNIT
#define TP  ABF_TARGET_POINT
#define CH  ABF_CHANNEL
#define FR  ABF_FRIENDLY
#define NT  ABF_NO_TARGET

// class, flags, buff, summon, mana[3], cooldown[3], range, area[3], valueA[3],
// valueB[3], duration[3], heroDuration[3]
const AbilityTypeData g_abilityTypes[AB_COUNT] =
{
    { ABC_HARVEST,      TU,          N,               N, {   0,   0,   0 }, { 0.0f, 0.0f, 0.0f },    90.0f, {   0.0f,   0.0f,   0.0f }, { 10.0f, 10.0f,  1.0f }, { 1.0f, 0.9f, 0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_BUILD,        TP,          N,               N, {   0,   0,   0 }, { 0.0f, 0.0f, 0.0f },    64.0f, {   0.0f,   0.0f,   0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_MISSILE_STUN, HF|TU,       BF_STUN,         N, {  75,  75,  75 }, { 9.0f, 9.0f, 9.0f },   600.0f, {   0.0f,   0.0f,   0.0f }, { 100.0f, 225.0f, 350.0f }, { 1000.0f, 1000.0f, 1000.0f }, {  5.0f,  5.0f,  5.0f }, { 3.0f, 3.0f, 3.0f } },
    { ABC_NOVA,         HF|NT,       BF_THUNDERSLOW,  N, {  90,  90,  90 }, { 6.0f, 6.0f, 6.0f },     0.0f, { 250.0f, 300.0f, 350.0f }, {  60.0f, 100.0f, 140.0f }, { 0.0f, 0.0f, 0.0f }, {  5.0f,  5.0f,  5.0f }, { 3.0f, 3.0f, 3.0f } },
    { ABC_BASH,         HF|PS,       BF_STUN,         N, {   0,   0,   0 }, { 0.0f, 0.0f, 0.0f },     0.0f, {   0.0f,   0.0f,   0.0f }, {  0.2f,  0.3f,  0.4f }, { 25.0f, 25.0f, 25.0f }, {  2.0f,  2.0f,  2.0f }, { 1.0f, 1.0f, 1.0f } },
    { ABC_TARGET_HEAL,  HF|TU|FR,    N,               N, {  65,  65,  65 }, { 5.0f, 5.0f, 5.0f },   800.0f, {   0.0f,   0.0f,   0.0f }, { 200.0f, 400.0f, 600.0f }, { 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_SELF_BUFF,    HF|NT,       BF_DIVINESHIELD, N, {  25,  25,  25 }, { 35.0f, 50.0f, 65.0f }, 0.0f, {   0.0f,   0.0f,   0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f }, { 15.0f, 30.0f, 45.0f }, { 15.0f, 30.0f, 45.0f } },
    { ABC_AURA,         HF|PS|FR,    BF_DEVOTION,     N, {   0,   0,   0 }, { 0.0f, 0.0f, 0.0f },     0.0f, { 900.0f, 900.0f, 900.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_CHANNEL_AREA, HF|TP|CH,    N,               N, {  75,  75,  75 }, { 6.0f, 6.0f, 6.0f },   800.0f, { 200.0f, 200.0f, 200.0f }, {  30.0f,  40.0f,  50.0f }, { 6.0f, 8.0f, 10.0f }, {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f } },
    { ABC_SUMMON,       HF|NT,       BF_TIMEDLIFE,    UT_SUMMON_ELEMENTAL, { 125, 125, 125 }, { 20.0f, 20.0f, 20.0f }, 0.0f, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.35f, 1.7f }, { 60.0f, 60.0f, 60.0f }, { 60.0f, 60.0f, 60.0f } },
    { ABC_AURA,         HF|PS|FR,    BF_BRILLIANCE,   N, {   0,   0,   0 }, { 0.0f, 0.0f, 0.0f },     0.0f, { 900.0f, 900.0f, 900.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_SELF_BUFF,    HF|NT,       BF_WINDWALK,     N, {  75,  75,  75 }, { 5.0f, 5.0f, 5.0f },     0.0f, {   0.0f,   0.0f,   0.0f }, {  40.0f,  70.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 20.0f, 30.0f, 40.0f }, { 20.0f, 30.0f, 40.0f } },
    { ABC_CRITICAL,     HF|PS,       N,               N, {   0,   0,   0 }, { 0.0f, 0.0f, 0.0f },     0.0f, {   0.0f,   0.0f,   0.0f }, {  0.15f, 0.15f, 0.15f }, { 2.0f, 3.0f, 4.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_EVASION,      HF|PS,       N,               N, {   0,   0,   0 }, { 0.0f, 0.0f, 0.0f },     0.0f, {   0.0f,   0.0f,   0.0f }, {  0.15f, 0.25f, 0.35f }, { 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_BOUNCE,       HF|TU,       N,               N, { 120, 125, 130 }, { 9.0f, 9.0f, 9.0f },   700.0f, { 500.0f, 500.0f, 500.0f }, {  85.0f, 125.0f, 180.0f }, { 4.0f, 6.0f, 8.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_SUMMON,       HF|NT,       BF_TIMEDLIFE,    UT_SUMMON_WOLF, {  75,  75,  75 }, { 15.0f, 15.0f, 15.0f }, 0.0f, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f }, { 1.0f, 1.3f, 1.6f }, { 60.0f, 60.0f, 60.0f }, { 60.0f, 60.0f, 60.0f } },
    { ABC_LINE,         HF|TP,       N,               N, { 100, 100, 100 }, { 8.0f, 8.0f, 8.0f },   800.0f, { 125.0f, 125.0f, 125.0f }, {  75.0f, 130.0f, 200.0f }, { 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_NOVA,         HF|NT,       BF_STUN,         N, {  90,  90,  90 }, { 6.0f, 6.0f, 6.0f },     0.0f, { 300.0f, 300.0f, 300.0f }, {  25.0f,  50.0f,  75.0f }, { 0.0f, 0.0f, 0.0f }, {  3.0f,  4.0f,  5.0f }, { 2.0f, 2.5f, 3.0f } },
    { ABC_AURA,         HF|PS|FR,    BF_ENDURANCE,    N, {   0,   0,   0 }, { 0.0f, 0.0f, 0.0f },     0.0f, { 900.0f, 900.0f, 900.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_TARGET_HEAL,  TU|FR|AC,    N,               N, {   5,   5,   5 }, { 1.0f, 1.0f, 1.0f },   450.0f, {   0.0f,   0.0f,   0.0f }, {  25.0f,  25.0f,  25.0f }, { 0.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  0.0f }, { 0.0f, 0.0f, 0.0f } },
    { ABC_TARGET_BUFF,  TU|AC,       BF_SLOW,         N, {  50,  50,  50 }, { 1.0f, 1.0f, 1.0f },   700.0f, {   0.0f,   0.0f,   0.0f }, {   0.0f,   0.0f,   0.0f }, { 0.0f, 0.0f, 0.0f }, { 60.0f, 60.0f, 60.0f }, { 10.0f, 10.0f, 10.0f } },
    { ABC_TARGET_BUFF,  TU|FR|AC,    BF_BLOODLUST,    N, {  40,  40,  40 }, { 2.0f, 2.0f, 2.0f },   600.0f, {   0.0f,   0.0f,   0.0f }, {   0.0f,   0.0f,   0.0f }, { 0.0f, 0.0f, 0.0f }, { 60.0f, 60.0f, 60.0f }, { 60.0f, 60.0f, 60.0f } },
    { ABC_TARGET_BUFF,  TU|FR|AC,    BF_REJUV,        N, {  75,  75,  75 }, { 3.0f, 3.0f, 3.0f },   400.0f, {   0.0f,   0.0f,   0.0f }, {   0.0f,   0.0f,   0.0f }, { 0.0f, 0.0f, 0.0f }, { 12.0f, 12.0f, 12.0f }, { 12.0f, 12.0f, 12.0f } },
    { ABC_ENVENOM,      PS,          BF_POISON,       N, {   0,   0,   0 }, { 0.0f, 0.0f, 0.0f },     0.0f, {   0.0f,   0.0f,   0.0f }, {   0.0f,   0.0f,   0.0f }, { 0.0f, 0.0f, 0.0f }, {  5.0f,  5.0f,  5.0f }, { 5.0f, 5.0f, 5.0f } },
};

#define POS BFF_POSITIVE
#define AUR BFF_AURA
#define DSP BFF_DISPELLABLE
#define DIS BFF_DISABLE

// class, flags, stackGroup, pad, armorAdd, attackSpeedMul, moveSpeedMul,
// damageMul, manaRegenAdd, periodicAmount
const BuffTypeData g_buffTypes[BF_COUNT] =
{
    { BFC_STUN,         DIS,         0, 0, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
    { BFC_STATMOD,      DSP,         1, 0, { 0.0f, 0.0f, 0.0f }, { 0.75f, 0.75f, 0.75f }, { 0.4f, 0.4f, 0.4f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
    { BFC_STATMOD,      DSP,         1, 0, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },    { 0.5f, 0.5f, 0.5f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
    { BFC_STATMOD,      POS|AUR,     2, 0, { 1.5f, 3.0f, 4.5f }, { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
    { BFC_STATMOD,      POS|AUR,     3, 0, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f }, { 0.75f, 1.5f, 2.25f }, { 0.0f, 0.0f, 0.0f } },
    { BFC_STATMOD,      POS|AUR,     4, 0, { 0.0f, 0.0f, 0.0f }, { 1.1f, 1.2f, 1.3f },    { 1.1f, 1.2f, 1.3f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
    { BFC_INVULNERABLE, POS,         5, 0, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
    { BFC_INVISIBLE,    POS,         6, 0, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },    { 1.1f, 1.4f, 1.7f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
    { BFC_STATMOD,      POS|DSP,     7, 0, { 0.0f, 0.0f, 0.0f }, { 1.4f, 1.4f, 1.4f },    { 1.25f, 1.25f, 1.25f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
    { BFC_PERIODIC,     POS|DSP,     8, 0, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 33.0f, 33.0f, 33.0f } },
    { BFC_PERIODIC,     DSP,         9, 0, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },    { 0.75f, 0.75f, 0.75f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { -10.0f, -10.0f, -10.0f } },
    { BFC_TIMEDLIFE,    0,          10, 0, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
    { BFC_SLEEP,        DIS,        11, 0, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f },    { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f } },
};

// class, level, charges, attr, str, agi, int, damage, armorX10, life, move,
// attackSpeedPct, useAmount, useArea
const ItemTypeData g_itemTypes[IT_COUNT] =
{
    { ITC_CHARGED,   1, 1, 0, 0, 0, 0, 0,  0,   0,  0,  0, 250,   0 },
    { ITC_CHARGED,   1, 1, 0, 0, 0, 0, 0,  0,   0,  0,  0, 150,   0 },
    { ITC_PERMANENT, 2, 0, 0, 0, 0, 0, 6,  0,   0,  0,  0,   0,   0 },
    { ITC_PERMANENT, 2, 0, 0, 0, 0, 0, 0, 20,   0,  0,  0,   0,   0 },
    { ITC_PERMANENT, 2, 0, 0, 0, 0, 0, 0,  0,   0, 50,  0,   0,   0 },
    { ITC_PERMANENT, 3, 0, 0, 0, 0, 0, 0,  0,   0,  0, 15,   0,   0 },
    { ITC_PERMANENT, 3, 0, 0, 6, 0, 0, 0,  0,   0,  0,  0,   0,   0 },
    { ITC_PERMANENT, 3, 0, 2, 0, 0, 6, 0,  0,   0,  0,  0,   0,   0 },
    { ITC_PERMANENT, 3, 0, 1, 0, 6, 0, 0,  0,   0,  0,  0,   0,   0 },
    { ITC_POWERUP,   2, 1, 0, 0, 0, 0, 0,  0,   0,  0,  0, 100,   0 },
    { ITC_POWERUP,   4, 1, 0, 0, 0, 0, 0,  0,   0,  0,  0,   1,   0 },
    { ITC_CHARGED,   4, 2, 0, 0, 0, 0, 0,  0,   0,  0,  0, 150, 600 },
    { ITC_PERMANENT, 4, 0, 0, 0, 0, 0, 0,  0, 150,  0,  0,   0,   0 },
    { ITC_POWERUP,   1, 1, 0, 0, 0, 0, 0,  0,   0,  0,  0, 100, 600 },
};

// race, effect, levels, prereqs, gold, goldPerLevel, lumber, lumberPerLevel,
// time, researchedAt
const UpgradeData g_upgrades[UP_COUNT] =
{
    { RACE_A, UPE_MELEE_DAMAGE,    3, N, 100, 75,  50, 125, 60, UT_A_BLACKSMITH, 0 },
    { RACE_A, UPE_RANGED_DAMAGE,   3, N, 100, 75,  50, 125, 60, UT_A_BLACKSMITH, 0 },
    { RACE_A, UPE_ARMOR,           3, N, 150, 75,  75, 125, 60, UT_A_LUMBERMILL, 0 },
    { RACE_A, UPE_CASTER_TRAINING, 2, N, 100, 75,  50,  75, 45, UT_A_SANCTUM,    0 },
    { RACE_B, UPE_MELEE_DAMAGE,    3, N, 100, 75, 100, 100, 60, UT_B_WARMILL,    0 },
    { RACE_B, UPE_RANGED_DAMAGE,   3, N, 100, 75, 100, 100, 60, UT_B_WARMILL,    0 },
    { RACE_B, UPE_ARMOR,           3, N, 150, 75, 150,  75, 60, UT_B_WARMILL,    0 },
    { RACE_B, UPE_CASTER_TRAINING, 2, N, 100, 75,  50,  75, 45, UT_B_LODGE,      0 },
};

const float g_damageTable[ATK_COUNT][DEF_COUNT] =
{
    { 1.00f, 1.50f, 1.00f, 0.70f, 1.00f, 1.00f, 0.05f, 1.00f },
    { 2.00f, 0.75f, 1.00f, 0.35f, 1.00f, 0.50f, 0.05f, 1.50f },
    { 1.00f, 0.50f, 1.00f, 1.50f, 1.00f, 0.50f, 0.05f, 1.50f },
    { 1.25f, 0.75f, 2.00f, 0.35f, 1.00f, 0.50f, 0.05f, 1.00f },
    { 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f },
    { 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 0.70f, 0.05f, 1.00f },
    { 1.00f, 1.00f, 1.00f, 0.50f, 1.00f, 1.00f, 0.05f, 1.00f },
};

const u16 g_heroXpNeeded[kMaxHeroLevel + 1] = { 0, 0, 200, 500, 900, 1400, 2000, 2700, 3500, 4400, 5400 };
const u16 g_creepXpByLevel[11]             = { 0, 25, 40, 60, 85, 115, 150, 190, 235, 285, 340 };
const u16 g_heroKillXpByLevel[11]          = { 0, 100, 120, 160, 220, 300, 400, 500, 600, 700, 800 };
const u8  g_creepXpFactorByHeroLevel[kMaxHeroLevel + 1] = { 100, 80, 70, 60, 50, 0, 0, 0, 0, 0, 0 };

// op, type, count, condition (minimum army food for AIS_ATTACK, loop target
// index for AIS_LOOP)
const AiBuildStep g_aiBuildOrderA[] =
{
    { AIS_TRAIN,    UT_A_WORKER,        5, 0 },
    { AIS_BUILD,    UT_A_FARM,          1, 0 },
    { AIS_TRAIN,    UT_A_WORKER,        7, 0 },
    { AIS_BUILD,    UT_A_BARRACKS,      1, 0 },
    { AIS_BUILD,    UT_A_ALTAR,         1, 0 },
    { AIS_TRAIN,    UT_A_WORKER,        9, 0 },
    { AIS_BUILD,    UT_A_FARM,          2, 0 },
    { AIS_HERO,     UT_A_HERO_MOUNTAIN, 1, 0 },
    { AIS_TRAIN,    UT_A_MELEE,         2, 0 },
    { AIS_BUILD,    UT_A_FARM,          3, 0 },
    { AIS_TRAIN,    UT_A_WORKER,       12, 0 },
    { AIS_BUILD,    UT_A_LUMBERMILL,    1, 0 },
    { AIS_TRAIN,    UT_A_MELEE,         4, 0 },
    { AIS_BUILD,    UT_A_BLACKSMITH,    1, 0 },
    { AIS_BUILD,    UT_A_FARM,          4, 0 },
    { AIS_TRAIN,    UT_A_RANGED,        3, 0 },
    { AIS_ATTACK,   0,                  0, 14 },
    { AIS_RESEARCH, UP_A_RANGED,        1, 0 },
    { AIS_BUILD,    UT_A_SANCTUM,       1, 0 },
    { AIS_BUILD,    UT_A_FARM,          6, 0 },
    { AIS_HERO,     UT_A_HERO_PALADIN,  2, 0 },
    { AIS_TRAIN,    UT_A_PRIEST,        2, 0 },
    { AIS_TRAIN,    UT_A_RANGED,        6, 0 },
    { AIS_BUILD,    UT_A_TOWER,         1, 0 },
    { AIS_TRAIN,    UT_A_SORCERESS,     2, 0 },
    { AIS_RESEARCH, UP_A_ARMOR,         1, 0 },
    { AIS_EXPAND,   UT_A_TOWNHALL,      2, 0 },
    { AIS_BUILD,    UT_A_WORKSHOP,      1, 0 },
    { AIS_BUILD,    UT_A_FARM,          8, 0 },
    { AIS_TRAIN,    UT_A_KNIGHT,        3, 0 },
    { AIS_TRAIN,    UT_A_SIEGE,         2, 0 },
    { AIS_ATTACK,   0,                  0, 30 },
    { AIS_RESEARCH, UP_A_MELEE,         1, 0 },
    { AIS_HERO,     UT_A_HERO_ARCHMAGE, 3, 0 },
    { AIS_TRAIN,    UT_A_KNIGHT,        6, 0 },
    { AIS_TRAIN,    UT_A_FLYER,         2, 0 },
    { AIS_BUILD,    UT_A_FARM,         11, 0 },
    { AIS_RESEARCH, UP_A_RANGED,        2, 0 },
    { AIS_RESEARCH, UP_A_CASTER,        1, 0 },
    { AIS_ATTACK,   0,                  0, 40 },
    { AIS_TRAIN,    UT_A_RANGED,       10, 0 },
    { AIS_RESEARCH, UP_A_MELEE,         3, 0 },
    { AIS_ATTACK,   0,                  0, 40 },
    { AIS_LOOP,     0,                  0, 39 },
};
const int g_aiBuildOrderLenA = COUNTOF(g_aiBuildOrderA);

const AiBuildStep g_aiBuildOrderB[] =
{
    { AIS_TRAIN,    UT_B_WORKER,          5, 0 },
    { AIS_BUILD,    UT_B_BURROW,          1, 0 },
    { AIS_TRAIN,    UT_B_WORKER,          7, 0 },
    { AIS_BUILD,    UT_B_BARRACKS,        1, 0 },
    { AIS_BUILD,    UT_B_ALTAR,           1, 0 },
    { AIS_TRAIN,    UT_B_WORKER,          9, 0 },
    { AIS_HERO,     UT_B_HERO_BLADEMASTER,1, 0 },
    { AIS_BUILD,    UT_B_BURROW,          2, 0 },
    { AIS_TRAIN,    UT_B_GRUNT,           2, 0 },
    { AIS_BUILD,    UT_B_WARMILL,         1, 0 },
    { AIS_TRAIN,    UT_B_WORKER,         12, 0 },
    { AIS_TRAIN,    UT_B_GRUNT,           4, 0 },
    { AIS_BUILD,    UT_B_BURROW,          3, 0 },
    { AIS_TRAIN,    UT_B_HEADHUNTER,      2, 0 },
    { AIS_ATTACK,   0,                    0, 14 },
    { AIS_RESEARCH, UP_B_MELEE,           1, 0 },
    { AIS_BUILD,    UT_B_LODGE,           1, 0 },
    { AIS_BUILD,    UT_B_BURROW,          5, 0 },
    { AIS_HERO,     UT_B_HERO_FARSEER,    2, 0 },
    { AIS_TRAIN,    UT_B_SHAMAN,          2, 0 },
    { AIS_TRAIN,    UT_B_GRUNT,           6, 0 },
    { AIS_BUILD,    UT_B_WATCHTOWER,      1, 0 },
    { AIS_TRAIN,    UT_B_DOCTOR,          2, 0 },
    { AIS_RESEARCH, UP_B_ARMOR,           1, 0 },
    { AIS_EXPAND,   UT_B_GREATHALL,       2, 0 },
    { AIS_BUILD,    UT_B_BESTIARY,        1, 0 },
    { AIS_BUILD,    UT_B_BURROW,          7, 0 },
    { AIS_TRAIN,    UT_B_RAIDER,          4, 0 },
    { AIS_TRAIN,    UT_B_CATAPULT,        2, 0 },
    { AIS_ATTACK,   0,                    0, 30 },
    { AIS_RESEARCH, UP_B_RANGED,          1, 0 },
    { AIS_HERO,     UT_B_HERO_CHIEFTAIN,  3, 0 },
    { AIS_TRAIN,    UT_B_WYVERN,          3, 0 },
    { AIS_TRAIN,    UT_B_GRUNT,           9, 0 },
    { AIS_BUILD,    UT_B_BURROW,          9, 0 },
    { AIS_RESEARCH, UP_B_MELEE,           2, 0 },
    { AIS_RESEARCH, UP_B_CASTER,          1, 0 },
    { AIS_ATTACK,   0,                    0, 40 },
    { AIS_TRAIN,    UT_B_HEADHUNTER,      8, 0 },
    { AIS_RESEARCH, UP_B_MELEE,           3, 0 },
    { AIS_ATTACK,   0,                    0, 40 },
    { AIS_LOOP,     0,                    0, 37 },
};
const int g_aiBuildOrderLenB = COUNTOF(g_aiBuildOrderB);

// Ability slot (0..2) learned at hero levels 1..10, per hero table entry.
const u8 g_aiSkillOrder[6][10] =
{
    { 0, 2, 0, 1, 0, 2, 2, 1, 1, 0 },
    { 0, 1, 1, 2, 1, 0, 0, 2, 2, 0 },
    { 0, 1, 0, 2, 0, 1, 1, 2, 2, 0 },
    { 1, 0, 1, 2, 1, 2, 2, 0, 0, 0 },
    { 0, 1, 0, 1, 0, 1, 2, 2, 2, 0 },
    { 1, 0, 1, 2, 1, 0, 0, 2, 2, 0 },
};

// level, count, unit types
const CreepCampTemplate g_creepCamps[8] =
{
    { 1, 2, { UT_CREEP_GNOLL, UT_CREEP_ARCHER, N, N } },
    { 2, 3, { UT_CREEP_GNOLL, UT_CREEP_GNOLL, UT_CREEP_ARCHER, N } },
    { 3, 2, { UT_CREEP_TROLL, UT_CREEP_BRUTE, N, N } },
    { 4, 3, { UT_CREEP_TROLL, UT_CREEP_TROLL, UT_CREEP_SHAMAN, N } },
    { 5, 3, { UT_CREEP_BRUTE, UT_CREEP_OGRE, UT_CREEP_SHAMAN, N } },
    { 6, 2, { UT_CREEP_OGRE, UT_CREEP_OGRE, N, N } },
    { 8, 3, { UT_CREEP_OGRE, UT_CREEP_SHAMAN, UT_CREEP_DRAGON, N } },
    { 3, 3, { UT_CREEP_ARCHER, UT_CREEP_GNOLL, UT_CREEP_TROLL, N } },
};

// Item types that can drop from a creep of a given item level (1..6).
const u8 g_itemDropTable[6][4] =
{
    { IT_HEAL_POTION, IT_MANA_POTION, IT_RUNE_MANA, IT_TOME_XP },
    { IT_CLAWS, IT_RING, IT_BOOTS, IT_TOME_XP },
    { IT_GLOVES, IT_BELT, IT_CIRCLET, IT_SLIPPERS },
    { IT_SCROLL_HEAL, IT_PERIAPT, IT_TOME_STR, IT_GLOVES },
    { IT_PERIAPT, IT_TOME_STR, IT_SCROLL_HEAL, IT_BELT },
    { IT_PERIAPT, IT_TOME_STR, IT_SCROLL_HEAL, IT_CLAWS },
};

const UnitTypeData* GetUnitType(int type)
{
    if (type < 0 || type >= UT_COUNT)
        return &g_unitTypes[0];
    return &g_unitTypes[type];
}

const HeroTypeData* GetHeroType(int unitType)
{
    for (int i = 0; i < (int)COUNTOF(g_heroTypes); ++i)
    {
        if (g_heroTypes[i].unitType == unitType)
            return &g_heroTypes[i];
    }
    return 0;
}

const AbilityTypeData* GetAbilityType(int ability)
{
    return &g_abilityTypes[ability < AB_COUNT ? ability : 0];
}

const BuffTypeData* GetBuffType(int buff)
{
    return &g_buffTypes[buff < BF_COUNT ? buff : 0];
}

const ItemTypeData* GetItemType(int item)
{
    return &g_itemTypes[item < IT_COUNT ? item : 0];
}

int WorkerTypeForRace(int race)
{
    return race == RACE_A ? UT_A_WORKER : UT_B_WORKER;
}

int TownHallTypeForRace(int race)
{
    return race == RACE_A ? UT_A_TOWNHALL : UT_B_GREATHALL;
}

int FarmTypeForRace(int race)
{
    return race == RACE_A ? UT_A_FARM : UT_B_BURROW;
}


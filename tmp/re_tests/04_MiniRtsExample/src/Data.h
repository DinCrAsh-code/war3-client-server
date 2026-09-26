// Static game data: unit/ability/buff/item/upgrade tables and the enums
// that index them. The tables themselves live in Data.cpp.

#ifndef MR_DATA_H
#define MR_DATA_H

#include "Base.h"

enum Race
{
    RACE_A = 0,
    RACE_B = 1,
    RACE_NEUTRAL = 2
};

enum AttackType
{
    ATK_NORMAL = 0,
    ATK_PIERCE,
    ATK_SIEGE,
    ATK_MAGIC,
    ATK_CHAOS,
    ATK_SPELL,
    ATK_HERO,
    ATK_COUNT
};

enum DefenseType
{
    DEF_LIGHT = 0,
    DEF_MEDIUM,
    DEF_HEAVY,
    DEF_FORTIFIED,
    DEF_NORMAL,
    DEF_HERO,
    DEF_DIVINE,
    DEF_UNARMORED,
    DEF_COUNT
};

enum WeaponType
{
    WPN_NONE = 0,
    WPN_INSTANT,
    WPN_MISSILE,
    WPN_ARTILLERY,
    WPN_MISSILE_SPLASH
};

enum UnitTypeFlags
{
    UTF_STRUCTURE    = 0x0001,
    UTF_HERO         = 0x0002,
    UTF_WORKER       = 0x0004,
    UTF_FLYING       = 0x0008,
    UTF_SUMMONED     = 0x0010,
    UTF_MECHANICAL   = 0x0020,
    UTF_TOWNHALL     = 0x0040,
    UTF_GOLDMINE     = 0x0080,
    UTF_LUMBERDROP   = 0x0100,
    UTF_CREEP        = 0x0200,
    UTF_ATTACKS_AIR  = 0x0400,
    UTF_ATTACKS_GND  = 0x0800,
    UTF_CASTER       = 0x1000,
    UTF_ALTAR        = 0x2000,
    UTF_RANGED       = 0x4000
};

enum UnitTypeId
{
    UT_A_WORKER = 0,
    UT_A_MELEE,
    UT_A_RANGED,
    UT_A_KNIGHT,
    UT_A_PRIEST,
    UT_A_SORCERESS,
    UT_A_SIEGE,
    UT_A_FLYER,
    UT_A_HERO_PALADIN,
    UT_A_HERO_ARCHMAGE,
    UT_A_HERO_MOUNTAIN,
    UT_A_TOWNHALL,
    UT_A_FARM,
    UT_A_BARRACKS,
    UT_A_ALTAR,
    UT_A_LUMBERMILL,
    UT_A_BLACKSMITH,
    UT_A_SANCTUM,
    UT_A_WORKSHOP,
    UT_A_TOWER,

    UT_B_WORKER,
    UT_B_GRUNT,
    UT_B_HEADHUNTER,
    UT_B_RAIDER,
    UT_B_SHAMAN,
    UT_B_DOCTOR,
    UT_B_CATAPULT,
    UT_B_WYVERN,
    UT_B_HERO_BLADEMASTER,
    UT_B_HERO_FARSEER,
    UT_B_HERO_CHIEFTAIN,
    UT_B_GREATHALL,
    UT_B_BURROW,
    UT_B_BARRACKS,
    UT_B_ALTAR,
    UT_B_WARMILL,
    UT_B_LODGE,
    UT_B_BESTIARY,
    UT_B_WATCHTOWER,

    UT_GOLDMINE,
    UT_CREEP_GNOLL,
    UT_CREEP_ARCHER,
    UT_CREEP_TROLL,
    UT_CREEP_BRUTE,
    UT_CREEP_OGRE,
    UT_CREEP_SHAMAN,
    UT_CREEP_DRAGON,
    UT_SUMMON_ELEMENTAL,
    UT_SUMMON_WOLF,

    UT_COUNT
};

enum AbilityTypeId
{
    AB_HARVEST = 0,
    AB_BUILD,
    AB_STORMBOLT,
    AB_THUNDERCLAP,
    AB_BASH,
    AB_HOLYLIGHT,
    AB_DIVINESHIELD,
    AB_DEVOTION,
    AB_BLIZZARD,
    AB_ELEMENTAL,
    AB_BRILLIANCE,
    AB_WINDWALK,
    AB_CRITICAL,
    AB_EVASION,
    AB_CHAINLIGHTNING,
    AB_FERALSPIRIT,
    AB_SHOCKWAVE,
    AB_WARSTOMP,
    AB_ENDURANCE,
    AB_HEAL,
    AB_SLOW,
    AB_BLOODLUST,
    AB_REJUV,
    AB_ENVENOM,
    AB_COUNT,
    AB_NONE = 0xFF
};

// Which ability implementation class handles a given ability type.
enum AbilityClass
{
    ABC_HARVEST = 0,
    ABC_BUILD,
    ABC_MISSILE_STUN,
    ABC_NOVA,
    ABC_BASH,
    ABC_TARGET_HEAL,
    ABC_SELF_BUFF,
    ABC_AURA,
    ABC_CHANNEL_AREA,
    ABC_SUMMON,
    ABC_CRITICAL,
    ABC_EVASION,
    ABC_BOUNCE,
    ABC_LINE,
    ABC_TARGET_BUFF,
    ABC_ENVENOM
};

enum AbilityFlags
{
    ABF_HERO        = 0x01,
    ABF_PASSIVE     = 0x02,
    ABF_AUTOCAST    = 0x04,
    ABF_TARGET_UNIT = 0x08,
    ABF_TARGET_POINT= 0x10,
    ABF_CHANNEL     = 0x20,
    ABF_FRIENDLY    = 0x40,
    ABF_NO_TARGET   = 0x80
};

enum BuffTypeId
{
    BF_STUN = 0,
    BF_SLOW,
    BF_THUNDERSLOW,
    BF_DEVOTION,
    BF_BRILLIANCE,
    BF_ENDURANCE,
    BF_DIVINESHIELD,
    BF_WINDWALK,
    BF_BLOODLUST,
    BF_REJUV,
    BF_POISON,
    BF_TIMEDLIFE,
    BF_SLEEP,
    BF_COUNT
};

enum BuffClass
{
    BFC_STUN = 0,
    BFC_STATMOD,
    BFC_INVULNERABLE,
    BFC_INVISIBLE,
    BFC_PERIODIC,
    BFC_TIMEDLIFE,
    BFC_SLEEP
};

enum BuffFlags
{
    BFF_POSITIVE   = 0x01,
    BFF_AURA       = 0x02,
    BFF_DISPELLABLE= 0x04,
    BFF_DISABLE    = 0x08
};

enum ItemTypeId
{
    IT_HEAL_POTION = 0,
    IT_MANA_POTION,
    IT_CLAWS,
    IT_RING,
    IT_BOOTS,
    IT_GLOVES,
    IT_BELT,
    IT_CIRCLET,
    IT_SLIPPERS,
    IT_TOME_XP,
    IT_TOME_STR,
    IT_SCROLL_HEAL,
    IT_PERIAPT,
    IT_RUNE_MANA,
    IT_COUNT
};

enum ItemClass
{
    ITC_PERMANENT = 0,
    ITC_CHARGED,
    ITC_POWERUP
};

enum UpgradeId
{
    UP_A_MELEE = 0,
    UP_A_RANGED,
    UP_A_ARMOR,
    UP_A_CASTER,
    UP_B_MELEE,
    UP_B_RANGED,
    UP_B_ARMOR,
    UP_B_CASTER,
    UP_COUNT
};

enum UpgradeEffect
{
    UPE_MELEE_DAMAGE = 0,
    UPE_RANGED_DAMAGE,
    UPE_ARMOR,
    UPE_CASTER_TRAINING
};

enum AttributeId
{
    ATTR_STR = 0,
    ATTR_AGI,
    ATTR_INT
};

enum AiStepOp
{
    AIS_BUILD = 0,
    AIS_TRAIN,
    AIS_HERO,
    AIS_RESEARCH,
    AIS_ATTACK,
    AIS_EXPAND,
    AIS_LOOP
};

struct UnitTypeData
{
    u16 flags;
    u8  race;
    u8  level;
    u16 life;
    u16 mana;
    s16 armorX10;
    u8  defenseType;
    u8  attackType;
    u8  weaponType;
    u8  damageDice;
    u8  damageSides;
    u8  footprint;
    u16 damageBase;
    u16 cooldownMs;
    u16 damagePointMs;
    u16 backswingMs;
    u16 range;
    u16 missileSpeed;
    u16 splashRadius;
    u16 moveSpeed;
    u16 turnRateDeg;
    u16 collision;
    u16 sightDay;
    u16 sightNight;
    u16 goldCost;
    u16 lumberCost;
    u16 buildTimeSec;
    s8  food;
    u8  bounty;
    u16 lifeRegenX100;
    u16 manaRegenX100;
    u8  abilities[4];
    u8  prereqs[2];
    u8  producedBy;
    u8  upgradeClass;
};

struct HeroTypeData
{
    u8    unitType;
    u8    primary;
    u8    abilities[3];
    float strBase;
    float agiBase;
    float intBase;
    float strPerLevel;
    float agiPerLevel;
    float intPerLevel;
};

struct AbilityTypeData
{
    u8    abilityClass;
    u8    flags;
    u8    buffType;
    u8    summonType;
    u16   manaCost[3];
    float cooldown[3];
    float range;
    float area[3];
    float valueA[3];
    float valueB[3];
    float duration[3];
    float heroDuration[3];
};

struct BuffTypeData
{
    u8    buffClass;
    u8    flags;
    u8    stackGroup;
    u8    pad;
    float armorAdd[3];
    float attackSpeedMul[3];
    float moveSpeedMul[3];
    float damageMul[3];
    float manaRegenAdd[3];
    float periodicAmount[3];
};

struct ItemTypeData
{
    u8    itemClass;
    u8    level;
    u8    charges;
    u8    bonusAttr;
    s16   bonusStr;
    s16   bonusAgi;
    s16   bonusInt;
    s16   bonusDamage;
    s16   bonusArmorX10;
    s16   bonusLife;
    u8    bonusMoveSpeed;
    u8    bonusAttackSpeedPct;
    u16   useAmount;
    u16   useArea;
};

struct UpgradeData
{
    u8  race;
    u8  effect;
    u8  levels;
    u8  prereqs;
    u16 goldBase;
    u16 goldPerLevel;
    u16 lumberBase;
    u16 lumberPerLevel;
    u16 timeSec;
    u8  researchedAt;
    u8  pad;
};

struct AiBuildStep
{
    u8 op;
    u8 type;
    u8 count;
    u8 condition;
};

struct CreepCampTemplate
{
    u8 level;
    u8 count;
    u8 types[4];
};

extern const UnitTypeData      g_unitTypes[UT_COUNT];
extern const HeroTypeData      g_heroTypes[6];
extern const AbilityTypeData   g_abilityTypes[AB_COUNT];
extern const BuffTypeData      g_buffTypes[BF_COUNT];
extern const ItemTypeData      g_itemTypes[IT_COUNT];
extern const UpgradeData       g_upgrades[UP_COUNT];
extern const float             g_damageTable[ATK_COUNT][DEF_COUNT];
extern const u16               g_heroXpNeeded[kMaxHeroLevel + 1];
extern const u16               g_creepXpByLevel[11];
extern const u16               g_heroKillXpByLevel[11];
extern const u8                g_creepXpFactorByHeroLevel[kMaxHeroLevel + 1];
extern const AiBuildStep       g_aiBuildOrderA[];
extern const AiBuildStep       g_aiBuildOrderB[];
extern const int               g_aiBuildOrderLenA;
extern const int               g_aiBuildOrderLenB;
extern const u8                g_aiSkillOrder[6][10];
extern const CreepCampTemplate g_creepCamps[8];
extern const u8                g_itemDropTable[6][4];

const UnitTypeData*    GetUnitType(int type);
const HeroTypeData*    GetHeroType(int unitType);
const AbilityTypeData* GetAbilityType(int ability);
const BuffTypeData*    GetBuffType(int buff);
const ItemTypeData*    GetItemType(int item);
int                    WorkerTypeForRace(int race);
int                    TownHallTypeForRace(int race);
int                    FarmTypeForRace(int race);

#endif

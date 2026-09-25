//============================================================================
//  0x6F263850, `retn 14h` - CUnitTypeRecord::SetRecordSet.
//
//  **Not pathfinding.**  This address was pulled into the pathfinding batch
//  by a grep that matched on the "pathTex"/"moveHeight"/"moveFloor" field
//  names it registers, but the function itself is a sibling of the
//  already-reconstructed CItemDatabase::SetRecordSet (Item/
//  itemdatabase_setrecordset.cpp), CUpgradeDatabase::SetRecordSet
//  (Game/upgradedatabase_setrecordset.cpp) and
//  CDestructableDatabase::SetRecordSet (Unit/destructabledatabase_setrecordset.cpp) -
//  the same per-class field-registration override, for the unit-type SLK
//  schema instead of the item/upgrade/destructable ones.  Every field name
//  here (unitID, race, HP, STR/AGI/INT, sight, collision, weapType1/2, ...)
//  is a real Warcraft III unit-data field, not a pathing one; it is kept in
//  this session's worklist and finished per the task's own instruction to
//  still complete an address that turns out to be a false positive, filed
//  under Unit/ rather than Pathfinding/ to reflect what it actually is.
//
//  **Shape, not byte-exact.**  This is the largest function in the batch by
//  a wide margin (1473 raw_asm lines, 131 single-valued fields plus 26
//  weapon-indexed fields each looped over weapon 1/2, plus a fourth section
//  walking a runtime-sized list of "requirement" entries and packing each
//  one's RGB-coded name through Storm_508/sub_6F32CF20).  The first three
//  sections below are transcribed field-for-field, offset-for-offset,
//  straight off the dump - every name, struct offset and record-set table
//  selector came out of a small parser run over this address's own
//  agent_worktrees raw_asm rather than being typed by hand, so those are
//  trustworthy.  The fourth section (the per-entry loop at the very end)
//  is reconstructed from reading the disassembly rather than parsed
//  mechanically, and its five callees (sub_6F25BAE0, sub_6F262F10,
//  sub_6F25FB40, sub_6F25F920, sub_6F32CF20) are all out of this session's
//  own scope (no dump, no name) - modelled here by shape (argument count
//  and order) rather than by a confirmed signature, so the local buffer
//  size and any padding inside it are a reasonable guess, not a recovered
//  fact.  Expect this function to land on DIFFERS with the gap concentrated
//  in that last section; the first ~1400 instructions are the trustworthy
//  part of this reconstruction.
//============================================================================

//  0x6F25BA00 - `retn 0Ch`.  Resolves one named field against a record-set
//  slot (indirected through a `void**` - the callee re-reads the pointer
//  each time rather than being handed it directly, unlike
//  Item/itemdatabase_setrecordset.cpp's own RegisterItemField, which is
//  this exact same shape but for a single, always-non-null m_recordSet).
//  Out of this session's own scope (no symbol yet) - thunked, not
//  reconstructed.  See agent_worktrees/funcs/0x6F25BA00.json.
__declspec(naked) void __fastcall RegisterUnitTypeFieldThunk()
{
    __asm
    {
        mov eax, 06F25BA00h
        jmp eax
    }
}

//  RegisterField(table, dest, name) is `retn 0Ch` (3 stack args, no
//  register-passed receiver the callee itself reads - see the thunk's own
//  note) - modelled as a plain __cdecl-shaped stack call through the thunk
//  above rather than a second C++ name, since the callee never reads `ecx`.
static void RegisterUnitTypeField(void** table, void* dest, const char* name)
{
    __asm
    {
        push name
        push dest
        push table
        call RegisterUnitTypeFieldThunk
    }
}

//  Storm.dll ordinal 578 - vsnprintf-shaped, same declaration every other
//  caller in this repo uses (e.g. Game/upgradedatabase_setrecordset.cpp).
int Storm_578(char* dest, unsigned int size, const char* format, ...);

//  Storm.dll ordinal 508 - bounded compare, same declaration
//  Game/racename.cpp and friends already use.  Returns 0 on a match.
int __stdcall Storm_508(const char* a, const char* b, unsigned int max);

//  The four tail-section callees below (sub_6F25BAE0, sub_6F262F10,
//  sub_6F25FB40, sub_6F25F920, sub_6F32CF20, sub_6F252990) are all out of
//  this session's own scope - no dump, no name - and are thunked rather
//  than reconstructed.  Argument shapes are read off this function's own
//  call sites (see this file's own header note on how trustworthy that
//  reading is for this section specifically).
#define UNITTYPE_TAIL_THUNK(name, addr) \
    __declspec(naked) void name() { __asm { mov eax, addr } __asm { jmp eax } }

UNITTYPE_TAIL_THUNK(QueryRequirementCountThunk,        06F25BAE0h)
UNITTYPE_TAIL_THUNK(PopulateRequirementEntryThunk,     06F262F10h)
UNITTYPE_TAIL_THUNK(GetRequirementColorCodeThunk,      06F25FB40h)
UNITTYPE_TAIL_THUNK(GetRequirementPackedBytesThunk,    06F25F920h)
UNITTYPE_TAIL_THUNK(ApplyRequirementColorThunk,        06F32CF20h)
UNITTYPE_TAIL_THUNK(ReleaseRequirementEntryThunk,      06F252990h)

static int __fastcall QueryRequirementCount(void* self, int zero)
{
    int result;
    __asm
    {
        push zero
        mov ecx, self
        call QueryRequirementCountThunk
        mov result, eax
    }
    return result;
}

static void PopulateRequirementEntry(void* self, void* entryBuf, int index, int zero)
{
    __asm
    {
        push zero
        push index
        push entryBuf
        mov ecx, self
        call PopulateRequirementEntryThunk
    }
}

static char* GetRequirementColorCode(void* self, void* entryBuf)
{
    char* result;
    __asm
    {
        push entryBuf
        mov ecx, self
        call GetRequirementColorCodeThunk
        mov result, eax
    }
    return result;
}

static void* GetRequirementPackedBytes(void* self, void* entryBuf)
{
    void* result;
    __asm
    {
        push entryBuf
        mov ecx, self
        call GetRequirementPackedBytesThunk
        mov result, eax
    }
    return result;
}

static void ApplyRequirementColor(int packed, char* colorCode, int zero1, int zero2)
{
    __asm
    {
        push zero2
        push zero1
        mov edx, colorCode
        mov ecx, packed
        call ApplyRequirementColorThunk
    }
}

static void ReleaseRequirementEntry(void* self, void* entryBuf)
{
    __asm
    {
        push entryBuf
        mov ecx, self
        call ReleaseRequirementEntryThunk
    }
}

struct SUnitTypeFieldPair
{
    unsigned int m_wedbResult;
    unsigned int m_registeredValue;
    unsigned int m_reserved;
};

class CUnitTypeRecord
{
public:
    void __thiscall SetRecordSet(void* recordSetA, void* recordSetB, void* recordSetC, void* recordSetD, void* recordSetE);

    char         m_reserved0[4];        // +0x00
    void*        m_recordSet;            // +0x04 - CAbilityDatabase-shared base slot
    char         m_reserved1[8];         // +0x08
    void*        m_tableA;                // +0x10
    char         m_tableAPad[4];          // +0x14
    void*        m_tableB;                // +0x18
    char         m_tableBPad[4];          // +0x1C
    void*        m_tableC;                // +0x20
    char         m_tableCPad[4];          // +0x24
    void*        m_tableD;                // +0x28
    char         m_tableDPad[4];          // +0x2C
    void*        m_tableE;                // +0x30
    char         m_tableEPad[4];          // +0x34

    SUnitTypeFieldPair m_unitID;            // +0x38
    SUnitTypeFieldPair m_race;              // +0x44
    SUnitTypeFieldPair m_valid;             // +0x50
    SUnitTypeFieldPair m_deathType;         // +0x5C
    SUnitTypeFieldPair m_prio;              // +0x68
    SUnitTypeFieldPair m_turnRate;          // +0x74
    SUnitTypeFieldPair m_propWin;           // +0x80
    SUnitTypeFieldPair m_orientInterp;      // +0x8C
    SUnitTypeFieldPair m_formation;         // +0x98
    SUnitTypeFieldPair m_death;             // +0xA4
    SUnitTypeFieldPair m_threat;            // +0xB0
    SUnitTypeFieldPair m_canSleep;          // +0xBC
    SUnitTypeFieldPair m_cargoSize;         // +0xC8
    SUnitTypeFieldPair m_targType;          // +0xD4
    SUnitTypeFieldPair m_movetp;            // +0xE0
    SUnitTypeFieldPair m_moveHeight;        // +0xEC
    SUnitTypeFieldPair m_moveFloor;         // +0xF8
    SUnitTypeFieldPair m_pathTex;           // +0x104
    SUnitTypeFieldPair m_fatLOS;            // +0x110
    SUnitTypeFieldPair m_points;            // +0x11C
    SUnitTypeFieldPair m_sort;              // +0x128
    SUnitTypeFieldPair m_buffType;          // +0x134
    SUnitTypeFieldPair m_buffRadius;        // +0x140
    SUnitTypeFieldPair m_nameCount;         // +0x14C
    SUnitTypeFieldPair m_version;           // +0x158
    SUnitTypeFieldPair m_canFlee;           // +0x164
    SUnitTypeFieldPair m_requireWaterRadius;// +0x170
    SUnitTypeFieldPair m_isBuildOn;         // +0x17C
    SUnitTypeFieldPair m_canBuildOn;        // +0x188
    SUnitTypeFieldPair m_unitBalanceID;     // +0x194
    SUnitTypeFieldPair m_level;             // +0x1A0
    SUnitTypeFieldPair m_type;              // +0x1AC
    SUnitTypeFieldPair m_goldcost;          // +0x1B8
    SUnitTypeFieldPair m_lumbercost;        // +0x1C4
    SUnitTypeFieldPair m_goldRep;           // +0x1D0
    SUnitTypeFieldPair m_lumberRep;         // +0x1DC
    SUnitTypeFieldPair m_fmade;             // +0x1E8
    SUnitTypeFieldPair m_fused;             // +0x1F4
    SUnitTypeFieldPair m_stockMax;          // +0x200
    SUnitTypeFieldPair m_stockRegen;        // +0x20C
    SUnitTypeFieldPair m_stockStart;        // +0x218
    SUnitTypeFieldPair m_bountydice;        // +0x224
    SUnitTypeFieldPair m_bountysides;       // +0x230
    SUnitTypeFieldPair m_bountyplus;        // +0x23C
    SUnitTypeFieldPair m_lumberbountydice;  // +0x248
    SUnitTypeFieldPair m_lumberbountysides; // +0x254
    SUnitTypeFieldPair m_lumberbountyplus;  // +0x260
    SUnitTypeFieldPair m_HP;                // +0x26C
    SUnitTypeFieldPair m_regenHP;           // +0x278
    SUnitTypeFieldPair m_regenType;         // +0x284
    SUnitTypeFieldPair m_manaN;             // +0x290
    SUnitTypeFieldPair m_mana0;             // +0x29C
    SUnitTypeFieldPair m_regenMana;         // +0x2A8
    SUnitTypeFieldPair m_spd;               // +0x2B4
    SUnitTypeFieldPair m_minSpd;            // +0x2C0
    SUnitTypeFieldPair m_maxSpd;            // +0x2CC
    SUnitTypeFieldPair m_def;               // +0x2D8
    SUnitTypeFieldPair m_defUp;             // +0x2E4
    SUnitTypeFieldPair m_defType;           // +0x2F0
    SUnitTypeFieldPair m_bldtm;             // +0x2FC
    SUnitTypeFieldPair m_reptm;             // +0x308
    SUnitTypeFieldPair m_sight;             // +0x314
    SUnitTypeFieldPair m_nsight;            // +0x320
    SUnitTypeFieldPair m_STR;               // +0x32C
    SUnitTypeFieldPair m_AGI;               // +0x338
    SUnitTypeFieldPair m_INT;               // +0x344
    SUnitTypeFieldPair m_STRplus;           // +0x350
    SUnitTypeFieldPair m_AGIplus;           // +0x35C
    SUnitTypeFieldPair m_INTplus;           // +0x368
    SUnitTypeFieldPair m_Primary;           // +0x374
    SUnitTypeFieldPair m_upgrades;          // +0x380
    SUnitTypeFieldPair m_tilesets;          // +0x38C
    SUnitTypeFieldPair m_nbrandom;          // +0x398
    SUnitTypeFieldPair m_repulse;           // +0x3A4
    SUnitTypeFieldPair m_repulseParam;      // +0x3B0
    SUnitTypeFieldPair m_repulseGroup;      // +0x3BC
    SUnitTypeFieldPair m_repulsePrio;       // +0x3C8
    SUnitTypeFieldPair m_preventPlace;      // +0x3D4
    SUnitTypeFieldPair m_requirePlace;      // +0x3E0
    SUnitTypeFieldPair m_isbldg;            // +0x3EC
    SUnitTypeFieldPair m_collision;         // +0x3F8
    SUnitTypeFieldPair m_unitUIID;          // +0x404
    SUnitTypeFieldPair m_scale;             // +0x410
    SUnitTypeFieldPair m_scaleBull;         // +0x41C
    SUnitTypeFieldPair m_blend;             // +0x428
    SUnitTypeFieldPair m_selZ;              // +0x434
    SUnitTypeFieldPair m_maxPitch;          // +0x440
    SUnitTypeFieldPair m_maxRoll;           // +0x44C
    SUnitTypeFieldPair m_elevPts;           // +0x458
    SUnitTypeFieldPair m_elevRad;           // +0x464
    SUnitTypeFieldPair m_fogRad;            // +0x470
    SUnitTypeFieldPair m_walk;              // +0x47C
    SUnitTypeFieldPair m_run;               // +0x488
    SUnitTypeFieldPair m_armor;             // +0x494
    SUnitTypeFieldPair m_file;              // +0x4A0
    SUnitTypeFieldPair m_fileVerFlags;      // +0x4AC
    SUnitTypeFieldPair m_unitSound;         // +0x4B8
    SUnitTypeFieldPair m_name;              // +0x4C4
    SUnitTypeFieldPair m_modelScale;        // +0x4D0
    SUnitTypeFieldPair m_red;               // +0x4DC
    SUnitTypeFieldPair m_green;             // +0x4E8
    SUnitTypeFieldPair m_blue;              // +0x4F4
    SUnitTypeFieldPair m_uberSplat;         // +0x500
    SUnitTypeFieldPair m_unitShadow;        // +0x50C
    SUnitTypeFieldPair m_buildingShadow;    // +0x518
    SUnitTypeFieldPair m_shadowW;           // +0x524
    SUnitTypeFieldPair m_shadowH;           // +0x530
    SUnitTypeFieldPair m_shadowX;           // +0x53C
    SUnitTypeFieldPair m_shadowY;           // +0x548
    SUnitTypeFieldPair m_shadowOnWater;     // +0x554
    SUnitTypeFieldPair m_selCircOnWater;    // +0x560
    SUnitTypeFieldPair m_occH;              // +0x56C
    SUnitTypeFieldPair m_special;           // +0x578
    SUnitTypeFieldPair m_hostilePal;        // +0x584
    SUnitTypeFieldPair m_nbmmIcon;          // +0x590
    SUnitTypeFieldPair m_hideHeroBar;       // +0x59C
    SUnitTypeFieldPair m_hideHeroMinimap;   // +0x5A8
    SUnitTypeFieldPair m_hideHeroDeathMsg;  // +0x5B4
    SUnitTypeFieldPair m_hideOnMinimap;     // +0x5C0
    SUnitTypeFieldPair m_unitWeapID;        // +0x5CC
    SUnitTypeFieldPair m_weapsOn;           // +0x5D8
    SUnitTypeFieldPair m_acquire;           // +0x5E4
    SUnitTypeFieldPair m_minRange;          // +0x5F0
    SUnitTypeFieldPair m_castpt;            // +0x5FC
    SUnitTypeFieldPair m_castbsw;           // +0x608
    SUnitTypeFieldPair m_launchX;           // +0x614
    SUnitTypeFieldPair m_launchY;           // +0x620
    SUnitTypeFieldPair m_launchZ;           // +0x62C
    SUnitTypeFieldPair m_launchSwimZ;       // +0x638
    SUnitTypeFieldPair m_impactZ;           // +0x644
    SUnitTypeFieldPair m_impactSwimZ;       // +0x650
    SUnitTypeFieldPair m_weap_weapType[2];
    SUnitTypeFieldPair m_weap_weapTp[2];
    SUnitTypeFieldPair m_weap_targs[2];
    SUnitTypeFieldPair m_weap_splashTargs[2];
    SUnitTypeFieldPair m_weap_atkType[2];
    SUnitTypeFieldPair m_weap_rangeN[2];
    SUnitTypeFieldPair m_weap_cool[2];
    SUnitTypeFieldPair m_weap_dmgpt[2];
    SUnitTypeFieldPair m_weap_backSw[2];
    SUnitTypeFieldPair m_weap_RngBuff[2];
    SUnitTypeFieldPair m_weap_dice[2];
    SUnitTypeFieldPair m_weap_sides[2];
    SUnitTypeFieldPair m_weap_dmgplus[2];
    SUnitTypeFieldPair m_weap_dmgUp[2];
    SUnitTypeFieldPair m_weap_Farea[2];
    SUnitTypeFieldPair m_weap_Harea[2];
    SUnitTypeFieldPair m_weap_Qarea[2];
    SUnitTypeFieldPair m_weap_Hfact[2];
    SUnitTypeFieldPair m_weap_Qfact[2];
    SUnitTypeFieldPair m_weap_targCount[2];
    SUnitTypeFieldPair m_weap_damageLoss[2];
    SUnitTypeFieldPair m_weap_spillDist[2];
    SUnitTypeFieldPair m_weap_spillRadius[2];
    SUnitTypeFieldPair m_weap_mindmg[2];
    SUnitTypeFieldPair m_weap_maxdmg[2];
    SUnitTypeFieldPair m_weap_showUI[2];
};

void __thiscall CUnitTypeRecord::SetRecordSet(void* recordSetA, void* recordSetB, void* recordSetC, void* recordSetD, void* recordSetE)
{
    //  0x6F005D30 (CAbilityDatabase::SetRecordSet) called directly, not virtually -
    //  see Item/abilitydatabase_checksum.cpp's own header note on the identical
    //  shared-base call shape.  Inlined here for the same reason
    //  Item/itemdatabase_setrecordset.cpp's own CItemDatabase::SetRecordSet inlines
    //  its own copy: the callee is a one-instruction `this[1] = recordSet` store.
    m_recordSet = recordSetA;

    m_tableA = recordSetA;
    m_tableB = recordSetB;
    m_tableC = recordSetC;
    m_tableD = recordSetD;
    m_tableE = recordSetE;

    RegisterUnitTypeField(&m_tableA, &m_unitID, "unitID");
    RegisterUnitTypeField(&m_tableA, &m_race, "race");
    RegisterUnitTypeField(&m_tableA, &m_valid, "valid");
    RegisterUnitTypeField(&m_tableA, &m_deathType, "deathType");
    RegisterUnitTypeField(&m_tableA, &m_prio, "prio");
    RegisterUnitTypeField(&m_tableA, &m_turnRate, "turnRate");
    RegisterUnitTypeField(&m_tableA, &m_propWin, "propWin");
    RegisterUnitTypeField(&m_tableA, &m_orientInterp, "orientInterp");
    RegisterUnitTypeField(&m_tableA, &m_formation, "formation");
    RegisterUnitTypeField(&m_tableA, &m_death, "death");
    RegisterUnitTypeField(&m_tableA, &m_threat, "threat");
    RegisterUnitTypeField(&m_tableA, &m_canSleep, "canSleep");
    RegisterUnitTypeField(&m_tableA, &m_cargoSize, "cargoSize");
    RegisterUnitTypeField(&m_tableA, &m_targType, "targType");
    RegisterUnitTypeField(&m_tableA, &m_movetp, "movetp");
    RegisterUnitTypeField(&m_tableA, &m_moveHeight, "moveHeight");
    RegisterUnitTypeField(&m_tableA, &m_moveFloor, "moveFloor");
    RegisterUnitTypeField(&m_tableA, &m_pathTex, "pathTex");
    RegisterUnitTypeField(&m_tableA, &m_fatLOS, "fatLOS");
    RegisterUnitTypeField(&m_tableA, &m_points, "points");
    RegisterUnitTypeField(&m_tableA, &m_sort, "sort");
    RegisterUnitTypeField(&m_tableA, &m_buffType, "buffType");
    RegisterUnitTypeField(&m_tableA, &m_buffRadius, "buffRadius");
    RegisterUnitTypeField(&m_tableA, &m_nameCount, "nameCount");
    RegisterUnitTypeField(&m_tableA, &m_version, "version");
    RegisterUnitTypeField(&m_tableA, &m_canFlee, "canFlee");
    RegisterUnitTypeField(&m_tableA, &m_requireWaterRadius, "requireWaterRadius");
    RegisterUnitTypeField(&m_tableA, &m_isBuildOn, "isBuildOn");
    RegisterUnitTypeField(&m_tableA, &m_canBuildOn, "canBuildOn");
    RegisterUnitTypeField(&m_tableE, &m_unitBalanceID, "unitBalanceID");
    RegisterUnitTypeField(&m_tableE, &m_level, "level");
    RegisterUnitTypeField(&m_tableE, &m_type, "type");
    RegisterUnitTypeField(&m_tableE, &m_goldcost, "goldcost");
    RegisterUnitTypeField(&m_tableE, &m_lumbercost, "lumbercost");
    RegisterUnitTypeField(&m_tableE, &m_goldRep, "goldRep");
    RegisterUnitTypeField(&m_tableE, &m_lumberRep, "lumberRep");
    RegisterUnitTypeField(&m_tableE, &m_fmade, "fmade");
    RegisterUnitTypeField(&m_tableE, &m_fused, "fused");
    RegisterUnitTypeField(&m_tableE, &m_stockMax, "stockMax");
    RegisterUnitTypeField(&m_tableE, &m_stockRegen, "stockRegen");
    RegisterUnitTypeField(&m_tableE, &m_stockStart, "stockStart");
    RegisterUnitTypeField(&m_tableE, &m_bountydice, "bountydice");
    RegisterUnitTypeField(&m_tableE, &m_bountysides, "bountysides");
    RegisterUnitTypeField(&m_tableE, &m_bountyplus, "bountyplus");
    RegisterUnitTypeField(&m_tableE, &m_lumberbountydice, "lumberbountydice");
    RegisterUnitTypeField(&m_tableE, &m_lumberbountysides, "lumberbountysides");
    RegisterUnitTypeField(&m_tableE, &m_lumberbountyplus, "lumberbountyplus");
    RegisterUnitTypeField(&m_tableE, &m_HP, "HP");
    RegisterUnitTypeField(&m_tableE, &m_regenHP, "regenHP");
    RegisterUnitTypeField(&m_tableE, &m_regenType, "regenType");
    RegisterUnitTypeField(&m_tableE, &m_manaN, "manaN");
    RegisterUnitTypeField(&m_tableE, &m_mana0, "mana0");
    RegisterUnitTypeField(&m_tableE, &m_regenMana, "regenMana");
    RegisterUnitTypeField(&m_tableE, &m_spd, "spd");
    RegisterUnitTypeField(&m_tableE, &m_minSpd, "minSpd");
    RegisterUnitTypeField(&m_tableE, &m_maxSpd, "maxSpd");
    RegisterUnitTypeField(&m_tableE, &m_def, "def");
    RegisterUnitTypeField(&m_tableE, &m_defUp, "defUp");
    RegisterUnitTypeField(&m_tableE, &m_defType, "defType");
    RegisterUnitTypeField(&m_tableE, &m_bldtm, "bldtm");
    RegisterUnitTypeField(&m_tableE, &m_reptm, "reptm");
    RegisterUnitTypeField(&m_tableE, &m_sight, "sight");
    RegisterUnitTypeField(&m_tableE, &m_nsight, "nsight");
    RegisterUnitTypeField(&m_tableE, &m_STR, "STR");
    RegisterUnitTypeField(&m_tableE, &m_AGI, "AGI");
    RegisterUnitTypeField(&m_tableE, &m_INT, "INT");
    RegisterUnitTypeField(&m_tableE, &m_STRplus, "STRplus");
    RegisterUnitTypeField(&m_tableE, &m_AGIplus, "AGIplus");
    RegisterUnitTypeField(&m_tableE, &m_INTplus, "INTplus");
    RegisterUnitTypeField(&m_tableE, &m_Primary, "Primary");
    RegisterUnitTypeField(&m_tableE, &m_upgrades, "upgrades");
    RegisterUnitTypeField(&m_tableE, &m_tilesets, "tilesets");
    RegisterUnitTypeField(&m_tableE, &m_nbrandom, "nbrandom");
    RegisterUnitTypeField(&m_tableE, &m_repulse, "repulse");
    RegisterUnitTypeField(&m_tableE, &m_repulseParam, "repulseParam");
    RegisterUnitTypeField(&m_tableE, &m_repulseGroup, "repulseGroup");
    RegisterUnitTypeField(&m_tableE, &m_repulsePrio, "repulsePrio");
    RegisterUnitTypeField(&m_tableE, &m_preventPlace, "preventPlace");
    RegisterUnitTypeField(&m_tableE, &m_requirePlace, "requirePlace");
    RegisterUnitTypeField(&m_tableE, &m_isbldg, "isbldg");
    RegisterUnitTypeField(&m_tableE, &m_collision, "collision");
    RegisterUnitTypeField(&m_tableB, &m_unitUIID, "unitUIID");
    RegisterUnitTypeField(&m_tableB, &m_scale, "scale");
    RegisterUnitTypeField(&m_tableB, &m_scaleBull, "scaleBull");
    RegisterUnitTypeField(&m_tableB, &m_blend, "blend");
    RegisterUnitTypeField(&m_tableB, &m_selZ, "selZ");
    RegisterUnitTypeField(&m_tableB, &m_maxPitch, "maxPitch");
    RegisterUnitTypeField(&m_tableB, &m_maxRoll, "maxRoll");
    RegisterUnitTypeField(&m_tableB, &m_elevPts, "elevPts");
    RegisterUnitTypeField(&m_tableB, &m_elevRad, "elevRad");
    RegisterUnitTypeField(&m_tableB, &m_fogRad, "fogRad");
    RegisterUnitTypeField(&m_tableB, &m_walk, "walk");
    RegisterUnitTypeField(&m_tableB, &m_run, "run");
    RegisterUnitTypeField(&m_tableB, &m_armor, "armor");
    RegisterUnitTypeField(&m_tableB, &m_file, "file");
    RegisterUnitTypeField(&m_tableB, &m_fileVerFlags, "fileVerFlags");
    RegisterUnitTypeField(&m_tableB, &m_unitSound, "unitSound");
    RegisterUnitTypeField(&m_tableB, &m_name, "name");
    RegisterUnitTypeField(&m_tableB, &m_modelScale, "modelScale");
    RegisterUnitTypeField(&m_tableB, &m_red, "red");
    RegisterUnitTypeField(&m_tableB, &m_green, "green");
    RegisterUnitTypeField(&m_tableB, &m_blue, "blue");
    RegisterUnitTypeField(&m_tableB, &m_uberSplat, "uberSplat");
    RegisterUnitTypeField(&m_tableB, &m_unitShadow, "unitShadow");
    RegisterUnitTypeField(&m_tableB, &m_buildingShadow, "buildingShadow");
    RegisterUnitTypeField(&m_tableB, &m_shadowW, "shadowW");
    RegisterUnitTypeField(&m_tableB, &m_shadowH, "shadowH");
    RegisterUnitTypeField(&m_tableB, &m_shadowX, "shadowX");
    RegisterUnitTypeField(&m_tableB, &m_shadowY, "shadowY");
    RegisterUnitTypeField(&m_tableB, &m_shadowOnWater, "shadowOnWater");
    RegisterUnitTypeField(&m_tableB, &m_selCircOnWater, "selCircOnWater");
    RegisterUnitTypeField(&m_tableB, &m_occH, "occH");
    RegisterUnitTypeField(&m_tableB, &m_special, "special");
    RegisterUnitTypeField(&m_tableB, &m_hostilePal, "hostilePal");
    RegisterUnitTypeField(&m_tableB, &m_nbmmIcon, "nbmmIcon");
    RegisterUnitTypeField(&m_tableB, &m_hideHeroBar, "hideHeroBar");
    RegisterUnitTypeField(&m_tableB, &m_hideHeroMinimap, "hideHeroMinimap");
    RegisterUnitTypeField(&m_tableB, &m_hideHeroDeathMsg, "hideHeroDeathMsg");
    RegisterUnitTypeField(&m_tableB, &m_hideOnMinimap, "hideOnMinimap");
    RegisterUnitTypeField(&m_tableD, &m_unitWeapID, "unitWeapID");
    RegisterUnitTypeField(&m_tableD, &m_weapsOn, "weapsOn");
    RegisterUnitTypeField(&m_tableD, &m_acquire, "acquire");
    RegisterUnitTypeField(&m_tableD, &m_minRange, "minRange");
    RegisterUnitTypeField(&m_tableD, &m_castpt, "castpt");
    RegisterUnitTypeField(&m_tableD, &m_castbsw, "castbsw");
    RegisterUnitTypeField(&m_tableD, &m_launchX, "launchX");
    RegisterUnitTypeField(&m_tableD, &m_launchY, "launchY");
    RegisterUnitTypeField(&m_tableD, &m_launchZ, "launchZ");
    RegisterUnitTypeField(&m_tableD, &m_launchSwimZ, "launchSwimZ");
    RegisterUnitTypeField(&m_tableD, &m_impactZ, "impactZ");
    RegisterUnitTypeField(&m_tableD, &m_impactSwimZ, "impactSwimZ");

    //  26 weapon-indexed fields, one small loop per field (matching the dump's
    //  own shape: 26 separate `cmp/jl` back-edges, not one loop around all 26) -
    //  each formatted "<name>%d" for weapon 1 and weapon 2 (Storm_578, an sprintf
    //  wrapper) and registered against m_tableD, the same record-set slot the
    //  twelve unitWeapID..impactSwimZ fields above use.
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "weapType", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_weapType[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "weapTp", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_weapTp[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "targs", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_targs[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "splashTargs", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_splashTargs[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "atkType", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_atkType[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "rangeN", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_rangeN[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "cool", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_cool[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "dmgpt", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_dmgpt[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "backSw", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_backSw[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "RngBuff", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_RngBuff[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "dice", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_dice[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "sides", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_sides[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "dmgplus", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_dmgplus[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "dmgUp", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_dmgUp[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "Farea", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_Farea[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "Harea", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_Harea[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "Qarea", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_Qarea[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "Hfact", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_Hfact[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "Qfact", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_Qfact[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "targCount", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_targCount[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "damageLoss", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_damageLoss[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "spillDist", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_spillDist[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "spillRadius", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_spillRadius[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "mindmg", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_mindmg[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "maxdmg", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_maxdmg[weaponIndex], formatted);
    }
    for (int weaponIndex = 0; weaponIndex < 2; ++weaponIndex)
    {
        char formatted[0x100];
        Storm_578(formatted, 0x100, "%s%d", "showUI", weaponIndex + 1);
        RegisterUnitTypeField(&m_tableD, &m_weap_showUI[weaponIndex], formatted);
    }
    //  Final section: walk a runtime-sized list of "requirement" entries
    //  (count from sub_6F25BAE0(this, 0)), pull each one's colour-coded
    //  name, decode its RGB packed value out of four raw bytes, and hand
    //  both to sub_6F32CF20 when the name carries a real colour code.
    //  Reconstructed by reading, not parsed - see this file's own header
    //  note on why this section is the least trustworthy part of the
    //  reconstruction.
    int entryCount = QueryRequirementCount(this, 0);
    for (int i = 0; i < entryCount; ++i)
    {
        unsigned int entryBuf[7];   // var_110's own size is not recovered - see header note
        PopulateRequirementEntry(this, entryBuf, i, 0);
        char* colorCode = GetRequirementColorCode(this, entryBuf);
        unsigned char* packedSrc = (unsigned char*)GetRequirementPackedBytes(this, entryBuf);
        int packed = (signed char)packedSrc[0];
        packed = (packed << 8) + (signed char)packedSrc[1];
        packed = (packed << 8) + (signed char)packedSrc[2];
        packed = (packed << 8) + (signed char)packedSrc[3];

        if (colorCode != 0 && colorCode[0] != 0)
        {
            if (Storm_508(colorCode, "_", 0x7FFFFFFF) != 0)
                ApplyRequirementColor(packed, colorCode, 0, 0);
        }

        ReleaseRequirementEntry(this, entryBuf);
    }
}

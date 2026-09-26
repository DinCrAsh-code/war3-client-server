// Top-level simulation: players, lockstep command pipeline, replay
// recorder, trigger script VM, neutral creeps, computer players, and the
// game object that owns everything.

#ifndef MR_GAME_H
#define MR_GAME_H

#include "Abilities.h"

// ---------------------------------------------------------------------------
// Player.cpp

enum PlayerFlags
{
    PF_ACTIVE   = 0x01,
    PF_AI       = 0x02,
    PF_DEFEATED = 0x04,
    PF_NEUTRAL  = 0x08,
    PF_PASSIVE  = 0x10
};

class CPlayer
{
public:
    void  Init(int id, int race, int team, u8 flags, const Vec2& start);
    bool  CanAfford(int gold, int lumber) const;
    bool  Spend(int gold, int lumber);
    void  Refund(int gold, int lumber);
    int   DepositGold(int amount);
    void  DepositLumber(int amount);
    float UpkeepRate() const;
    bool  HasFood(int food) const;
    void  ReserveFood(int food);
    void  ReleaseFood(int food);
    bool  MeetsRequirements(const UnitTypeData* data) const;
    bool  IsEnemy(int other) const;
    bool  IsAlly(int other) const;
    void  OnUnitCreated(const CUnit* unit, bool complete);
    void  OnUnitCompleted(const CUnit* unit);
    void  OnUnitRemoved(const CUnit* unit);
    int   CountOfType(int type) const { return m_typeCount[type]; }
    int   CountIncludingPending(int type) const { return m_typeCount[type] + m_typePending[type]; }
    int   UpgradeLevel(int upgrade) const { return m_upgradeLevel[upgrade]; }
    int   UpgradeFor(const UnitTypeData* data, int effect) const;
    float DamageBonus(const UnitTypeData* data) const;
    float ArmorBonus(const UnitTypeData* data) const;
    int   StructureCount() const;
    bool  IsDefeated() const { return (m_flags & PF_DEFEATED) != 0; }

    u8    m_id;
    u8    m_race;
    u8    m_team;
    u8    m_flags;
    int   m_gold;
    int   m_lumber;
    int   m_foodUsed;
    int   m_foodCap;
    u16   m_typeCount[UT_COUNT];
    u16   m_typePending[UT_COUNT];
    u8    m_upgradeLevel[UP_COUNT];
    u8    m_upgradePending[UP_COUNT];
    u32   m_allyMask;
    Vec2  m_start;
    int   m_heroCount;
    int   m_goldMined;
    int   m_lumberHarvested;
    int   m_unitsKilled;
    int   m_unitsLost;
};

// ---------------------------------------------------------------------------
// Commands.cpp

enum CommandType
{
    CMD_ORDER = 0,
    CMD_LEARN,
    CMD_AUTOCAST,
    CMD_COUNT
};

const int kMaxCommandUnits = 12;
const int kTurnTicks       = 5;
const int kTurnLatency     = 2;

struct Command
{
    u8     type;
    u8     player;
    u8     numUnits;
    u8     queued;
    Handle units[kMaxCommandUnits];
    COrder order;
};

class CReplay;

class CCommandSystem
{
public:
    void Init(CReplay* replay);
    bool Queue(const Command& cmd);
    void EndTurn(int turn);
    void ExecuteTurn(int turn);
    int  Executed() const { return m_executed; }
    int  Rejected() const { return m_rejected; }

    static bool Encode(CBitWriter& w, const Command& cmd);
    static bool Decode(CBitReader& r, Command& cmd);

private:
    void Apply(const Command& cmd);

    struct TurnPacket
    {
        int turn;
        u32 length;
        u8  data[2048];
    };

    CReplay*   m_replay;
    TurnPacket m_scheduled[kTurnLatency + 2];
    u8         m_outgoing[2048];
    CBitWriter m_writer;
    int        m_pendingCommands;
    int        m_executed;
    int        m_rejected;
};

// ---------------------------------------------------------------------------
// Replay.cpp

class CReplay
{
public:
    void Init();
    void RecordTurn(int turn, const u8* data, u32 length);
    void RecordSync(int turn, u32 checksum);
    u32  Finalize();
    bool Verify() const;
    u32  CompressedSize() const { return (u32)m_blocks.Count(); }

private:
    void Append(const void* data, u32 length);
    void FlushBlock();

    u8          m_raw[8192];
    u32         m_rawLength;
    TArray<u8>  m_blocks;
    TArray<u8>  m_shadow;
    u32         m_crc;
    int         m_numBlocks;
    int         m_numTurns;
};

// ---------------------------------------------------------------------------
// Script.cpp

enum ScriptValueType
{
    SV_NULL = 0,
    SV_INT,
    SV_REAL,
    SV_HANDLE,
    SV_BOOL
};

struct ScriptValue
{
    u8 type;
    union
    {
        s32   i;
        float f;
        u32   h;
    };
};

enum TriggerKind
{
    TRG_TIMER = 0,
    TRG_DEATH
};

class CScriptVM;
typedef void (*ScriptNative)(CScriptVM* vm, ScriptValue* args, int argc, ScriptValue* result);

class CScriptVM
{
public:
    bool Load(const u8* code, u32 length);
    bool Call(int function, const ScriptValue* args, int argc, ScriptValue* result);
    void Run();
    int  RegisterTrigger(int kind, int function, float period);
    void FireDeathTriggers(CUnit* dying, CUnit* killer);
    int  OpsExecuted() const { return m_totalOps; }

    CUnit* m_eventUnit;
    CUnit* m_eventKiller;

private:
    struct Function
    {
        u16 offset;
        u8  numArgs;
        u8  numLocals;
    };

    struct Trigger
    {
        u8    kind;
        u8    function;
        u8    enabled;
        u8    pad;
        u32   timerId;
        float period;
    };

    int  Execute(int function, int base);
    bool Push(const ScriptValue& v);
    bool Pop(ScriptValue& v);
    static void TimerCallback(void* context, u32 param);

    const u8*   m_code;
    u32         m_codeLength;
    Function    m_functions[32];
    int         m_numFunctions;
    ScriptValue m_globals[32];
    int         m_numGlobals;
    ScriptValue m_stack[256];
    int         m_sp;
    int         m_depth;
    Trigger     m_triggers[16];
    int         m_numTriggers;
    int         m_totalOps;
    bool        m_faulted;
};

extern const ScriptNative g_scriptNatives[];
extern const int          g_numScriptNatives;
extern const u8           g_mapScript[];
extern const u32          g_mapScriptSize;

// ---------------------------------------------------------------------------
// Creeps.cpp

struct CreepCamp
{
    Vec2   pos;
    u8     templateIndex;
    u8     level;
    u8     alive;
    u8     aggro;
    Handle units[4];
    float  aggroTimer;
    Handle aggroTarget;
};

class CCreepManager
{
public:
    void Init();
    void SpawnCamps();
    void Update(float dt);
    void OnCreepDamaged(CUnit* creep, CUnit* attacker);
    void OnCreepDeath(CUnit* creep, CUnit* killer);
    int  FindBestCamp(const Vec2& from, float strength, int maxLevel) const;
    int  CampCount() const { return m_numCamps; }
    const CreepCamp& Camp(int i) const { return m_camps[i]; }

private:
    void UpdateCamp(CreepCamp& camp, float dt);
    void SetCampSleeping(CreepCamp& camp, bool sleeping);
    void AggroCamp(CreepCamp& camp, CUnit* target);
    void LeashCamp(CreepCamp& camp);
    void RollItemDrop(int level, const Vec2& pos);

    CreepCamp m_camps[12];
    int       m_numCamps;
    bool      m_night;
};

// ---------------------------------------------------------------------------
// AI.cpp

enum AiState
{
    AI_BUILDUP = 0,
    AI_CREEPING,
    AI_ATTACKING,
    AI_RETREATING,
    AI_DEFENDING
};

class CAIController
{
public:
    void Init(int player);
    void Think();

    static void OnUnitDamaged(const EventData& e, void* context);

private:
    void   ScanUnits();
    void   ManageWorkers();
    void   ManageBuildOrder();
    void   ManageFood();
    void   ManageHeroes();
    void   ManageArmy();
    void   ManageMicro();
    bool   ExecuteStep(const AiBuildStep& step, int bonus);
    int    BuildersEnRoute(int type) const;
    bool   TryBuild(int type, bool expansion);
    bool   TryTrain(int type);
    bool   TryResearch(int upgrade);
    bool   FindBuildSpot(int type, bool expansion, Vec2& out) const;
    CUnit* PickBuilder(const Vec2& spot) const;
    float  EstimateStrength(const CUnit* unit) const;
    float  ArmyStrength() const;
    float  EnemyStrengthNear(const Vec2& p, float radius) const;
    Vec2   ArmyCenter() const;
    bool   ChooseAttackTarget(Vec2& out) const;
    void   SetState(int state);
    void   HeroCast(CHero* hero);
    void   HeroLearn(CHero* hero, int index);
    void   HeroItems(CHero* hero);
    void   OrderArmy(int orderId, Handle target, const Vec2& p);
    void   SendOrder(CUnit* unit, int orderId, Handle target, const Vec2& p, int param);
    void   SendOrderEx(CUnit* unit, int orderId, int flags, Handle target, const Vec2& p, int param);
    void   SendGroupOrder(CUnit** units, int count, int orderId, Handle target, const Vec2& p);
    bool   IsRecent(int key) const;
    void   MarkRecent(int key);

    int    m_player;
    int    m_step;
    int    m_loopCount;
    int    m_state;
    float  m_stateTime;
    int    m_waves;
    int    m_campTarget;
    bool   m_underAttack;
    Vec2   m_threatPos;
    float  m_threatTime;
    Vec2   m_attackTarget;
    Vec2   m_home;
    int    m_minAttackFood;
    float  m_lastArmyOrder;
    float  m_lastMicro;
    int    m_recent[UT_COUNT + UP_COUNT + 6];

    CUnit* m_workers[64];
    int    m_numWorkers;
    CUnit* m_army[96];
    int    m_numArmy;
    int    m_armyFood;
    CHero* m_heroes[3];
    int    m_numHeroes;
    CUnit* m_buildings[64];
    int    m_numBuildings;
    CUnit* m_halls[4];
    int    m_numHalls;
    int    m_idleWorkers;
};

// ---------------------------------------------------------------------------
// Game.cpp

const int kMaxHandles   = 4096;
const int kGameMaxTicks = kTicksPerSecond * 60 * 30;

class CGame
{
public:
    static void* operator new(size_t size) { return MemAlloc((u32)size); }
    static void operator delete(void* p) { MemFree(p); }

    bool  Init(u32 seed, int numPlayers);
    u32   Run(int maxTicks);
    void  Shutdown();
    void  Tick();

    CUnit* CreateUnit(int type, int owner, const Vec2& pos, float facing, bool constructing = false);
    CItem* CreateItem(int type, const Vec2& pos);
    void   AddMissile(CMissile* missile);
    void   OnUnitDeath(CUnit* unit, CUnit* killer);

    CWidget*       GetWidget(Handle h) const;
    CUnit*         GetUnit(Handle h) const;
    CItem*         GetItem(Handle h) const;
    CDestructable* GetTree(Handle h) const;
    CDestructable* TreeAtCell(int cell) const;

    CDestructable* FindNearestTree(const Vec2& p, float maxRange) const;
    CUnit*         FindDropoff(int player, const Vec2& p, bool lumber) const;
    CGoldMine*     FindNearestMine(const Vec2& p, float maxRange) const;
    CItem*         FindNearestItem(const Vec2& p, float maxRange) const;

    bool  IsNight() const;
    bool  IsVisibleTo(int player, const CUnit* unit) const;
    u32   ComputeChecksum() const;
    void  CheckVictory();
    void  EndGame(int winner);
    void  SetDefeated(int player);
    int   UnitCount() const { return m_units.Count(); }
    CUnit* UnitAt(int i) const { return m_units[i]; }
    float Time() const { return m_time; }

    CPlayer        m_players[kMaxPlayers];
    CAIController  m_ai[kMaxRealPlayers];
    CTerrain       m_terrain;
    CPathFinder    m_pathFinder;
    CSpatialGrid   m_grid;
    CFogOfWar      m_fog;
    CHandleTable   m_handles;
    CTimerQueue    m_timers;
    CEventBus      m_events;
    CRandom        m_rng;
    CCommandSystem m_commands;
    CReplay        m_replay;
    CScriptVM      m_script;
    CCreepManager  m_creeps;

    TArray<CUnit*>         m_units;
    TArray<CMissile*>      m_missiles;
    TArray<CItem*>         m_items;
    TArray<CDestructable*> m_trees;

    u16   m_treeIndex[kCellCount];
    int   m_numPlayers;
    int   m_tick;
    int   m_turn;
    float m_time;
    float m_timeOfDay;
    bool  m_gameOver;
    int   m_winner;
    u32   m_lastChecksum;

private:
    void SetupPlayers(u32 seed);
    void SpawnStartingUnits(int player);
    void UpdateTimeOfDay();
    void UpdateUnits();
    void UpdateMissiles();
    void UpdateFog();
    void UpdateAI();
    void CleanupDead();
    void DestroyUnit(CUnit* unit);

    static void OnUnitDamagedEvent(const EventData& e, void* context);
};

extern CGame* g_game;

#endif

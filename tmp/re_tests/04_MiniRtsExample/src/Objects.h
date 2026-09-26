// In-world objects: widgets (anything that has life and a position),
// units, heroes, buildings, gold mines, trees, items, and missiles.

#ifndef MR_OBJECTS_H
#define MR_OBJECTS_H

#include "Data.h"
#include "World.h"

class CUnit;
class CHero;
class CAbility;
class CWidget;
class CBuff;
class CItem;

enum WidgetKind
{
    WK_UNIT = 1,
    WK_ITEM,
    WK_DESTRUCTABLE
};

enum WidgetState
{
    US_DEAD         = 0x0001,
    US_HIDDEN       = 0x0002,
    US_INVULNERABLE = 0x0004,
    US_STUNNED      = 0x0008,
    US_CONSTRUCTING = 0x0010,
    US_SLEEPING     = 0x0020,
    US_INVISIBLE    = 0x0040,
    US_REMOVE       = 0x0080,
    US_CHANNELING   = 0x0100,
    US_INSIDE       = 0x0200,
    US_STATS_DIRTY  = 0x0400,
    US_GUARDING     = 0x0800
};

enum OrderId
{
    ORD_NONE = 0,
    ORD_STOP,
    ORD_HOLD,
    ORD_MOVE,
    ORD_ATTACK,
    ORD_ATTACK_MOVE,
    ORD_PATROL,
    ORD_SMART,
    ORD_HARVEST,
    ORD_RETURN,
    ORD_BUILD,
    ORD_TRAIN,
    ORD_RESEARCH,
    ORD_CAST,
    ORD_PICKUP,
    ORD_USE_ITEM,
    ORD_REVIVE,
    ORD_RALLY,
    ORD_CANCEL,
    ORD_LEARN,
    ORD_AUTOCAST,
    ORD_COUNT
};

enum OrderResult
{
    OR_CONTINUE = 0,
    OR_DONE,
    OR_FAILED
};

enum OrderFlags
{
    OF_QUEUED       = 0x01,
    OF_TARGET_UNIT  = 0x02,
    OF_TARGET_POINT = 0x04,
    OF_TARGET_ITEM  = 0x08
};

struct COrder
{
    u8     id;
    u8     flags;
    u16    param;
    Handle target;
    Vec2   point;
};

struct StatBlock
{
    float maxLife;
    float maxMana;
    float armor;
    float damageBase;
    int   dice;
    int   sides;
    float cooldown;
    float attackSpeed;
    float moveSpeed;
    float range;
    float lifeRegen;
    float manaRegen;
    float damageMul;
    float damageFlat;
    float sight;
};

enum AttackFlags
{
    AF_MISS    = 0x01,
    AF_CRIT    = 0x02,
    AF_BASH    = 0x04,
    AF_SPELL   = 0x08,
    AF_SPLASH  = 0x10,
    AF_RANGED  = 0x20,
    AF_NOREACT = 0x40
};

struct AttackContext
{
    CUnit*   source;
    CWidget* target;
    float    damage;
    u8       attackType;
    u8       flags;
    u8       weaponType;
    u8       sourceOwner;
    Vec2     impactPoint;
    float    splashRadius;
};

enum CastTargetKind
{
    CT_NONE = 0,
    CT_UNIT,
    CT_POINT
};

struct CastTarget
{
    Handle unit;
    Vec2   point;
    u8     kind;
};

// ---------------------------------------------------------------------------

class CWidget : public CObject
{
public:
    explicit CWidget(int kind);
    virtual ~CWidget();

    virtual void  Update(float dt);
    virtual float ApplyDamage(CUnit* source, float amount, int attackType, int flags);
    virtual void  Kill(CUnit* killer);
    virtual float ArmorValue() const;
    virtual int   DefenseType() const;
    virtual float CollisionRadius() const;

    bool IsAlive() const { return (m_state & US_DEAD) == 0; }
    bool HasState(u16 s) const { return (m_state & s) != 0; }

    Vec2  m_pos;
    float m_life;
    float m_maxLife;
    u16   m_state;
    u8    m_kind;
    u8    m_owner;
};

class CDestructable : public CWidget
{
public:
    explicit CDestructable(int cell);
    virtual ~CDestructable();

    virtual float ApplyDamage(CUnit* source, float amount, int attackType, int flags);
    virtual void  Kill(CUnit* killer);
    virtual int   DefenseType() const;

    int  Harvest(int amount);

    u16 m_cell;
    u16 m_lumber;
    u8  m_harvesters;
};

class CItem : public CWidget
{
public:
    explicit CItem(int itemType);
    virtual ~CItem();

    virtual void Kill(CUnit* killer);
    virtual int  DefenseType() const;

    const ItemTypeData* Data() const { return GetItemType(m_itemType); }

    u8     m_itemType;
    u8     m_charges;
    Handle m_carrier;
};

// ---------------------------------------------------------------------------

class CUnit : public CWidget
{
public:
    CUnit(int type, int owner);
    virtual ~CUnit();

    virtual void  Update(float dt);
    virtual float ApplyDamage(CUnit* source, float amount, int attackType, int flags);
    virtual void  Kill(CUnit* killer);
    virtual float ArmorValue() const;
    virtual int   DefenseType() const;
    virtual float CollisionRadius() const;
    virtual void  AddBonusStats(StatBlock& stats);
    virtual bool  HandleImmediateOrder(const COrder& order);
    virtual int   Level() const;
    virtual void  OnKilledUnit(CUnit* victim);

    // Unit.cpp
    void      RecalcStats();
    void      SetPosition(const Vec2& p);
    void      UpdateRegen(float dt);
    void      Heal(float amount);
    void      RestoreMana(float amount);
    bool      IsEnemyOf(const CUnit* other) const;
    bool      IsAllyOf(const CUnit* other) const;
    bool      IsTargetable() const;
    CBuff*    AddBuff(int buffType, int level, float duration, CUnit* source);
    CBuff*    FindBuff(int buffType) const;
    void      RemoveBuff(int buffType);
    void      RemoveBuffs(bool negativeOnly);
    void      UpdateBuffs(float dt);
    CAbility* AddAbility(int abilityType, int level);
    CAbility* FindAbility(int abilityType) const;
    void      UpdateAbilities(float dt);
    void      InterruptChannel();
    float     SightRadius() const;

    // Movement.cpp
    bool MoveTo(const Vec2& goal, float stopRange);
    int  UpdateMovement(float dt);
    void StopMoving();
    bool FaceTowards(const Vec2& p, float dt);
    Vec2 ComputeSeparation() const;

    // Combat.cpp
    bool   CanAttack(const CWidget* target) const;
    bool   InAttackRange(const CWidget* target, float slack) const;
    int    UpdateAttack(CWidget* target, float dt, bool allowChase);
    void   LaunchAttack(CWidget* target);
    CUnit* AcquireTarget(float range);
    float  RollDamage();

    // Orders.cpp
    bool IssueOrder(const COrder& order, bool queued);
    void ClearOrders();
    void NextOrder();
    void ProcessOrder(float dt);
    int  OrderMove(float dt);
    int  OrderAttack(float dt);
    int  OrderAttackMove(float dt);
    int  OrderPatrol(float dt);
    int  OrderHold(float dt);
    int  OrderCast(float dt);
    int  OrderPickup(float dt);
    int  OrderIdle(float dt);

    bool IsStructure() const { return (m_data->flags & UTF_STRUCTURE) != 0; }
    bool IsHero() const { return (m_data->flags & UTF_HERO) != 0; }
    bool IsWorker() const { return (m_data->flags & UTF_WORKER) != 0; }
    bool IsFlying() const { return (m_data->flags & UTF_FLYING) != 0; }

    const UnitTypeData* m_data;
    StatBlock  m_stats;
    u8         m_type;
    u8         m_attackPhase;
    u8         m_numAbilities;
    u8         m_stunCount;
    u8         m_invulnCount;
    u8         m_pad;
    float      m_mana;
    float      m_facing;
    float      m_statScale;

    COrder     m_order;
    COrder     m_queue[kOrderQueueSize];
    int        m_queueCount;

    CPath      m_path;
    Vec2       m_moveGoal;
    float      m_moveStopRange;
    float      m_repathTimer;
    float      m_pathBestDist;
    int        m_stuckTicks;
    bool       m_moving;

    Handle     m_attackTarget;
    float      m_attackTimer;
    float      m_cooldown;
    float      m_acquireTimer;
    Vec2       m_orderOrigin;

    CBuff*     m_buffs;
    CAbility*  m_abilities[kMaxAbilitySlots];

    float      m_corpseTimer;
    float      m_lastDamagedTime;
    Handle     m_lastAttacker;
    Vec2       m_guardPos;
    int        m_campIndex;
    int        m_kills;

    CUnit*     m_gridNext;
    CUnit*     m_gridPrev;
    int        m_gridBucket;
};

class CHero : public CUnit
{
public:
    CHero(int type, int owner);
    virtual ~CHero();

    virtual void  Update(float dt);
    virtual void  Kill(CUnit* killer);
    virtual void  AddBonusStats(StatBlock& stats);
    virtual bool  HandleImmediateOrder(const COrder& order);
    virtual int   Level() const;

    void  AddExperience(int xp);
    void  LevelUp();
    bool  LearnAbility(int slot);
    bool  CanLearn(int slot) const;
    bool  PickupItem(CItem* item);
    bool  UseItem(int slot);
    int   FreeInventorySlot() const;
    float Attribute(int attr) const;
    void  Revive(const Vec2& at);
    int   ReviveCost() const;

    const HeroTypeData* m_heroData;
    int   m_level;
    int   m_xp;
    int   m_skillPoints;
    float m_tomeStr;
    u8    m_abilityLevels[3];
    CItem* m_inventory[kInventorySlots];
};

enum QueueKind
{
    QK_TRAIN = 0,
    QK_RESEARCH,
    QK_REVIVE
};

class CBuilding : public CUnit
{
public:
    CBuilding(int type, int owner);
    virtual ~CBuilding();

    virtual void  Update(float dt);
    virtual void  Kill(CUnit* killer);
    virtual float CollisionRadius() const;
    virtual bool  HandleImmediateOrder(const COrder& order);

    void BeginConstruction(CUnit* builder);
    void UpdateConstruction(float dt);
    void FinishConstruction();
    bool EnqueueTrain(int unitType);
    bool EnqueueResearch(int upgrade);
    bool EnqueueRevive(CHero* hero);
    void CancelEntry(int index);
    void UpdateQueue(float dt);
    void CompleteEntry();
    bool FindSpawnPoint(Vec2& out) const;
    int  QueueLength() const { return m_queueCount; }

    struct QueueEntry
    {
        u8     kind;
        u8     type;
        u8     foodReserved;
        u8     pad;
        float  progress;
        float  total;
        Handle hero;
    };

    QueueEntry m_trainQueue[kTrainQueueSize];
    int        m_queueCount;
    float      m_buildProgress;
    float      m_buildTime;
    Handle     m_builder;
    Vec2       m_rally;
    bool       m_hasRally;
};

class CGoldMine : public CBuilding
{
public:
    CGoldMine(int owner, int gold);
    virtual ~CGoldMine();

    virtual void Update(float dt);
    virtual void Kill(CUnit* killer);

    bool TryEnter(CUnit* worker);
    void ReleaseOccupant();

    int    m_gold;
    Handle m_occupant;
    float  m_occupyTimer;
    int    m_workersAssigned;
};

// ---------------------------------------------------------------------------
// Missile.cpp

class CMissile : public CObject
{
public:
    CMissile(CUnit* source, const Vec2& from, float speed);
    virtual ~CMissile();

    virtual void OnImpact(CWidget* target, const Vec2& at) = 0;
    virtual bool IsHoming() const;

    bool Update(float dt);
    void SetTarget(CWidget* target);

    Handle m_source;
    Handle m_target;
    Vec2   m_pos;
    Vec2   m_targetPoint;
    float  m_speed;
    float  m_traveled;
    u8     m_owner;
    bool   m_done;
};

class CAttackMissile : public CMissile
{
public:
    CAttackMissile(CUnit* source, const Vec2& from, const AttackContext& ctx);
    virtual void OnImpact(CWidget* target, const Vec2& at);
    virtual bool IsHoming() const;

    AttackContext m_ctx;
};

class CSpellMissile : public CMissile
{
public:
    CSpellMissile(CUnit* source, const Vec2& from, float speed, float damage, int buffType, int level, float duration);
    virtual void OnImpact(CWidget* target, const Vec2& at);

    float m_damage;
    int   m_buffType;
    int   m_level;
    float m_duration;
};

class CBounceMissile : public CMissile
{
public:
    CBounceMissile(CUnit* source, const Vec2& from, float damage, int bounces, float range);
    virtual void OnImpact(CWidget* target, const Vec2& at);

    float  m_damage;
    int    m_bouncesLeft;
    float  m_bounceRange;
    Handle m_hit[8];
    int    m_numHit;
};

// ---------------------------------------------------------------------------
// Combat.cpp

float ArmorDamageFactor(float armor);
float DamageTypeFactor(int attackType, int defenseType);
void  ResolveAttack(AttackContext& ctx);
float DealDamage(CUnit* source, CWidget* target, float amount, int attackType, int flags);
int   DealSplashDamage(CUnit* source, int owner, const Vec2& center, float radius, float amount,
                       int attackType, int flags, CWidget* primary);
void  AwardKill(CUnit* killer, CUnit* victim);
int   ExperienceForKill(const CUnit* victim, int heroLevel);

// Spatial query filters (context = FilterContext*).
struct FilterContext
{
    const CUnit* self;
    int          player;
    u32          flags;
    float        minLifeRatio;
};

enum FilterFlags
{
    FF_ENEMY      = 0x01,
    FF_ALLY       = 0x02,
    FF_GROUND     = 0x04,
    FF_AIR        = 0x08,
    FF_STRUCTURE  = 0x10,
    FF_NO_STRUCT  = 0x20,
    FF_HERO       = 0x40,
    FF_INJURED    = 0x80,
    FF_VISIBLE    = 0x100,
    FF_NOT_SELF   = 0x200,
    FF_HAS_MANA   = 0x400
};

bool FilterUnit(CUnit* unit, void* context);
bool FilterAttackable(CUnit* unit, void* context);

#endif

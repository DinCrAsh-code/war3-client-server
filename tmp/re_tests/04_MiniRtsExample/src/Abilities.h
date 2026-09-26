// Abilities (active spells, passives, auras, worker behaviours) and buffs.

#ifndef MR_ABILITIES_H
#define MR_ABILITIES_H

#include "Objects.h"

enum CastError
{
    CE_OK = 0,
    CE_COOLDOWN,
    CE_NO_MANA,
    CE_BAD_TARGET,
    CE_OUT_OF_RANGE,
    CE_PASSIVE,
    CE_DISABLED,
    CE_NOT_LEARNED
};

class CAbility : public CObject
{
public:
    CAbility(CUnit* owner, int type, int level);
    virtual ~CAbility();

    virtual void  OnAdded();
    virtual void  OnRemoved();
    virtual void  Update(float dt);
    virtual int   CheckCast(const CastTarget& target) const;
    virtual bool  Cast(const CastTarget& target);
    virtual bool  UpdateChannel(float dt);
    virtual void  EndChannel();
    virtual void  OnAttackLaunch(AttackContext& ctx);
    virtual void  OnAttackHit(AttackContext& ctx);
    virtual bool  OnIncomingAttack(AttackContext& ctx);
    virtual int   OrderUpdate(COrder& order, float dt);
    virtual bool  TryAutoCast();

    const AbilityTypeData* Data() const { return m_data; }
    int   LevelIndex() const { return m_level > 0 ? m_level - 1 : 0; }
    float ManaCost() const { return (float)m_data->manaCost[LevelIndex()]; }
    bool  IsReady() const { return m_cooldown <= 0.0f && m_level > 0; }
    bool  IsPassive() const { return (m_data->flags & ABF_PASSIVE) != 0; }
    void  StartCooldown();
    bool  PayCost();
    float Duration(const CUnit* target) const;

    CUnit*                 m_owner;
    const AbilityTypeData* m_data;
    u8                     m_type;
    u8                     m_level;
    bool                   m_autoCast;
    bool                   m_channeling;
    float                  m_cooldown;
    float                  m_timer;
};

CAbility* CreateAbility(CUnit* owner, int type, int level);

class CAbilHarvest : public CAbility
{
public:
    CAbilHarvest(CUnit* owner, int type, int level);
    virtual void OnRemoved();
    virtual int  OrderUpdate(COrder& order, float dt);

    int  UpdateGold(COrder& order, float dt);
    int  UpdateLumber(COrder& order, float dt);
    int  ReturnCargo(COrder& order, float dt);
    void ReleaseClaims();

    enum State
    {
        HS_IDLE = 0,
        HS_TO_MINE,
        HS_WAIT_MINE,
        HS_IN_MINE,
        HS_TO_TREE,
        HS_CHOPPING,
        HS_RETURNING
    };

    u8     m_state;
    u8     m_cargoKind;
    u8     m_cargo;
    Handle m_resource;
    Handle m_mine;
};

class CAbilBuild : public CAbility
{
public:
    CAbilBuild(CUnit* owner, int type, int level);
    virtual int OrderUpdate(COrder& order, float dt);

    Handle m_building;
};

class CAbilMissileStun : public CAbility
{
public:
    CAbilMissileStun(CUnit* owner, int type, int level);
    virtual bool Cast(const CastTarget& target);
};

class CAbilNova : public CAbility
{
public:
    CAbilNova(CUnit* owner, int type, int level);
    virtual int  CheckCast(const CastTarget& target) const;
    virtual bool Cast(const CastTarget& target);
};

class CAbilBash : public CAbility
{
public:
    CAbilBash(CUnit* owner, int type, int level);
    virtual void OnAttackLaunch(AttackContext& ctx);
    virtual void OnAttackHit(AttackContext& ctx);
};

class CAbilTargetHeal : public CAbility
{
public:
    CAbilTargetHeal(CUnit* owner, int type, int level);
    virtual int  CheckCast(const CastTarget& target) const;
    virtual bool Cast(const CastTarget& target);
    virtual bool TryAutoCast();
};

class CAbilSelfBuff : public CAbility
{
public:
    CAbilSelfBuff(CUnit* owner, int type, int level);
    virtual bool Cast(const CastTarget& target);
    virtual void OnAttackLaunch(AttackContext& ctx);
};

class CAbilAura : public CAbility
{
public:
    CAbilAura(CUnit* owner, int type, int level);
    virtual void Update(float dt);
    virtual void OnRemoved();

    static void ApplyToUnit(CUnit* unit, void* context);
};

class CAbilChannelArea : public CAbility
{
public:
    CAbilChannelArea(CUnit* owner, int type, int level);
    virtual bool Cast(const CastTarget& target);
    virtual bool UpdateChannel(float dt);
    virtual void EndChannel();

    void DropWave();

    Vec2 m_center;
    int  m_wavesLeft;
};

class CAbilSummon : public CAbility
{
public:
    CAbilSummon(CUnit* owner, int type, int level);
    virtual bool Cast(const CastTarget& target);
};

class CAbilCritical : public CAbility
{
public:
    CAbilCritical(CUnit* owner, int type, int level);
    virtual void OnAttackLaunch(AttackContext& ctx);
};

class CAbilEvasion : public CAbility
{
public:
    CAbilEvasion(CUnit* owner, int type, int level);
    virtual bool OnIncomingAttack(AttackContext& ctx);
};

class CAbilBounce : public CAbility
{
public:
    CAbilBounce(CUnit* owner, int type, int level);
    virtual bool Cast(const CastTarget& target);
};

class CAbilLine : public CAbility
{
public:
    CAbilLine(CUnit* owner, int type, int level);
    virtual bool Cast(const CastTarget& target);
};

class CAbilTargetBuff : public CAbility
{
public:
    CAbilTargetBuff(CUnit* owner, int type, int level);
    virtual int  CheckCast(const CastTarget& target) const;
    virtual bool Cast(const CastTarget& target);
    virtual bool TryAutoCast();
};

class CAbilEnvenom : public CAbility
{
public:
    CAbilEnvenom(CUnit* owner, int type, int level);
    virtual void OnAttackHit(AttackContext& ctx);
};

// ---------------------------------------------------------------------------
// Buff.cpp

class CBuff : public CObject
{
public:
    CBuff(int type, int level, float duration);
    virtual ~CBuff();

    virtual void OnApply(CUnit* unit);
    virtual void OnRemove(CUnit* unit);
    virtual void Update(CUnit* unit, float dt);
    virtual void ModifyStats(StatBlock& stats) const;
    virtual void Refresh(float duration, int level);

    bool IsPositive() const { return (m_data->flags & BFF_POSITIVE) != 0; }

    const BuffTypeData* m_data;
    u8     m_type;
    u8     m_level;
    bool   m_expired;
    u8     m_pad;
    float  m_remaining;
    float  m_tick;
    Handle m_source;
    CBuff* m_next;
};

CBuff* CreateBuff(int type, int level, float duration);

class CBuffStun : public CBuff
{
public:
    CBuffStun(int type, int level, float duration);
    virtual void OnApply(CUnit* unit);
    virtual void OnRemove(CUnit* unit);
};

class CBuffStatMod : public CBuff
{
public:
    CBuffStatMod(int type, int level, float duration);
    virtual void ModifyStats(StatBlock& stats) const;
};

class CBuffInvulnerable : public CBuff
{
public:
    CBuffInvulnerable(int type, int level, float duration);
    virtual void OnApply(CUnit* unit);
    virtual void OnRemove(CUnit* unit);
};

class CBuffInvisible : public CBuff
{
public:
    CBuffInvisible(int type, int level, float duration);
    virtual void OnApply(CUnit* unit);
    virtual void OnRemove(CUnit* unit);
    virtual void ModifyStats(StatBlock& stats) const;
};

class CBuffPeriodic : public CBuff
{
public:
    CBuffPeriodic(int type, int level, float duration);
    virtual void Update(CUnit* unit, float dt);
    virtual void ModifyStats(StatBlock& stats) const;
};

class CBuffTimedLife : public CBuff
{
public:
    CBuffTimedLife(int type, int level, float duration);
    virtual void Update(CUnit* unit, float dt);
    virtual void Refresh(float duration, int level);
};

class CBuffSleep : public CBuff
{
public:
    CBuffSleep(int type, int level, float duration);
    virtual void OnApply(CUnit* unit);
    virtual void OnRemove(CUnit* unit);
};

#endif

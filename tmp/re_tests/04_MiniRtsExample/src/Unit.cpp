#include "Game.h"

CUnit::CUnit(int type, int owner) : CWidget(WK_UNIT)
{
    m_type = (u8)type;
    m_owner = (u8)owner;
    m_data = GetUnitType(type);
    m_statScale = 1.0f;
    m_life = (float)m_data->life;
    m_maxLife = m_life;
    m_mana = (float)m_data->mana * 0.5f;
    m_facing = 0.0f;
    m_attackPhase = 0;
    m_numAbilities = 0;
    m_stunCount = 0;
    m_invulnCount = 0;
    m_order.id = ORD_NONE;
    m_queueCount = 0;
    m_path.Reset();
    m_moving = false;
    m_attackTarget = 0;
    m_cooldown = 0.0f;
    m_acquireTimer = (float)(type % 10) * 0.05f;
    m_buffs = 0;
    m_corpseTimer = 0.0f;
    m_lastDamagedTime = -100.0f;
    m_lastAttacker = 0;
    m_campIndex = -1;
    m_kills = 0;
    m_gridNext = 0;
    m_gridPrev = 0;
    m_gridBucket = -1;
    m_state |= US_STATS_DIRTY;

    for (int i = 0; i < kMaxAbilitySlots; ++i)
        m_abilities[i] = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_data->abilities[i] != AB_NONE)
            AddAbility(m_data->abilities[i], 1);
    }
}

CUnit::~CUnit()
{
    for (int i = 0; i < m_numAbilities; ++i)
    {
        m_abilities[i]->OnRemoved();
        delete m_abilities[i];
    }
    CBuff* b = m_buffs;
    while (b)
    {
        CBuff* next = b->m_next;
        delete b;
        b = next;
    }
}

void CUnit::Update(float dt)
{
    if (!IsAlive())
    {
        UpdateBuffs(0.0f);
        m_corpseTimer -= dt;
        if (m_corpseTimer <= 0.0f)
            m_state |= US_REMOVE;
        return;
    }

    if (m_state & US_STATS_DIRTY)
        RecalcStats();

    UpdateBuffs(dt);
    if (!IsAlive())
        return;

    UpdateRegen(dt);
    UpdateAbilities(dt);
    if (m_cooldown > 0.0f)
        m_cooldown -= dt;

    if (m_state & (US_STUNNED | US_SLEEPING | US_INSIDE))
        return;

    ProcessOrder(dt);
}

float CUnit::ApplyDamage(CUnit* source, float amount, int attackType, int flags)
{
    if (!IsAlive() || m_invulnCount > 0 || (m_state & US_INVULNERABLE))
        return 0.0f;

    if (m_state & US_SLEEPING)
        RemoveBuff(BF_SLEEP);

    m_lastDamagedTime = g_game->Time();
    m_lastAttacker = source ? source->m_handle : 0;

    float dealt = CWidget::ApplyDamage(source, amount, attackType, flags);
    if (!(flags & AF_NOREACT) && dealt > 0.0f)
        g_game->m_events.Fire(EVT_UNIT_DAMAGED, this, source, dealt, flags);
    return dealt;
}

void CUnit::Kill(CUnit* killer)
{
    if (!IsAlive())
        return;

    m_life = 0.0f;
    m_state |= US_DEAD;
    InterruptChannel();
    ClearOrders();
    StopMoving();
    m_attackTarget = 0;
    m_attackPhase = 0;

    // Buffs are only flagged here; the list is purged by UpdateBuffs so a
    // buff that kills its own carrier never frees itself mid-update.
    for (CBuff* b = m_buffs; b; b = b->m_next)
        b->m_expired = true;

    g_game->m_grid.Remove(this);
    m_corpseTimer = IsStructure() ? 2.0f : 8.0f;

    if (killer && killer != this)
        killer->OnKilledUnit(this);
    g_game->OnUnitDeath(this, killer);
}

float CUnit::ArmorValue() const
{
    return m_stats.armor;
}

int CUnit::DefenseType() const
{
    return m_data->defenseType;
}

float CUnit::CollisionRadius() const
{
    return (float)m_data->collision;
}

void CUnit::AddBonusStats(StatBlock& stats)
{
    (void)stats;
}

bool CUnit::HandleImmediateOrder(const COrder& order)
{
    switch (order.id)
    {
    case ORD_STOP:
        InterruptChannel();
        ClearOrders();
        StopMoving();
        m_attackTarget = 0;
        m_orderOrigin = m_pos;
        return true;

    case ORD_AUTOCAST:
        {
            CAbility* a = FindAbility(order.param);
            if (a && (a->Data()->flags & ABF_AUTOCAST))
                a->m_autoCast = (order.flags & OF_QUEUED) == 0;
        }
        return true;

    default:
        return false;
    }
}

int CUnit::Level() const
{
    return m_data->level;
}

void CUnit::OnKilledUnit(CUnit* victim)
{
    (void)victim;
    ++m_kills;
}

void CUnit::RecalcStats()
{
    float lifeRatio = m_maxLife > 0.0f ? m_life / m_maxLife : 1.0f;
    float manaRatio = m_stats.maxMana > 0.0f ? m_mana / m_stats.maxMana : 0.5f;
    const UnitTypeData* d = m_data;
    const CPlayer& player = g_game->m_players[m_owner];

    StatBlock s;
    s.maxLife = (float)d->life * m_statScale;
    s.maxMana = (float)d->mana;
    s.armor = (float)d->armorX10 * 0.1f + player.ArmorBonus(d);
    s.damageBase = (float)d->damageBase * m_statScale;
    s.dice = d->damageDice;
    s.sides = d->damageSides;
    s.cooldown = (float)d->cooldownMs * 0.001f;
    s.attackSpeed = 1.0f;
    s.moveSpeed = (float)d->moveSpeed;
    s.range = (float)d->range;
    s.lifeRegen = (float)d->lifeRegenX100 * 0.01f;
    s.manaRegen = (float)d->manaRegenX100 * 0.01f;
    s.damageMul = 1.0f;
    s.damageFlat = player.DamageBonus(d);
    s.sight = (float)(g_game->IsNight() ? d->sightNight : d->sightDay);

    int caster = player.UpgradeFor(d, UPE_CASTER_TRAINING);
    if (caster > 0)
    {
        s.maxMana += 100.0f * (float)caster;
        s.maxLife += 60.0f * (float)caster;
        s.damageBase += 2.0f * (float)caster;
    }

    AddBonusStats(s);
    for (CBuff* b = m_buffs; b; b = b->m_next)
    {
        if (!b->m_expired)
            b->ModifyStats(s);
    }

    s.attackSpeed = ClampF(s.attackSpeed, 0.2f, 5.0f);
    if (d->moveSpeed > 0)
        s.moveSpeed = ClampF(s.moveSpeed, 100.0f, 522.0f);
    if (s.maxLife < 1.0f)
        s.maxLife = 1.0f;

    m_stats = s;
    m_maxLife = s.maxLife;
    if (IsAlive())
        m_life = MaxF(1.0f, lifeRatio * m_maxLife);
    m_mana = manaRatio * s.maxMana;
    m_state &= ~US_STATS_DIRTY;
}

void CUnit::SetPosition(const Vec2& p)
{
    m_pos.x = ClampF(p.x, kCellSize * 2.0f, kMapSize - kCellSize * 2.0f);
    m_pos.y = ClampF(p.y, kCellSize * 2.0f, kMapSize - kCellSize * 2.0f);
    g_game->m_grid.Move(this);
}

void CUnit::UpdateRegen(float dt)
{
    if (m_state & US_CONSTRUCTING)
        return;
    if (m_life < m_maxLife)
        m_life = MinF(m_maxLife, m_life + m_stats.lifeRegen * dt);
    if (m_mana < m_stats.maxMana)
        m_mana = MinF(m_stats.maxMana, m_mana + m_stats.manaRegen * dt);
}

void CUnit::Heal(float amount)
{
    if (!IsAlive())
        return;
    m_life = MinF(m_maxLife, m_life + amount);
}

void CUnit::RestoreMana(float amount)
{
    if (!IsAlive())
        return;
    m_mana = MinF(m_stats.maxMana, m_mana + amount);
}

bool CUnit::IsEnemyOf(const CUnit* other) const
{
    return g_game->m_players[m_owner].IsEnemy(other->m_owner);
}

bool CUnit::IsAllyOf(const CUnit* other) const
{
    return g_game->m_players[m_owner].IsAlly(other->m_owner);
}

bool CUnit::IsTargetable() const
{
    if (!IsAlive())
        return false;
    if (m_state & (US_HIDDEN | US_INSIDE | US_INVULNERABLE))
        return false;
    return m_invulnCount == 0;
}

CBuff* CUnit::AddBuff(int buffType, int level, float duration, CUnit* source)
{
    if (!IsAlive())
        return 0;

    CBuff* existing = FindBuff(buffType);
    if (existing)
    {
        existing->Refresh(duration, level);
        existing->m_source = source ? source->m_handle : 0;
        m_state |= US_STATS_DIRTY;
        return existing;
    }

    CBuff* b = CreateBuff(buffType, level, duration);
    if (!b)
        return 0;
    b->m_source = source ? source->m_handle : 0;
    b->m_next = m_buffs;
    m_buffs = b;
    b->OnApply(this);
    m_state |= US_STATS_DIRTY;
    return b;
}

CBuff* CUnit::FindBuff(int buffType) const
{
    for (CBuff* b = m_buffs; b; b = b->m_next)
    {
        if (b->m_type == buffType && !b->m_expired)
            return b;
    }
    return 0;
}

void CUnit::RemoveBuff(int buffType)
{
    CBuff* b = FindBuff(buffType);
    if (b)
        b->m_expired = true;
}

void CUnit::RemoveBuffs(bool negativeOnly)
{
    for (CBuff* b = m_buffs; b; b = b->m_next)
    {
        if (negativeOnly && b->IsPositive())
            continue;
        if (b->m_type == BF_TIMEDLIFE)
            continue;
        b->m_expired = true;
    }
}

void CUnit::UpdateBuffs(float dt)
{
    CBuff** link = &m_buffs;
    while (*link)
    {
        CBuff* b = *link;
        if (!b->m_expired && dt > 0.0f)
            b->Update(this, dt);

        if (b->m_expired)
        {
            *link = b->m_next;
            b->OnRemove(this);
            delete b;
            m_state |= US_STATS_DIRTY;
        }
        else
        {
            link = &b->m_next;
        }
    }
}

CAbility* CUnit::AddAbility(int abilityType, int level)
{
    if (m_numAbilities >= kMaxAbilitySlots)
        return 0;
    CAbility* a = CreateAbility(this, abilityType, level);
    if (!a)
        return 0;
    m_abilities[m_numAbilities++] = a;
    a->OnAdded();
    return a;
}

CAbility* CUnit::FindAbility(int abilityType) const
{
    for (int i = 0; i < m_numAbilities; ++i)
    {
        if (m_abilities[i]->m_type == abilityType)
            return m_abilities[i];
    }
    return 0;
}

void CUnit::UpdateAbilities(float dt)
{
    for (int i = 0; i < m_numAbilities; ++i)
    {
        CAbility* a = m_abilities[i];
        a->Update(dt);
        if (a->m_autoCast && a->m_level > 0 && !(m_state & (US_CHANNELING | US_STUNNED | US_INSIDE)) &&
            m_order.id != ORD_CAST)
        {
            a->TryAutoCast();
        }
    }
}

void CUnit::InterruptChannel()
{
    if (!(m_state & US_CHANNELING))
        return;
    for (int i = 0; i < m_numAbilities; ++i)
    {
        if (m_abilities[i]->m_channeling)
            m_abilities[i]->EndChannel();
    }
    m_state &= ~US_CHANNELING;
}

float CUnit::SightRadius() const
{
    return m_stats.sight;
}

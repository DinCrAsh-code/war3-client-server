#include "Game.h"

CBuff::CBuff(int type, int level, float duration)
{
    m_data = GetBuffType(type);
    m_type = (u8)type;
    m_level = (u8)ClampI(level, 1, 3);
    m_expired = false;
    m_pad = 0;
    m_remaining = duration;
    m_tick = 0.0f;
    m_source = 0;
    m_next = 0;
}

CBuff::~CBuff()
{
}

void CBuff::OnApply(CUnit* unit)
{
    (void)unit;
}

void CBuff::OnRemove(CUnit* unit)
{
    (void)unit;
}

// A duration of zero means the buff stays until removed explicitly.
void CBuff::Update(CUnit* unit, float dt)
{
    (void)unit;
    if (m_remaining > 0.0f)
    {
        m_remaining -= dt;
        if (m_remaining <= 0.0f)
            m_expired = true;
    }
}

void CBuff::ModifyStats(StatBlock& stats) const
{
    (void)stats;
}

void CBuff::Refresh(float duration, int level)
{
    if (duration > m_remaining)
        m_remaining = duration;
    if (level > m_level)
        m_level = (u8)ClampI(level, 1, 3);
}

CBuff* CreateBuff(int type, int level, float duration)
{
    switch (GetBuffType(type)->buffClass)
    {
    case BFC_STUN:         return new CBuffStun(type, level, duration);
    case BFC_STATMOD:      return new CBuffStatMod(type, level, duration);
    case BFC_INVULNERABLE: return new CBuffInvulnerable(type, level, duration);
    case BFC_INVISIBLE:    return new CBuffInvisible(type, level, duration);
    case BFC_PERIODIC:     return new CBuffPeriodic(type, level, duration);
    case BFC_TIMEDLIFE:    return new CBuffTimedLife(type, level, duration);
    case BFC_SLEEP:        return new CBuffSleep(type, level, duration);
    default:               return 0;
    }
}

// ---------------------------------------------------------------------------

CBuffStun::CBuffStun(int type, int level, float duration) : CBuff(type, level, duration)
{
}

void CBuffStun::OnApply(CUnit* unit)
{
    ++unit->m_stunCount;
    unit->m_state |= US_STUNNED;
    unit->InterruptChannel();
    unit->StopMoving();
    unit->m_attackPhase = 0;
}

void CBuffStun::OnRemove(CUnit* unit)
{
    if (unit->m_stunCount > 0)
        --unit->m_stunCount;
    if (unit->m_stunCount == 0)
        unit->m_state &= ~US_STUNNED;
}

CBuffStatMod::CBuffStatMod(int type, int level, float duration) : CBuff(type, level, duration)
{
}

void CBuffStatMod::ModifyStats(StatBlock& stats) const
{
    int i = m_level - 1;
    stats.armor += m_data->armorAdd[i];
    stats.attackSpeed *= m_data->attackSpeedMul[i];
    stats.moveSpeed *= m_data->moveSpeedMul[i];
    stats.damageMul *= m_data->damageMul[i];
    stats.manaRegen += m_data->manaRegenAdd[i];
}

CBuffInvulnerable::CBuffInvulnerable(int type, int level, float duration) : CBuff(type, level, duration)
{
}

void CBuffInvulnerable::OnApply(CUnit* unit)
{
    ++unit->m_invulnCount;
    unit->m_state |= US_INVULNERABLE;
    unit->RemoveBuffs(true);
}

void CBuffInvulnerable::OnRemove(CUnit* unit)
{
    if (unit->m_invulnCount > 0)
        --unit->m_invulnCount;
    if (unit->m_invulnCount == 0)
        unit->m_state &= ~US_INVULNERABLE;
}

CBuffInvisible::CBuffInvisible(int type, int level, float duration) : CBuff(type, level, duration)
{
}

void CBuffInvisible::OnApply(CUnit* unit)
{
    unit->m_state |= US_INVISIBLE;
}

void CBuffInvisible::OnRemove(CUnit* unit)
{
    unit->m_state &= ~US_INVISIBLE;
}

void CBuffInvisible::ModifyStats(StatBlock& stats) const
{
    stats.moveSpeed *= m_data->moveSpeedMul[m_level - 1];
}

CBuffPeriodic::CBuffPeriodic(int type, int level, float duration) : CBuff(type, level, duration)
{
}

// Heals (positive amount) or damages (negative amount) once per second.
void CBuffPeriodic::Update(CUnit* unit, float dt)
{
    CBuff::Update(unit, dt);
    m_tick += dt;
    while (m_tick >= 1.0f && unit->IsAlive())
    {
        m_tick -= 1.0f;
        float amount = m_data->periodicAmount[m_level - 1];
        if (amount > 0.0f)
            unit->Heal(amount);
        else
            DealDamage(g_game->GetUnit(m_source), unit, -amount, ATK_SPELL, AF_SPELL | AF_NOREACT);
    }
}

void CBuffPeriodic::ModifyStats(StatBlock& stats) const
{
    stats.moveSpeed *= m_data->moveSpeedMul[m_level - 1];
}

CBuffTimedLife::CBuffTimedLife(int type, int level, float duration) : CBuff(type, level, duration)
{
}

void CBuffTimedLife::Update(CUnit* unit, float dt)
{
    CBuff::Update(unit, dt);
    if (m_expired)
        unit->Kill(0);
}

void CBuffTimedLife::Refresh(float duration, int level)
{
    (void)duration;
    (void)level;
}

CBuffSleep::CBuffSleep(int type, int level, float duration) : CBuff(type, level, duration)
{
}

void CBuffSleep::OnApply(CUnit* unit)
{
    unit->m_state |= US_SLEEPING;
    unit->StopMoving();
}

void CBuffSleep::OnRemove(CUnit* unit)
{
    unit->m_state &= ~US_SLEEPING;
}

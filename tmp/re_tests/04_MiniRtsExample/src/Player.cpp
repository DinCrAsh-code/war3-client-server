#include "Game.h"

static const int kFoodLimit        = 100;
static const int kUpkeepLowFood    = 50;
static const int kUpkeepHighFood   = 80;

void CPlayer::Init(int id, int race, int team, u8 flags, const Vec2& start)
{
    m_id = (u8)id;
    m_race = (u8)race;
    m_team = (u8)team;
    m_flags = flags;
    m_gold = 0;
    m_lumber = 0;
    m_foodUsed = 0;
    m_foodCap = 0;
    memset(m_typeCount, 0, sizeof(m_typeCount));
    memset(m_typePending, 0, sizeof(m_typePending));
    memset(m_upgradeLevel, 0, sizeof(m_upgradeLevel));
    memset(m_upgradePending, 0, sizeof(m_upgradePending));
    m_allyMask = 1u << id;
    m_start = start;
    m_heroCount = 0;
    m_goldMined = 0;
    m_lumberHarvested = 0;
    m_unitsKilled = 0;
    m_unitsLost = 0;
}

bool CPlayer::CanAfford(int gold, int lumber) const
{
    return m_gold >= gold && m_lumber >= lumber;
}

bool CPlayer::Spend(int gold, int lumber)
{
    if (!CanAfford(gold, lumber))
        return false;
    m_gold -= gold;
    m_lumber -= lumber;
    return true;
}

void CPlayer::Refund(int gold, int lumber)
{
    m_gold += gold;
    m_lumber += lumber;
}

float CPlayer::UpkeepRate() const
{
    if (m_foodUsed > kUpkeepHighFood)
        return 0.4f;
    if (m_foodUsed > kUpkeepLowFood)
        return 0.7f;
    return 1.0f;
}

// Mined gold is taxed by upkeep; lumber is not.
int CPlayer::DepositGold(int amount)
{
    int net = (int)((float)amount * UpkeepRate() + 0.5f);
    m_gold += net;
    m_goldMined += net;
    return net;
}

void CPlayer::DepositLumber(int amount)
{
    m_lumber += amount;
    m_lumberHarvested += amount;
}

bool CPlayer::HasFood(int food) const
{
    int cap = m_foodCap < kFoodLimit ? m_foodCap : kFoodLimit;
    return m_foodUsed + food <= cap;
}

void CPlayer::ReserveFood(int food)
{
    if (food > 0)
        m_foodUsed += food;
}

void CPlayer::ReleaseFood(int food)
{
    if (food > 0)
        m_foodUsed -= food;
    if (m_foodUsed < 0)
        m_foodUsed = 0;
}

bool CPlayer::MeetsRequirements(const UnitTypeData* data) const
{
    for (int i = 0; i < 2; ++i)
    {
        int req = data->prereqs[i];
        if (req != 0xFF && m_typeCount[req] == 0)
            return false;
    }
    return true;
}

bool CPlayer::IsEnemy(int other) const
{
    if (other == m_id || other < 0 || other >= kMaxPlayers)
        return false;
    if ((m_flags & PF_PASSIVE) || other == kPlayerNeutralPassive)
        return false;
    return (m_allyMask & (1u << other)) == 0;
}

bool CPlayer::IsAlly(int other) const
{
    if (other < 0 || other >= kMaxPlayers)
        return false;
    return (m_allyMask & (1u << other)) != 0;
}

void CPlayer::OnUnitCreated(const CUnit* unit, bool complete)
{
    const UnitTypeData* d = unit->m_data;
    if (!complete)
    {
        ++m_typePending[unit->m_type];
        return;
    }

    ++m_typeCount[unit->m_type];
    if (d->food > 0)
        m_foodUsed += d->food;
    else
        m_foodCap -= d->food;
    if (d->flags & UTF_HERO)
        ++m_heroCount;
}

void CPlayer::OnUnitCompleted(const CUnit* unit)
{
    const UnitTypeData* d = unit->m_data;
    if (m_typePending[unit->m_type] > 0)
        --m_typePending[unit->m_type];
    ++m_typeCount[unit->m_type];
    if (d->food < 0)
        m_foodCap -= d->food;
}

void CPlayer::OnUnitRemoved(const CUnit* unit)
{
    const UnitTypeData* d = unit->m_data;
    if (unit->m_state & US_CONSTRUCTING)
    {
        if (m_typePending[unit->m_type] > 0)
            --m_typePending[unit->m_type];
        return;
    }

    if (m_typeCount[unit->m_type] > 0)
        --m_typeCount[unit->m_type];
    if (d->food > 0)
        ReleaseFood(d->food);
    else
        m_foodCap += d->food;
}

int CPlayer::UpgradeFor(const UnitTypeData* data, int effect) const
{
    if (data->race > RACE_B)
        return 0;
    for (int i = 0; i < UP_COUNT; ++i)
    {
        const UpgradeData& u = g_upgrades[i];
        if (u.race != data->race || u.effect != effect)
            continue;
        switch (effect)
        {
        case UPE_MELEE_DAMAGE:
            if (data->upgradeClass != 1)
                return 0;
            break;
        case UPE_RANGED_DAMAGE:
            if (data->upgradeClass != 2)
                return 0;
            break;
        case UPE_ARMOR:
            if (data->upgradeClass == 0 || data->upgradeClass == 3)
                return 0;
            break;
        case UPE_CASTER_TRAINING:
            if (data->upgradeClass != 3)
                return 0;
            break;
        default:
            break;
        }
        return m_upgradeLevel[i];
    }
    return 0;
}

float CPlayer::DamageBonus(const UnitTypeData* data) const
{
    int melee = UpgradeFor(data, UPE_MELEE_DAMAGE);
    int ranged = UpgradeFor(data, UPE_RANGED_DAMAGE);
    int level = melee > ranged ? melee : ranged;
    return (float)(level * (data->damageDice > 0 ? data->damageDice : 1)) * 1.5f;
}

float CPlayer::ArmorBonus(const UnitTypeData* data) const
{
    return (float)UpgradeFor(data, UPE_ARMOR) * 2.0f;
}

int CPlayer::StructureCount() const
{
    int count = 0;
    for (int t = 0; t < UT_COUNT; ++t)
    {
        if (g_unitTypes[t].flags & UTF_STRUCTURE)
            count += m_typeCount[t] + m_typePending[t];
    }
    return count;
}

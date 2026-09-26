#include "Game.h"

static const float kLifePerStr      = 25.0f;
static const float kRegenPerStr     = 0.05f;
static const float kManaPerInt      = 15.0f;
static const float kManaRegenPerInt = 0.05f;
static const float kArmorPerAgi     = 0.3f;
static const float kSpeedPerAgi     = 0.02f;

CHero::CHero(int type, int owner) : CUnit(type, owner)
{
    m_heroData = GetHeroType(type);
    m_level = 1;
    m_xp = 0;
    m_skillPoints = 1;
    m_tomeStr = 0.0f;
    for (int i = 0; i < 3; ++i)
        m_abilityLevels[i] = 0;
    for (int i = 0; i < kInventorySlots; ++i)
        m_inventory[i] = 0;
}

CHero::~CHero()
{
}

int CHero::Level() const
{
    return m_level;
}

float CHero::Attribute(int attr) const
{
    if (!m_heroData)
        return 0.0f;

    float growth = (float)(m_level - 1);
    float value;
    switch (attr)
    {
    case ATTR_STR:
        value = m_heroData->strBase + m_heroData->strPerLevel * growth + m_tomeStr;
        break;
    case ATTR_AGI:
        value = m_heroData->agiBase + m_heroData->agiPerLevel * growth;
        break;
    default:
        value = m_heroData->intBase + m_heroData->intPerLevel * growth;
        break;
    }

    for (int i = 0; i < kInventorySlots; ++i)
    {
        if (!m_inventory[i])
            continue;
        const ItemTypeData* d = m_inventory[i]->Data();
        if (attr == ATTR_STR)
            value += (float)d->bonusStr;
        else if (attr == ATTR_AGI)
            value += (float)d->bonusAgi;
        else
            value += (float)d->bonusInt;
    }
    return (float)FloorToInt(value);
}

void CHero::AddBonusStats(StatBlock& s)
{
    float str = Attribute(ATTR_STR);
    float agi = Attribute(ATTR_AGI);
    float intel = Attribute(ATTR_INT);

    s.maxLife += str * kLifePerStr;
    s.lifeRegen += str * kRegenPerStr;
    s.maxMana += intel * kManaPerInt;
    s.manaRegen += intel * kManaRegenPerInt;
    s.armor += agi * kArmorPerAgi - 2.0f;
    s.attackSpeed *= 1.0f + agi * kSpeedPerAgi;
    if (m_heroData)
        s.damageBase += Attribute(m_heroData->primary);

    for (int i = 0; i < kInventorySlots; ++i)
    {
        if (!m_inventory[i])
            continue;
        const ItemTypeData* d = m_inventory[i]->Data();
        s.damageFlat += (float)d->bonusDamage;
        s.armor += (float)d->bonusArmorX10 * 0.1f;
        s.maxLife += (float)d->bonusLife;
        s.moveSpeed += (float)d->bonusMoveSpeed;
        s.attackSpeed *= 1.0f + (float)d->bonusAttackSpeedPct * 0.01f;
    }
}

void CHero::Update(float dt)
{
    // Dead heroes stay in the world (hidden) until revived at an altar.
    if (!IsAlive())
    {
        UpdateBuffs(0.0f);
        return;
    }
    CUnit::Update(dt);
}

void CHero::Kill(CUnit* killer)
{
    if (!IsAlive())
        return;
    CUnit::Kill(killer);
    m_state |= US_HIDDEN;
    m_corpseTimer = 0.0f;
}

bool CHero::HandleImmediateOrder(const COrder& order)
{
    switch (order.id)
    {
    case ORD_LEARN:
        LearnAbility(order.param);
        return true;
    case ORD_USE_ITEM:
        UseItem(order.param);
        return true;
    default:
        return CUnit::HandleImmediateOrder(order);
    }
}

void CHero::AddExperience(int xp)
{
    if (m_level >= kMaxHeroLevel || xp <= 0)
        return;
    m_xp += xp;
    while (m_level < kMaxHeroLevel && m_xp >= g_heroXpNeeded[m_level + 1])
        LevelUp();
}

void CHero::LevelUp()
{
    ++m_level;
    ++m_skillPoints;
    RecalcStats();
    g_game->m_events.Fire(EVT_HERO_LEVEL, this, 0, (float)m_level, 0);
}

bool CHero::CanLearn(int slot) const
{
    if (slot < 0 || slot > 2 || m_skillPoints <= 0 || !m_heroData)
        return false;
    int current = m_abilityLevels[slot];
    if (current >= 3)
        return false;
    return m_level >= current * 2 + 1;
}

bool CHero::LearnAbility(int slot)
{
    if (!CanLearn(slot))
        return false;

    ++m_abilityLevels[slot];
    --m_skillPoints;
    int type = m_heroData->abilities[slot];
    CAbility* a = FindAbility(type);
    if (a)
        a->m_level = m_abilityLevels[slot];
    else
        AddAbility(type, m_abilityLevels[slot]);
    m_state |= US_STATS_DIRTY;
    return true;
}

int CHero::FreeInventorySlot() const
{
    for (int i = 0; i < kInventorySlots; ++i)
    {
        if (!m_inventory[i])
            return i;
    }
    return -1;
}

static void RestoreManaNearby(CUnit* unit, void* context)
{
    CHero* hero = static_cast<CHero*>(context);
    if (unit->IsAlive() && hero->IsAllyOf(unit))
        unit->RestoreMana((float)GetItemType(IT_RUNE_MANA)->useAmount);
}

static void HealNearby(CUnit* unit, void* context)
{
    CHero* hero = static_cast<CHero*>(context);
    if (unit->IsAlive() && hero->IsAllyOf(unit) && !unit->IsStructure())
        unit->Heal((float)GetItemType(IT_SCROLL_HEAL)->useAmount);
}

bool CHero::PickupItem(CItem* item)
{
    if (!item || !item->IsAlive() || item->m_carrier)
        return false;

    const ItemTypeData* d = item->Data();
    if (d->itemClass == ITC_POWERUP)
    {
        switch (item->m_itemType)
        {
        case IT_TOME_XP:
            AddExperience(d->useAmount);
            break;
        case IT_TOME_STR:
            m_tomeStr += (float)d->useAmount;
            m_state |= US_STATS_DIRTY;
            break;
        case IT_RUNE_MANA:
            g_game->m_grid.Visit(m_pos, (float)d->useArea, RestoreManaNearby, this);
            break;
        default:
            break;
        }
        item->Kill(this);
        return true;
    }

    int slot = FreeInventorySlot();
    if (slot < 0)
        return false;
    m_inventory[slot] = item;
    item->m_carrier = m_handle;
    item->m_state |= US_HIDDEN;
    m_state |= US_STATS_DIRTY;
    g_game->m_events.Fire(EVT_ITEM_PICKUP, this, 0, 0.0f, item->m_itemType);
    return true;
}

bool CHero::UseItem(int slot)
{
    if (slot < 0 || slot >= kInventorySlots || !m_inventory[slot])
        return false;
    CItem* item = m_inventory[slot];
    const ItemTypeData* d = item->Data();
    if (d->itemClass != ITC_CHARGED || item->m_charges == 0)
        return false;

    switch (item->m_itemType)
    {
    case IT_HEAL_POTION:
        Heal((float)d->useAmount);
        break;
    case IT_MANA_POTION:
        RestoreMana((float)d->useAmount);
        break;
    case IT_SCROLL_HEAL:
        g_game->m_grid.Visit(m_pos, (float)d->useArea, HealNearby, this);
        break;
    default:
        return false;
    }

    if (--item->m_charges == 0)
    {
        m_inventory[slot] = 0;
        item->Kill(this);
        m_state |= US_STATS_DIRTY;
    }
    return true;
}

int CHero::ReviveCost() const
{
    return (int)((float)m_data->goldCost * (0.4f + 0.1f * (float)m_level));
}

void CHero::Revive(const Vec2& at)
{
    if (IsAlive())
        return;

    m_state &= ~(US_DEAD | US_HIDDEN | US_REMOVE | US_STUNNED | US_INVISIBLE);
    m_stunCount = 0;
    m_invulnCount = 0;
    m_corpseTimer = 0.0f;
    ClearOrders();
    m_pos = at;
    m_orderOrigin = at;
    g_game->m_grid.Insert(this);
    RecalcStats();
    m_life = m_maxLife;
    m_mana = m_stats.maxMana;

    CPlayer& p = g_game->m_players[m_owner];
    ++p.m_typeCount[m_type];
    p.m_foodUsed += m_data->food;
}

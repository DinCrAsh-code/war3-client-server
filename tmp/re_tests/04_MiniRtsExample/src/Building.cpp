#include "Game.h"

static const float kReviveBaseTime  = 20.0f;
static const float kReviveLevelTime = 5.0f;
static const float kMineVisitTime   = 0.8f;

CBuilding::CBuilding(int type, int owner) : CUnit(type, owner)
{
    m_queueCount = 0;
    m_buildProgress = 1.0f;
    m_buildTime = (float)m_data->buildTimeSec;
    m_builder = 0;
    m_hasRally = false;
    m_facing = kPi * 1.5f;
}

CBuilding::~CBuilding()
{
}

float CBuilding::CollisionRadius() const
{
    return (float)m_data->footprint * kCellSize * 0.5f;
}

void CBuilding::Update(float dt)
{
    if (!IsAlive())
    {
        CUnit::Update(dt);
        return;
    }

    if (m_state & US_CONSTRUCTING)
    {
        if (m_state & US_STATS_DIRTY)
            RecalcStats();
        UpdateConstruction(dt);
        return;
    }

    CUnit::Update(dt);
    UpdateQueue(dt);
}

void CBuilding::BeginConstruction(CUnit* builder)
{
    m_state |= US_CONSTRUCTING;
    m_buildProgress = 0.0f;
    m_life = MaxF(1.0f, m_maxLife * 0.1f);
    m_builder = builder ? builder->m_handle : 0;
    if (builder)
    {
        builder->m_state |= US_INSIDE | US_HIDDEN;
        builder->StopMoving();
        g_game->m_grid.Remove(builder);
    }
}

void CBuilding::UpdateConstruction(float dt)
{
    float step = dt / MaxF(m_buildTime, 1.0f);
    m_buildProgress += step;
    m_life = MinF(m_maxLife, m_life + m_maxLife * 0.9f * step);
    if (m_buildProgress >= 1.0f)
        FinishConstruction();
}

static void ReleaseWorker(CBuilding* building, CUnit* worker)
{
    worker->m_state &= ~(US_INSIDE | US_HIDDEN);
    Vec2 spot;
    if (!building->FindSpawnPoint(spot))
        spot = building->m_pos;
    worker->m_pos = spot;
    worker->m_orderOrigin = spot;
    if (worker->IsAlive())
        g_game->m_grid.Insert(worker);
}

void CBuilding::FinishConstruction()
{
    m_state &= ~US_CONSTRUCTING;
    m_buildProgress = 1.0f;
    g_game->m_players[m_owner].OnUnitCompleted(this);

    CUnit* builder = g_game->GetUnit(m_builder);
    m_builder = 0;
    if (builder)
        ReleaseWorker(this, builder);

    m_state |= US_STATS_DIRTY;
    g_game->m_events.Fire(EVT_BUILD_FINISHED, this, builder, 0.0f, m_type);
}

bool CBuilding::FindSpawnPoint(Vec2& out) const
{
    int size = m_data->footprint;
    int cx = CTerrain::WorldToCell(m_pos.x);
    int cy = CTerrain::WorldToCell(m_pos.y) - size / 2 - 1;
    if (!g_game->m_terrain.FindNearestWalkable(cx, cy, size + 4))
        return false;
    out = Vec2(CTerrain::CellCenter(cx), CTerrain::CellCenter(cy));
    return true;
}

bool CBuilding::EnqueueTrain(int unitType)
{
    if ((m_state & US_CONSTRUCTING) || m_queueCount >= kTrainQueueSize)
        return false;

    const UnitTypeData* d = GetUnitType(unitType);
    if (d->producedBy != m_type)
        return false;

    CPlayer& p = g_game->m_players[m_owner];
    if (!p.MeetsRequirements(d))
        return false;
    if ((d->flags & UTF_HERO) && p.m_heroCount + p.m_typePending[unitType] >= 3)
        return false;
    if ((d->flags & UTF_HERO) && p.CountIncludingPending(unitType) > 0)
        return false;
    if (!p.Spend(d->goldCost, d->lumberCost))
        return false;

    QueueEntry& e = m_trainQueue[m_queueCount++];
    e.kind = QK_TRAIN;
    e.type = (u8)unitType;
    e.foodReserved = 0;
    e.progress = 0.0f;
    e.total = (float)d->buildTimeSec;
    e.hero = 0;
    ++p.m_typePending[unitType];
    return true;
}

bool CBuilding::EnqueueResearch(int upgrade)
{
    if ((m_state & US_CONSTRUCTING) || m_queueCount >= kTrainQueueSize || upgrade < 0 || upgrade >= UP_COUNT)
        return false;

    const UpgradeData& u = g_upgrades[upgrade];
    if (u.researchedAt != m_type)
        return false;

    CPlayer& p = g_game->m_players[m_owner];
    int level = p.m_upgradeLevel[upgrade] + p.m_upgradePending[upgrade];
    if (level >= u.levels)
        return false;
    int gold = u.goldBase + u.goldPerLevel * level;
    int lumber = u.lumberBase + u.lumberPerLevel * level;
    if (!p.Spend(gold, lumber))
        return false;

    QueueEntry& e = m_trainQueue[m_queueCount++];
    e.kind = QK_RESEARCH;
    e.type = (u8)upgrade;
    e.foodReserved = 0;
    e.progress = 0.0f;
    e.total = (float)u.timeSec;
    e.hero = 0;
    ++p.m_upgradePending[upgrade];
    return true;
}

bool CBuilding::EnqueueRevive(CHero* hero)
{
    if (!(m_data->flags & UTF_ALTAR) || !hero || hero->IsAlive() || hero->m_owner != m_owner)
        return false;
    if ((m_state & US_CONSTRUCTING) || m_queueCount >= kTrainQueueSize)
        return false;
    for (int i = 0; i < m_queueCount; ++i)
    {
        if (m_trainQueue[i].kind == QK_REVIVE && m_trainQueue[i].hero == hero->m_handle)
            return false;
    }

    CPlayer& p = g_game->m_players[m_owner];
    if (!p.Spend(hero->ReviveCost(), 0))
        return false;

    QueueEntry& e = m_trainQueue[m_queueCount++];
    e.kind = QK_REVIVE;
    e.type = hero->m_type;
    e.foodReserved = 0;
    e.progress = 0.0f;
    e.total = kReviveBaseTime + kReviveLevelTime * (float)hero->m_level;
    e.hero = hero->m_handle;
    return true;
}

void CBuilding::CancelEntry(int index)
{
    if (index < 0 || index >= m_queueCount)
        return;

    QueueEntry& e = m_trainQueue[index];
    CPlayer& p = g_game->m_players[m_owner];
    if (e.kind == QK_TRAIN)
    {
        const UnitTypeData* d = GetUnitType(e.type);
        p.Refund(d->goldCost, d->lumberCost);
        if (e.foodReserved)
            p.ReleaseFood(d->food);
        --p.m_typePending[e.type];
    }
    else if (e.kind == QK_RESEARCH)
    {
        const UpgradeData& u = g_upgrades[e.type];
        int level = p.m_upgradeLevel[e.type] + p.m_upgradePending[e.type] - 1;
        p.Refund(u.goldBase + u.goldPerLevel * level, u.lumberBase + u.lumberPerLevel * level);
        --p.m_upgradePending[e.type];
    }

    for (int i = index + 1; i < m_queueCount; ++i)
        m_trainQueue[i - 1] = m_trainQueue[i];
    --m_queueCount;
}

void CBuilding::UpdateQueue(float dt)
{
    if (m_queueCount == 0)
        return;

    QueueEntry& e = m_trainQueue[0];
    if (e.kind == QK_TRAIN && !e.foodReserved)
    {
        int food = GetUnitType(e.type)->food;
        CPlayer& p = g_game->m_players[m_owner];
        if (food > 0 && !p.HasFood(food))
            return;
        p.ReserveFood(food);
        e.foodReserved = 1;
    }

    e.progress += dt;
    if (e.progress >= e.total)
        CompleteEntry();
}

void CBuilding::CompleteEntry()
{
    QueueEntry e = m_trainQueue[0];
    for (int i = 1; i < m_queueCount; ++i)
        m_trainQueue[i - 1] = m_trainQueue[i];
    --m_queueCount;

    CPlayer& p = g_game->m_players[m_owner];
    Vec2 spot;
    if (!FindSpawnPoint(spot))
        spot = m_pos;

    switch (e.kind)
    {
    case QK_TRAIN:
        {
            --p.m_typePending[e.type];
            if (e.foodReserved)
                p.ReleaseFood(GetUnitType(e.type)->food);
            CUnit* u = g_game->CreateUnit(e.type, m_owner, spot, m_facing);
            if (!u)
                break;
            if (m_hasRally)
            {
                COrder o;
                o.id = ORD_MOVE;
                o.flags = OF_TARGET_POINT;
                o.param = 0;
                o.target = 0;
                o.point = m_rally;
                u->IssueOrder(o, false);
            }
            g_game->m_events.Fire(EVT_UNIT_TRAINED, u, this, 0.0f, e.type);
        }
        break;

    case QK_RESEARCH:
        --p.m_upgradePending[e.type];
        ++p.m_upgradeLevel[e.type];
        for (int i = 0; i < g_game->UnitCount(); ++i)
        {
            CUnit* u = g_game->UnitAt(i);
            if (u->m_owner == m_owner)
                u->m_state |= US_STATS_DIRTY;
        }
        g_game->m_events.Fire(EVT_RESEARCH_DONE, this, 0, 0.0f, e.type);
        break;

    case QK_REVIVE:
        {
            CUnit* u = g_game->GetUnit(e.hero);
            if (u && u->IsHero())
                static_cast<CHero*>(u)->Revive(spot);
        }
        break;

    default:
        break;
    }
}

bool CBuilding::HandleImmediateOrder(const COrder& order)
{
    switch (order.id)
    {
    case ORD_TRAIN:
        EnqueueTrain(order.param);
        return true;
    case ORD_RESEARCH:
        EnqueueResearch(order.param);
        return true;
    case ORD_REVIVE:
        {
            CUnit* u = g_game->GetUnit(order.target);
            if (u && u->IsHero())
                EnqueueRevive(static_cast<CHero*>(u));
        }
        return true;
    case ORD_RALLY:
        m_rally = order.point;
        m_hasRally = true;
        return true;
    case ORD_CANCEL:
        CancelEntry(m_queueCount - 1);
        return true;
    default:
        return CUnit::HandleImmediateOrder(order);
    }
}

void CBuilding::Kill(CUnit* killer)
{
    if (!IsAlive())
        return;

    // Queued work is lost with the building; only reserved food and the
    // pending counters are unwound.
    CPlayer& p = g_game->m_players[m_owner];
    for (int i = 0; i < m_queueCount; ++i)
    {
        QueueEntry& e = m_trainQueue[i];
        if (e.kind == QK_TRAIN)
        {
            --p.m_typePending[e.type];
            if (e.foodReserved)
                p.ReleaseFood(GetUnitType(e.type)->food);
        }
        else if (e.kind == QK_RESEARCH)
        {
            --p.m_upgradePending[e.type];
        }
    }
    m_queueCount = 0;

    CUnit* builder = g_game->GetUnit(m_builder);
    m_builder = 0;
    if (builder)
        ReleaseWorker(this, builder);

    CUnit::Kill(killer);
}

// ---------------------------------------------------------------------------

CGoldMine::CGoldMine(int owner, int gold) : CBuilding(UT_GOLDMINE, owner)
{
    m_gold = gold;
    m_occupant = 0;
    m_occupyTimer = 0.0f;
    m_workersAssigned = 0;
}

CGoldMine::~CGoldMine()
{
}

void CGoldMine::Update(float dt)
{
    CBuilding::Update(dt);
    if (!IsAlive() || !m_occupant)
        return;

    m_occupyTimer -= dt;
    if (m_occupyTimer <= 0.0f)
        ReleaseOccupant();
}

bool CGoldMine::TryEnter(CUnit* worker)
{
    if (!IsAlive() || m_gold <= 0)
        return false;
    if (m_occupant && g_game->GetUnit(m_occupant))
        return false;

    m_occupant = worker->m_handle;
    m_occupyTimer = kMineVisitTime;
    worker->m_state |= US_INSIDE | US_HIDDEN;
    worker->StopMoving();
    g_game->m_grid.Remove(worker);
    return true;
}

void CGoldMine::ReleaseOccupant()
{
    CUnit* worker = g_game->GetUnit(m_occupant);
    m_occupant = 0;
    if (!worker)
        return;

    int trip = (int)GetAbilityType(AB_HARVEST)->valueA[0];
    int amount = m_gold < trip ? m_gold : trip;
    m_gold -= amount;

    ReleaseWorker(this, worker);
    CAbility* a = worker->FindAbility(AB_HARVEST);
    if (a)
    {
        CAbilHarvest* h = static_cast<CAbilHarvest*>(a);
        h->m_cargo = (u8)amount;
        h->m_cargoKind = 1;
        h->m_state = CAbilHarvest::HS_RETURNING;
    }

    if (m_gold <= 0)
        Kill(0);
}

void CGoldMine::Kill(CUnit* killer)
{
    if (!IsAlive())
        return;
    if (m_occupant)
    {
        CUnit* worker = g_game->GetUnit(m_occupant);
        m_occupant = 0;
        if (worker)
            ReleaseWorker(this, worker);
    }
    CBuilding::Kill(killer);
}

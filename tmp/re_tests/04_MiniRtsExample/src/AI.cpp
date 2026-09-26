#include "Game.h"

// Computer player. Runs twice a second and only ever acts through the
// lockstep command queue, like a human player would.

static const int   kWorkersPerMine    = 6;
static const float kMineAssignRange   = 1400.0f;
static const float kHeroThreatRange   = 800.0f;
static const float kDefendRadius      = 1000.0f;
static const float kRetreatRatio      = 1.4f;
static const float kArmyReorderDelay  = 6.0f;
static const float kMicroDelay        = 2.0f;
static const int   kRecentTicks       = 30;
static const int   kMaxWorkers        = 22;

enum RecentKey
{
    RK_LEARN_BASE = UT_COUNT + UP_COUNT,
    RK_REVIVE     = RK_LEARN_BASE + 3,
    RK_COUNT      = RK_REVIVE + 3
};

void CAIController::Init(int player)
{
    m_player = player;
    m_step = 0;
    m_loopCount = 0;
    m_state = AI_BUILDUP;
    m_stateTime = 0.0f;
    m_waves = 0;
    m_campTarget = -1;
    m_underAttack = false;
    m_threatTime = -100.0f;
    m_minAttackFood = 0;
    m_lastArmyOrder = -100.0f;
    m_lastMicro = 0.0f;
    m_home = g_game->m_players[player].m_start;
    for (int i = 0; i < (int)COUNTOF(m_recent); ++i)
        m_recent[i] = -1000;
    g_game->m_events.Register(EVT_UNIT_DAMAGED, OnUnitDamaged, this);
}

bool CAIController::IsRecent(int key) const
{
    return g_game->m_tick - m_recent[key] < kRecentTicks;
}

void CAIController::MarkRecent(int key)
{
    m_recent[key] = g_game->m_tick;
}

void CAIController::OnUnitDamaged(const EventData& e, void* context)
{
    CAIController* ai = static_cast<CAIController*>(context);
    CUnit* victim = e.unit;
    CUnit* attacker = e.other;
    if (!victim || !attacker || victim->m_owner != ai->m_player)
        return;
    if (!victim->IsStructure() && !victim->IsWorker())
        return;
    if (Distance(victim->m_pos, ai->m_home) > kDefendRadius)
        return;
    ai->m_underAttack = true;
    ai->m_threatPos = attacker->m_pos;
    ai->m_threatTime = g_game->Time();
}

// ---------------------------------------------------------------------------
// Command helpers

void CAIController::SendOrderEx(CUnit* unit, int orderId, int flags, Handle target, const Vec2& p, int param)
{
    Command cmd;
    cmd.type = CMD_ORDER;
    cmd.player = (u8)m_player;
    cmd.numUnits = 1;
    cmd.queued = 0;
    cmd.units[0] = unit->m_handle;
    cmd.order.id = (u8)orderId;
    cmd.order.flags = (u8)flags;
    cmd.order.param = (u16)param;
    cmd.order.target = target;
    cmd.order.point = p;
    g_game->m_commands.Queue(cmd);
}

void CAIController::SendOrder(CUnit* unit, int orderId, Handle target, const Vec2& p, int param)
{
    int flags = target ? OF_TARGET_UNIT : OF_TARGET_POINT;
    if (orderId == ORD_PICKUP)
        flags = OF_TARGET_ITEM;
    else if (orderId == ORD_TRAIN || orderId == ORD_RESEARCH || orderId == ORD_USE_ITEM || orderId == ORD_STOP)
        flags = 0;
    SendOrderEx(unit, orderId, flags, target, p, param);
}

void CAIController::SendGroupOrder(CUnit** units, int count, int orderId, Handle target, const Vec2& p)
{
    int i = 0;
    while (i < count)
    {
        Command cmd;
        cmd.type = CMD_ORDER;
        cmd.player = (u8)m_player;
        cmd.queued = 0;
        cmd.numUnits = 0;
        while (i < count && cmd.numUnits < kMaxCommandUnits)
            cmd.units[cmd.numUnits++] = units[i++]->m_handle;
        cmd.order.id = (u8)orderId;
        cmd.order.flags = (u8)(target ? OF_TARGET_UNIT : OF_TARGET_POINT);
        cmd.order.param = 0;
        cmd.order.target = target;
        cmd.order.point = p;
        g_game->m_commands.Queue(cmd);
    }
}

void CAIController::OrderArmy(int orderId, Handle target, const Vec2& p)
{
    CUnit* group[96 + 3];
    int n = 0;
    for (int i = 0; i < m_numArmy; ++i)
        group[n++] = m_army[i];
    for (int i = 0; i < m_numHeroes; ++i)
    {
        if (m_heroes[i]->IsAlive())
            group[n++] = m_heroes[i];
    }
    if (n > 0)
        SendGroupOrder(group, n, orderId, target, p);
    m_lastArmyOrder = g_game->Time();
}

// ---------------------------------------------------------------------------

void CAIController::ScanUnits()
{
    m_numWorkers = 0;
    m_numArmy = 0;
    m_armyFood = 0;
    m_numHeroes = 0;
    m_numBuildings = 0;
    m_numHalls = 0;
    m_idleWorkers = 0;

    for (int i = 0; i < g_game->UnitCount(); ++i)
    {
        CUnit* u = g_game->UnitAt(i);
        if (u->m_owner != m_player)
            continue;

        if (u->IsHero())
        {
            if (m_numHeroes < (int)COUNTOF(m_heroes))
                m_heroes[m_numHeroes++] = static_cast<CHero*>(u);
            continue;
        }
        if (!u->IsAlive())
            continue;

        if (u->IsStructure())
        {
            if (m_numBuildings < (int)COUNTOF(m_buildings))
                m_buildings[m_numBuildings++] = u;
            if ((u->m_data->flags & UTF_TOWNHALL) && !(u->m_state & US_CONSTRUCTING) && m_numHalls < (int)COUNTOF(m_halls))
                m_halls[m_numHalls++] = u;
        }
        else if (u->IsWorker())
        {
            if (m_numWorkers < (int)COUNTOF(m_workers))
                m_workers[m_numWorkers++] = u;
            if (u->m_order.id == ORD_NONE && !(u->m_state & US_INSIDE))
                ++m_idleWorkers;
        }
        else if (!(u->m_state & US_HIDDEN))
        {
            if (m_numArmy < (int)COUNTOF(m_army))
                m_army[m_numArmy++] = u;
            m_armyFood += MaxI(u->m_data->food, 1);
        }
    }

    if (m_numHalls > 0)
        m_home = m_halls[0]->m_pos;
}

void CAIController::ManageWorkers()
{
    CPlayer& p = g_game->m_players[m_player];

    for (int i = 0; i < m_numWorkers; ++i)
    {
        CUnit* w = m_workers[i];
        if (w->m_order.id != ORD_NONE || (w->m_state & US_INSIDE))
            continue;

        CGoldMine* mine = g_game->FindNearestMine(w->m_pos, kMineAssignRange);
        bool nearHall = false;
        if (mine)
        {
            for (int h = 0; h < m_numHalls; ++h)
            {
                if (Distance(m_halls[h]->m_pos, mine->m_pos) < 900.0f)
                    nearHall = true;
            }
        }

        if (mine && nearHall && mine->m_workersAssigned < kWorkersPerMine)
        {
            SendOrder(w, ORD_HARVEST, mine->m_handle, mine->m_pos, 0);
            ++mine->m_workersAssigned;
            continue;
        }

        // Trees near the base are the safe ones (creep camps sit further out).
        CDestructable* tree = g_game->FindNearestTree(m_home, 1600.0f);
        if (tree)
            SendOrder(w, ORD_HARVEST, tree->m_handle, tree->m_pos, 0);
    }

    // Keep the worker count topped up once the scripted opening is over.
    int wanted = MinI(kMaxWorkers, 10 + 6 * m_numHalls);
    if (p.m_lumber > 1000)
        wanted = MinI(wanted, kWorkersPerMine * m_numHalls + 4);
    int workerType = WorkerTypeForRace(p.m_race);
    if (m_step > 12 && p.CountIncludingPending(workerType) < wanted)
        TryTrain(workerType);

    // Expand when the home mine is running dry.
    CGoldMine* homeMine = g_game->FindNearestMine(m_home, 900.0f);
    int hallType = TownHallTypeForRace(p.m_race);
    if ((!homeMine || homeMine->m_gold < 3000) && m_numHalls < 3 && m_numArmy >= 4 &&
        p.m_typePending[hallType] + BuildersEnRoute(hallType) == 0)
    {
        TryBuild(hallType, true);
    }
}

void CAIController::ManageFood()
{
    CPlayer& p = g_game->m_players[m_player];
    int cap = MinI(p.m_foodCap, 100);
    if (cap >= 100)
        return;

    int farm = FarmTypeForRace(p.m_race);
    int provided = -GetUnitType(farm)->food;
    int pending = p.m_typePending[farm];
    for (int i = 0; i < m_numWorkers; ++i)
    {
        if (m_workers[i]->m_order.id == ORD_BUILD && m_workers[i]->m_order.param == farm)
            ++pending;
    }

    int headroom = cap + pending * provided - p.m_foodUsed;
    if (headroom <= 4 + m_numBuildings / 4 && pending < 2)
        TryBuild(farm, false);
}

int CAIController::BuildersEnRoute(int type) const
{
    int count = 0;
    for (int i = 0; i < m_numWorkers; ++i)
    {
        const CUnit* w = m_workers[i];
        if (w->m_order.id == ORD_BUILD && w->m_order.param == type)
        {
            const CAbility* a = w->FindAbility(AB_BUILD);
            if (!a || !static_cast<const CAbilBuild*>(a)->m_building)
                ++count;
        }
    }
    return count + (IsRecent(type) ? 1 : 0);
}

bool CAIController::ExecuteStep(const AiBuildStep& step, int bonus)
{
    CPlayer& p = g_game->m_players[m_player];
    switch (step.op)
    {
    case AIS_BUILD:
        if (p.CountIncludingPending(step.type) + BuildersEnRoute(step.type) >= step.count)
            return true;
        TryBuild(step.type, false);
        return false;

    case AIS_EXPAND:
        if (p.CountIncludingPending(step.type) + BuildersEnRoute(step.type) >= step.count)
            return true;
        {
            Vec2 spot;
            if (!FindBuildSpot(step.type, true, spot))
                return true;
        }
        TryBuild(step.type, true);
        return false;

    case AIS_TRAIN:
        if (p.CountIncludingPending(step.type) >= step.count + bonus)
            return true;
        // Food-capped: let the rest of the list run instead of stalling.
        if (p.m_foodCap >= 100 && !p.HasFood(GetUnitType(step.type)->food))
            return true;
        TryTrain(step.type);
        return false;

    case AIS_HERO:
        if (p.m_heroCount >= step.count || p.CountIncludingPending(step.type) > 0)
            return true;
        TryTrain(step.type);
        return false;

    case AIS_RESEARCH:
        if (p.m_upgradeLevel[step.type] + p.m_upgradePending[step.type] >= step.count)
            return true;
        TryResearch(step.type);
        return false;

    case AIS_ATTACK:
        m_minAttackFood = step.condition;
        return true;

    default:
        return true;
    }
}

// War3-style build list: every pass walks the whole list from the top, so
// anything lost (a dead barracks, a failed build) is redone before newer
// steps. The first unsatisfied step blocks the rest so resources are spent
// in list order. Reaching the loop marker raises army targets for the
// steps after the loop point, which keeps late-game production going.
void CAIController::ManageBuildOrder()
{
    CPlayer& p = g_game->m_players[m_player];
    const AiBuildStep* steps = p.m_race == RACE_A ? g_aiBuildOrderA : g_aiBuildOrderB;
    int length = p.m_race == RACE_A ? g_aiBuildOrderLenA : g_aiBuildOrderLenB;
    int loopStart = steps[length - 1].op == AIS_LOOP ? steps[length - 1].condition : length;

    for (int i = 0; i < length; ++i)
    {
        const AiBuildStep& s = steps[i];
        if (s.op == AIS_LOOP)
        {
            if (m_loopCount < 20)
                ++m_loopCount;
            m_step = i;
            return;
        }

        int bonus = 0;
        if (i >= loopStart && s.op == AIS_TRAIN && !(GetUnitType(s.type)->flags & UTF_WORKER))
            bonus = m_loopCount * 2;
        if (!ExecuteStep(s, bonus))
        {
            m_step = i;
            return;
        }
    }
    m_step = length;
}

bool CAIController::TryTrain(int type)
{
    if (IsRecent(type))
        return false;

    CPlayer& p = g_game->m_players[m_player];
    const UnitTypeData* d = GetUnitType(type);
    if (!p.MeetsRequirements(d) || !p.CanAfford(d->goldCost, d->lumberCost))
        return false;
    if (d->food > 0 && !p.HasFood(d->food))
        return false;

    CBuilding* best = 0;
    for (int i = 0; i < m_numBuildings; ++i)
    {
        CUnit* b = m_buildings[i];
        if (b->m_type != d->producedBy || (b->m_state & US_CONSTRUCTING))
            continue;
        CBuilding* building = static_cast<CBuilding*>(b);
        if (building->QueueLength() >= 2)
            continue;
        if (!best || building->QueueLength() < best->QueueLength())
            best = building;
    }
    if (!best)
        return false;

    SendOrder(best, ORD_TRAIN, 0, best->m_pos, type);
    MarkRecent(type);
    return true;
}

bool CAIController::TryResearch(int upgrade)
{
    int key = UT_COUNT + upgrade;
    if (IsRecent(key))
        return false;

    CPlayer& p = g_game->m_players[m_player];
    const UpgradeData& u = g_upgrades[upgrade];
    int level = p.m_upgradeLevel[upgrade] + p.m_upgradePending[upgrade];
    if (!p.CanAfford(u.goldBase + u.goldPerLevel * level, u.lumberBase + u.lumberPerLevel * level))
        return false;

    for (int i = 0; i < m_numBuildings; ++i)
    {
        CUnit* b = m_buildings[i];
        if (b->m_type != u.researchedAt || (b->m_state & US_CONSTRUCTING))
            continue;
        if (static_cast<CBuilding*>(b)->QueueLength() >= 2)
            continue;
        SendOrder(b, ORD_RESEARCH, 0, b->m_pos, upgrade);
        MarkRecent(key);
        return true;
    }
    return false;
}

bool CAIController::TryBuild(int type, bool expansion)
{
    if (IsRecent(type))
        return false;

    CPlayer& p = g_game->m_players[m_player];
    const UnitTypeData* d = GetUnitType(type);
    if (!p.MeetsRequirements(d) || !p.CanAfford(d->goldCost, d->lumberCost))
        return false;

    Vec2 spot;
    if (!FindBuildSpot(type, expansion, spot))
        return false;
    CUnit* builder = PickBuilder(spot);
    if (!builder)
        return false;

    SendOrder(builder, ORD_BUILD, 0, spot, type);
    MarkRecent(type);
    return true;
}

CUnit* CAIController::PickBuilder(const Vec2& spot) const
{
    CUnit* best = 0;
    float bestScore = 1e9f;
    for (int i = 0; i < m_numWorkers; ++i)
    {
        CUnit* w = m_workers[i];
        if ((w->m_state & US_INSIDE) || w->m_order.id == ORD_BUILD)
            continue;

        float score = Distance(w->m_pos, spot);
        // Prefer lumber workers and idle workers over gold miners.
        CWidget* target = g_game->GetWidget(w->m_order.target);
        if (w->m_order.id == ORD_HARVEST && target && target->m_kind == WK_UNIT)
            score += 600.0f;
        if (w->m_order.id == ORD_NONE)
            score -= 300.0f;
        if (score < bestScore)
        {
            bestScore = score;
            best = w;
        }
    }
    return best;
}

// Spiral search around the base (or an expansion mine) for a spot with a
// one-cell margin that keeps clear of the hall-to-mine worker lane.
bool CAIController::FindBuildSpot(int type, bool expansion, Vec2& out) const
{
    const UnitTypeData* d = GetUnitType(type);
    CTerrain& terrain = g_game->m_terrain;
    int size = d->footprint;
    Vec2 center(kMapSize * 0.5f, kMapSize * 0.5f);

    Vec2 origin = m_home;
    int minRing = 4;
    int maxRing = 18;
    CGoldMine* mine = g_game->FindNearestMine(m_home, 900.0f);

    if (expansion)
    {
        float bestDist = 1e9f;
        CGoldMine* bestMine = 0;
        for (int i = 0; i < g_game->UnitCount(); ++i)
        {
            CUnit* u = g_game->UnitAt(i);
            if (!(u->m_data->flags & UTF_GOLDMINE) || !u->IsAlive())
                continue;
            bool claimed = false;
            for (int j = 0; j < g_game->UnitCount() && !claimed; ++j)
            {
                CUnit* s = g_game->UnitAt(j);
                if (s->IsAlive() && (s->m_data->flags & UTF_TOWNHALL) && Distance(s->m_pos, u->m_pos) < 800.0f)
                    claimed = true;
            }
            float dist = Distance(u->m_pos, m_home);
            if (!claimed && dist < bestDist)
            {
                bestDist = dist;
                bestMine = static_cast<CGoldMine*>(u);
            }
        }
        if (!bestMine)
            return false;
        mine = bestMine;
        origin = mine->m_pos;
        minRing = 6;
        maxRing = 10;
    }

    Vec2 hallPos = expansion ? origin : m_home;
    Vec2 toCenter = Normalize(center - m_home);
    int ox = CTerrain::WorldToCell(origin.x);
    int oy = CTerrain::WorldToCell(origin.y);

    for (int pass = 0; pass < 2; ++pass)
    {
        for (int r = minRing; r <= maxRing; ++r)
        {
            for (int k = 0; k < r * 8; ++k)
            {
                int side = k / (r * 2);
                int t = k % (r * 2) - r;
                int cx = ox;
                int cy = oy;
                switch (side)
                {
                case 0: cx += t; cy -= r; break;
                case 1: cx += r; cy += t; break;
                case 2: cx -= t; cy += r; break;
                default: cx -= r; cy -= t; break;
                }

                if (!terrain.CanBuildFootprint(cx, cy, size + 2))
                    continue;
                Vec2 spot = FootprintCenter(cx, cy, size);

                if (mine && !expansion)
                {
                    Vec2 lane = ClosestPointOnSegment(hallPos, mine->m_pos, spot);
                    if (Distance(lane, spot) < 4.0f * kCellSize + (float)size * 16.0f)
                        continue;
                }
                if (expansion && Distance(spot, mine->m_pos) < 5.0f * kCellSize)
                    continue;

                // First pass is picky about direction: defenses face the
                // map center, farms hide behind the base.
                if (pass == 0 && !expansion)
                {
                    float facing = (spot - m_home).Dot(toCenter);
                    if ((d->flags & UTF_ATTACKS_GND) && facing < 0.0f)
                        continue;
                    if (d->food < 0 && !(d->flags & UTF_TOWNHALL) && facing > 64.0f)
                        continue;
                }

                out = spot;
                return true;
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Army evaluation

float CAIController::EstimateStrength(const CUnit* unit) const
{
    if (!unit->IsAlive() || unit->m_data->weaponType == WPN_NONE)
        return 0.0f;
    return unit->m_life * (unit->m_stats.damageBase + 5.0f) / MaxF(unit->m_stats.cooldown, 0.5f);
}

float CAIController::ArmyStrength() const
{
    float total = 0.0f;
    for (int i = 0; i < m_numArmy; ++i)
        total += EstimateStrength(m_army[i]);
    for (int i = 0; i < m_numHeroes; ++i)
        total += EstimateStrength(m_heroes[i]) * 1.5f;
    return total;
}

float CAIController::EnemyStrengthNear(const Vec2& p, float radius) const
{
    FilterContext ctx;
    ctx.self = 0;
    ctx.player = m_player;
    ctx.flags = FF_ENEMY | FF_VISIBLE;
    ctx.minLifeRatio = 0.0f;

    CUnit* enemies[48];
    int n = g_game->m_grid.Collect(p, radius, FilterUnit, &ctx, enemies, 48);
    float total = 0.0f;
    for (int i = 0; i < n; ++i)
        total += EstimateStrength(enemies[i]);
    return total;
}

Vec2 CAIController::ArmyCenter() const
{
    Vec2 sum(0.0f, 0.0f);
    int n = 0;
    for (int i = 0; i < m_numArmy; ++i)
    {
        sum += m_army[i]->m_pos;
        ++n;
    }
    for (int i = 0; i < m_numHeroes; ++i)
    {
        if (m_heroes[i]->IsAlive())
        {
            sum += m_heroes[i]->m_pos;
            ++n;
        }
    }
    if (n == 0)
        return m_home;
    return sum * (1.0f / (float)n);
}

bool CAIController::ChooseAttackTarget(Vec2& out) const
{
    const CPlayer& me = g_game->m_players[m_player];
    float bestDist = 1e9f;
    bool found = false;

    for (int i = 0; i < g_game->UnitCount(); ++i)
    {
        CUnit* u = g_game->UnitAt(i);
        if (!u->IsAlive() || !u->IsStructure() || u->m_owner >= kMaxRealPlayers || !me.IsEnemy(u->m_owner))
            continue;
        if (!g_game->m_fog.IsExplored(m_player, u->m_pos))
            continue;
        float d = Distance(u->m_pos, m_home);
        if (d < bestDist)
        {
            bestDist = d;
            out = u->m_pos;
            found = true;
        }
    }
    if (found)
        return true;

    for (int p = 0; p < g_game->m_numPlayers; ++p)
    {
        const CPlayer& other = g_game->m_players[p];
        if (p == m_player || other.IsDefeated() || !me.IsEnemy(p))
            continue;
        float d = Distance(other.m_start, m_home);
        if (d < bestDist)
        {
            bestDist = d;
            out = other.m_start;
            found = true;
        }
    }
    return found;
}

void CAIController::SetState(int state)
{
    m_state = state;
    m_stateTime = g_game->Time();
    m_lastArmyOrder = -100.0f;
}

void CAIController::ManageArmy()
{
    float now = g_game->Time();
    float strength = ArmyStrength();
    Vec2 center = ArmyCenter();
    Vec2 mapCenter(kMapSize * 0.5f, kMapSize * 0.5f);
    Vec2 rally = m_home + Normalize(mapCenter - m_home) * 320.0f;
    bool reorder = now - m_lastArmyOrder > kArmyReorderDelay;
    bool threatened = m_underAttack && now - m_threatTime < 8.0f;
    if (!threatened)
        m_underAttack = false;

    int heroLevel = 0;
    bool heroAlive = false;
    for (int i = 0; i < m_numHeroes; ++i)
    {
        if (m_heroes[i]->IsAlive())
        {
            heroAlive = true;
            heroLevel = MaxI(heroLevel, m_heroes[i]->m_level);
        }
    }

    if (threatened && m_state != AI_DEFENDING && m_state != AI_ATTACKING)
        SetState(AI_DEFENDING);

    switch (m_state)
    {
    case AI_BUILDUP:
        if (m_minAttackFood > 0 && m_armyFood >= m_minAttackFood && heroAlive && ChooseAttackTarget(m_attackTarget))
        {
            ++m_waves;
            SetState(AI_ATTACKING);
            OrderArmy(ORD_ATTACK_MOVE, 0, m_attackTarget);
            break;
        }
        if (heroAlive && m_armyFood >= 4)
        {
            int camp = g_game->m_creeps.FindBestCamp(center, strength, heroLevel + 2);
            if (camp >= 0)
            {
                m_campTarget = camp;
                SetState(AI_CREEPING);
                OrderArmy(ORD_ATTACK_MOVE, 0, g_game->m_creeps.Camp(camp).pos);
                break;
            }
        }
        if (reorder && Distance(center, rally) > 400.0f)
            OrderArmy(ORD_ATTACK_MOVE, 0, rally);
        break;

    case AI_CREEPING:
        {
            const CreepCamp& camp = g_game->m_creeps.Camp(m_campTarget);
            if (camp.alive == 0)
            {
                SetState(AI_BUILDUP);
                break;
            }
            if (EnemyStrengthNear(center, 900.0f) > strength * kRetreatRatio)
            {
                SetState(AI_RETREATING);
                OrderArmy(ORD_MOVE, 0, rally);
                break;
            }
            if (reorder)
                OrderArmy(ORD_ATTACK_MOVE, 0, camp.pos);
        }
        break;

    case AI_ATTACKING:
        {
            float enemy = EnemyStrengthNear(center, 900.0f);
            if ((enemy > strength * kRetreatRatio && now - m_stateTime > 10.0f) ||
                m_armyFood * 3 < m_minAttackFood || !heroAlive)
            {
                SetState(AI_RETREATING);
                OrderArmy(ORD_MOVE, 0, rally);
                break;
            }
            if (Distance(center, m_attackTarget) < 400.0f && enemy <= 0.0f)
            {
                if (!ChooseAttackTarget(m_attackTarget))
                {
                    SetState(AI_BUILDUP);
                    break;
                }
                reorder = true;
            }
            if (reorder)
            {
                OrderArmy(ORD_ATTACK_MOVE, 0, m_attackTarget);
                m_lastArmyOrder = now;
            }
        }
        break;

    case AI_RETREATING:
        if (Distance(center, m_home) < 700.0f || now - m_stateTime > 25.0f)
            SetState(AI_BUILDUP);
        break;

    case AI_DEFENDING:
        if (!threatened)
        {
            SetState(AI_BUILDUP);
            OrderArmy(ORD_MOVE, 0, rally);
            break;
        }
        if (reorder)
            OrderArmy(ORD_ATTACK_MOVE, 0, m_threatPos);
        break;

    default:
        SetState(AI_BUILDUP);
        break;
    }
}

// Pull badly hurt units out of fights and focus ranged fire on the
// weakest enemy nearby.
void CAIController::ManageMicro()
{
    float now = g_game->Time();
    if (now - m_lastMicro < kMicroDelay || m_state == AI_RETREATING)
        return;
    m_lastMicro = now;

    CUnit* hurt[16];
    int numHurt = 0;
    CUnit* ranged[24];
    int numRanged = 0;

    for (int i = 0; i < m_numArmy; ++i)
    {
        CUnit* u = m_army[i];
        if (u->m_data->flags & UTF_SUMMONED)
            continue;
        if (u->m_life < u->m_maxLife * 0.25f && u->m_attackTarget && numHurt < 16)
            hurt[numHurt++] = u;
        else if ((u->m_data->flags & UTF_RANGED) && u->m_attackTarget && numRanged < 24)
            ranged[numRanged++] = u;
    }

    if (numHurt > 0)
        SendGroupOrder(hurt, numHurt, ORD_MOVE, 0, m_home);

    if (numRanged >= 2)
    {
        FilterContext ctx;
        ctx.self = ranged[0];
        ctx.player = m_player;
        ctx.flags = FF_ENEMY | FF_VISIBLE | FF_NO_STRUCT;
        ctx.minLifeRatio = 0.0f;

        CUnit* enemies[24];
        int n = g_game->m_grid.Collect(ArmyCenter(), 700.0f, FilterAttackable, &ctx, enemies, 24);
        CUnit* weakest = 0;
        for (int i = 0; i < n; ++i)
        {
            if (!weakest || enemies[i]->m_life < weakest->m_life)
                weakest = enemies[i];
        }
        if (weakest && weakest->m_life < weakest->m_maxLife * 0.6f)
            SendGroupOrder(ranged, numRanged, ORD_ATTACK, weakest->m_handle, weakest->m_pos);
    }
}

// ---------------------------------------------------------------------------
// Heroes

static int HeroTableIndex(const CHero* hero)
{
    for (int i = 0; i < 6; ++i)
    {
        if (g_heroTypes[i].unitType == hero->m_type)
            return i;
    }
    return 0;
}

void CAIController::HeroLearn(CHero* hero, int index)
{
    if (hero->m_skillPoints <= 0 || IsRecent(RK_LEARN_BASE + index))
        return;

    int learned = hero->m_abilityLevels[0] + hero->m_abilityLevels[1] + hero->m_abilityLevels[2];
    int slot = g_aiSkillOrder[HeroTableIndex(hero)][ClampI(learned, 0, 9)];
    if (!hero->CanLearn(slot))
    {
        slot = -1;
        for (int s = 0; s < 3; ++s)
        {
            if (hero->CanLearn(s))
            {
                slot = s;
                break;
            }
        }
        if (slot < 0)
            return;
    }

    Command cmd;
    cmd.type = CMD_LEARN;
    cmd.player = (u8)m_player;
    cmd.numUnits = 1;
    cmd.queued = 0;
    cmd.units[0] = hero->m_handle;
    cmd.order.id = ORD_LEARN;
    cmd.order.flags = 0;
    cmd.order.param = (u16)slot;
    cmd.order.target = 0;
    cmd.order.point = hero->m_pos;
    g_game->m_commands.Queue(cmd);
    MarkRecent(RK_LEARN_BASE + index);
}

void CAIController::HeroItems(CHero* hero)
{
    for (int s = 0; s < kInventorySlots; ++s)
    {
        CItem* item = hero->m_inventory[s];
        if (!item)
            continue;
        bool use = false;
        switch (item->m_itemType)
        {
        case IT_HEAL_POTION:
            use = hero->m_life < hero->m_maxLife * 0.4f;
            break;
        case IT_MANA_POTION:
            use = hero->m_mana < hero->m_stats.maxMana * 0.25f;
            break;
        case IT_SCROLL_HEAL:
            use = hero->m_life < hero->m_maxLife * 0.5f && hero->m_attackTarget;
            break;
        default:
            break;
        }
        if (use)
        {
            SendOrder(hero, ORD_USE_ITEM, 0, hero->m_pos, s);
            return;
        }
    }

    if (hero->m_attackTarget || hero->m_order.id == ORD_PICKUP)
        return;
    CItem* item = g_game->FindNearestItem(hero->m_pos, 600.0f);
    if (!item)
        return;
    if (hero->FreeInventorySlot() < 0 && item->Data()->itemClass != ITC_POWERUP)
        return;
    SendOrder(hero, ORD_PICKUP, item->m_handle, item->m_pos, 0);
}

void CAIController::HeroCast(CHero* hero)
{
    FilterContext ctx;
    ctx.self = hero;
    ctx.player = m_player;
    ctx.flags = FF_ENEMY | FF_VISIBLE | FF_NO_STRUCT;
    ctx.minLifeRatio = 0.0f;

    CUnit* enemies[24];
    int numEnemies = g_game->m_grid.Collect(hero->m_pos, kHeroThreatRange, FilterUnit, &ctx, enemies, 24);
    if (numEnemies == 0 && hero->m_life > hero->m_maxLife * 0.3f)
        return;

    for (int slot = 0; slot < 3; ++slot)
    {
        if (hero->m_abilityLevels[slot] == 0)
            continue;
        CAbility* a = hero->FindAbility(hero->m_heroData->abilities[slot]);
        if (!a || a->IsPassive() || !a->IsReady() || hero->m_mana < a->ManaCost())
            continue;

        const AbilityTypeData* d = a->Data();
        int i = a->LevelIndex();
        CUnit* target = 0;
        Vec2 point = hero->m_pos;
        bool cast = false;

        switch (d->abilityClass)
        {
        case ABC_MISSILE_STUN:
        case ABC_BOUNCE:
            {
                float bestScore = -1.0f;
                for (int k = 0; k < numEnemies; ++k)
                {
                    CUnit* e = enemies[k];
                    if (Distance(e->m_pos, hero->m_pos) > d->range + 100.0f)
                        continue;
                    float score = e->m_stats.damageBase + (e->IsHero() ? 200.0f : 0.0f) + e->m_life * 0.05f;
                    if (score > bestScore)
                    {
                        bestScore = score;
                        target = e;
                    }
                }
                cast = target && (d->abilityClass == ABC_MISSILE_STUN || numEnemies >= 2);
            }
            break;

        case ABC_NOVA:
            {
                int inArea = 0;
                for (int k = 0; k < numEnemies; ++k)
                {
                    if (Distance(enemies[k]->m_pos, hero->m_pos) < d->area[i] * 0.8f)
                        inArea += enemies[k]->IsHero() ? 2 : 1;
                }
                cast = inArea >= 3;
            }
            break;

        case ABC_TARGET_HEAL:
            {
                FilterContext allies;
                allies.self = hero;
                allies.player = m_player;
                allies.flags = FF_ALLY | FF_NO_STRUCT | FF_INJURED;
                allies.minLifeRatio = 0.45f;
                CUnit* hurt[16];
                int n = g_game->m_grid.Collect(hero->m_pos, d->range, FilterUnit, &allies, hurt, 16);
                float bestMissing = d->valueA[i] * 0.5f;
                for (int k = 0; k < n; ++k)
                {
                    float missing = hurt[k]->m_maxLife - hurt[k]->m_life + (hurt[k]->IsHero() ? 150.0f : 0.0f);
                    if (missing > bestMissing)
                    {
                        bestMissing = missing;
                        target = hurt[k];
                    }
                }
                cast = target != 0;
            }
            break;

        case ABC_SELF_BUFF:
            if (d->buffType == BF_DIVINESHIELD)
                cast = hero->m_life < hero->m_maxLife * 0.3f && numEnemies > 0;
            else
                cast = !(hero->m_state & US_INVISIBLE) &&
                       (hero->m_life < hero->m_maxLife * 0.25f || (numEnemies > 0 && !hero->m_attackTarget));
            break;

        case ABC_CHANNEL_AREA:
            {
                int bestCount = 0;
                for (int k = 0; k < numEnemies; ++k)
                {
                    if (Distance(enemies[k]->m_pos, hero->m_pos) > d->range)
                        continue;
                    int count = 0;
                    for (int m = 0; m < numEnemies; ++m)
                    {
                        if (Distance(enemies[k]->m_pos, enemies[m]->m_pos) < d->area[i])
                            ++count;
                    }
                    if (count > bestCount)
                    {
                        bestCount = count;
                        point = enemies[k]->m_pos;
                    }
                }
                cast = bestCount >= 3;
            }
            break;

        case ABC_SUMMON:
            cast = numEnemies > 0;
            break;

        case ABC_LINE:
            {
                int bestCount = 0;
                for (int k = 0; k < numEnemies; ++k)
                {
                    Vec2 end = hero->m_pos + Normalize(enemies[k]->m_pos - hero->m_pos) * d->range;
                    int count = 0;
                    for (int m = 0; m < numEnemies; ++m)
                    {
                        if (SegmentHitsCircle(hero->m_pos, end, enemies[m]->m_pos, d->area[i]))
                            ++count;
                    }
                    if (count > bestCount)
                    {
                        bestCount = count;
                        point = enemies[k]->m_pos;
                    }
                }
                cast = bestCount >= 2;
            }
            break;

        default:
            break;
        }

        if (!cast)
            continue;

        if (target)
            SendOrderEx(hero, ORD_CAST, OF_TARGET_UNIT, target->m_handle, target->m_pos, a->m_type);
        else if (d->flags & ABF_TARGET_POINT)
            SendOrderEx(hero, ORD_CAST, OF_TARGET_POINT, 0, point, a->m_type);
        else
            SendOrderEx(hero, ORD_CAST, 0, 0, hero->m_pos, a->m_type);
        return;
    }
}

void CAIController::ManageHeroes()
{
    CPlayer& p = g_game->m_players[m_player];
    for (int i = 0; i < m_numHeroes; ++i)
    {
        CHero* hero = m_heroes[i];
        if (!hero->IsAlive())
        {
            if (IsRecent(RK_REVIVE + i) || !p.CanAfford(hero->ReviveCost(), 0))
                continue;
            for (int b = 0; b < m_numBuildings; ++b)
            {
                CUnit* altar = m_buildings[b];
                if (!(altar->m_data->flags & UTF_ALTAR) || (altar->m_state & US_CONSTRUCTING))
                    continue;
                if (static_cast<CBuilding*>(altar)->QueueLength() > 0)
                    continue;
                SendOrder(altar, ORD_REVIVE, hero->m_handle, altar->m_pos, 0);
                MarkRecent(RK_REVIVE + i);
                break;
            }
            continue;
        }

        HeroLearn(hero, i);
        HeroItems(hero);
        if (hero->m_order.id != ORD_CAST)
            HeroCast(hero);
    }
}

void CAIController::Think()
{
    CPlayer& p = g_game->m_players[m_player];
    if (p.IsDefeated())
        return;

    ScanUnits();
    ManageWorkers();
    ManageFood();
    ManageBuildOrder();
    ManageHeroes();
    ManageArmy();
    ManageMicro();
}

#include "Game.h"

static const float kLeashRange     = 900.0f;
static const float kAggroTimeout   = 6.0f;
static const float kCampSpread     = 80.0f;
static const float kNearBaseCells  = 30.0f;
static const float kCenterCells    = 10.0f;

void CCreepManager::Init()
{
    m_numCamps = 0;
    m_night = false;
}

void CCreepManager::SpawnCamps()
{
    CTerrain& terrain = g_game->m_terrain;
    CRandom& rng = g_game->m_rng;
    Vec2 center(kMapSize * 0.5f, kMapSize * 0.5f);

    for (int i = 0; i < terrain.CampSiteCount() && m_numCamps < (int)COUNTOF(m_camps); ++i)
    {
        Vec2 site = terrain.CampSite(i);

        float nearestStart = 1e9f;
        for (int p = 0; p < g_game->m_numPlayers; ++p)
            nearestStart = MinF(nearestStart, Distance(site, g_game->m_players[p].m_start));

        // Weak camps near bases, strong ones toward the middle.
        int templ;
        if (Distance(site, center) < kCenterCells * kCellSize)
            templ = 6;
        else if (nearestStart < kNearBaseCells * kCellSize)
        {
            static const int nearTemplates[3] = { 0, 1, 7 };
            templ = nearTemplates[rng.Range(0, 2)];
        }
        else
            templ = rng.Range(2, 5);

        const CreepCampTemplate& t = g_creepCamps[templ];
        CreepCamp& camp = m_camps[m_numCamps];
        camp.pos = site;
        camp.templateIndex = (u8)templ;
        camp.level = t.level;
        camp.alive = 0;
        camp.aggro = 0;
        camp.aggroTimer = 0.0f;
        camp.aggroTarget = 0;

        float facing = AngleTo(site, center);
        for (int k = 0; k < 4; ++k)
        {
            camp.units[k] = 0;
            if (k >= t.count || t.types[k] == 0xFF)
                continue;
            Vec2 p = PolarOffset(site, kCampSpread, facing + (float)k * (kTwoPi / (float)t.count));
            CUnit* u = g_game->CreateUnit(t.types[k], kPlayerNeutralHostile, p, facing);
            if (!u)
                continue;
            u->m_campIndex = m_numCamps;
            u->m_guardPos = p;
            camp.units[k] = u->m_handle;
            ++camp.alive;
        }
        ++m_numCamps;
    }
}

void CCreepManager::SetCampSleeping(CreepCamp& camp, bool sleeping)
{
    for (int k = 0; k < 4; ++k)
    {
        CUnit* u = g_game->GetUnit(camp.units[k]);
        if (!u || !u->IsAlive())
            continue;
        if (sleeping)
            u->AddBuff(BF_SLEEP, 1, 0.0f, 0);
        else
            u->RemoveBuff(BF_SLEEP);
    }
}

void CCreepManager::AggroCamp(CreepCamp& camp, CUnit* target)
{
    camp.aggro = 1;
    camp.aggroTimer = kAggroTimeout;
    camp.aggroTarget = target ? target->m_handle : 0;

    for (int k = 0; k < 4; ++k)
    {
        CUnit* u = g_game->GetUnit(camp.units[k]);
        if (!u || !u->IsAlive())
            continue;
        u->RemoveBuff(BF_SLEEP);
        if (!target || u->m_attackTarget)
            continue;
        if (!u->CanAttack(target))
            continue;
        COrder o;
        o.id = ORD_ATTACK;
        o.flags = OF_TARGET_UNIT;
        o.param = 0;
        o.target = target->m_handle;
        o.point = target->m_pos;
        u->IssueOrder(o, false);
    }
}

// Pull the camp home and top the creeps back up, like a reset.
void CCreepManager::LeashCamp(CreepCamp& camp)
{
    camp.aggro = 0;
    camp.aggroTarget = 0;
    for (int k = 0; k < 4; ++k)
    {
        CUnit* u = g_game->GetUnit(camp.units[k]);
        if (!u || !u->IsAlive())
            continue;
        COrder o;
        o.id = ORD_MOVE;
        o.flags = OF_TARGET_POINT;
        o.param = 0;
        o.target = 0;
        o.point = u->m_guardPos;
        u->IssueOrder(o, false);
        u->m_attackTarget = 0;
        u->Heal(u->m_maxLife * 0.5f);
    }
}

void CCreepManager::UpdateCamp(CreepCamp& camp, float dt)
{
    if (camp.alive == 0 || !camp.aggro)
        return;

    camp.aggroTimer -= dt;
    bool fighting = false;
    for (int k = 0; k < 4; ++k)
    {
        CUnit* u = g_game->GetUnit(camp.units[k]);
        if (!u || !u->IsAlive())
            continue;
        if (Distance(u->m_pos, camp.pos) > kLeashRange)
        {
            LeashCamp(camp);
            return;
        }
        if (u->m_attackTarget)
            fighting = true;
    }

    if (camp.aggroTimer <= 0.0f && !fighting)
    {
        LeashCamp(camp);
        if (m_night)
            SetCampSleeping(camp, true);
    }
}

void CCreepManager::Update(float dt)
{
    bool night = g_game->IsNight();
    if (night != m_night)
    {
        m_night = night;
        for (int i = 0; i < m_numCamps; ++i)
        {
            if (!m_camps[i].aggro)
                SetCampSleeping(m_camps[i], night);
        }
    }

    for (int i = 0; i < m_numCamps; ++i)
        UpdateCamp(m_camps[i], dt);
}

void CCreepManager::OnCreepDamaged(CUnit* creep, CUnit* attacker)
{
    if (creep->m_campIndex < 0 || creep->m_campIndex >= m_numCamps || !attacker)
        return;
    CreepCamp& camp = m_camps[creep->m_campIndex];
    if (!camp.aggro || !g_game->GetUnit(camp.aggroTarget))
        AggroCamp(camp, attacker);
    else
        camp.aggroTimer = kAggroTimeout;
}

void CCreepManager::RollItemDrop(int level, const Vec2& pos)
{
    int itemLevel = ClampI((level + 1) / 2 + (level >= 6 ? 1 : 0), 1, 6);
    int type = g_itemDropTable[itemLevel - 1][g_game->m_rng.Range(0, 3)];
    g_game->CreateItem(type, pos);
}

void CCreepManager::OnCreepDeath(CUnit* creep, CUnit* killer)
{
    (void)killer;
    if (creep->m_campIndex < 0 || creep->m_campIndex >= m_numCamps)
        return;

    CreepCamp& camp = m_camps[creep->m_campIndex];
    for (int k = 0; k < 4; ++k)
    {
        if (camp.units[k] == creep->m_handle)
            camp.units[k] = 0;
    }
    if (camp.alive > 0)
        --camp.alive;
    if (camp.alive == 0)
    {
        camp.aggro = 0;
        RollItemDrop(camp.level, creep->m_pos);
    }
}

// Best camp the given army can take: prefers the highest level it can
// comfortably beat, then the shortest walk.
int CCreepManager::FindBestCamp(const Vec2& from, float strength, int maxLevel) const
{
    int best = -1;
    float bestScore = 1e9f;
    for (int i = 0; i < m_numCamps; ++i)
    {
        const CreepCamp& camp = m_camps[i];
        if (camp.alive == 0 || camp.level > maxLevel)
            continue;

        float power = 0.0f;
        for (int k = 0; k < 4; ++k)
        {
            CUnit* u = g_game->GetUnit(camp.units[k]);
            if (u && u->IsAlive())
                power += u->m_life * (u->m_stats.damageBase + 5.0f) / MaxF(u->m_stats.cooldown, 0.5f);
        }
        if (strength < power * 1.4f)
            continue;

        float score = Distance(from, camp.pos) - (float)camp.level * 250.0f;
        if (score < bestScore)
        {
            bestScore = score;
            best = i;
        }
    }
    return best;
}

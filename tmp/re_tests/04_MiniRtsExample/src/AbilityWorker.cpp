#include "Game.h"

enum CargoKind
{
    CARGO_NONE = 0,
    CARGO_GOLD,
    CARGO_LUMBER
};

static const float kTreeSearchRange = 1200.0f;
static const float kMineSearchRange = 1500.0f;

CAbilHarvest::CAbilHarvest(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
    m_state = HS_IDLE;
    m_cargoKind = CARGO_NONE;
    m_cargo = 0;
    m_resource = 0;
    m_mine = 0;
}

void CAbilHarvest::OnRemoved()
{
    ReleaseClaims();
}

void CAbilHarvest::ReleaseClaims()
{
    if (m_mine && m_state != HS_IDLE && m_state != HS_TO_TREE && m_state != HS_CHOPPING)
    {
        CUnit* u = g_game->GetUnit(m_mine);
        if (u && (u->m_data->flags & UTF_GOLDMINE))
        {
            CGoldMine* mine = static_cast<CGoldMine*>(u);
            if (mine->m_workersAssigned > 0)
                --mine->m_workersAssigned;
        }
    }
    m_state = HS_IDLE;
}

int CAbilHarvest::OrderUpdate(COrder& order, float dt)
{
    if (order.id == ORD_RETURN || m_state == HS_RETURNING)
        return ReturnCargo(order, dt);

    if (m_state == HS_IDLE)
    {
        CWidget* w = g_game->GetWidget(order.target);
        if (w && w->m_kind == WK_UNIT && (static_cast<CUnit*>(w)->m_data->flags & UTF_GOLDMINE))
        {
            m_mine = w->m_handle;
            m_state = HS_TO_MINE;
            ++static_cast<CGoldMine*>(w)->m_workersAssigned;
            if (m_cargoKind == CARGO_LUMBER)
                m_cargo = 0;
        }
        else if (w && w->m_kind == WK_DESTRUCTABLE)
        {
            m_resource = w->m_handle;
            m_state = HS_TO_TREE;
            if (m_cargoKind == CARGO_GOLD)
                m_cargo = 0;
        }
        else if (m_cargo > 0)
        {
            m_state = HS_RETURNING;
            return OR_CONTINUE;
        }
        else
        {
            return OR_FAILED;
        }
    }

    switch (m_state)
    {
    case HS_TO_MINE:
    case HS_WAIT_MINE:
    case HS_IN_MINE:
        return UpdateGold(order, dt);
    case HS_TO_TREE:
    case HS_CHOPPING:
        return UpdateLumber(order, dt);
    default:
        return OR_FAILED;
    }
}

int CAbilHarvest::UpdateGold(COrder& order, float dt)
{
    CUnit* u = g_game->GetUnit(m_mine);
    CGoldMine* mine = (u && (u->m_data->flags & UTF_GOLDMINE) && u->IsAlive()) ? static_cast<CGoldMine*>(u) : 0;
    if (!mine)
    {
        mine = g_game->FindNearestMine(m_owner->m_pos, kMineSearchRange);
        if (!mine)
        {
            m_state = HS_IDLE;
            return OR_FAILED;
        }
        m_mine = mine->m_handle;
        order.target = m_mine;
        ++mine->m_workersAssigned;
        m_state = HS_TO_MINE;
        m_owner->StopMoving();
    }

    if (m_state == HS_TO_MINE)
    {
        float reach = mine->CollisionRadius() + m_owner->CollisionRadius() + 48.0f;
        if (DistSq(m_owner->m_pos, mine->m_pos) > reach * reach)
        {
            if (!m_owner->m_moving && !m_owner->MoveTo(mine->m_pos, reach - 8.0f))
                return OR_FAILED;
            if (m_owner->UpdateMovement(dt) < 0)
            {
                if (DistSq(m_owner->m_pos, mine->m_pos) > reach * reach * 1.5f)
                    return OR_FAILED;
            }
            else
            {
                return OR_CONTINUE;
            }
        }
        m_owner->StopMoving();
        m_state = HS_WAIT_MINE;
    }

    if (m_state == HS_WAIT_MINE)
    {
        m_owner->FaceTowards(mine->m_pos, dt);
        if (mine->TryEnter(m_owner))
            m_state = HS_IN_MINE;
        return OR_CONTINUE;
    }

    // HS_IN_MINE: the mine flips us to HS_RETURNING when it lets us out.
    if (!(m_owner->m_state & US_INSIDE))
        m_state = HS_TO_MINE;
    return OR_CONTINUE;
}

int CAbilHarvest::UpdateLumber(COrder& order, float dt)
{
    CDestructable* tree = g_game->GetTree(m_resource);
    if (!tree || !tree->IsAlive())
    {
        tree = g_game->FindNearestTree(m_owner->m_pos, kTreeSearchRange);
        if (!tree)
        {
            if (m_cargo > 0)
            {
                m_state = HS_RETURNING;
                return OR_CONTINUE;
            }
            m_state = HS_IDLE;
            return OR_FAILED;
        }
        m_resource = tree->m_handle;
        order.target = m_resource;
        m_state = HS_TO_TREE;
        m_owner->StopMoving();
    }

    if (m_state == HS_TO_TREE)
    {
        float reach = m_owner->CollisionRadius() + 44.0f;
        if (DistSq(m_owner->m_pos, tree->m_pos) > reach * reach)
        {
            if (!m_owner->m_moving && !m_owner->MoveTo(tree->m_pos, reach - 6.0f))
                return OR_FAILED;
            int r = m_owner->UpdateMovement(dt);
            if (r < 0 && DistSq(m_owner->m_pos, tree->m_pos) > reach * reach * 2.0f)
            {
                // Unreachable tree: forget it and let the next tick search again.
                m_resource = 0;
                return OR_CONTINUE;
            }
            if (r == 0)
                return OR_CONTINUE;
        }
        m_owner->StopMoving();
        m_state = HS_CHOPPING;
        m_timer = m_data->valueB[1];
    }

    m_owner->FaceTowards(tree->m_pos, dt);
    if (m_timer > 0.0f)
        return OR_CONTINUE;

    int got = tree->Harvest((int)m_data->valueA[2]);
    if (got > 0)
    {
        if (m_cargoKind != CARGO_LUMBER)
            m_cargo = 0;
        m_cargoKind = CARGO_LUMBER;
        m_cargo = (u8)(m_cargo + got);
    }
    m_timer = m_data->valueB[1];

    if (m_cargo >= (int)m_data->valueA[1])
        m_state = HS_RETURNING;
    else if (!tree->IsAlive())
        m_state = HS_TO_TREE;
    return OR_CONTINUE;
}

int CAbilHarvest::ReturnCargo(COrder& order, float dt)
{
    if (m_cargo == 0)
    {
        if (order.id == ORD_RETURN)
            return OR_DONE;
        m_state = (m_cargoKind == CARGO_GOLD) ? HS_TO_MINE : HS_TO_TREE;
        return OR_CONTINUE;
    }

    CUnit* drop = g_game->FindDropoff(m_owner->m_owner, m_owner->m_pos, m_cargoKind == CARGO_LUMBER);
    if (!drop)
        return OR_FAILED;

    float reach = drop->CollisionRadius() + m_owner->CollisionRadius() + 48.0f;
    if (DistSq(m_owner->m_pos, drop->m_pos) > reach * reach)
    {
        if (!m_owner->m_moving && !m_owner->MoveTo(drop->m_pos, reach - 8.0f))
            return OR_FAILED;
        int r = m_owner->UpdateMovement(dt);
        if (r == 0)
            return OR_CONTINUE;
        if (r < 0 && DistSq(m_owner->m_pos, drop->m_pos) > reach * reach * 1.5f)
            return OR_FAILED;
    }
    m_owner->StopMoving();

    CPlayer& p = g_game->m_players[m_owner->m_owner];
    int kind = m_cargoKind;
    if (kind == CARGO_GOLD)
        p.DepositGold(m_cargo);
    else
        p.DepositLumber(m_cargo);
    m_cargo = 0;

    if (order.id == ORD_RETURN)
    {
        order.id = ORD_HARVEST;
        order.target = (kind == CARGO_GOLD) ? m_mine : m_resource;
    }
    m_state = (kind == CARGO_GOLD) ? HS_TO_MINE : HS_TO_TREE;
    return OR_CONTINUE;
}

// ---------------------------------------------------------------------------

CAbilBuild::CAbilBuild(CUnit* owner, int type, int level) : CAbility(owner, type, level)
{
    m_building = 0;
}

int CAbilBuild::OrderUpdate(COrder& order, float dt)
{
    if (m_building)
    {
        CUnit* b = g_game->GetUnit(m_building);
        if (!b || !b->IsAlive())
        {
            m_building = 0;
            return OR_FAILED;
        }
        if (b->m_state & US_CONSTRUCTING)
            return OR_CONTINUE;
        m_building = 0;
        return OR_DONE;
    }

    const UnitTypeData* d = GetUnitType(order.param);
    if (!(d->flags & UTF_STRUCTURE))
        return OR_FAILED;

    float reach = (float)d->footprint * kCellSize * 0.5f + m_owner->CollisionRadius() + 40.0f;
    if (DistSq(m_owner->m_pos, order.point) > reach * reach)
    {
        if (!m_owner->m_moving && !m_owner->MoveTo(order.point, reach - 8.0f))
            return OR_FAILED;
        int r = m_owner->UpdateMovement(dt);
        if (r == 0)
            return OR_CONTINUE;
        if (r < 0 && DistSq(m_owner->m_pos, order.point) > reach * reach * 2.0f)
            return OR_FAILED;
    }
    m_owner->StopMoving();

    int cx = CTerrain::WorldToCell(order.point.x);
    int cy = CTerrain::WorldToCell(order.point.y);
    if (!g_game->m_terrain.CanBuildFootprint(cx, cy, d->footprint))
        return OR_FAILED;

    CPlayer& p = g_game->m_players[m_owner->m_owner];
    if (!p.MeetsRequirements(d) || !p.Spend(d->goldCost, d->lumberCost))
        return OR_FAILED;

    Vec2 center = FootprintCenter(cx, cy, d->footprint);
    CUnit* u = g_game->CreateUnit(order.param, m_owner->m_owner, center, kPi * 1.5f, true);
    if (!u)
    {
        p.Refund(d->goldCost, d->lumberCost);
        return OR_FAILED;
    }

    static_cast<CBuilding*>(u)->BeginConstruction(m_owner);
    m_building = u->m_handle;
    return OR_CONTINUE;
}

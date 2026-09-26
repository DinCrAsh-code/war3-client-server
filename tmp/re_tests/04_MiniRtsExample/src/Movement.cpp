#include "Game.h"

static const float kWaypointReach = 10.0f;
static const int   kStuckRepath   = 20;
static const int   kStuckGiveUp   = 80;

bool CUnit::MoveTo(const Vec2& goal, float stopRange)
{
    m_moveGoal = goal;
    m_moveStopRange = stopRange;
    m_repathTimer = 1.0f;
    m_pathBestDist = Distance(m_pos, goal);

    if (m_stats.moveSpeed <= 0.0f)
    {
        m_moving = false;
        return false;
    }

    if (IsFlying())
    {
        m_path.Reset();
        m_path.points[0] = goal;
        m_path.count = 1;
        m_path.goal = goal;
        m_moving = true;
        return true;
    }

    int clearance = m_data->collision > 24 ? 1 : 0;
    m_moving = g_game->m_pathFinder.FindPath(m_pos, goal, clearance, m_path);
    if (!m_moving && clearance > 0)
        m_moving = g_game->m_pathFinder.FindPath(m_pos, goal, 0, m_path);
    return m_moving;
}

void CUnit::StopMoving()
{
    m_moving = false;
    m_path.Reset();
    m_stuckTicks = 0;
}

bool CUnit::FaceTowards(const Vec2& p, float dt)
{
    if (m_data->turnRateDeg == 0)
        return true;
    if (DistSq(p, m_pos) < 1.0f)
        return true;

    float target = AngleTo(m_pos, p);
    float step = (float)m_data->turnRateDeg * kDegToRad * dt;
    m_facing = TurnTowards(m_facing, target, step);
    return AbsF(AngleDiff(m_facing, target)) < 0.35f;
}

// Soft push away from overlapping ground units.
Vec2 CUnit::ComputeSeparation() const
{
    CUnit* nearby[12];
    FilterContext ctx;
    ctx.self = this;
    ctx.player = m_owner;
    ctx.flags = FF_GROUND | FF_NO_STRUCT | FF_NOT_SELF;
    ctx.minLifeRatio = 0.0f;
    ctx.flags &= ~(FF_ENEMY | FF_ALLY);

    float radius = CollisionRadius() * 2.0f;
    int n = g_game->m_grid.Collect(m_pos, radius, FilterUnit, &ctx, nearby, 12);

    Vec2 push(0.0f, 0.0f);
    for (int i = 0; i < n; ++i)
    {
        Vec2 away = m_pos - nearby[i]->m_pos;
        float d = Length(away);
        float minDist = CollisionRadius() + nearby[i]->CollisionRadius();
        if (d >= minDist)
            continue;
        if (d < 0.01f)
        {
            float a = (float)((m_handle * 37u) % 628u) * 0.01f;
            away = Vec2(FastCos(a), FastSin(a));
            d = 1.0f;
        }
        float overlap = (minDist - d) / minDist;
        push += away * (overlap * 80.0f / d);
    }
    return push;
}

// Returns 1 when arrived, -1 on failure, 0 while still moving.
int CUnit::UpdateMovement(float dt)
{
    float distToGoal = Distance(m_pos, m_moveGoal);
    if (distToGoal <= m_moveStopRange)
    {
        StopMoving();
        return 1;
    }
    if (!m_moving)
        return -1;

    if (m_path.Done())
    {
        // Path ran out (truncated or partial): repath while it still helps.
        if (distToGoal + 32.0f < m_pathBestDist && m_path.partial)
        {
            m_pathBestDist = distToGoal;
            Vec2 goal = m_moveGoal;
            float stop = m_moveStopRange;
            float best = m_pathBestDist;
            if (MoveTo(goal, stop))
            {
                m_pathBestDist = best;
                return 0;
            }
        }
        StopMoving();
        return distToGoal <= m_moveStopRange + 48.0f ? 1 : -1;
    }

    float speed = m_stats.moveSpeed;
    if (speed <= 0.0f)
        return -1;

    Vec2 wp = m_path.points[m_path.current];
    bool facing = FaceTowards(wp, dt);
    float angleOff = AbsF(AngleDiff(m_facing, AngleTo(m_pos, wp)));
    if (!facing && angleOff > 1.2f)
        return 0;

    Vec2 dir = Normalize(wp - m_pos);
    float remain = Distance(m_pos, wp);
    float step = MinF(speed * dt, remain);
    Vec2 next = m_pos + dir * step;

    if (!IsFlying())
    {
        next += ComputeSeparation() * dt;
        CTerrain& t = g_game->m_terrain;
        int cx = CTerrain::WorldToCell(next.x);
        int cy = CTerrain::WorldToCell(next.y);
        if (!t.IsWalkable(cx, cy))
        {
            Vec2 slideX(next.x, m_pos.y);
            Vec2 slideY(m_pos.x, next.y);
            if (t.IsWalkable(CTerrain::WorldToCell(slideX.x), CTerrain::WorldToCell(slideX.y)))
                next = slideX;
            else if (t.IsWalkable(CTerrain::WorldToCell(slideY.x), CTerrain::WorldToCell(slideY.y)))
                next = slideY;
            else
                next = m_pos;
        }
    }

    float moved = Distance(next, m_pos);
    SetPosition(next);

    if (moved < speed * dt * 0.2f)
        ++m_stuckTicks;
    else if (m_stuckTicks > 0)
        --m_stuckTicks;

    if (Distance(m_pos, wp) < kWaypointReach)
        ++m_path.current;

    if (m_stuckTicks > kStuckGiveUp)
    {
        StopMoving();
        return -1;
    }
    if (m_stuckTicks == kStuckRepath)
    {
        int stuck = m_stuckTicks;
        Vec2 goal = m_moveGoal;
        if (!MoveTo(goal, m_moveStopRange))
            return -1;
        m_stuckTicks = stuck + 1;
    }
    return 0;
}

#include "Objects.h"

// Coarse bucket grid with intrusive doubly-linked lists threaded through
// the units themselves.

void CSpatialGrid::Init()
{
    for (int i = 0; i < kGridBuckets * kGridBuckets; ++i)
        m_heads[i] = 0;
}

int CSpatialGrid::BucketFor(const Vec2& p)
{
    int bx = ClampI(FloorToInt(p.x / kBucketSize), 0, kGridBuckets - 1);
    int by = ClampI(FloorToInt(p.y / kBucketSize), 0, kGridBuckets - 1);
    return by * kGridBuckets + bx;
}

void CSpatialGrid::BucketRange(const Vec2& center, float radius, int& x0, int& y0, int& x1, int& y1) const
{
    x0 = ClampI(FloorToInt((center.x - radius) / kBucketSize), 0, kGridBuckets - 1);
    y0 = ClampI(FloorToInt((center.y - radius) / kBucketSize), 0, kGridBuckets - 1);
    x1 = ClampI(FloorToInt((center.x + radius) / kBucketSize), 0, kGridBuckets - 1);
    y1 = ClampI(FloorToInt((center.y + radius) / kBucketSize), 0, kGridBuckets - 1);
}

void CSpatialGrid::Insert(CUnit* unit)
{
    int b = BucketFor(unit->m_pos);
    unit->m_gridBucket = b;
    unit->m_gridPrev = 0;
    unit->m_gridNext = m_heads[b];
    if (m_heads[b])
        m_heads[b]->m_gridPrev = unit;
    m_heads[b] = unit;
}

void CSpatialGrid::Remove(CUnit* unit)
{
    if (unit->m_gridBucket < 0)
        return;
    if (unit->m_gridPrev)
        unit->m_gridPrev->m_gridNext = unit->m_gridNext;
    else
        m_heads[unit->m_gridBucket] = unit->m_gridNext;
    if (unit->m_gridNext)
        unit->m_gridNext->m_gridPrev = unit->m_gridPrev;
    unit->m_gridNext = 0;
    unit->m_gridPrev = 0;
    unit->m_gridBucket = -1;
}

void CSpatialGrid::Move(CUnit* unit)
{
    if (unit->m_gridBucket < 0)
        return;
    if (BucketFor(unit->m_pos) == unit->m_gridBucket)
        return;
    Remove(unit);
    Insert(unit);
}

int CSpatialGrid::Visit(const Vec2& center, float radius, UnitVisitFn fn, void* context) const
{
    int x0, y0, x1, y1;
    BucketRange(center, radius, x0, y0, x1, y1);
    float r2 = radius * radius;
    int visited = 0;

    for (int by = y0; by <= y1; ++by)
    {
        for (int bx = x0; bx <= x1; ++bx)
        {
            CUnit* u = m_heads[by * kGridBuckets + bx];
            while (u)
            {
                CUnit* next = u->m_gridNext;
                if (DistSq(u->m_pos, center) <= r2)
                {
                    fn(u, context);
                    ++visited;
                }
                u = next;
            }
        }
    }
    return visited;
}

int CSpatialGrid::Collect(const Vec2& center, float radius, UnitFilterFn filter, void* context,
                          CUnit** out, int maxOut) const
{
    int x0, y0, x1, y1;
    BucketRange(center, radius, x0, y0, x1, y1);
    int count = 0;

    for (int by = y0; by <= y1; ++by)
    {
        for (int bx = x0; bx <= x1; ++bx)
        {
            for (CUnit* u = m_heads[by * kGridBuckets + bx]; u; u = u->m_gridNext)
            {
                float reach = radius + u->CollisionRadius();
                if (DistSq(u->m_pos, center) > reach * reach)
                    continue;
                if (filter && !filter(u, context))
                    continue;
                out[count++] = u;
                if (count >= maxOut)
                    return count;
            }
        }
    }
    return count;
}

CUnit* CSpatialGrid::FindNearest(const Vec2& center, float radius, UnitFilterFn filter, void* context) const
{
    int x0, y0, x1, y1;
    BucketRange(center, radius, x0, y0, x1, y1);
    CUnit* best = 0;
    float bestD = radius * radius;

    for (int by = y0; by <= y1; ++by)
    {
        for (int bx = x0; bx <= x1; ++bx)
        {
            for (CUnit* u = m_heads[by * kGridBuckets + bx]; u; u = u->m_gridNext)
            {
                float d = DistSq(u->m_pos, center);
                if (d > bestD)
                    continue;
                if (filter && !filter(u, context))
                    continue;
                best = u;
                bestD = d;
            }
        }
    }
    return best;
}

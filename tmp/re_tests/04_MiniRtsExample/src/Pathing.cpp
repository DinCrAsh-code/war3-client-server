#include "World.h"

// Grid A* (8-connected, no corner cutting) with an indexed binary heap and
// per-search stamps instead of clearing the node arrays, followed by
// line-of-sight string pulling to turn the cell chain into waypoints.

static const int kMaxExpansions = 4000;
static const u32 kCostStraight  = 10;
static const u32 kCostDiagonal  = 14;
static const u32 kInfinite      = 0xFFFFFFFFu;

static const int s_dirX[8] = { 1, -1, 0, 0, 1, 1, -1, -1 };
static const int s_dirY[8] = { 0, 0, 1, -1, 1, -1, 1, -1 };

void CPathFinder::Init(CTerrain* terrain)
{
    m_terrain = terrain;
    m_currentStamp = 0;
    m_heapSize = 0;
    m_searches = 0;
    memset(m_stamp, 0, sizeof(m_stamp));
}

int CPathFinder::Heuristic(int cell, int goal) const
{
    int dx = AbsI((cell % kMapCells) - (goal % kMapCells));
    int dy = AbsI((cell / kMapCells) - (goal / kMapCells));
    int diag = MinI(dx, dy);
    int straight = MaxI(dx, dy) - diag;
    return diag * (int)kCostDiagonal + straight * (int)kCostStraight;
}

void CPathFinder::SiftUp(int pos)
{
    int cell = m_heap[pos];
    while (pos > 0)
    {
        int parent = (pos - 1) >> 1;
        int pc = m_heap[parent];
        if (m_f[pc] <= m_f[cell])
            break;
        m_heap[pos] = pc;
        m_heapIndex[pc] = pos;
        pos = parent;
    }
    m_heap[pos] = cell;
    m_heapIndex[cell] = pos;
}

void CPathFinder::SiftDown(int pos)
{
    int cell = m_heap[pos];
    for (;;)
    {
        int child = pos * 2 + 1;
        if (child >= m_heapSize)
            break;
        if (child + 1 < m_heapSize && m_f[m_heap[child + 1]] < m_f[m_heap[child]])
            ++child;
        int cc = m_heap[child];
        if (m_f[cc] >= m_f[cell])
            break;
        m_heap[pos] = cc;
        m_heapIndex[cc] = pos;
        pos = child;
    }
    m_heap[pos] = cell;
    m_heapIndex[cell] = pos;
}

void CPathFinder::HeapPush(int cell)
{
    m_heap[m_heapSize] = cell;
    m_heapIndex[cell] = m_heapSize;
    ++m_heapSize;
    SiftUp(m_heapSize - 1);
}

int CPathFinder::HeapPop()
{
    int top = m_heap[0];
    --m_heapSize;
    if (m_heapSize > 0)
    {
        m_heap[0] = m_heap[m_heapSize];
        m_heapIndex[m_heap[0]] = 0;
        SiftDown(0);
    }
    m_heapIndex[top] = -1;
    return top;
}

void CPathFinder::HeapDecrease(int cell)
{
    SiftUp(m_heapIndex[cell]);
}

bool CPathFinder::LineWalkable(const Vec2& a, const Vec2& b, int clearance) const
{
    float dist = Distance(a, b);
    int steps = (int)(dist / (kCellSize * 0.5f)) + 1;
    Vec2 step = (b - a) * (1.0f / (float)steps);
    Vec2 p = a;
    int lastX = -1;
    int lastY = -1;
    for (int i = 0; i <= steps; ++i)
    {
        int cx = CTerrain::WorldToCell(p.x);
        int cy = CTerrain::WorldToCell(p.y);
        if (cx != lastX || cy != lastY)
        {
            if (!m_terrain->IsWalkableFor(cx, cy, clearance))
                return false;
            lastX = cx;
            lastY = cy;
        }
        p += step;
    }
    return true;
}

int CPathFinder::Reconstruct(int endCell, int startCell, int* cells, int maxCells) const
{
    int count = 0;
    int cell = endCell;
    while (cell >= 0 && count < maxCells)
    {
        cells[count++] = cell;
        if (cell == startCell)
            break;
        cell = m_parent[cell];
    }

    for (int i = 0; i < count / 2; ++i)
    {
        int tmp = cells[i];
        cells[i] = cells[count - 1 - i];
        cells[count - 1 - i] = tmp;
    }
    return count;
}

void CPathFinder::SmoothPath(const int* cells, int numCells, int clearance, CPath& out) const
{
    out.count = 0;
    out.current = 0;
    if (numCells <= 0)
        return;

    int anchor = 0;
    while (anchor < numCells - 1 && out.count < kMaxPathPoints)
    {
        Vec2 from(CTerrain::CellCenter(cells[anchor] % kMapCells), CTerrain::CellCenter(cells[anchor] / kMapCells));
        int furthest = anchor + 1;
        int lookahead = MinI(numCells - 1, anchor + 20);
        for (int i = lookahead; i > anchor + 1; --i)
        {
            Vec2 to(CTerrain::CellCenter(cells[i] % kMapCells), CTerrain::CellCenter(cells[i] / kMapCells));
            if (LineWalkable(from, to, clearance))
            {
                furthest = i;
                break;
            }
        }
        out.points[out.count++] = Vec2(CTerrain::CellCenter(cells[furthest] % kMapCells),
                                       CTerrain::CellCenter(cells[furthest] / kMapCells));
        anchor = furthest;
    }

    if (anchor < numCells - 1)
        out.partial = true;
}

bool CPathFinder::FindPath(const Vec2& from, const Vec2& to, int clearance, CPath& out)
{
    ++m_searches;
    out.Reset();
    out.goal = to;

    int sx = CTerrain::WorldToCell(from.x);
    int sy = CTerrain::WorldToCell(from.y);
    int gx = CTerrain::WorldToCell(to.x);
    int gy = CTerrain::WorldToCell(to.y);
    if (!m_terrain->IsWalkable(sx, sy))
        m_terrain->FindNearestWalkable(sx, sy, 4);
    bool goalMoved = false;
    if (!m_terrain->IsWalkableFor(gx, gy, clearance))
    {
        if (!m_terrain->FindNearestWalkable(gx, gy, 8))
            return false;
        goalMoved = true;
    }

    Vec2 goalPoint = goalMoved ? Vec2(CTerrain::CellCenter(gx), CTerrain::CellCenter(gy)) : to;
    if (LineWalkable(from, goalPoint, clearance))
    {
        out.points[0] = goalPoint;
        out.count = 1;
        out.partial = goalMoved;
        return true;
    }

    ++m_currentStamp;
    if (m_currentStamp == 0)
    {
        memset(m_stamp, 0, sizeof(m_stamp));
        m_currentStamp = 1;
    }

    int start = sy * kMapCells + sx;
    int goal = gy * kMapCells + gx;
    m_heapSize = 0;

    m_stamp[start] = m_currentStamp;
    m_g[start] = 0;
    m_f[start] = (u32)Heuristic(start, goal);
    m_parent[start] = -1;
    m_closed[start] = 0;
    HeapPush(start);

    int best = start;
    int bestH = Heuristic(start, goal);
    int expansions = 0;
    bool found = false;

    while (m_heapSize > 0 && expansions < kMaxExpansions)
    {
        int cell = HeapPop();
        m_closed[cell] = 1;
        ++expansions;

        if (cell == goal)
        {
            found = true;
            best = cell;
            break;
        }

        int h = Heuristic(cell, goal);
        if (h < bestH)
        {
            bestH = h;
            best = cell;
        }

        int cx = cell % kMapCells;
        int cy = cell / kMapCells;
        for (int d = 0; d < 8; ++d)
        {
            int nx = cx + s_dirX[d];
            int ny = cy + s_dirY[d];
            if (!m_terrain->IsWalkableFor(nx, ny, clearance))
                continue;
            if (d >= 4 && (!m_terrain->IsWalkableFor(cx + s_dirX[d], cy, clearance) ||
                           !m_terrain->IsWalkableFor(cx, cy + s_dirY[d], clearance)))
                continue;

            int next = ny * kMapCells + nx;
            if (m_stamp[next] != m_currentStamp)
            {
                m_stamp[next] = m_currentStamp;
                m_g[next] = kInfinite;
                m_closed[next] = 0;
                m_heapIndex[next] = -1;
            }
            if (m_closed[next])
                continue;

            u32 g = m_g[cell] + (d >= 4 ? kCostDiagonal : kCostStraight);
            if (g >= m_g[next])
                continue;

            m_g[next] = g;
            m_f[next] = g + (u32)Heuristic(next, goal);
            m_parent[next] = cell;
            if (m_heapIndex[next] >= 0)
                HeapDecrease(next);
            else
                HeapPush(next);
        }
    }

    if (best == start)
        return false;

    static int cells[kCellCount];
    int numCells = Reconstruct(best, start, cells, kCellCount);
    SmoothPath(cells, numCells, clearance, out);
    if (!found || goalMoved)
        out.partial = true;
    else if (out.count > 0 && !out.partial)
        out.points[out.count - 1] = to;
    return out.count > 0;
}

#include "World.h"

static const int kVertsPerRow = kMapCells + 1;

int CTerrain::WorldToCell(float v)
{
    return ClampI(FloorToInt(v / kCellSize), 0, kMapCells - 1);
}

float CTerrain::CellCenter(int c)
{
    return ((float)c + 0.5f) * kCellSize;
}

bool CTerrain::InBounds(int cx, int cy) const
{
    return cx >= 0 && cy >= 0 && cx < kMapCells && cy < kMapCells;
}

u8 CTerrain::Flags(int cx, int cy) const
{
    if (!InBounds(cx, cy))
        return CELL_NOWALK | CELL_NOBUILD | CELL_EDGE;
    return m_flags[cy * kMapCells + cx];
}

void CTerrain::ClearFlags(int cx, int cy, u8 mask)
{
    if (!InBounds(cx, cy))
        return;
    m_flags[cy * kMapCells + cx] &= (u8)~mask;
    ++m_revision;
}

int CTerrain::CliffLevel(int cx, int cy) const
{
    if (!InBounds(cx, cy))
        return 0;
    return m_cliff[cy * kMapCells + cx];
}

float CTerrain::HeightAt(const Vec2& p) const
{
    float fx = ClampF(p.x / kCellSize, 0.0f, (float)kMapCells - 0.001f);
    float fy = ClampF(p.y / kCellSize, 0.0f, (float)kMapCells - 0.001f);
    int x = (int)fx;
    int y = (int)fy;
    float tx = fx - (float)x;
    float ty = fy - (float)y;

    const float* row0 = &m_height[y * kVertsPerRow + x];
    const float* row1 = row0 + kVertsPerRow;
    float top = Lerp(row0[0], row0[1], tx);
    float bottom = Lerp(row1[0], row1[1], tx);
    return Lerp(top, bottom, ty);
}

bool CTerrain::IsWalkable(int cx, int cy) const
{
    return (Flags(cx, cy) & CELL_NOWALK) == 0;
}

bool CTerrain::IsWalkableFor(int cx, int cy, int clearance) const
{
    for (int y = cy - clearance; y <= cy + clearance; ++y)
    {
        for (int x = cx - clearance; x <= cx + clearance; ++x)
        {
            if (Flags(x, y) & CELL_NOWALK)
                return false;
        }
    }
    return true;
}

bool CTerrain::CanBuildFootprint(int cx, int cy, int size) const
{
    int x0 = cx - size / 2;
    int y0 = cy - size / 2;
    int level = CliffLevel(cx, cy);
    for (int y = y0; y < y0 + size; ++y)
    {
        for (int x = x0; x < x0 + size; ++x)
        {
            u8 f = Flags(x, y);
            if (f & (CELL_NOWALK | CELL_NOBUILD | CELL_STRUCTURE | CELL_TREE | CELL_RESERVED))
                return false;
            if (CliffLevel(x, y) != level)
                return false;
        }
    }
    return true;
}

void CTerrain::MarkFootprint(int cx, int cy, int size, bool set)
{
    int x0 = cx - size / 2;
    int y0 = cy - size / 2;
    for (int y = y0; y < y0 + size; ++y)
    {
        for (int x = x0; x < x0 + size; ++x)
        {
            if (!InBounds(x, y))
                continue;
            u8& f = m_flags[y * kMapCells + x];
            if (set)
                f |= CELL_STRUCTURE | CELL_NOWALK | CELL_NOBUILD;
            else
                f &= (u8)~(CELL_STRUCTURE | CELL_NOWALK | CELL_NOBUILD);
        }
    }
    ++m_revision;
}

// Ring-by-ring search outward from (cx, cy).
bool CTerrain::FindNearestWalkable(int& cx, int& cy, int maxRadius) const
{
    if (IsWalkable(cx, cy))
        return true;

    for (int r = 1; r <= maxRadius; ++r)
    {
        for (int d = -r; d <= r; ++d)
        {
            const int candidates[4][2] =
            {
                { cx + d, cy - r }, { cx + d, cy + r },
                { cx - r, cy + d }, { cx + r, cy + d }
            };
            for (int k = 0; k < 4; ++k)
            {
                if (IsWalkable(candidates[k][0], candidates[k][1]))
                {
                    cx = candidates[k][0];
                    cy = candidates[k][1];
                    return true;
                }
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Generation

float CTerrain::LatticeValue(int x, int y, u32 seed) const
{
    u32 h = (u32)x * 374761393u + (u32)y * 668265263u + seed * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return (float)(h & 0xFFFF) / 65535.0f;
}

float CTerrain::ValueNoise(float x, float y, u32 seed) const
{
    int ix = FloorToInt(x);
    int iy = FloorToInt(y);
    float tx = SmoothStep(x - (float)ix);
    float ty = SmoothStep(y - (float)iy);

    float v00 = LatticeValue(ix, iy, seed);
    float v10 = LatticeValue(ix + 1, iy, seed);
    float v01 = LatticeValue(ix, iy + 1, seed);
    float v11 = LatticeValue(ix + 1, iy + 1, seed);
    return Lerp(Lerp(v00, v10, tx), Lerp(v01, v11, tx), ty);
}

float CTerrain::FractalNoise(float x, float y, u32 seed, int octaves) const
{
    float sum = 0.0f;
    float amplitude = 1.0f;
    float norm = 0.0f;
    for (int i = 0; i < octaves; ++i)
    {
        sum += ValueNoise(x, y, seed + (u32)i * 31u) * amplitude;
        norm += amplitude;
        amplitude *= 0.5f;
        x *= 2.0f;
        y *= 2.0f;
    }
    return sum / norm;
}

void CTerrain::BuildHeights(u32 seed)
{
    for (int y = 0; y < kVertsPerRow; ++y)
    {
        for (int x = 0; x < kVertsPerRow; ++x)
        {
            float n = FractalNoise((float)x / 22.0f, (float)y / 22.0f, seed, 4);
            m_height[y * kVertsPerRow + x] = n * 4.2f - 0.4f;
        }
    }
}

void CTerrain::FlattenArea(const Vec2& center, float radius)
{
    float cx = center.x / kCellSize;
    float cy = center.y / kCellSize;
    float r = radius / kCellSize;
    float outer = r * 1.5f;
    float target = 1.5f;

    for (int y = 0; y < kVertsPerRow; ++y)
    {
        for (int x = 0; x < kVertsPerRow; ++x)
        {
            float dx = (float)x - cx;
            float dy = (float)y - cy;
            float d = sqrtf(dx * dx + dy * dy);
            if (d >= outer)
                continue;
            float& h = m_height[y * kVertsPerRow + x];
            if (d <= r)
                h = target;
            else
                h = Lerp(target, h, SmoothStep((d - r) / (outer - r)));
        }
    }
}

// Smooth corridor between two points: heights along the corridor are
// interpolated between the endpoint heights so no cliff edge survives.
void CTerrain::CarveRamp(const Vec2& a, const Vec2& b, float width)
{
    Vec2 ca(a.x / kCellSize, a.y / kCellSize);
    Vec2 cb(b.x / kCellSize, b.y / kCellSize);
    float w = width / kCellSize;
    float ha = HeightAt(a);
    float hb = HeightAt(b);
    if (ha < 0.6f) ha = 0.6f;
    if (hb < 0.6f) hb = 0.6f;

    Vec2 ab = cb - ca;
    float lenSq = ab.LengthSq();
    if (lenSq < 1.0f)
        return;

    for (int y = 0; y < kVertsPerRow; ++y)
    {
        for (int x = 0; x < kVertsPerRow; ++x)
        {
            Vec2 p((float)x, (float)y);
            float t = ClampF((p - ca).Dot(ab) / lenSq, 0.0f, 1.0f);
            Vec2 closest = ca + ab * t;
            float d = sqrtf(DistSq(p, closest));
            if (d > w)
                continue;
            float& h = m_height[y * kVertsPerRow + x];
            float along = Lerp(ha, hb, t);
            float blend = d < w * 0.6f ? 1.0f : 1.0f - (d - w * 0.6f) / (w * 0.4f);
            h = Lerp(h, along, blend);
        }
    }
}

void CTerrain::QuantizeCliffs()
{
    for (int y = 0; y < kMapCells; ++y)
    {
        for (int x = 0; x < kMapCells; ++x)
        {
            const float* v = &m_height[y * kVertsPerRow + x];
            float avg = (v[0] + v[1] + v[kVertsPerRow] + v[kVertsPerRow + 1]) * 0.25f;
            int level = ClampI(FloorToInt(avg), 0, 3);
            m_cliff[y * kMapCells + x] = (s8)level;
            m_flags[y * kMapCells + x] = avg < 0.35f ? (u8)(CELL_WATER | CELL_NOWALK | CELL_NOBUILD) : (u8)0;
        }
    }
}

void CTerrain::RebuildFlags()
{
    static const int dx[4] = { 1, -1, 0, 0 };
    static const int dy[4] = { 0, 0, 1, -1 };

    for (int y = 0; y < kMapCells; ++y)
    {
        for (int x = 0; x < kMapCells; ++x)
        {
            u8& f = m_flags[y * kMapCells + x];
            if (x < 2 || y < 2 || x >= kMapCells - 2 || y >= kMapCells - 2)
            {
                f |= CELL_EDGE | CELL_NOWALK | CELL_NOBUILD;
                continue;
            }

            const float* v = &m_height[y * kVertsPerRow + x];
            float h = (v[0] + v[1] + v[kVertsPerRow] + v[kVertsPerRow + 1]) * 0.25f;
            int level = m_cliff[y * kMapCells + x];
            for (int k = 0; k < 4; ++k)
            {
                int nx = x + dx[k];
                int ny = y + dy[k];
                const float* nv = &m_height[ny * kVertsPerRow + nx];
                float nh = (nv[0] + nv[1] + nv[kVertsPerRow] + nv[kVertsPerRow + 1]) * 0.25f;
                if (m_cliff[ny * kMapCells + nx] != level)
                {
                    // Steep step between levels is an impassable cliff edge,
                    // a gentle one is a ramp (walkable, not buildable).
                    if (AbsF(nh - h) > 0.3f)
                        f |= CELL_NOWALK | CELL_NOBUILD;
                    else
                        f |= CELL_NOBUILD;
                }
            }
        }
    }
}

void CTerrain::PlaceForests(CRandom& rng, const Vec2* keepClear, int numKeepClear)
{
    u32 seed = rng.Next();
    m_numTrees = 0;

    for (int y = 3; y < kMapCells - 3; ++y)
    {
        for (int x = 3; x < kMapCells - 3; ++x)
        {
            if (m_flags[y * kMapCells + x] & (CELL_NOWALK | CELL_NOBUILD))
                continue;
            Vec2 p(CellCenter(x), CellCenter(y));

            bool nearKeep = false;
            float bestKeepDist = 1e9f;
            int bestKeep = -1;
            for (int i = 0; i < numKeepClear; ++i)
            {
                float d = Distance(p, keepClear[i]);
                if (d < 9.0f * kCellSize)
                    nearKeep = true;
                if (d < bestKeepDist)
                {
                    bestKeepDist = d;
                    bestKeep = i;
                }
            }
            if (nearKeep)
                continue;

            bool place = FractalNoise((float)x / 9.0f, (float)y / 9.0f, seed, 3) > 0.64f;

            // Each base gets a tree line on the side facing away from the
            // map center so workers always have lumber nearby.
            if (!place && bestKeep >= 0 && bestKeep < 4 && bestKeepDist < 13.0f * kCellSize)
            {
                Vec2 center(kMapSize * 0.5f, kMapSize * 0.5f);
                float away = AngleTo(center, keepClear[bestKeep]);
                float toCell = AngleTo(keepClear[bestKeep], p);
                if (AbsF(AngleDiff(away, toCell)) < 0.9f)
                    place = rng.Chance(0.85f);
            }

            if (place && m_numTrees < (int)COUNTOF(m_treeCells))
            {
                m_flags[y * kMapCells + x] |= CELL_TREE | CELL_NOWALK | CELL_NOBUILD;
                m_treeCells[m_numTrees++] = (u16)(y * kMapCells + x);
            }
        }
    }
}

void CTerrain::PickExpansionsAndCamps(CRandom& rng, const Vec2* starts, int numStarts)
{
    Vec2 center(kMapSize * 0.5f, kMapSize * 0.5f);

    m_numExpansions = 0;
    for (int i = 0; i < numStarts && i < 4; ++i)
    {
        const Vec2& a = starts[i];
        const Vec2& b = starts[(i + 1) % numStarts];
        Vec2 mid = (a + b) * 0.5f;
        mid = mid + (center - mid) * 0.15f;
        m_expansions[m_numExpansions++] = mid;
    }

    m_numCampSites = 0;
    for (int i = 0; i < m_numExpansions; ++i)
        m_campSites[m_numCampSites++] = m_expansions[i] + (center - m_expansions[i]) * 0.3f;
    for (int i = 0; i < numStarts && m_numCampSites < 12; ++i)
    {
        Vec2 toCenter = center - starts[i];
        m_campSites[m_numCampSites++] = starts[i] + toCenter * 0.45f;
    }
    while (m_numCampSites < 12)
    {
        float angle = rng.RangeF(0.0f, kTwoPi);
        float dist = rng.RangeF(4.0f, 12.0f) * kCellSize;
        m_campSites[m_numCampSites++] = PolarOffset(center, dist, angle);
    }

}

void CTerrain::Generate(CRandom& rng, const Vec2* starts, int numStarts)
{
    m_revision = 0;
    BuildHeights(rng.Next());

    Vec2 center(kMapSize * 0.5f, kMapSize * 0.5f);
    for (int i = 0; i < numStarts; ++i)
        FlattenArea(starts[i], 12.0f * kCellSize);
    FlattenArea(center, 6.0f * kCellSize);
    for (int i = 0; i < numStarts; ++i)
    {
        CarveRamp(starts[i], center, 4.0f * kCellSize);
        CarveRamp(starts[i], starts[(i + 1) % numStarts], 3.0f * kCellSize);
    }

    PickExpansionsAndCamps(rng, starts, numStarts);
    for (int i = 0; i < m_numExpansions; ++i)
        FlattenArea(m_expansions[i], 6.0f * kCellSize);

    QuantizeCliffs();
    RebuildFlags();

    Vec2 keep[8];
    int numKeep = 0;
    for (int i = 0; i < numStarts && numKeep < 4; ++i)
        keep[numKeep++] = starts[i];
    for (int i = 0; i < m_numExpansions && numKeep < 8; ++i)
        keep[numKeep++] = m_expansions[i];
    PlaceForests(rng, keep, numKeep);

    for (int i = 0; i < m_numCampSites; ++i)
    {
        int cx = WorldToCell(m_campSites[i].x);
        int cy = WorldToCell(m_campSites[i].y);
        if (FindNearestWalkable(cx, cy, 10))
            m_campSites[i] = Vec2(CellCenter(cx), CellCenter(cy));
    }
}

#include "World.h"

// Per-player visibility grid. Every refresh clears the visible bit and
// re-reveals around each unit by casting rays outward; a ray stops when it
// climbs to a higher cliff level than the viewer or hits a tree (ground
// viewers only). The explored bit is sticky.

static const int kFogRays = 64;

void CFogOfWar::Init(CTerrain* terrain)
{
    m_terrain = terrain;
    memset(m_state, 0, sizeof(m_state));
}

void CFogOfWar::BeginUpdate(int player)
{
    u8* s = m_state[player];
    for (int i = 0; i < kCellCount; ++i)
        s[i] &= (u8)~FOG_VISIBLE;
}

void CFogOfWar::CastRay(int player, int cx, int cy, float dx, float dy, int steps, int viewerLevel, bool flying)
{
    float x = (float)cx + 0.5f;
    float y = (float)cy + 0.5f;
    u8* s = m_state[player];

    for (int i = 0; i < steps; ++i)
    {
        x += dx;
        y += dy;
        int ix = (int)x;
        int iy = (int)y;
        if (!m_terrain->InBounds(ix, iy))
            return;

        s[iy * kMapCells + ix] |= FOG_VISIBLE | FOG_EXPLORED;
        if (flying)
            continue;
        if (m_terrain->CliffLevel(ix, iy) > viewerLevel)
            return;
        if (m_terrain->Flags(ix, iy) & CELL_TREE)
            return;
    }
}

void CFogOfWar::Reveal(int player, const Vec2& p, float radius, bool flying)
{
    int cx = CTerrain::WorldToCell(p.x);
    int cy = CTerrain::WorldToCell(p.y);
    int steps = (int)(radius / kCellSize);
    int level = m_terrain->CliffLevel(cx, cy);

    m_state[player][cy * kMapCells + cx] |= FOG_VISIBLE | FOG_EXPLORED;
    for (int r = 0; r < kFogRays; ++r)
    {
        float a = (float)r * (kTwoPi / (float)kFogRays);
        CastRay(player, cx, cy, FastCos(a), FastSin(a), steps, level, flying);
    }
}

bool CFogOfWar::IsVisible(int player, const Vec2& p) const
{
    int cx = CTerrain::WorldToCell(p.x);
    int cy = CTerrain::WorldToCell(p.y);
    return (m_state[player][cy * kMapCells + cx] & FOG_VISIBLE) != 0;
}

bool CFogOfWar::IsExplored(int player, const Vec2& p) const
{
    int cx = CTerrain::WorldToCell(p.x);
    int cy = CTerrain::WorldToCell(p.y);
    return (m_state[player][cy * kMapCells + cx] & FOG_EXPLORED) != 0;
}


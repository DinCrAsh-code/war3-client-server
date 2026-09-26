// Map-level systems: terrain grid, pathfinding, spatial partitioning of
// units, and per-player fog of war.

#ifndef MR_WORLD_H
#define MR_WORLD_H

#include "Engine.h"

class CUnit;

const int   kMapCells  = 96;
const float kCellSize  = 32.0f;
const float kMapSize   = kMapCells * kCellSize;
const int   kCellCount = kMapCells * kMapCells;

enum CellFlags
{
    CELL_NOWALK    = 0x01,
    CELL_NOBUILD   = 0x02,
    CELL_WATER     = 0x04,
    CELL_TREE      = 0x08,
    CELL_STRUCTURE = 0x10,
    CELL_EDGE      = 0x20,
    CELL_RESERVED  = 0x40
};

// ---------------------------------------------------------------------------
// Terrain.cpp

class CTerrain
{
public:
    void  Generate(CRandom& rng, const Vec2* starts, int numStarts);
    u8    Flags(int cx, int cy) const;
    void  ClearFlags(int cx, int cy, u8 mask);
    int   CliffLevel(int cx, int cy) const;
    float HeightAt(const Vec2& p) const;
    bool  InBounds(int cx, int cy) const;
    bool  IsWalkable(int cx, int cy) const;
    bool  IsWalkableFor(int cx, int cy, int clearance) const;
    bool  CanBuildFootprint(int cx, int cy, int size) const;
    void  MarkFootprint(int cx, int cy, int size, bool set);
    bool  FindNearestWalkable(int& cx, int& cy, int maxRadius) const;
    int   TreeCount() const { return m_numTrees; }
    u16   TreeCell(int i) const { return m_treeCells[i]; }
    int   ExpansionCount() const { return m_numExpansions; }
    Vec2  Expansion(int i) const { return m_expansions[i]; }
    int   CampSiteCount() const { return m_numCampSites; }
    Vec2  CampSite(int i) const { return m_campSites[i]; }
    u32   Revision() const { return m_revision; }

    static int   WorldToCell(float v);
    static float CellCenter(int c);

private:
    float LatticeValue(int x, int y, u32 seed) const;
    float ValueNoise(float x, float y, u32 seed) const;
    float FractalNoise(float x, float y, u32 seed, int octaves) const;
    void  BuildHeights(u32 seed);
    void  QuantizeCliffs();
    void  FlattenArea(const Vec2& center, float radius);
    void  CarveRamp(const Vec2& a, const Vec2& b, float width);
    void  PlaceForests(CRandom& rng, const Vec2* keepClear, int numKeepClear);
    void  PickExpansionsAndCamps(CRandom& rng, const Vec2* starts, int numStarts);
    void  RebuildFlags();

    float m_height[(kMapCells + 1) * (kMapCells + 1)];
    s8    m_cliff[kCellCount];
    u8    m_flags[kCellCount];
    u16   m_treeCells[1500];
    int   m_numTrees;
    Vec2  m_expansions[4];
    int   m_numExpansions;
    Vec2  m_campSites[12];
    int   m_numCampSites;
    u32   m_revision;
};

// Center of a size x size footprint anchored at cell (cx, cy): odd sizes
// sit on the cell center, even sizes on the cell's lower-left corner.
inline Vec2 FootprintCenter(int cx, int cy, int size)
{
    float half = (size & 1) ? kCellSize * 0.5f : 0.0f;
    return Vec2((float)cx * kCellSize + half, (float)cy * kCellSize + half);
}

// ---------------------------------------------------------------------------
// Pathing.cpp

const int kMaxPathPoints = 24;

struct CPath
{
    Vec2 points[kMaxPathPoints];
    int  count;
    int  current;
    Vec2 goal;
    bool partial;

    void Reset() { count = 0; current = 0; partial = false; }
    bool Done() const { return current >= count; }
};

class CPathFinder
{
public:
    void Init(CTerrain* terrain);
    bool FindPath(const Vec2& from, const Vec2& to, int clearance, CPath& out);
    bool LineWalkable(const Vec2& a, const Vec2& b, int clearance) const;
    int  SearchCount() const { return m_searches; }

private:
    int  Heuristic(int cell, int goal) const;
    void HeapPush(int cell);
    int  HeapPop();
    void HeapDecrease(int cell);
    void SiftUp(int pos);
    void SiftDown(int pos);
    int  Reconstruct(int endCell, int startCell, int* cells, int maxCells) const;
    void SmoothPath(const int* cells, int numCells, int clearance, CPath& out) const;

    CTerrain* m_terrain;
    u32       m_g[kCellCount];
    u32       m_f[kCellCount];
    s32       m_parent[kCellCount];
    u16       m_stamp[kCellCount];
    u8        m_closed[kCellCount];
    s32       m_heapIndex[kCellCount];
    s32       m_heap[kCellCount];
    int       m_heapSize;
    u16       m_currentStamp;
    int       m_searches;
};

// ---------------------------------------------------------------------------
// SpatialGrid.cpp

const int   kGridBuckets = 24;
const float kBucketSize  = kMapSize / kGridBuckets;

typedef bool (*UnitFilterFn)(CUnit* unit, void* context);
typedef void (*UnitVisitFn)(CUnit* unit, void* context);

class CSpatialGrid
{
public:
    void   Init();
    void   Insert(CUnit* unit);
    void   Remove(CUnit* unit);
    void   Move(CUnit* unit);
    int    Visit(const Vec2& center, float radius, UnitVisitFn fn, void* context) const;
    int    Collect(const Vec2& center, float radius, UnitFilterFn filter, void* context,
                   CUnit** out, int maxOut) const;
    CUnit* FindNearest(const Vec2& center, float radius, UnitFilterFn filter, void* context) const;

private:
    static int BucketFor(const Vec2& p);
    void       BucketRange(const Vec2& center, float radius, int& x0, int& y0, int& x1, int& y1) const;

    CUnit* m_heads[kGridBuckets * kGridBuckets];
};

// ---------------------------------------------------------------------------
// Fog.cpp

class CFogOfWar
{
public:
    void Init(CTerrain* terrain);
    void BeginUpdate(int player);
    void Reveal(int player, const Vec2& p, float radius, bool flying);
    bool IsVisible(int player, const Vec2& p) const;
    bool IsExplored(int player, const Vec2& p) const;

private:
    void CastRay(int player, int cx, int cy, float dx, float dy, int steps, int viewerLevel, bool flying);

    enum { FOG_VISIBLE = 1, FOG_EXPLORED = 2 };

    CTerrain* m_terrain;
    u8        m_state[kMaxPlayers][kCellCount];
};

#endif

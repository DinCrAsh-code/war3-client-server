//============================================================================
//  0x6F29D690 - the terrain-tint sample a unit's own RefreshTerrainTint
//  (unit_terraintint.cpp) reads its colour from.
//
//  Most of the time this is nothing more than "the terrain tint at the
//  unit's own position": ask CUnit::GetPosition (this build never asks for
//  ground height or bounds here - 0/-1/0) and hand the point straight to
//  QueryTerrainTintAt.
//
//  When CAgentWar3::m_flags bit 0x2000 is set, the unit has a real
//  footprint and the shipped code samples the terrain at the four cardinal
//  edge midpoints of that footprint instead of at the unit's own point,
//  and takes the per-channel MAXIMUM of the four - a unit standing across
//  a light/dark terrain seam picks up the brighter tint from whichever
//  side is lit, rather than the tint directly underfoot.
//
//  The half-width offset (UnitTypeFloatField1A0_6F29C150, this unit
//  type's own +0x1A0 field) is computed once and its negation
//  (`fchs`) is a real, shared local: it is spilled to its own stack slot
//  and reloaded for both the west and south samples rather than
//  re-negated twice, which is why only two of the four corners store an
//  intermediate before adding the unit's own coordinate and two do not.
//  Every corner's "offset axis" is written as coordinate + halfOffset and
//  its "unchanged axis" as coordinate + 0.0f - a literal 0.0f passed to a
//  shared per-corner point setter, not folded away, because IEEE add does
//  not let the compiler treat x+0.0f as free.  Written the same way here so
//  the two shared-zero loads the compiler folds into one `fldz` per corner
//  come out in the same order.
//
//  Own translation unit: called once, from unit_terraintint.cpp, and calls
//  nothing this repo has not already reconstructed elsewhere
//  (CUnit::GetPosition, QueryTerrainTintAt, UnitTypeFloatField1A0_6F29C150).
//============================================================================
#include "unit.h"
#include "fvec3.h"
#include "packedcolor.h"

//  0x6F00BF60 - stormsingleton_terrain.cpp.
unsigned int* __fastcall QueryTerrainTintAt(unsigned int* out, const FVec3* point);
//  0x6F29C150 - unittypequeries.cpp.
float __fastcall UnitTypeFloatField1A0_6F29C150(unsigned int fourCC);

//  Per-channel maximum of the four sampled colours, in the shipped
//  east/west/north/south comparison order.
static unsigned char Max4(unsigned char a, unsigned char b,
                          unsigned char c, unsigned char d)
{
    unsigned char m = a;
    if (b > m) m = b;
    if (c > m) m = c;
    if (d > m) m = d;
    return m;
}

unsigned int* CUnit::SampleFootprintTerrainTint(unsigned int* out)
{
    FVec3 pos;
    GetPosition(&pos, 0, -1, 0);

    if (!(m_flags & 0x2000))
    {
        return QueryTerrainTintAt(out, &pos);
    }

    float half = UnitTypeFloatField1A0_6F29C150(m_footprintType);
    float negHalf = -half;

    FVec3 p;
    SPackedColor east, west, north, south;

    p.m_x = pos.m_x + half;
    p.m_y = pos.m_y + 0.0f;
    p.m_z = pos.m_z + 0.0f;
    QueryTerrainTintAt((unsigned int*)&east, &p);

    p.m_x = negHalf;
    p.m_x = p.m_x + pos.m_x;
    p.m_y = pos.m_y + 0.0f;
    p.m_z = pos.m_z + 0.0f;
    QueryTerrainTintAt((unsigned int*)&west, &p);

    p.m_x = pos.m_x + 0.0f;
    p.m_y = pos.m_y + half;
    p.m_z = pos.m_z + 0.0f;
    QueryTerrainTintAt((unsigned int*)&north, &p);

    p.m_x = pos.m_x + 0.0f;
    p.m_y = negHalf + pos.m_y;
    p.m_z = pos.m_z + 0.0f;
    QueryTerrainTintAt((unsigned int*)&south, &p);

    SPackedColor result;
    *(unsigned int*)&result = 0;
    result.m_c2 = Max4(east.m_c2, west.m_c2, north.m_c2, south.m_c2);
    result.m_c1 = Max4(east.m_c1, west.m_c1, north.m_c1, south.m_c1);
    result.m_c0 = Max4(east.m_c0, west.m_c0, north.m_c0, south.m_c0);
    result.m_c3 = Max4(east.m_c3, west.m_c3, north.m_c3, south.m_c3);

    *out = *(unsigned int*)&result;
    return out;
}

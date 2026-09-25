//============================================================================
//  0x6F47E9E0 / 0x6F47EC80 / 0x6F47EF40 / 0x6F47F1F0 / 0x6F47F4C0 /
//  0x6F47F7B0 - the six agent enumerators.
//
//  Every one of them is the same seven steps:
//
//    1. take the query radius and the centre into *grid* space - the centre
//       relative to the map origin, both through game.h's WorldToGrid;
//    2. build the axis-aligned square that circumscribes the circle;
//    3. borrow the next AgileAgentQuery off CGameData::m_queryPool, growing
//       the pool by one when the depth has caught up with it - that is what
//       makes running a query from inside a visit callback safe;
//    4. scale the square by the grid's cells-per-world-unit and turn it into
//       the half-open cell rectangle covering it;
//    5. point the query at that rectangle and that grid and collect;
//    6. walk the collected set, applying the *real* circular test the cell
//       rectangle could only approximate, plus whatever type filter this
//       variant carries;
//    7. give the pool slot back.
//
//  What separates the six is two independent choices, neither of which is a
//  run-time branch here because 0x6F47FAF0 / 0x6F47FB50 have already made
//  them:
//
//    - the distance test.  EnumerateInCircle* compare against the query
//      radius squared, so a candidate counts when its *centre* is inside the
//      circle.  EnumerateTouchingCircle* compare against (radius + the
//      candidate's own collision radius) squared, so a candidate counts when
//      its own circle merely touches.  The touching three still compute the
//      plain radius squared first and never read it - that dead multiply is
//      in all three shipped bodies, and it is a real call, so it is written
//      out here rather than dropped;
//    - the type filter: none, an exact agile type id, or "derived from".
//
//  The circular test is a hardware-float compare on the encoded CFloats,
//  the same AsHardwareFloat trick positionrangecheck.cpp uses and for the
//  same reason - and `<=` rather than `!(a > b)`, because `<=` is the
//  comparison MSVC gives `test ah,5` / `jnp` and that is what is in the
//  binary.
//============================================================================
#include "agentquery.h"
#include "boxmath.h"
#include "agent.h"

//  0x6F6EF3B0 - Math/softfloat_distsq.cpp; also declared in pathmove.h,
//  which this file does not need for anything else.
CFloat __fastcall CFloatDistanceSquared(const CGridVec2& a,
                                        const CGridVec2& b);

//  By *reference*, unlike positionrangecheck.cpp's own copy of this helper:
//  the shipped code `fld`s the two squared distances straight out of the
//  slots they were computed into.  Taking the argument by value instead
//  materialises a second copy of each into a fresh slot first, four extra
//  instructions per comparison.
static inline float AsHardwareFloat(const CFloat& value)
{
    return *(const float*)&value;
}

//----------------------------------------------------------------------------
//  0x6F47E9E0 - centre inside the circle, no type filter.
//----------------------------------------------------------------------------
void SQueryRadius::EnumerateInCircle(const int* x, const int* y,
                                     unsigned int mask, UnitQueryVisitFn fn,
                                     void* ctx) const
{
    CTimeSync* sync = g_pTimeSync;

    CFloat radius = WorldToGrid(m_radius);

    CGameData* gd = g_pGameData;

    CFloat gy = *(const CFloat*)y - gd->m_originY;
    gy = WorldToGrid(gy);
    CFloat gx = *(const CFloat*)x - g_pGameData->m_originX;
    CGridVec2 centre(WorldToGrid(gx), gy);

    SCellBoxF box(centre.m_y - radius, centre.m_x - radius,
                  centre.m_y + radius, centre.m_x + radius);

    if ((unsigned int)gd->m_queryDepth >= gd->m_queryPool.m_count)
        gd->m_queryPool.SetCount(gd->m_queryDepth + 1);

    //  Read the depth, publish depth + 1, *then* index with the old value:
    //  the shipped stream loads m_data before the store, which a plain
    //  post-increment subscript does not produce.
    int depth = gd->m_queryDepth;
    AgileAgentQuery** pool = gd->m_queryPool.m_data;
    gd->m_queryDepth = depth + 1;
    AgileAgentQuery* q = pool[depth];

    q->m_running = 1;
    q->m_excludeMask = mask;
    q->SetAnchor(0);

    CFloat scale(sync->GetCellGrid()->m_cellScale.m_bits);
    SCellBoxF cellsPerUnit(scale, scale, scale, scale);
    q->SetBox(
        sync->GetCellGrid()->FloatBoxToCellBox(MulCellBoxF(box, cellsPerUnit)));
    q->SetGrid(sync->GetCellGrid());
    q->Refresh();

    CFloat radiusSq = radius * radius;

    unsigned int total = q->m_count;
    unsigned int i = 0;
    while (i < total)
    {
        CPathTrace* trace =
            (CPathTrace*)((SAgentQueryHit*)q->m_data)[i].m_reg->m_nodeRecord;

        CGridVec2 pos(trace->m_origin);
        CFloat distSq = CFloatDistanceSquared(pos, centre);

        if (!(AsHardwareFloat(distSq) > AsHardwareFloat(radiusSq)))
        {
            SAgileAgent* owner = trace->m_owner;
            if (owner->m_retired == 0)
            {
                CAgent* agent = owner->m_agent;
                if (agent != 0 && fn(agent, ctx) == 0)
                    break;
            }
        }
        i++;
    }

    q->m_running = 0;
    gd->m_queryDepth--;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  0x6F47EF40 - centre inside the circle, exact agile type id.
//----------------------------------------------------------------------------
void SQueryRadius::EnumerateInCircleOfType(const int* x, const int* y,
                                           unsigned int mask,
                                           unsigned int type,
                                           UnitQueryVisitFn fn,
                                           void* ctx) const
{
    CTimeSync* sync = g_pTimeSync;

    CFloat radius = WorldToGrid(m_radius);

    CGameData* gd = g_pGameData;

    CFloat gy = *(const CFloat*)y - gd->m_originY;
    gy = WorldToGrid(gy);
    CFloat gx = *(const CFloat*)x - g_pGameData->m_originX;
    CGridVec2 centre(WorldToGrid(gx), gy);

    SCellBoxF box(centre.m_y - radius, centre.m_x - radius,
                  centre.m_y + radius, centre.m_x + radius);

    if ((unsigned int)gd->m_queryDepth >= gd->m_queryPool.m_count)
        gd->m_queryPool.SetCount(gd->m_queryDepth + 1);

    //  Read the depth, publish depth + 1, *then* index with the old value:
    //  the shipped stream loads m_data before the store, which a plain
    //  post-increment subscript does not produce.
    int depth = gd->m_queryDepth;
    AgileAgentQuery** pool = gd->m_queryPool.m_data;
    gd->m_queryDepth = depth + 1;
    AgileAgentQuery* q = pool[depth];

    q->m_running = 1;
    q->m_excludeMask = mask;
    q->SetAnchor(0);

    CFloat scale(sync->GetCellGrid()->m_cellScale.m_bits);
    SCellBoxF cellsPerUnit(scale, scale, scale, scale);
    q->SetBox(
        sync->GetCellGrid()->FloatBoxToCellBox(MulCellBoxF(box, cellsPerUnit)));
    q->SetGrid(sync->GetCellGrid());
    q->Refresh();

    CFloat radiusSq = radius * radius;

    unsigned int total = q->m_count;
    unsigned int i = 0;
    while (i < total)
    {
        CPathTrace* trace =
            (CPathTrace*)((SAgentQueryHit*)q->m_data)[i].m_reg->m_nodeRecord;

        CGridVec2 pos(trace->m_origin);
        CFloat distSq = CFloatDistanceSquared(pos, centre);

        if (!(AsHardwareFloat(distSq) > AsHardwareFloat(radiusSq)))
        {
            SAgileAgent* owner = trace->m_owner;
            if (owner->m_retired == 0)
            {
                CAgent* agent = owner->m_agent;
                if (agent != 0 &&
                    agent->GetAgileTypeId() == type &&
                    fn(agent, ctx) == 0)
                    break;
            }
        }
        i++;
    }

    q->m_running = 0;
    gd->m_queryDepth--;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  0x6F47EC80 - centre inside the circle, agile type derived from `base`.
//----------------------------------------------------------------------------
void SQueryRadius::EnumerateInCircleDerivedFrom(const int* x, const int* y,
                                                unsigned int mask,
                                                unsigned int base,
                                                UnitQueryVisitFn fn,
                                                void* ctx) const
{
    CTimeSync* sync = g_pTimeSync;

    CFloat radius = WorldToGrid(m_radius);

    CGameData* gd = g_pGameData;

    CFloat gy = *(const CFloat*)y - gd->m_originY;
    gy = WorldToGrid(gy);
    CFloat gx = *(const CFloat*)x - g_pGameData->m_originX;
    CGridVec2 centre(WorldToGrid(gx), gy);

    SCellBoxF box(centre.m_y - radius, centre.m_x - radius,
                  centre.m_y + radius, centre.m_x + radius);

    if ((unsigned int)gd->m_queryDepth >= gd->m_queryPool.m_count)
        gd->m_queryPool.SetCount(gd->m_queryDepth + 1);

    //  Read the depth, publish depth + 1, *then* index with the old value:
    //  the shipped stream loads m_data before the store, which a plain
    //  post-increment subscript does not produce.
    int depth = gd->m_queryDepth;
    AgileAgentQuery** pool = gd->m_queryPool.m_data;
    gd->m_queryDepth = depth + 1;
    AgileAgentQuery* q = pool[depth];

    q->m_running = 1;
    q->m_excludeMask = mask;
    q->SetAnchor(0);

    CFloat scale(sync->GetCellGrid()->m_cellScale.m_bits);
    SCellBoxF cellsPerUnit(scale, scale, scale, scale);
    q->SetBox(
        sync->GetCellGrid()->FloatBoxToCellBox(MulCellBoxF(box, cellsPerUnit)));
    q->SetGrid(sync->GetCellGrid());
    q->Refresh();

    CFloat radiusSq = radius * radius;

    unsigned int total = q->m_count;
    unsigned int i = 0;
    while (i < total)
    {
        CPathTrace* trace =
            (CPathTrace*)((SAgentQueryHit*)q->m_data)[i].m_reg->m_nodeRecord;

        CGridVec2 pos(trace->m_origin);
        CFloat distSq = CFloatDistanceSquared(pos, centre);

        if (!(AsHardwareFloat(distSq) > AsHardwareFloat(radiusSq)))
        {
            SAgileAgent* owner = trace->m_owner;
            if (owner->m_retired == 0)
            {
                CAgent* agent = owner->m_agent;
                if (agent != 0 &&
                    gd->IsTypeDerivedFrom(agent->GetAgileTypeId(), base) &&
                    fn(agent, ctx) == 0)
                    break;
            }
        }
        i++;
    }

    q->m_running = 0;
    gd->m_queryDepth--;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  0x6F47F1F0 - the candidate's own circle need only touch, no type filter.
//----------------------------------------------------------------------------
void SQueryRadius::EnumerateTouchingCircle(const int* x, const int* y,
                                           unsigned int mask,
                                           UnitQueryVisitFn fn,
                                           void* ctx) const
{
    CTimeSync* sync = g_pTimeSync;

    CFloat radius = WorldToGrid(m_radius);

    CGameData* gd = g_pGameData;

    CFloat gy = *(const CFloat*)y - gd->m_originY;
    gy = WorldToGrid(gy);
    CFloat gx = *(const CFloat*)x - g_pGameData->m_originX;
    CGridVec2 centre(WorldToGrid(gx), gy);

    SCellBoxF box(centre.m_y - radius, centre.m_x - radius,
                  centre.m_y + radius, centre.m_x + radius);

    if ((unsigned int)gd->m_queryDepth >= gd->m_queryPool.m_count)
        gd->m_queryPool.SetCount(gd->m_queryDepth + 1);

    //  Read the depth, publish depth + 1, *then* index with the old value:
    //  the shipped stream loads m_data before the store, which a plain
    //  post-increment subscript does not produce.
    int depth = gd->m_queryDepth;
    AgileAgentQuery** pool = gd->m_queryPool.m_data;
    gd->m_queryDepth = depth + 1;
    AgileAgentQuery* q = pool[depth];

    q->m_running = 1;
    q->m_excludeMask = mask;
    q->SetAnchor(0);

    CFloat scale(sync->GetCellGrid()->m_cellScale.m_bits);
    SCellBoxF cellsPerUnit(scale, scale, scale, scale);
    q->SetBox(
        sync->GetCellGrid()->FloatBoxToCellBox(MulCellBoxF(box, cellsPerUnit)));
    q->SetGrid(sync->GetCellGrid());
    q->Refresh();

    //  Computed and never read - all three "touching" bodies carry it, and
    //  it is a real call, so the shipped stream has it too.  The threshold
    //  this variant actually compares against is built per candidate below.
    CFloat radiusSq = radius * radius;

    unsigned int total = q->m_count;
    unsigned int i = 0;
    while (i < total)
    {
        CPathTrace* trace =
            (CPathTrace*)((SAgentQueryHit*)q->m_data)[i].m_reg->m_nodeRecord;

        CGridVec2 pos(trace->m_origin);
        CFloat distSq = CFloatDistanceSquared(pos, centre);

        CFloat reach = radius + CFloat(trace->m_radius);

        if (!(AsHardwareFloat(distSq) > AsHardwareFloat(reach * reach)))
        {
            SAgileAgent* owner = trace->m_owner;
            if (owner->m_retired == 0)
            {
                CAgent* agent = owner->m_agent;
                if (agent != 0 &&
                    fn(agent, ctx) == 0)
                    break;
            }
        }
        i++;
    }

    q->m_running = 0;
    gd->m_queryDepth--;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  0x6F47F7B0 - touching, exact agile type id.
//----------------------------------------------------------------------------
void SQueryRadius::EnumerateTouchingCircleOfType(const int* x, const int* y,
                                                 unsigned int mask,
                                                 unsigned int type,
                                                 UnitQueryVisitFn fn,
                                                 void* ctx) const
{
    CTimeSync* sync = g_pTimeSync;

    CFloat radius = WorldToGrid(m_radius);

    CGameData* gd = g_pGameData;

    CFloat gy = *(const CFloat*)y - gd->m_originY;
    gy = WorldToGrid(gy);
    CFloat gx = *(const CFloat*)x - g_pGameData->m_originX;
    CGridVec2 centre(WorldToGrid(gx), gy);

    SCellBoxF box(centre.m_y - radius, centre.m_x - radius,
                  centre.m_y + radius, centre.m_x + radius);

    if ((unsigned int)gd->m_queryDepth >= gd->m_queryPool.m_count)
        gd->m_queryPool.SetCount(gd->m_queryDepth + 1);

    //  Read the depth, publish depth + 1, *then* index with the old value:
    //  the shipped stream loads m_data before the store, which a plain
    //  post-increment subscript does not produce.
    int depth = gd->m_queryDepth;
    AgileAgentQuery** pool = gd->m_queryPool.m_data;
    gd->m_queryDepth = depth + 1;
    AgileAgentQuery* q = pool[depth];

    q->m_running = 1;
    q->m_excludeMask = mask;
    q->SetAnchor(0);

    CFloat scale(sync->GetCellGrid()->m_cellScale.m_bits);
    SCellBoxF cellsPerUnit(scale, scale, scale, scale);
    q->SetBox(
        sync->GetCellGrid()->FloatBoxToCellBox(MulCellBoxF(box, cellsPerUnit)));
    q->SetGrid(sync->GetCellGrid());
    q->Refresh();

    //  Computed and never read - all three "touching" bodies carry it, and
    //  it is a real call, so the shipped stream has it too.  The threshold
    //  this variant actually compares against is built per candidate below.
    CFloat radiusSq = radius * radius;

    unsigned int total = q->m_count;
    unsigned int i = 0;
    while (i < total)
    {
        CPathTrace* trace =
            (CPathTrace*)((SAgentQueryHit*)q->m_data)[i].m_reg->m_nodeRecord;

        CGridVec2 pos(trace->m_origin);
        CFloat distSq = CFloatDistanceSquared(pos, centre);

        CFloat reach = radius + CFloat(trace->m_radius);

        if (!(AsHardwareFloat(distSq) > AsHardwareFloat(reach * reach)))
        {
            SAgileAgent* owner = trace->m_owner;
            if (owner->m_retired == 0)
            {
                CAgent* agent = owner->m_agent;
                if (agent != 0 &&
                    agent->GetAgileTypeId() == type &&
                    fn(agent, ctx) == 0)
                    break;
            }
        }
        i++;
    }

    q->m_running = 0;
    gd->m_queryDepth--;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  0x6F47F4C0 - touching, agile type derived from `base`.
//----------------------------------------------------------------------------
void SQueryRadius::EnumerateTouchingCircleDerivedFrom(const int* x,
                                                      const int* y,
                                                      unsigned int mask,
                                                      unsigned int base,
                                                      UnitQueryVisitFn fn,
                                                      void* ctx) const
{
    CTimeSync* sync = g_pTimeSync;

    CFloat radius = WorldToGrid(m_radius);

    CGameData* gd = g_pGameData;

    CFloat gy = *(const CFloat*)y - gd->m_originY;
    gy = WorldToGrid(gy);
    CFloat gx = *(const CFloat*)x - g_pGameData->m_originX;
    CGridVec2 centre(WorldToGrid(gx), gy);

    SCellBoxF box(centre.m_y - radius, centre.m_x - radius,
                  centre.m_y + radius, centre.m_x + radius);

    if ((unsigned int)gd->m_queryDepth >= gd->m_queryPool.m_count)
        gd->m_queryPool.SetCount(gd->m_queryDepth + 1);

    //  Read the depth, publish depth + 1, *then* index with the old value:
    //  the shipped stream loads m_data before the store, which a plain
    //  post-increment subscript does not produce.
    int depth = gd->m_queryDepth;
    AgileAgentQuery** pool = gd->m_queryPool.m_data;
    gd->m_queryDepth = depth + 1;
    AgileAgentQuery* q = pool[depth];

    q->m_running = 1;
    q->m_excludeMask = mask;
    q->SetAnchor(0);

    CFloat scale(sync->GetCellGrid()->m_cellScale.m_bits);
    SCellBoxF cellsPerUnit(scale, scale, scale, scale);
    q->SetBox(
        sync->GetCellGrid()->FloatBoxToCellBox(MulCellBoxF(box, cellsPerUnit)));
    q->SetGrid(sync->GetCellGrid());
    q->Refresh();

    //  Computed and never read - all three "touching" bodies carry it, and
    //  it is a real call, so the shipped stream has it too.  The threshold
    //  this variant actually compares against is built per candidate below.
    CFloat radiusSq = radius * radius;

    unsigned int total = q->m_count;
    unsigned int i = 0;
    while (i < total)
    {
        CPathTrace* trace =
            (CPathTrace*)((SAgentQueryHit*)q->m_data)[i].m_reg->m_nodeRecord;

        CGridVec2 pos(trace->m_origin);
        CFloat distSq = CFloatDistanceSquared(pos, centre);

        CFloat reach = radius + CFloat(trace->m_radius);

        if (!(AsHardwareFloat(distSq) > AsHardwareFloat(reach * reach)))
        {
            SAgileAgent* owner = trace->m_owner;
            if (owner->m_retired == 0)
            {
                CAgent* agent = owner->m_agent;
                if (agent != 0 &&
                    gd->IsTypeDerivedFrom(agent->GetAgileTypeId(), base) &&
                    fn(agent, ctx) == 0)
                    break;
            }
        }
        i++;
    }

    q->m_running = 0;
    gd->m_queryDepth--;
}

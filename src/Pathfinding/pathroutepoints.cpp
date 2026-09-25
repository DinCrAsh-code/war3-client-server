//============================================================================
//  0x6F49A470 - CPathSearch::BuildRoutePoints.
//
//  Walk the chain of node records the query produced (linked by the index
//  at record+0x1C, -1 terminating) and push one grid point per node into
//  the caller's point cache, converting each node's integer cell
//  coordinates to CFloats and offsetting them to the cell centre.
//
//  Two wrinkles the shipped code carries:
//
//  * In mode 2 a node can name a *block* of cells rather than one cell -
//    record+0x22 is the block's log2 size - and the point has to land
//    inside the block rather than on the boundary between it and the next
//    one, so a coordinate sitting exactly on the block's top edge is
//    stepped back by one before the conversion.
//
//  * A node whose record+0x23 "kind" byte is set gets a second point
//    pushed after it, carrying that kind in its y component against a
//    fixed sentinel x (g_pathSpanEpsilon).  Those markers are counted in
//    m_extraPoints, which is the only thing CPathClient::RunPathQuery
//    reads that field for.
//
//  The first and last points are then overwritten with the caller's own
//  start and end, which is why the route always begins and ends exactly
//  where it was asked to rather than at a cell centre.
//============================================================================
#include "pathfind.h"
#include "pathmove.h"

//  flt_6FAAE4D4 - already declared as a hardware `float` elsewhere
//  (pathtracegrid.cpp); MSVC mangles the type into the symbol, so it is
//  re-declared here with that same type and reinterpreted at the one place
//  it is used as a CFloat rather than given a second, unlinkable spelling.
extern const float g_slopeThreshold0;      // flt_6FAAE4D4

//  dword_6FAAE4B8 - the cell-centre offset added to both coordinates.  Only
//  this function reads it, so it keeps its address for a name.
extern const CFloat g_unk6FAAE4B8;         // 0x6FAAE4B8

//  One node record: 9 dwords.  Only the five fields this walk touches are
//  named.
struct SPathRouteNode
{
    int           m_x;            // +0x00
    int           m_y;            // +0x04
    char          m_reserved08[0x1C - 0x08];
    int           m_next;         // +0x1C  -1 terminates
    char          m_reserved20[0x22 - 0x20];
    unsigned char m_blockShift;   // +0x22
    unsigned char m_kind;         // +0x23
};

void CPathSearch::BuildRoutePoints(int firstNode, SPathPointCache* cache,
                                   const CGridVec2* endPoint,
                                   const CGridVec2* startPoint)
{
    //  The two arms build their sum into two different temporaries in the
    //  shipped code, which is what an `if`/`else` over two separate
    //  CFloat locals produces - not one local assigned twice.
    int blockMode = (m_reserved90 == 2);

    const CFloat* base = blockMode ? &g_slopeThreshold1
                                   : (const CFloat*)&g_slopeThreshold0;
    CFloat centre = *base + g_unk6FAAE4B8;

    CGridVec2 marker(g_pathSpanEpsilon, g_pathSpanEpsilon);

    int index = firstNode;
    do
    {
        SPathRouteNode* node = (SPathRouteNode*)((char*)m_routeNodes + index * 36);
        int y = node->m_y;
        int x = node->m_x;
        //  Unsigned: the shipped guard is `jbe`, not `jle`.
        unsigned int shift = node->m_blockShift;

        if (blockMode && shift > 0)
        {
            //  Step back off the block's own top edge so the point lands
            //  inside the block rather than on the seam with the next one.
            int span = 1 << shift;
            int topY = ((y >> shift) << shift) + span - 1;
            int topX = ((x >> shift) << shift) + span - 1;
            if (x == topX)
                x -= 1;
            if (y == topY)
                y -= 1;
        }

        CGridVec2 point(kCFloatNoInit);
        point.m_x = CFloatFromInt(x) + centre;
        point.m_y = CFloatFromInt(y) + centre;
        cache->Grow(&point, 1);

        //  Widened before the test, not tested as a byte: the shipped code
        //  zero-extends the kind once and uses the same register for both
        //  the test and the conversion.
        unsigned int kind = node->m_kind;
        if (kind != 0)
        {
            marker.m_y = CFloatFromInt(kind);
            cache->Grow(&marker, 1);
            m_extraPoints += 1;
        }

        index = node->m_next;
    }
    while (index != -1);

    CGridVec2* points = cache->Points();
    points[cache->m_count - 1] = *endPoint;
    cache->Points()[0] = *startPoint;
}
